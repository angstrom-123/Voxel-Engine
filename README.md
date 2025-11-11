# Work In Progress!

# A high-performance voxel engine in C
Currently a work-in-progress, still contains many bugs and is incomplete.
This is my first time making a non-trivial game, my first time working with the GPU, and my first serious C project. 

# Build and Run
If using a trackpad, ensure that you disable any OS settings that disable your 
mouse when typing (such as "Disable Touchpad While Typing" on Ubuntu)

The build process is only set up on Linux, this will not run on Windows or MacOs. Support for other platforms will be added in the future.

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
./bin/[platform]/output/Voxel-Engine-release
```
# Features
- Infinite worlds
- Chunk mesh optimization
- Multi-threaded engine
- Custom linear algebra and utility library (libem)
- Only using header-only external libraries (Sokol, Nuklear)
- Smoothly handles 32-chunk render distance on a regular laptop

# Coming Soon
- Physics for colission with world
- Saving edited chunks for persistence
- Loading world saves
- Various graphical improvements
- Proper Windows support

# Screenshots 

<img width="1920" height="1080" alt="20251111_21h17m24s_grim" src="https://github.com/user-attachments/assets/d16f4f26-b1ac-4751-8782-507200ad9de3" />
<img width="1920" height="1080" alt="20251111_21h18m44s_grim" src="https://github.com/user-attachments/assets/e5f49ec6-3dfa-465b-b9a8-3476a710041c" />
<img width="1920" height="1080" alt="20251111_21h20m40s_grim" src="https://github.com/user-attachments/assets/c7649fce-31c5-4f3f-a9a7-42d91c08acad" />
