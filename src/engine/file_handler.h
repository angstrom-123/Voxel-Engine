// #ifndef FILE_HANDLER_H
// #define FILE_HANDLER_H
//
// #include <stdint.h>
// #include <stdio.h>  // fprintf, sprintf
// #include <stdlib.h> // exit
// #include <string.h> // strlen
//
// #include "constants.h"
// #include "geometry_types.h"
// #include "state.h"
//
// /* TODO: if windows then replace all posix fns like mkdir with win versions. */
// #ifdef PLAT_WINDOWS
// // TODO: Add windows support, first fix the build config for windows.
// #endif // PLAT_WINDOWS
//
// #ifdef PLAT_LINUX
// #include <dirent.h>
// #include <errno.h>
// #include <unistd.h>
// #include <sys/stat.h>
// #define __USE_XOPEN_EXTENDED
// #include <ftw.h>
// #endif // PLAT_LINUX
//
// typedef enum flag_offset {
//     FLAG_EDITED,
//     FLAG_NUM
// } flag_offset_e;
//
// typedef struct load_data {
//     uint32_t seed;
// } load_data_t;
//
// extern bool file_make_world_dir(char *world_name);
// // extern void file_write_chunk(char *world_name, chunk_t *chunk);
// // extern chunk_t *file_get_chunk(char *world_name, int32_t x, int32_t z);
// extern char **file_get_world_names(size_t *len);
// extern void file_rename_world_dir(char *world_name, char *new_name);
// extern void file_delete_world_dir(char *world_name);
// extern void file_make_world_desc(game_desc_t desc);
// extern load_data_t *file_get_world_data(char *world_name);
//
// #endif
