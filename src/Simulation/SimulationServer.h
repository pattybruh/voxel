#ifndef SIMULATION_SERVER_H 
#define SIMULATION_SERVER_H

#include <memory>
#include <string>
#include <grpcpp/server.h>
#include "SimulationService.h"

class SimulationServer{
private:
    SimulationServiceImpl m_service;
    std::unique_ptr<grpc::Server> m_server;
public:
    explicit SimulationServer(const std::string& addr);
    ~SimulationServer();

    void processPendingCmds(Simulation& simulation);
};

#endif // SIMULATION_SERVER_H