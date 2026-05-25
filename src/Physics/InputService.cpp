//
// Created by Patrick Li
//
#include "InputService.h"
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <iostream>
#include <queue>
#include <mutex>
#include <memory>
#include <string>

#include "../generated/simulation.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;

using voxel::SimulationInput;
using voxel::Keystroke;
using voxel::EnvResponse;

static std::unique_ptr<Server> server;

std::queue<char> input_queue;
std::mutex input_queue_m;

class InputServiceImpl final : public SimulationInput::Service {
	Status InputCommand(ServerContext *context, const Keystroke *request, EnvResponse *response) override {
		std::scoped_lock<std::mutex> lock(input_queue_m);
		std::cout << "Received keystroke: " << request->key() << std::endl;
		if(!request->key().empty()) {
			input_queue.push(request->key().back());
		}
		return Status::OK;
	}

	Status InputSequence(ServerContext *context, ServerReader<Keystroke> *reader, EnvResponse *response) override {
		Keystroke key;
		while (reader->Read(&key))
		{
			std::cout << "Received keystroke in sequence: " << key.key() << std::endl;
		}
		return Status::OK;
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	InputServiceImpl service;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	server = builder.BuildAndStart();
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}

void CloseServer() {
    if(server){
    	std::cout << "Server shutting down" << std::endl;
		server->Shutdown();
    }
}

bool GetInput(char& command) {
	std::scoped_lock<std::mutex> lock(input_queue_m);
	if(!input_queue.empty()) {
		command = input_queue.front();
		input_queue.pop();
		return true;
	}
	return false;
}