# A High Performance Voxel Engine Written in C (WIP)
This work in progress is my first ever attempt at using C for a large project, making a game, and programming the GPU.
The engine is powerful and optimized, able to run effectively on a standard laptop. Many features are already implemented,
and many more are planned, or on the way!

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
- [ ] Multiplayer
- [ ] Cascaded Shadow Maps

# Screenshots 
<img width="1920" height="1080" alt="20260203_01h40m16s_grim" src="https://github.com/user-attachments/assets/1fea6a23-e8a3-45cc-89f9-53a5110f9455" />
<img width="1920" height="1080" alt="20260203_01h44m43s_grim" src="https://github.com/user-attachments/assets/4478c4ee-05df-409a-bb7e-50eec73736f5" />
<img width="1920" height="1080" alt="20260203_01h45m12s_grim" src="https://github.com/user-attachments/assets/ed67ba22-b65f-455f-81a0-295b10af80c0" />
<img width="1920" height="1080" alt="20260203_01h45m40s_grim" src="https://github.com/user-attachments/assets/0981bbee-0d77-4dba-8915-283772103345" />

