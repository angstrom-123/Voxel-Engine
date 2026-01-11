#include "files.h"

bool file_exists(file_t *res) 
{ 
    #ifdef PLAT_LINUX
        return _file_exists_linux(res);
    #elif defined(PLAT_WINDOWS)
        return _file_exists_windows(res);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_dir_get(file_t *file)
{
    #ifdef PLAT_LINUX
        return _file_dir_exists_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_dir_exists_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_create(file_t *file)
{
    #ifdef PLAT_LINUX
        return _file_create_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_create_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_open(file_t *file, file_usage_e usage)
{
    #ifdef PLAT_LINUX
        return _file_open_linux(file, usage);
    #elif defined(PLAT_WINDOWS)
        return _file_open_windows(file, usage);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_close(file_t *file)
{
    #ifdef PLAT_LINUX
        return _file_close_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_open_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_delete(file_t *file)
{
    #ifdef PLAT_LINUX
        return _file_delete_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_delete_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_dir_delete(file_t *file)
{
    #ifdef PLAT_LINUX
        return _file_delete_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_delete_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_rename(file_t *file, const char *new_name)
{
    #ifdef PLAT_LINUX
        return _file_rename_linux(file, new_name);
    #elif defined(PLAT_WINDOWS)
        return _file_rename_windows(file, new_name);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_read_bytes(file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes])
{
    #ifdef PLAT_LINUX
        return _file_read_bytes_linux(file, num_bytes, bytes);
    #elif defined(PLAT_WINDOWS)
        return _file_read_bytes_windows(file, num_bytes, btyes);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_read_lines(file_t *file, size_t *num_lines, size_t line_len, 
                     char lines[*num_lines][line_len])
{
    #ifdef PLAT_LINUX
        return _file_read_lines_linux(file, num_lines, line_len, lines);
    #elif defined(PLAT_WINDOWS)
        return _file_read_lines_windows(file, num_lines, line_len, lines);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_write_bytes(file_t *file, size_t num_bytes, uint8_t bytes[num_bytes])
{
    #ifdef PLAT_LINUX
        return _file_write_bytes_linux(file, num_bytes, bytes);
    #elif defined(PLAT_WINDOWS)
        return _file_write_bytes_windows(file, num_bytes, bytes);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_write_lines(file_t *file, size_t num_lines, size_t line_len, 
                      char lines[num_lines][line_len])
{
    #ifdef PLAT_LINUX
        return _file_write_lines_linux(file, num_lines, line_len, lines);
    #elif defined(PLAT_WINDOWS)
        return _file_write_lines_windows(file, num_lines, line_len, lines);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}
