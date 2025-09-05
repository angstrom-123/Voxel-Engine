#include "file_handler.h"

#include <string.h>

char *file_make_world_dir(char *wn)
{
    char *dir_name = malloc(sizeof(char) * MAX_DIRNAME_LEN);
    sprintf(dir_name, "data/%s/", wn);

    #ifdef _WIN32
        /* TODO: Use WIN API for dir. */
        fprintf("Windows not supported yet.");
        exit(1);
    #else // linux
        DIR *d_ptr = opendir(dir_name);
        if (d_ptr)
        {
            closedir(d_ptr);
            return NULL;
        } 
        else if (ENOENT == errno)
        {
            int32_t res = mkdir(dir_name, 0755);
            if (res != 0)
            {
                fprintf(stderr, "Failed to create world folder.\n");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Failed to create world folder.\n");
            exit(1);
        }
    #endif

    return dir_name;
}

void file_write_chunk(char *dn, chunk_t *c)
{
    /* Create new or overwrite old chunk file. */
    char *file_name = malloc(sizeof(char) * MAX_FILENAME_LEN);
    sprintf(file_name, "%s%i_%i.mrc", dn, c->x, c->z);
    FILE *f_ptr = fopen(file_name, "wb");
    if (!f_ptr)
    {
        fprintf(stderr, "Failed to open chunk file for writing.\n");
        exit(1);
    }

    /* Write chunk flags. */
    uint8_t flags = 0;
    if (c->edited) flags |= (1 << FLAG_EDITED);
    fprintf(f_ptr, "%c%hhu", 'a', flags);

    /* Write block data. */
    uint8_t *ptr = (uint8_t *) c->blocks->types;
    for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
    {
        uint8_t type = *ptr;
        fprintf(f_ptr, "%hhu", type);
        ptr++;
    }
    
    fclose(f_ptr);
    free(file_name);
}

chunk_t *file_get_chunk(char *dn, int32_t x, int32_t z)
{
    char *file_name = malloc(sizeof(char) * MAX_FILENAME_LEN);
    sprintf(file_name, "%s%i_%i.mrc", dn, x, z);
    FILE *f_ptr = fopen(file_name, "rb");
    if (!f_ptr)
        return NULL; // No chunk file for this coordinate.

    chunk_t *c = malloc(sizeof(chunk_t));
    c->blocks = malloc(sizeof(chunk_data_t));

    /* Extract flag data. */
    uint8_t flags = fgetc(f_ptr);

    /* Extract block data. */
    for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
    {
        const uint8_t x = i / (CHUNK_HEIGHT * CHUNK_SIZE);
        const uint8_t y = (i / CHUNK_SIZE) % CHUNK_HEIGHT;
        const uint8_t z = i % CHUNK_SIZE;

        c->blocks->types[x][y][z] = fgetc(f_ptr);
    }

    c->x = x;
    c->z = z;
    c->dirty = false;
    c->edited = (flags & (1 << FLAG_EDITED)) == 1;
    c->creation_frame = 0;
    c->offsets.v_ofst = 0;
    c->offsets.i_ofst = 0;

    fclose(f_ptr);
    free(file_name);

    return c;
}

char **file_get_filenames(char *dir_name, size_t *len)
{
    size_t num_files = 64;
    size_t res_len = 0;
    char **res = malloc(num_files * sizeof(char *));

    #ifdef _WIN32 
        /* TODO: Use WIN API for dir. */
        fprintf("Windows not supported yet.");
        exit(1);
    #else // linux
        DIR *d_ptr = opendir(dir_name);
        if (d_ptr)
        {
            struct dirent *de;
            while ((de = readdir(d_ptr)) != NULL)
            {
                char *name = de->d_name;
                if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
                    continue;

                res[res_len] = calloc(MAX_FILENAME_LEN, sizeof(char));
                strncpy(res[res_len], name, MAX_FILENAME_LEN);
                res_len++;

                if (res_len == num_files)
                {
                    num_files *= 1.5;
                    res = realloc(res, num_files * sizeof(char *));
                }
            }

            closedir(d_ptr);
        }
    #endif

    res = realloc(res, res_len * sizeof(char *));
    *len = res_len;
    return res;
}
