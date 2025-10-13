# To-Do List
## Bugfix
- Async chunk loader: Maybe my assumption about the way the threads access 
  the datastructures is wrong. 

## Priority
- Think of a better way to do async chunk loading.
- Fix issue with having to click twice to register in pause menu.
- Saving edited chunks as files: Currently Broken!
    - World creation UI (new / load / delete, seed, etc)
    - New World Dir, subdir for each world 
- Deferred Rendering!!
- Colissions with world
- Placing / Deleting blocks 
- Add Windows Support (Probably just need to setup the build system)
- Chunk LOD (lower res for distant chunks 16x16 -> 8x8 -> 4x4 -> 2x2 -> 1x1)

## Non Priority
- Use file walk and not hardcoded names for asset loading
- Shadow Mapping
- Batch Draw Calls
- MULTIPLAYER :)
- Radial blur god rays (Volumetric lighting)
- Ambient Occlusion
- Add more variety to world generation
- Frustum culling
- Fix drawing order (transparent blocks last)
