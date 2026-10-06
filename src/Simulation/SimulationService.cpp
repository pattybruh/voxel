#include "SimulationService.h"
#include <chrono>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>

namespace {

constexpr int SINGLE_AGENT_ID = 0;

void convertObservation(const Observation& src, voxel::Observation* dst){
    for(int i=0; i<Heightmap::SIZE; ++i){
        for(int j=0; j<Heightmap::SIZE; ++j){
            dst->add_heights(src.heightmap.heights[i][j]);
            dst->add_valids(src.heightmap.valids[i][j]);
        }
    }
    dst->set_map_x_sz(Heightmap::SIZE);
    dst->set_map_z_sz(Heightmap::SIZE);
    dst->mutable_position()->set_x(src.position.x);
    dst->mutable_position()->set_y(src.position.y);
    dst->mutable_position()->set_z(src.position.z);
    dst->mutable_velocity()->set_x(src.velocity.x);
    dst->mutable_velocity()->set_y(src.velocity.y);
    dst->mutable_velocity()->set_z(src.velocity.z);
    dst->set_is_grounded(src.is_grounded);
}

} // namespace

grpc::Status SimulationServiceImpl::Reset(
    grpc::ServerContext* context,
    const voxel::ResetRequest* request,
    voxel::ResetResponse* response
) {
    if (context->IsCancelled()) {
        return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
    }
    auto pending = std::make_shared<PendingCommand>();
    pending->command = ResetCommand{request->seed()};
    auto future = pending->promise.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_commands.push(pending);
    }
    try {
        while(future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready){
            if(context->IsCancelled()){
                std::lock_guard<std::mutex> lock(m_queue_mutex);
                if(pending->status == CmdStatus::QUEUED){
                    pending->status = CmdStatus::CANCELLED;
                }
                return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
            }
        }
        if (context->IsCancelled()) {
            return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
        }
        convertObservation(future.get().observation, response->mutable_obs());
        return grpc::Status::OK;
    } catch (const std::exception& error) {
        return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Reset failed: ") + error.what());
    } catch (...) {
        return grpc::Status(grpc::StatusCode::INTERNAL, "Reset failed: unknown exception");
    }
}

grpc::Status SimulationServiceImpl::Step(
    grpc::ServerContext* context,
    const voxel::StepRequest* request,
    voxel::StepResponse* response
) {
    if (context->IsCancelled()) {
        return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
    }
    float inX = request->dir_x();
    float inZ = request->dir_z();
    if(!std::isfinite(inX) || !std::isfinite(inZ)){
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Movement inputs must be finite");
    }
    if(inX > 1.0f){
        inX = 1.0f;
    }
    else if(inX < -1.0f){
        inX = -1.0f;
    }
    if(inZ > 1.0f){
        inZ = 1.0f;
    }
    else if(inZ < -1.0f){
        inZ = -1.0f;
    }
    auto pending = std::make_shared<PendingCommand>();
    pending->command = InputAction{inX, inZ, SINGLE_AGENT_ID, request->jump()};
    auto future = pending->promise.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_commands.push(pending);
    }
    try {
        while(future.wait_for(std::chrono::milliseconds(10)) != std::future_status::ready){
            if(context->IsCancelled()){
                std::lock_guard<std::mutex> lock(m_queue_mutex);
                if(pending->status == CmdStatus::QUEUED){
                    pending->status = CmdStatus::CANCELLED;
                }
                return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
            }
        }
        if (context->IsCancelled()) {
            return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
        }
        StepResult res = future.get();
        convertObservation(res.observation, response->mutable_obs());
        response->set_reward(res.reward);
        response->set_terminated(res.terminated);
        response->set_truncated(res.truncated);
        return grpc::Status::OK;
    } catch (const std::exception& error) {
        return grpc::Status(grpc::StatusCode::INTERNAL,
                            std::string("Step failed: ") + error.what());
    } catch (...) {
        return grpc::Status(grpc::StatusCode::INTERNAL, "Step failed: unknown exception");
    }
}

void SimulationServiceImpl::processPendingCmds(Simulation& simulation){
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    if(m_commands.empty()){
        return;
    }
    std::shared_ptr<PendingCommand> cmd = m_commands.front();
    m_commands.pop();
    const bool cancelled = cmd->status == CmdStatus::CANCELLED;
    if(!cancelled){
        cmd->status = CmdStatus::EXECUTING;
    }

    lock.unlock();

    if(cancelled){
        cmd->promise.set_exception(std::make_exception_ptr(std::runtime_error("Command cancelled")));
        return;
    }
    StepResult result;
    try {
        if(const auto* reset = std::get_if<ResetCommand>(&(cmd->command))){
            simulation.reset(reset->seed);
        }
        else{
            const auto& action = std::get<InputAction>(cmd->command);
            simulation.step({action});
        }
        result = simulation.observe(SINGLE_AGENT_ID);
    } catch (...) {
        const auto error = std::current_exception();
        {
            std::lock_guard<std::mutex> status_lock(m_queue_mutex);
            cmd->status = CmdStatus::FINISHED;
        }
        cmd->promise.set_exception(error);
        return;
    }
    {
        std::lock_guard<std::mutex> status_lock(m_queue_mutex);
        cmd->status = CmdStatus::FINISHED;
    }
    cmd->promise.set_value(std::move(result));
}
