# fluid-simulation

[Jos Stam's - Real-Time Fluid Dynamics for Games](https://graphics.cs.cmu.edu/nsp/course/15-464/Fall09/papers/StamFluidforGames.pdf)
  
### Controls
* `LEFT MOUSE BUTTON` Paint fluid
* `RIGHT MOUSE BUTTON` Paint wall
* `SPACE` Reset to blank canvas
  
### Software Details
* Single threaded application
* Simulation updated at a fixed timestep of 30FPS (easy to modify)
* CPU writing raw colour data to texture then passing to OPENGL --> GPU
* Upscaled a 8x with bilinear filter (easy to modify)

<img width="537" height="571" alt="Screenshot from 2026-02-11 00-32-14" src="https://github.com/user-attachments/assets/edac9622-cc3e-4f7d-a354-9c74a82c3c75" />
