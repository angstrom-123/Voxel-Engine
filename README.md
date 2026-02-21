# A High Performance Voxel Engine Written in C
This engine is my first ever attempt at using C for a large project (~15,000 LOC), making a game, 
and programming the GPU. The engine is quite powerful and decently optimized, able to run effectively 
on a standard office laptop.

# Thoughts
I have brought the engine to a state where I think it is complete enough to end development for now.
It has been a long journey and I have learned a lot, but I think that it is time for me to move on.
If I were to write this engine again, there are many things that I would do differently, and perhaps 
I will revisit this, or something similar, again in the future.

# Build
> [!IMPORTANT]
> If testing on a laptop, make sure that cursor movement while typing is allowed.

> [!NOTE]
> Currently only Linux build is supported, with GNU Make.

## Optimized build:
```
make release
```
## Unoptimized build with logging:
```
make debug
```
## Optimized build emitting profiling data:
```
make profiling
```
# Run
```
./bin/output/Voxel-Engine-[platform]
```

# Checklist
- [x] Infinite Worlds
- [x] Physics-based Movement and Colissions
- [x] Multi-threaded Engine
- [x] Runs on a Low-End Device
- [x] Shadow Mapping
- [x] Text Support
- [x] Linux Support
- [ ] Windows Support
- [ ] Transparency Rendering
- [ ] Memory Allocation Optimisation
- [ ] Cascaded Shadow Maps

# Screenshots 
<img width="1920" height="1080" alt="20260221_15h43m51s_grim" src="https://github.com/user-attachments/assets/81cedb56-73d1-41f8-a781-951c65308bc6" />
<img width="1920" height="1080" alt="20260221_15h50m34s_grim" src="https://github.com/user-attachments/assets/83cbf34c-8672-4c34-9a14-a82c3eea75bc" />
<img width="1920" height="1080" alt="20260221_15h44m56s_grim" src="https://github.com/user-attachments/assets/9b19fc98-339b-460e-9919-5f8b60f50ad9" />
<img width="1920" height="1080" alt="20260221_15h51m45s_grim" src="https://github.com/user-attachments/assets/736af524-053b-466f-97d3-7d3221c012e6" />
<img width="1920" height="1080" alt="20260221_15h54m35s_grim" src="https://github.com/user-attachments/assets/afc7071c-aa83-47bd-978f-d1660614b251" />
<img width="1920" height="1080" alt="20260221_15h52m04s_grim" src="https://github.com/user-attachments/assets/8a5ebb8d-cfa4-4239-a823-29b45e367793" />
