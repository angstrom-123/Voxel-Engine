#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdint.h>
#include <stdio.h>  // fprintf, sprintf
#include <stdlib.h> // exit
#include <string.h> // strlen

#include "geometry_types.h"

/* TODO: if windows then replace all posix fns like mkdir with win versions. */
#ifdef _WIN32
// TODO: Add windows support, first fix the build config for windows.
#else // linux
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#endif // _WIN32 || linux

#define MAX_DIRNAME_LEN 24
#define MAX_FILENAME_LEN 96

typedef enum flag_offset {
    FLAG_EDITED,
    FLAG_NUM
} flag_offset_e;

extern char *file_make_world_dir(char *world_name);
extern void file_write_chunk(char *dir_name, chunk_t *chunk);
extern chunk_t *file_get_chunk(char *dir_name, int32_t x, int32_t z);
extern char **file_get_filenames(char *dir_name, size_t *len);

#endif
