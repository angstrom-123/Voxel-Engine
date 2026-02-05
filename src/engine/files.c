#include "files.h"

static uint8_t *rle(uint8_t *data, uint16_t *len)
{
    uint16_t buf_size = 2048;
    uint16_t buf_len = 0;
    uint8_t *res = malloc(buf_size);

    uint8_t run_len = 0;
    uint8_t run_val = CUBETYPE_NUM;
    for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
    {
        uint8_t val = data[i];

        if ((run_len > 0 && val != run_val) || run_len == UINT8_MAX)
        {
            if (buf_len + 2 >= buf_size) res = realloc(res, (buf_size *= 2));
            res[buf_len++] = run_val;
            res[buf_len++] = run_len;
            run_val = val;
            run_len = 0 ;
        }

        run_val = val;
        run_len++;
    }

    // Write final run.
    if (buf_len + 2 >= buf_size) res = realloc(res, (buf_size *= 2));
    res[buf_len++] = run_val;
    res[buf_len++] = run_len;

    *len = buf_len;
    return res;
}

static void unrle(uint8_t *data, uint8_t *res, size_t len)
{
    size_t read = 0;
    size_t i = 0;
    while (read < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE)
    {
        ENGINE_ASSERT(i < len, "Overrun chunk data");
        uint8_t run_val = data[i++];
        uint8_t run_len = data[i++];
        memset(&res[read], run_val, run_len);
        read += run_len;
    }

    ENGINE_ASSERT(read == CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, "Failed to read chunk file");
}

void chunk_file_name(ivec2 pos, char *res)
{
    // X and Y are int32_t so take up to 11 chars (for -INT32_MAX)
    snprintf(res, CHUNK_FILENAME_LEN, "chunk_%011i_%011i", pos.x, pos.y);
}

chunk_data_t *decode_chunk_file(file_t *file)
{
    if ((file->flags & FILEFLAG_DIR) || !(file->flags & FILEFLAG_BINARY)) return false;

    RUNTIME_ASSERT(file_open(file, USAGE_READ_BIN), "Failed to open chunk file for decoding");

    chunk_data_t *res = malloc(sizeof(chunk_data_t));
    res->edited = false;

    size_t buf_len = CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE;
    uint8_t *buf = malloc(buf_len);
    file_read_bytes(file, &buf_len, buf);
    unrle(buf, (uint8_t *) res->types, buf_len);

    return res;
}

chunk_file_t encode_chunk_file(const chunk_data_t *cd)
{
    chunk_file_t cf;
    cf.data = rle((uint8_t *) cd->types, &cf.size);
    return cf;
}

bool write_chunk_file(file_t *file, chunk_file_t cf)
{
    return file_write_bytes(file, cf.size, cf.data);
}

bool write_meta_file(file_t *file, meta_file_t mf)
{
    snprintf(mf.lines[0], sizeof(mf.lines[0]), "%.5f,%.5f,%.5f", DECOMPOSE_3(mf.pos));
    snprintf(mf.lines[1], sizeof(mf.lines[1]), "%.5f,%.5f,%.5f,%.5f", DECOMPOSE_4(mf.rot));
    snprintf(mf.lines[2], sizeof(mf.lines[2]), "%.5f", mf.pitch);
    snprintf(mf.lines[3], sizeof(mf.lines[3]), "%.5f", mf.yaw);
    snprintf(mf.lines[4], sizeof(mf.lines[4]), "%u", mf.seed);
    snprintf(mf.lines[5], sizeof(mf.lines[5]), "%lu", mf.time);
    return file_write_lines(file, 
                            sizeof(mf.lines) / sizeof(mf.lines[0]), 
                            sizeof(mf.lines[0]) / sizeof(mf.lines[0][0]), 
                            mf.lines);
}

bool file_exists(file_t *file) 
{ 
    #ifdef PLAT_LINUX
        return _file_exists_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_exists_windows(file);
    #else 
        _ASSERT("RUNTIME", false, "Platform must be linux or windows");
    #endif
}

bool file_dir_exists(file_t *file) 
{ 
    #ifdef PLAT_LINUX
        return _file_dir_exists_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_dir_exists_windows(file);
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
        return _file_dir_delete_linux(file);
    #elif defined(PLAT_WINDOWS)
        return _file_dir_delete_windows(file);
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
