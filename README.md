# Voxel Simulation Engine for AI Training

A voxel based 3D simulation engine written in **C++** and **OpenGL**, designed to serve as an interactive environment for both **autonomous AI agents**. The goal of this project is to provide a lightweight, extensible sandbox where agents can perceive, interact, and learn in a fully simulated world.

---

## Current Features

- **Chunk Based Terrain System**  
  - Procedural Perlin noise generation for infinite-like terrain.  
  - Supports **worlds exceeding 2 million voxels**.  
  - Dynamic loading/unloading of up to **256 chunks** with constant memory usage under **250 MB**.  

- **Physics & Environment**  
  - Custom physics engine with **AABB collision detection** and tunable friction.  
  - Real time voxel state changes (block breaking, placement, environment updates).  
  - Stable simulation at **60+ FPS** in **256×256×64 voxel worlds**.  

- **Rendering Optimizations**  
  - **Batched mesh updates**, reducing draw calls by ~70%.  
  - Smooth frame rates even in large procedurally generated environments.  

- **Entity Framework**  
  - Extensible system for dozens of agents.  
  - Basic perception through raycasts and proximity queries.  

---

## Tech Stack

- **C++**
- **OpenGL**
- **GLFW/GLAD**
- **glm** for math utilities  

---

## Building and Running

The project uses CMake to configure and build the C++ executable.

Run these commands from the project root directory:

```bash
# Configure the project (needed the first time, or after changing the build configuration)
cmake -S . -B build

# Compile the project
cmake --build build
```

The compiled executable is created at `build/Voxel`.

To run the simulator:

```bash
./build/Voxel
```

The simulator starts a gRPC server on port `50051`. Keep the simulator running while starting the Python agent in a second terminal.

The Python agent uses the Conda `base` environment, which contains the Python gRPC package. From the project root, run:

```bash
conda activate base
python3 agents/test.py
```

The agent connects to the simulator at `localhost:50051`. It waits for commands typed into the terminal and sends them to the running simulator. For example, entering `w` sends a `w` command.

If `conda activate base` is not recognized, initialize Conda for the Z shell and restart the terminal:

```bash
conda init zsh
```

---

## What’s Next

Planned features to extend simulation and AI training capabilities:

- **Forces & Advanced Physics**  
  - Add support for forces (gravity, impulses) for more realistic dynamics.  
  - Elastic and inelastic collisions for voxel objects.  

- **AI Agent Interfaces**  
  - Sensor systems (vision cones, ray sensors, neighborhood queries).  
  - Action APIs (movement, block placement, interaction).  
  - Python scripting support to let users attach their own AI models.  

- **Model Integration**  
  -  .

---

## Screenshots / Demo 

---

## Long-Term Vision

The long-term goal is to provide a **simulation platform for training intelligent agents** in a voxel based world, similar to how robotics simulations are used for reinforcement learning, but with a focus on extensibility and performance.

---
