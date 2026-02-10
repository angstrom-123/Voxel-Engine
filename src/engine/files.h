#ifndef FILES_H
#define FILES_H

#include <libem/em_math.h>
#include <libem/em_binary.h>
#include <errno.h>

#include "logger.h"
#include "base.h"
#include "file_meta.h"
#include "geometry_types.h"

#ifdef PLAT_LINUX

#include <dirent.h>
#include <sys/stat.h>

#elif defined(PLAT_WINDOWS)

#include "dirent/dirent.h"

#endif

extern int scandir(const char *dirp, 
                   struct dirent ***namelist, 
                   int (*filter)(const struct dirent *), 
                   int (*compar)(const struct dirent **, const struct dirent **));

#define CHUNK_FILENAME_LEN 30

typedef struct file {
    FILE *fptr;
    DIR *dptr;
    uint8_t flags;
    size_t offset;
    const char *path;
    const char *base;
    const char *name;
} file_t;

typedef struct chunk_file {
    uint16_t size;
    uint8_t *data;
} chunk_file_t;

typedef struct meta_file {
    vec3 pos;
    quat rot;
    float pitch;
    float yaw;
    uint32_t seed;
    uint64_t time;
    char lines[6][STD_BUFLEN];
} meta_file_t;

extern void chunk_file_name(ivec2 pos, char *res);
extern chunk_data_t *decode_chunk_file(file_t *file);
extern chunk_file_t encode_chunk_file(const chunk_data_t *data);
extern bool write_chunk_file(file_t *file, chunk_file_t cf);
extern bool write_meta_file(file_t *file, meta_file_t mf);

extern bool file_exists(file_t *file);
extern bool file_dir_exists(file_t *file);
extern bool file_create(file_t *file);
extern bool file_open(file_t *file, file_usage_e usage);
extern bool file_close(file_t *file);
extern bool file_delete(file_t *file);
extern bool file_dir_delete(file_t *file);
extern bool file_rename(file_t *file, const char *new_name);
extern bool file_read_bytes(file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes]);
extern bool file_read_lines(file_t *file, size_t *num_lines, size_t line_len, 
                            char lines[*num_lines][line_len]);
extern bool file_write_bytes(file_t *file, size_t num_bytes, uint8_t bytes[num_bytes]);
extern bool file_write_lines(file_t *file, size_t num_lines, size_t line_len, 
                             char lines[num_lines][line_len]);

#endif
