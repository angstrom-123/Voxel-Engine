#ifndef FILES_H
#define FILES_H

#include <libem/em_math.h>
#include <libem/em_binary.h>

#include "file_meta.h"
#include "geometry_types.h"
#include "geometry.h"

#ifdef PLAT_LINUX
    #include "files_linux.h"
    #define file_t linux_file_t
#elif defined(PLAT_WINDOWS)
    #include "files_windows.h"
    #define file_t windows_file_t
#endif

#define CHUNK_FILENAME_LEN 30
#define SUBCHUNK_COUNT (CHUNK_HEIGHT / SUBCHUNK_HEIGHT)

typedef struct subchunk {
    uint8_t data[CHUNK_SIZE][SUBCHUNK_HEIGHT][CHUNK_SIZE];
} subchunk_t;

typedef struct chunk_file {
    struct chunk_header {
        uint16_t version;
        uint16_t file_size;
        uint16_t sc_offset[SUBCHUNK_COUNT];
    } header;
    struct chunk_body {
        uint8_t *data;
    } body;
} chunk_file_t;

typedef chunk_data_t *(*gen_func)(ivec2, uint32_t);

extern void chunk_file_name(ivec2 pos, char *res);
extern chunk_data_t *decode_chunk_file(file_t *file, gen_func gf, ivec2 pos, uint32_t seed);
extern chunk_file_t encode_chunk_file(const chunk_data_t *data);
extern bool write_chunk_file(file_t *file, chunk_file_t cf);

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
