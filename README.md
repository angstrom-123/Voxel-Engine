# A simple remake of Minecraft in C
Currently a work-in-progress, still contains many bugs and is incomplete.
This project is intended as a way to test the graphics library "Sokol".

# Build and Run
## NOTE:
If using a trackpad, ensure that you disable any OS settings that disable your 
mouse when typing (such as "Disable Touchpad While Typing" on Ubuntu)

This project currently has no external dependencies to run. The build process 
is only set up on Linux, this will not run on Windows or MacOs. Support for 
other platforms will be added in the future.

If editing shaders, an executable of Sokol's sokol-shdc must be present in the 
project's root as this is used to compile GLSL shaders into c headers for use 
with the project.

## Clone
First, clone the project to a local directory using:
```
git clone https://github.com/angstrom-123/Minecraft-Remake.git
```
## Build
This project uses GNU make which is required to build. 
To build the project, run:
```
make install
```
## Run
If successful, the executable will be placed in the /build/ directory.
Run the executable with:
```
./build/minecraft
```
## Shaders
If you wish to recompile the shaders, then an executable of Sokol SHDC is required 
to be present in the project's root directory. Installation instructions can be 
found on its GitHub page.
To compile the shaders, run:
```
make shaders
```
Then, to apply the changes, build the project as usual:
```
make install
```
# Features
- Infinite world generation with dynamic chunk loading / unloading
- Use of VBO and IBO to optimize resource bindings
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
