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
    enum class CmdStatus{
        QUEUED,
        EXECUTING,
        FINISHED,
        CANCELLED,
    };

    struct ResetCommand{
        uint32_t id;
        uint32_t seed;
    };

    struct RegisterAgentCommand{
        glm::vec2 position;
    };

    struct RegisteredAgent{
        uint32_t id;
    };

    using Command = std::variant<ResetCommand, RegisterAgentCommand, InputAction>;
    using CommandResult = std::variant<StepResult, RegisteredAgent, grpc::Status>;

    struct PendingCommand{
        Command command;
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

    grpc::Status RegisterAgent(
        grpc::ServerContext* context,
        const voxel::Vec3* request,
        voxel::RegistrationResponse* response
    ) override;

    void processPendingCmds(Simulation& simulation);
    void closeQueue();
};

#endif //SIMULATION_SERVICE_H
