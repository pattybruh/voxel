block class /
finish chunk class
    chunk has draw call that takes in renderer
    this way chunk can call renderer->draw(va,ib) on its own

MVP: agent can reach a flag on generated terrain.

### Roadmap

1. Start the agent at a known location on set seed map. Put a flag at a reachable location.
   Success is touching the flag before a fixed number of actions.

2. Make one action correspond to a known amount of simulation time.
   Current input is tied to framerate, decouple physics from rendering.

3. Define small action and observation.
   Action: can be movement in 2 directions + jump.
   Observation: position, veloctiy, isGrounded, flag direction, sensor data
   Sensor: can be simple nearby height grid

4. Expose gRPC Reset and Step.
   Reset(seed): creates an episode and returns its first observation
   Step(action): applies the action for fixed number of physics ticks and returns next observation, reward, ended or not

5. Prove the environment works before training.
   Try a hand written policy that heads toward the flag.
   Check resets with the same seed gives the same terrain and start, that the flag is reachable, and that success and timeout are reported correctly.

   Then train one agent and compare its success rate against a random policy on new seeds

### After
Actions/Observations factor in current facing direction
    directional movements
    sensor data

Better sensors: camera, general sensor framework