#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef _WIN32
#define WORLD_DATA_DIR "data\\"
#define TEXTURE_DATA_DIR "res\\tex\\"
#else // linux
#define WORLD_DATA_DIR "data/"
#define TEXTURE_DATA_DIR "res/tex/"
#endif // linux

#define MAX_WORLD_NAME_LEN 24
#define MAX_SEED_LEN 32
#define MAX_CHUNK_FILE_LEN 27

#define CURRENT 0
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4

#endif
