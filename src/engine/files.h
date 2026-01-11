#ifndef FILES_H
#define FILES_H

#include "file_meta.h"

#ifdef PLAT_LINUX
    #include "files_linux.h"
    #define file_t linux_file_t
#elif defined(PLAT_WINDOWS)
    #include "files_windows.h"
    #define file_t windows_file_t
#endif

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
