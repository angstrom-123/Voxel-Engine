#include "files_linux.h"

bool _file_exists_linux(linux_file_t *file)
{
    FILE *fptr = fopen(file->path, "r");
    if (fptr) 
    {
        fclose(fptr);
        return true;
    }
    return false;
}

bool _file_dir_exists_linux(linux_file_t *file)
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

bool _file_create_linux(linux_file_t *file)
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

bool _file_open_linux(linux_file_t *file, file_usage_e usage)
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

bool _file_close_linux(linux_file_t *file)
{
    if (!(file->flags & FILEFLAG_OPEN)) return false;

    if (fclose(file->fptr) == EOF) return false;
    file->fptr = NULL;
    file->flags = 0;

    return true;
}

bool _file_delete_linux(linux_file_t *file)
{
    if ((file->flags & FILEFLAG_OPEN)) return false;

    if (remove(file->path) == EOF) return false;
    file->flags = 0;

    return true;
}

static int filter_no_upward(const struct dirent *d)
{
    if (strcmp(d->d_name, ".") == 0) return 0;
    if (strcmp(d->d_name, "..") == 0) return 0;
    return 1;
}

bool _file_dir_delete_linux(linux_file_t *file)
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

        linux_file_t f = {
            .base = file->path,
            .path = d_path,
            .name = d->d_name
        };

        if (S_ISREG(statbuf.st_mode))
        {
            if (!_file_delete_linux(&f)) return false;
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            f.flags |= FILEFLAG_DIR;
            if (!_file_dir_delete_linux(&f)) return false;
        }
        else 
        {
            return false;
        }
    }

    if (!_file_delete_linux(file)) return false;

    return true;
}

bool _file_rename_linux(linux_file_t *file, const char *new_name)
{
    if (file->flags & FILEFLAG_OPEN) return false;

    char new_path[128];
    snprintf(new_path, 128, "%s%s", file->base, new_name);

    if (rename(file->path, new_path) == EOF) return false;

    file->path = new_path;
    file->name = new_name;

    return true;
}

bool _file_read_bytes_linux(linux_file_t *file, size_t *num_bytes, uint8_t bytes[*num_bytes])
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

bool _file_read_lines_linux(linux_file_t *file, size_t *num_lines, size_t line_len,
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

bool _file_write_bytes_linux(linux_file_t *file, size_t num_bytes, uint8_t bytes[num_bytes])
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

bool _file_write_lines_linux(linux_file_t *file, size_t num_lines, size_t line_len, 
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
