# Work In Progress!

# A high-performance voxel engine in C
Currently a work-in-progress, still contains many bugs and is incomplete.
This project is intended as a way to test the graphics library "Sokol".

# Build and Run
## NOTE:
If using a trackpad, ensure that you disable any OS settings that disable your 
mouse when typing (such as "Disable Touchpad While Typing" on Ubuntu)

This project currently has no external dependencies to run. The build process 
is only set up on Linux, this will not run on Windows or MacOs. Support for 
other platforms will be added in the future.

## Clone
First, clone the project to a local directory using:
```
git clone https://github.com/angstrom-123/Voxel-Engine.git
```
## Build
This project uses GNU make which is required to build. 
To build the project, run:
```
make release
```
## Run
If successful, the executable will be placed in the output directory.
Run the executable with:
```
./bin/[platform]/output/minecraft-release
```
# Features
- Infinite world generation with dynamic chunk loading / unloading
- Chunk remeshing to reduce unseen quads
- BMP texture atlas (custom parser) with mipmapping
- Bespoke Perlin Noise implementation
- My own math library for matrices, vectors, and more
- Fully controllable FPS-style player controller

# Screenshots 
## Custom Perlin Noise Implementation
<img width="519" height="287" alt="Screenshot from 2025-07-12 17-27-26" src="https://github.com/user-attachments/assets/b403c4f3-09c6-4749-af82-459cc2a7a48b" />

## Terrain generation with 2D Perlin Noise
<img width="1308" height="786" alt="Screenshot from 2025-07-18 17-23-22" src="https://github.com/user-attachments/assets/6356ae1a-2520-4392-833a-28006a74217e" />
<img width="1308" height="786" alt="Screenshot from 2025-07-18 17-24-11" src="https://github.com/user-attachments/assets/90b03a79-1c1c-40a3-b900-f454ccd9df56" />
