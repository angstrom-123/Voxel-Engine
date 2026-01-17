#ifndef FILES_LINUX_H
#define FILES_LINUX_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include <libem/em_binary.h>

#include "logger.h"
#include "file_meta.h"

typedef struct linux_file {
    FILE *fptr;
    DIR *dptr;
    uint8_t flags;
    size_t offset;
    const char *path;
    const char *base;
    const char *name;
} linux_file_t;

extern int scandir(const char *dirp, 
                   struct dirent ***namelist, 
                   int (*filter)(const struct dirent *), 
                   int (*compar)(const struct dirent **, const struct dirent **));

extern bool _file_exists_linux(linux_file_t *file);
extern bool _file_dir_exists_linux(linux_file_t *file);
extern bool _file_create_linux(linux_file_t *file);
extern bool _file_open_linux(linux_file_t *file, file_usage_e usage);
extern bool _file_close_linux(linux_file_t *file);
extern bool _file_delete_linux(linux_file_t *file);
extern bool _file_dir_delete_linux(linux_file_t *file);
extern bool _file_rename_linux(linux_file_t *file, const char *new_name);
extern bool _file_read_bytes_linux(linux_file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes]);
extern bool _file_read_lines_linux(linux_file_t *file, size_t *num_lines, size_t line_len, 
                                   char lines[*num_lines][line_len]);
extern bool _file_write_bytes_linux(linux_file_t *file, size_t num_bytes, 
                                    uint8_t bytes[num_bytes]);
extern bool _file_write_lines_linux(linux_file_t *file, size_t num_lines, size_t line_len, 
                                    char lines[num_lines][line_len]);

#endif
