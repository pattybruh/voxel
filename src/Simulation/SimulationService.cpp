#include "SimulationService.h"
#include <chrono>
#include <cmath>
#include <exception>
#include <limits>
#include <optional>
#include <string>
#include <utility>

namespace {

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
    pending->command = ResetCommand{request->id(), request->seed()};
    auto future = pending->promise.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if(m_queue_closed){
            return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Server shutting down");
        }
        m_cmd_queue.push(pending);
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
        CommandResult result = future.get();
        if(const auto* status = std::get_if<grpc::Status>(&result)){
            return *status;
        }
        convertObservation(std::get<StepResult>(result).observation, response->mutable_obs());
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
    if (request->id() > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Unknown agent id");
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
    pending->command = InputAction{inX, inZ, static_cast<int>(request->id()), request->jump()};
    auto future = pending->promise.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if (m_queue_closed) {
            return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Server shutting down");
        }
        m_cmd_queue.push(pending);
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
        CommandResult result = future.get();
        if(const auto* status = std::get_if<grpc::Status>(&result)){
            return *status;
        }
        const StepResult& res = std::get<StepResult>(result);
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


grpc::Status SimulationServiceImpl::RegisterAgent(
    grpc::ServerContext* context,
    const voxel::Vec3* request,
    voxel::RegistrationResponse* response
) {
    if (context->IsCancelled()) {
        return grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled");
    }
    auto pending = std::make_shared<PendingCommand>();
    pending->command = RegisterAgentCommand{glm::vec2{request->x(), request->z()}};
    auto future = pending->promise.get_future();
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        if(m_queue_closed){
            return grpc::Status(grpc::StatusCode::UNAVAILABLE, "Server shutting down");
        }
        m_cmd_queue.push(pending);
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
        CommandResult result = future.get();
        if(const auto* status = std::get_if<grpc::Status>(&result)){
            return *status;
        }
        response->set_id(std::get<RegisteredAgent>(result).id);
        return grpc::Status::OK;
    } catch (const std::exception& error) {
        return grpc::Status(grpc::StatusCode::INTERNAL, std::string("Register agent failed: ") + error.what());
    } catch (...) {
        return grpc::Status(grpc::StatusCode::INTERNAL, "Register agent failed: unknown exception");
    }
}


void SimulationServiceImpl::processPendingCmds(Simulation& simulation){
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    if(m_cmd_queue.empty()){
        return;
    }
    std::shared_ptr<PendingCommand> cmd = m_cmd_queue.front();
    m_cmd_queue.pop();
    const bool cancelled = cmd->status == CmdStatus::CANCELLED;
    if(!cancelled){
        cmd->status = CmdStatus::EXECUTING;
    }

    lock.unlock();

    if(cancelled){
        cmd->promise.set_value(grpc::Status(grpc::StatusCode::CANCELLED, "Request cancelled"));
        return;
    }
    std::optional<CommandResult> result;
    try {
        if(const auto* reset = std::get_if<ResetCommand>(&(cmd->command))){
            if(!simulation.has_agent(reset->id)) {
                result.emplace(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Unknown agent id"));
            }
            else{
                if(!simulation.reset(reset->seed)) {
                    result.emplace(grpc::Status(grpc::StatusCode::FAILED_PRECONDITION,
                        "An agent's spawn position is unavailable on the requested terrain"));
                }
                else{
                    result.emplace(simulation.observe(static_cast<int>(reset->id)));
                }
            }
        }
        else if(const auto* registerCmd = std::get_if<RegisterAgentCommand>(&(cmd->command))){
            if(simulation.agent_cnt() >= 1){
                result.emplace(grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED,
                    "Only 1 agent allowed currently"));
            }
            else{
                const int id = simulation.add_agent(registerCmd->position);
                if(id < 0){
                    result.emplace(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                        "Agent spawn position is invalid or has no surface"));
                }
                else{
                    result.emplace(RegisteredAgent{static_cast<uint32_t>(id)});
                }
            }
        }
        else{
            const auto& action = std::get<InputAction>(cmd->command);
            if(!simulation.has_agent(static_cast<uint32_t>(action.agentId))) {
                result.emplace(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Unknown agent id"));
            }
            else {
                simulation.step({action});
                result.emplace(simulation.observe(action.agentId));
            }
        }
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
    cmd->promise.set_value(std::move(*result));
}

void SimulationServiceImpl::closeQueue(){
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    if(m_queue_closed){
        return;
    }
    m_queue_closed = true;
    while(!m_cmd_queue.empty()){
        auto cmd = m_cmd_queue.front();
        m_cmd_queue.pop();
        cmd->status = CmdStatus::CANCELLED;
        cmd->promise.set_value(grpc::Status(grpc::StatusCode::UNAVAILABLE, "Server shutting down"));
    }
}
