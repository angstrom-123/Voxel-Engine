#ifndef BASE_H
#define BASE_H 

#ifdef PLAT_LINUX
#define WORLD_DATA_DIR "app_data/"
#define TEXTURE_DATA_DIR "res/tex/"
#define COL_NRM "\x1B[0m"
#define COL_ERR "\x1B[31m"
#define COL_OKK "\x1B[32m"
#define COL_WRN "\x1B[33m"
#define COL_RST "\x1B[0m"
#elif defined(PLAT_WINDOWS)
#define WORLD_DATA_DIR "app_data\\"
#define TEXTURE_DATA_DIR "res\\tex\\"
#define COL_NRM
#define COL_ERR
#define COL_OKK
#define COL_WRN
#define COL_RST
#endif

#define THREAD_AWAIT_NS 50000000

typedef enum chunk_data_index {
    CURRENT,
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTHEAST,
    NORTHWEST,
    SOUTHEAST,
    SOUTHWEST,
    DATA_INDEX_NUM
} chunk_data_index_e;

typedef enum directions {
    DIR_NORTH = 1,
    DIR_EAST  = 2,
    DIR_SOUTH = 4,
    DIR_WEST  = 8
} directions_e;

#define QUOTE(...) #__VA_ARGS__

#endif
