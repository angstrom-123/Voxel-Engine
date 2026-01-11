# To-Do List
## Consider Refactoring
- Change raycast (raycast.c) to use AABB/ray collision instead of ray trace
- Apply DT to jump cooldown (camera_controller.c) for better consistency

## General
- World collision sliding against blocks
- Saving edited chunks as files for persistence
- Chunk LOD (lower res for distant chunks 16x16 -> 8x8 -> 4x4 -> 2x2 -> 1x1)
- Greedy meshing for chunks (decreases required bandwidth by a LOT)
- World creation UI (new / load / delete, seed, etc)
- Add more variety to world generation
- Use file walk and not hardcoded names for asset loading
- Add Windows Support (Mainly just need to setup the build system)
- Multiplayer

## Graphics
- Radial blur god rays (Volumetric lighting)
- Screen Space Ambient Occlusion
- Fix drawing order (transparent blocks last)
- Frustum culling (Currently rendering takes like 1% of frame time so don't worry)
