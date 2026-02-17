#include "files.h"

static int filter_no_upward(const struct dirent *d)
{
    if (d->d_name[0] == '.') return 0;
    return 1;
}

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
    FILE *fptr = fopen(file->path, "r");
    if (fptr) 
    {
        fclose(fptr);
        return true;
    }
    return false;
}

bool file_dir_exists(file_t *file) 
{ 
    DIR *dptr = opendir(file->path);
    if (dptr) 
    {
        closedir(dptr);
        return true;
    }

    RUNTIME_ASSERT(errno == ENOENT, "File open error must be 'not exists'");
    return false;
}

bool file_list_dir(file_t *file, size_t *num_names, char res[*num_names][STD_BUFLEN])
{
    if (!(file->flags & FILEFLAG_DIR)) return false;

    struct dirent **dirs;
    int32_t n = scandir(file->path, &dirs, filter_no_upward, NULL);
    if (n == -1) return false;

    size_t i;
    for (i = 0; i < (size_t) n; i++)
    {
        if (i < *num_names)
            strncpy(res[i], dirs[i]->d_name, STD_BUFLEN);

        free(dirs[i]);
    }
    free(dirs);

    *num_names = i;
    return true;
}

bool file_create(file_t *file)
{
    if (file->flags & FILEFLAG_DIR)
    {
        if (mkdir(file->path, 0700) == EOF) return false;
        file->dptr = opendir(file->path);
        if (!file->dptr) return false;
        closedir(file->dptr);
        return true;
    }
    else 
    {
        file->fptr = fopen(file->path, "w");
        if (!file->fptr) return false;

        file->flags |= FILEFLAG_OPEN | FILEFLAG_WRITE;
        return true;
    }
    return false;
}

bool file_open(file_t *file, file_usage_e usage)
{
    if (file->flags & FILEFLAG_OPEN) return false;

    const char *usage_str;
    uint8_t new_flags = FILEFLAG_OPEN;

    switch (usage) {
    case USAGE_READ:
        usage_str = "r";
        new_flags |= FILEFLAG_READ;
        break;
    case USAGE_READ_BIN:
        usage_str = "rb";
        new_flags |= FILEFLAG_READ | FILEFLAG_BINARY;
        break;
    case USAGE_WRITE:
        usage_str = "w";
        new_flags |= FILEFLAG_WRITE;
        break;
    case USAGE_WRITE_BIN:
        usage_str = "wb";
        new_flags |= FILEFLAG_WRITE | FILEFLAG_BINARY;
        break;
    default:
        _ASSERT("RUNTIME", false, "File open usage is invalid");
    };

    file->fptr = fopen(file->path, usage_str);
    if (!file->fptr) return false;
    file->flags = new_flags;

    return true;
}

bool file_close(file_t *file)
{
    if (!(file->flags & FILEFLAG_OPEN)) return false;

    if (fclose(file->fptr) == EOF) return false;
    file->fptr = NULL;
    file->flags = 0;

    return true;
}

bool file_delete(file_t *file)
{
    if ((file->flags & FILEFLAG_OPEN)) return false;

    if (remove(file->path) == EOF) return false;
    file->flags = 0;

    return true;
}

bool file_dir_delete(file_t *file)
{
    if ((file->flags & FILEFLAG_OPEN) || !(file->flags & FILEFLAG_DIR)) return false;

    struct dirent **res;
    size_t n = scandir(file->path, &res, filter_no_upward, NULL);

    for (size_t i = 0; i < n; i++)
    {
        struct dirent *d = res[i];
        char d_path[128];
        snprintf(d_path, 128, "%s" SEP "%s", file->path, d->d_name);

        struct stat statbuf;
        if (stat(d_path, &statbuf) != 0)
            return false;

        file_t f = {
            .base = file->path,
            .path = d_path,
            .name = d->d_name
        };

        if (S_ISREG(statbuf.st_mode))
        {
            if (!file_delete(&f)) return false;
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            f.flags |= FILEFLAG_DIR;
            if (!file_dir_delete(&f)) return false;
        }
        else 
        {
            return false;
        }
    }

    if (!file_delete(file)) return false;

    return true;
}

bool file_rename(file_t *file, const char *new_name)
{
    if (file->flags & FILEFLAG_OPEN) return false;

    char new_path[128];
    snprintf(new_path, 128, "%s%s", file->base, new_name);

    if (rename(file->path, new_path) == EOF) return false;

    file->path = new_path;
    file->name = new_name;

    return true;
}

bool file_read_bytes(file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes])
{
    if (!(file->flags & FILEFLAG_OPEN) 
            || !(file->flags & FILEFLAG_READ) 
            || !(file->flags & FILEFLAG_BINARY)) 
        return false;

    if (!num_bytes)
    {
        fseek(file->fptr, 0, SEEK_END);
        size_t len = ftell(file->fptr);
        num_bytes = &len;
    }

    bool res = binary_read(file->fptr, file->offset, num_bytes, bytes);
    file->offset += *num_bytes;

    return res;
}

bool file_read_lines(file_t *file, size_t *num_lines, size_t line_len, 
                     char lines[*num_lines][line_len])
{
    if (!(file->flags & FILEFLAG_OPEN) 
            || !(file->flags & FILEFLAG_READ) 
            || (file->flags & FILEFLAG_BINARY)) 
        return false;

    for (size_t i = 0; i < *num_lines; i++)
    {
        fseek(file->fptr, file->offset, SEEK_SET);
        if (!fgets(lines[i], line_len, file->fptr))
        {
            *num_lines = i;
            return false;
        }
        file->offset += strlen(lines[i]);
    }

    return true;
}

bool file_write_bytes(file_t *file, size_t num_bytes, uint8_t bytes[num_bytes])
{
    if ((file->flags & FILEFLAG_DIR) 
            || !(file->flags & FILEFLAG_OPEN) 
            || !(file->flags & FILEFLAG_WRITE) 
            || !(file->flags & FILEFLAG_BINARY)) 
        return false;

    fseek(file->fptr, 0, SEEK_END);
    if (fwrite(bytes, 1, num_bytes, file->fptr) == 0) return false;

    file->offset += num_bytes;

    return true;
}

bool file_write_lines(file_t *file, size_t num_lines, size_t line_len, 
                      char lines[num_lines][line_len])
{
    if ((file->flags & FILEFLAG_DIR)
            || !(file->flags & FILEFLAG_OPEN) 
            || !(file->flags & FILEFLAG_WRITE) 
            || (file->flags & FILEFLAG_BINARY)) 
        return false;

    for (size_t i = 0; i < num_lines; i++)
    {
        fseek(file->fptr, 0, SEEK_END);
        if (fputs(lines[i], file->fptr) == EOF) return false;
        if (fputc('\n', file->fptr) == EOF) return false;
    }

    return true;
}
