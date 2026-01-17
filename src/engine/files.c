#include "files.h"

void chunk_file_name(ivec2 pos, char *res)
{
    // X and Y are int32_t so take up to 11 chars (for -INT32_MAX)
    snprintf(res, CHUNK_FILENAME_LEN, "chunk_%011i_%011i", pos.x, pos.y);
}

chunk_data_t *decode_chunk_file(file_t *file, gen_func gf, ivec2 pos, uint32_t seed)
{
    if ((file->flags & FILEFLAG_DIR) || !(file->flags & FILEFLAG_BINARY)) return false;

    chunk_file_t cf;
    RUNTIME_ASSERT(file_open(file, USAGE_READ_BIN), "Failed to open chunk file for decoding");
    
    // Read header
    size_t h_siz = sizeof(struct chunk_header);
    uint8_t h_buf[h_siz];
    RUNTIME_ASSERT(file_read_bytes(file, &h_siz, h_buf), "Failed to read chunk file header");

    // Get size and offsets from header
    size_t h_buf_ptr = 0;
    cf.header.version = READ_U16_MOVE(h_buf, h_buf_ptr);
    cf.header.file_size = READ_U16_MOVE(h_buf, h_buf_ptr);
    uint8_t edit_flags = 0;
    for (size_t i = 0; i < SUBCHUNK_COUNT; i++)
    {
        cf.header.sc_offset[i] = READ_U16_MOVE(h_buf, h_buf_ptr);
        if (cf.header.sc_offset[i] != 0) edit_flags |= (1 << i);
    }

    // Read body and close file
    size_t b_siz = cf.header.file_size - sizeof(struct chunk_header);
    cf.body.data = malloc(b_siz);
    RUNTIME_ASSERT(file_read_bytes(file, &b_siz, cf.body.data), "Failed to read chunk file body");
    RUNTIME_ASSERT(file_close(file), "Failed to close chunk file");

    // Get block data from body 
    chunk_data_t *res = (edit_flags == UINT8_MAX)
                      ? malloc(sizeof(chunk_data_t))
                      : gf(pos, seed);
    res->edited_subchunk = edit_flags;
    res->edited = false;

    size_t b_buf_ptr = 0;
    for (size_t i = 0; i < SUBCHUNK_COUNT; i++)
    {
        if (!(edit_flags & (1 << i))) continue; // Skip unsaved subchunks.

        uint8_t val = 0;
        uint8_t len = 0;
        for (size_t x = 0; x < CHUNK_SIZE; x++)
        {
        for (size_t z = 0; z < CHUNK_SIZE; z++)
        {
        for (size_t y = SUBCHUNK_HEIGHT * i; y < SUBCHUNK_HEIGHT * i + SUBCHUNK_HEIGHT; y++)
        {
            if (len == 0)
            {
                val = cf.body.data[b_buf_ptr++];
                len = cf.body.data[b_buf_ptr++];
            }

            res->types[x][y][z] = val;
            len--;
        }}}
    }

    return res;
}

chunk_file_t encode_chunk_file(const chunk_data_t *cd)
{
    chunk_file_t res;

    const size_t MAX_RUN_LEN = 255;
    size_t buf_siz = 128;
    size_t buf_len = 0;
    res.body.data = malloc(buf_siz);

    for (size_t i = 0; i < SUBCHUNK_COUNT; i++)
    {
        if (!(cd->edited_subchunk & (1 << i)))
            continue;

        res.header.sc_offset[i] = buf_len + sizeof(struct chunk_header);

        uint8_t val = 0;
        uint8_t len = 0;
        for (size_t x = 0; x < CHUNK_SIZE; x++)
        {
        for (size_t z = 0; z < CHUNK_SIZE; z++)
        {
        for (size_t y = SUBCHUNK_HEIGHT * i; y < SUBCHUNK_HEIGHT * i + SUBCHUNK_HEIGHT; y++)
        {
            const uint8_t new_val = cd->types[x][y][z];

            if (len == 0) 
            {
                val = new_val; 
            }
            else if (len >= MAX_RUN_LEN || new_val != val)
            {
                if (buf_len + 2 >= buf_siz) 
                    res.body.data = realloc(res.body.data, (buf_siz *= 2));

                res.body.data[buf_len++] = val;
                res.body.data[buf_len++] = len;

                val = new_val;
                len = 0; // Set to 0 because increments to 1 at end of loop.
            }

            len++;
        }}}

        if (len > 0)
        {
            if (buf_len + 2 >= buf_siz) 
                res.body.data = realloc(res.body.data, (buf_siz *= 2));

            res.body.data[buf_len++] = val;
            res.body.data[buf_len++] = len;
        }

        ENGINE_LOG_WARN("Data size @subchunk(%zu): %zu.", 
                        i, buf_len -res.header.sc_offset[i]);
    }

    res.header.version = 1;
    res.header.file_size = buf_len + sizeof(struct chunk_header);

    return res;
}

bool write_chunk_file(file_t *file, chunk_file_t cf)
{
    #define cfh cf.header
    if ((file->flags & FILEFLAG_DIR) 
            || !(file->flags & FILEFLAG_BINARY)
            || !(file->flags & FILEFLAG_OPEN)
            || !(file->flags & FILEFLAG_WRITE)) return false;

    // Header 
    size_t h_len = 0;
    uint8_t h_buf[sizeof(struct chunk_header)];
    
    WRITE_U16_MOVE(cfh.version, h_buf, h_len);
    WRITE_U16_MOVE(cfh.file_size, h_buf, h_len);
    for (size_t i = 0; i < SUBCHUNK_COUNT; i++)
    {
        WRITE_U16_MOVE(cfh.sc_offset[i], h_buf, h_len);
    }

    if (!file_write_bytes(file, h_len, h_buf)) return false;

    // Body
    size_t b_len = cfh.file_size - sizeof(struct chunk_header);
    if (!file_write_bytes(file, b_len, cf.body.data)) return false;

    return true;
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
