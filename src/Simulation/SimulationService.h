#ifndef SIMULATION_SERVICE_H
#define SIMULATION_SERVICE_H

#include "simulation.grpc.pb.h"
#include "Simulation.h"

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

    struct PendingCommand{
        std::variant<ResetCommand, InputAction> command;
        std::promise<StepResult> promise;
    };

    std::mutex m_queue_mutex;
    std::queue<std::shared_ptr<PendingCommand>> m_commands;

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
};

#endif //SIMULATION_SERVICE_H