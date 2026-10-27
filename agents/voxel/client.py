from dataclasses import dataclass
import math

import grpc
from google.protobuf.message import Message

from ._generated import simulation_pb2, simulation_pb2_grpc


@dataclass(frozen=True)
class Position:
    """Spawn coordinates; the simulator chooses the surface height."""

    x: float
    z: float


@dataclass(frozen=True)
class Action:
    dir_x: float
    dir_z: float
    jump: bool = False


class SimulationRpcError(RuntimeError):
    def __init__(self, operation: str, address: str, agent_id: int | None,
                 error: grpc.RpcError):
        self.operation = operation
        self.address = address
        self.agent_id = agent_id
        self.code = error.code()
        self.details = error.details()
        super().__init__(
            f"{operation} at {address} (agent_id={agent_id}) failed: "
            f"{self.code.name}: {self.details}"
        )


class SimulationClient:
    def __init__(self, address: str, *, rpc_timeout: float = 5.0,
                 reset_timeout: float = 30.0):
        for name, timeout in (("rpc_timeout", rpc_timeout), ("reset_timeout", reset_timeout)):
            if not math.isfinite(timeout) or timeout <= 0:
                raise ValueError(f"{name} must be finite and positive")
        self.address = address
        self.rpc_timeout = rpc_timeout
        self.reset_timeout = reset_timeout
        self.channel = grpc.insecure_channel(address)
        self.stub = simulation_pb2_grpc.SimulationServiceStub(self.channel)
        self.id: int | None = None
        self._closed = False

    def __enter__(self) -> "SimulationClient":
        self._ensure_open()
        return self

    def __exit__(self, exc_type, exc_value, traceback) -> None:
        self.close()

    def close(self) -> None:
        """Release the channel. This does not unregister the robot on the server."""
        if not self._closed:
            self._closed = True
            self.channel.close()

    def _ensure_open(self) -> None:
        if self._closed:
            raise RuntimeError("SimulationClient is closed; create a new client/session")

    def _require_registration(self) -> None:
        self._ensure_open()
        if self.id is None:
            raise RuntimeError("Call register_agent() before reset() or step()")

    def _call(self, operation: str, rpc, request: Message, *, timeout: float):
        self._ensure_open()
        try:
            return rpc(request, timeout=timeout)
        except grpc.RpcError as error:
            rejected = {
                grpc.StatusCode.INVALID_ARGUMENT,
                grpc.StatusCode.FAILED_PRECONDITION,
                grpc.StatusCode.RESOURCE_EXHAUSTED,
                grpc.StatusCode.PERMISSION_DENIED,
                grpc.StatusCode.UNAUTHENTICATED,
                grpc.StatusCode.UNIMPLEMENTED,
            }
            if error.code() not in rejected:
                # Stop the client entirely for unknown errors
                self.close()
            raise SimulationRpcError(operation, self.address, self.id, error) from error

    def register_agent(self, pos: Position) -> bool:
        self._ensure_open()
        if self.id is not None:
            raise RuntimeError("This client already has a registered agent")
        request = simulation_pb2.Vec3(x=pos.x, y=0, z=pos.z)
        response = self._call("RegisterAgent", self.stub.RegisterAgent, request,
                              timeout=self.rpc_timeout)
        self.id = response.id
        return True

    def reset(self, seed: int) -> simulation_pb2.ResetResponse:
        self._require_registration()
        request = simulation_pb2.ResetRequest(id=self.id, seed=seed)
        return self._call("Reset", self.stub.Reset, request, timeout=self.reset_timeout)

    def step(self, action: Action) -> simulation_pb2.StepResponse:
        self._require_registration()
        request = simulation_pb2.StepRequest(
            dir_x=action.dir_x, dir_z=action.dir_z, id=self.id, jump=action.jump,
        )
        return self._call("Step", self.stub.Step, request, timeout=self.rpc_timeout)
