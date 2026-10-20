#include "SimulationServer.h"
#include <iostream>
#include <stdexcept>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>

SimulationServer::SimulationServer(const std::string& addr){
    grpc::ServerBuilder builder;
	builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
	builder.RegisterService(&m_service);
	m_server = builder.BuildAndStart();
    if(!m_server){
        throw std::runtime_error("Failed to start simulation server at " + addr);
    }
    std::cout << "Simulation gRPC server listening on " << addr << std::endl;
}

SimulationServer::~SimulationServer(){
    m_service.closeQueue();
    m_server->Shutdown();
}

void SimulationServer::processPendingCmds(Simulation& simulation){
    m_service.processPendingCmds(simulation);
}
