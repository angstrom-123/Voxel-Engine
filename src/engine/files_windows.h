#ifndef FILES_WINDOWS_H
#define FILES_WINDOWS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "logger.h"
#include "file_meta.h"

typedef struct windows_file {
    FILE *fptr;
    uint8_t flags;
    char *path;
    char *name;
} windows_file_t;

extern bool _file_exists_windows(windows_file_t *file);
extern bool _file_dir_exists_windows(windows_file_t *file);
extern bool _file_create_windows(windows_file_t *file);
extern bool _file_open_windows(windows_file_t *file, file_usage_e usage);
extern bool _file_close_windows(windows_file_t *file);
extern bool _file_delete_windows(windows_file_t *file);
extern bool _file_dir_delete_windows(windows_file_t *file);
extern bool _file_rename_windows(windows_file_t *file, const char *new_name);
extern bool _file_read_bytes_windows(windows_file_t *file, size_t *num_bytes, 
                                     uint8_t bytes[*num_bytes]);
extern bool _file_read_lines_windows(windows_file_t *file, size_t *num_lines, 
                                     size_t line_len, char lines[*num_lines][line_len]);
extern bool _file_write_bytes_windows(windows_file_t *file, size_t num_bytes, 
                                      uint8_t bytes[num_bytes]);
extern bool _file_write_lines_windows(windows_file_t *file, size_t num_lines, 
                                      size_t line_len, char lines[num_lines][line_len]);

#endif
