#include "files_windows.h"

bool _file_exists_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Exists file windows");
    return false;
}

bool _file_dir_exists_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Exists dir windows");
    return false;
}

bool _file_create_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Create file windows");
    return false;
}

bool _file_open_windows(windows_file_t *file, file_usage_e usage)
{
    (void) file; (void) usage;
    ENGINE_UNIMPLEMENTED("Open file windows");
    return false;
}

bool _file_close_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Close file windows");
    return false;
}

bool _file_delete_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Delete file windows");
    return false;
}

bool _file_dir_delete_windows(windows_file_t *file)
{
    (void) file;
    ENGINE_UNIMPLEMENTED("Delete dir windows");
    return false;
}

bool _file_rename_windows(windows_file_t *file, const char *new_name)
{
    (void) file; (void) new_name;
    ENGINE_UNIMPLEMENTED("Rename file windows");
    return false;
}

bool _file_read_bytes_windows(windows_file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes])
{
    (void) file; (void) num_bytes; (void) bytes;
    ENGINE_UNIMPLEMENTED("Read bytes from file windows");
    return false;
}

bool _file_read_lines_windows(windows_file_t *file, size_t *num_lines, size_t line_len, 
                              char lines[*num_lines][line_len])
{
    (void) file; (void) num_lines; (void) lines;
    ENGINE_UNIMPLEMENTED("Read lines from file windows");
    return false;
}

bool _file_write_bytes_windows(windows_file_t *file, size_t num_bytes, uint8_t bytes[num_bytes])
{
    (void) file; (void) num_bytes; (void) bytes;
    ENGINE_UNIMPLEMENTED("Write bytes to file windows");
    return false;
}

bool _file_write_lines_windows(windows_file_t *file, size_t num_lines, size_t line_len,
                               char lines[num_lines][line_len])
{
    (void) file; (void) num_lines; (void) lines;
    ENGINE_UNIMPLEMENTED("Write lines to file windows");
    return false;
}
