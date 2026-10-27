import gymnasium as gym
import numpy as np

from agents.voxel.client import Action, SimulationClient


class VoxelEnv(gym.Env):
    """Base class for Voxel environments."""

    def __init__(self, addr: str = "localhost:50051"):
        self.client = SimulationClient(addr)
        self.observation_space = gym.spaces.Dict(
            {
                "heightmap": gym.spaces.Box(
                    low=-np.inf,
                    high=np.inf,
                    shape=(9,9),
                    dtype=np.float32
                ),
                "validmap": gym.spaces.MultiBinary(
                    n=(9,9),
                    seed=None
                ),
            }
        )
        self.action_space = gym.spaces.Dict(
            {
                "direction": gym.spaces.Box(
                    low=-1.0,
                    high=1.0,
                    shape=(2,),
                    dtype=np.float32
                ),
                "jump": gym.spaces.Discrete(
                    n=2,
                    start=0,
                    dtype=np.uint8
                ),
            }
        )

    def step(self, action: ActType) -> tuple[ObsType, float, bool, bool, dict]:
        raise NotImplementedError

    def reset(self, *, seed: int | None = None, options: dict | None = None) -> tuple[ObsType, dict]:
        raise NotImplementedError

    def close(self) -> None:
        raise NotImplementedError