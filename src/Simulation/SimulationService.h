#ifndef SIMULATION_SERVICE_H
#define SIMULATION_SERVICE_H

#include "simulation.grpc.pb.h"
#include "Simulation.h"

#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <variant>

class SimulationServiceImpl final : public voxel::SimulationService::Service {
private:
    struct ResetCommand{
        uint32_t seed;
    };

    enum class CmdStatus{
        QUEUED,
        EXECUTING,
        FINISHED,
        CANCELLED,
    };

    using CommandResult = std::variant<StepResult, grpc::Status>;

    struct PendingCommand{
        std::variant<ResetCommand, InputAction> command;
        std::promise<CommandResult> promise;
        CmdStatus status = CmdStatus::QUEUED;
    };

    std::mutex m_queue_mutex;
    std::queue<std::shared_ptr<PendingCommand>> m_cmd_queue;
    bool m_queue_closed = false;

public:
    grpc::Status Reset(
        grpc::ServerContext* context,
        const voxel::ResetRequest* request,
        voxel::ResetResponse* response
    ) override;

    grpc::Status Step(
        grpc::ServerContext* context,
        const voxel::StepRequest* request,
        voxel::StepResponse* response
    ) override;

    void processPendingCmds(Simulation& simulation);
    void closeQueue();
};

#endif //SIMULATION_SERVICE_H
