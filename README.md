# A simple remake of Minecraft in C
This project is intended as a way to test the graphics library "Sokol".

# Build and Run
This project currently has no external dependencies to run. The build process 
is only set up on Linux, this will not run on Windows or MacOs. Support for 
other platforms will be added in the future.

If editing shaders, an executable of Sokol's sokol-shdc must be present in the 
project's root as this is used to compile GLSL shaders into c headers for use 
with the project.
