import os
import sys

# Find the absolute path to the 'generated' folder relative to this script
script_dir = os.path.dirname(os.path.abspath(__file__))
generated_path = os.path.abspath(os.path.join(script_dir, '..', 'generated'))
if generated_path not in sys.path:
    sys.path.insert(0, generated_path)

import grpc
import simulation_pb2
import simulation_pb2_grpc

def run():
    with grpc.insecure_channel('localhost:50051') as channel:
        stub = simulation_pb2_grpc.SimulationInputStub(channel)
        print("Type 'q' and press Enter to quit.")
        print("------------------------------------\n")
        
        while True:
            user_input = input("Enter command: ").strip()
            if user_input.lower() == 'q':
                print("Exiting client application. Goodbye!")
                break
                
            if not user_input:
                continue

            keystroke = simulation_pb2.Keystroke(key=user_input)
            
            try:
                response = stub.InputCommand(keystroke)
            except grpc.RpcError as e:
                print(f"Failed to send command: {e.details()}")

if __name__ == '__main__':
    run()