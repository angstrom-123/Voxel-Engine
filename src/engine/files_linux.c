#include "files_linux.h"

bool _file_exists_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Get file linux");
    return false;
}

bool _file_dir_exists_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Get dir linux");
    return false;
}

bool _file_create_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Create file linux");
    return false;
}

bool _file_open_linux(linux_file_t *file, file_usage_e usage)
{
    (void) file; (void) usage;
    ENGINE_UNIMPLEMENTED("Open file linux");
    return false;
}

bool _file_close_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Close file linux");
    return false;
}

bool _file_delete_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Delete file linux");
    return false;
}

bool _file_dir_delete_linux(linux_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Delete dir linux");
    return false;
}

bool _file_rename_linux(linux_file_t *file, const char *new_name)
{
    (void) file; (void) new_name;
    ENGINE_UNIMPLEMENTED("Rename file linux");
    return false;
}

bool _file_read_bytes_linux(linux_file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes])
{
    (void) file; (void) num_bytes; (void) bytes;
    ENGINE_UNIMPLEMENTED("Read bytes from file linux");
    return false;
}

bool _file_read_lines_linux(linux_file_t *file, size_t *num_lines, size_t line_len,
                            char lines[*num_lines][line_len])
{
    (void) file; (void) num_lines; (void) lines;
    ENGINE_UNIMPLEMENTED("Read lines from file linux");
    return false;
}

bool _file_write_bytes_linux(linux_file_t *file, size_t num_bytes, uint8_t bytes[num_bytes])
{
    (void) file; (void) num_bytes; (void) bytes;
    ENGINE_UNIMPLEMENTED("Write bytes to file linux");
    return false;
}

bool _file_write_lines_linux(linux_file_t *file, size_t num_lines, size_t line_len, 
                             char lines[num_lines][line_len])
{
    (void) file; (void) num_lines; (void) lines;
    ENGINE_UNIMPLEMENTED("Write lines to file linux");
    return false;
}
