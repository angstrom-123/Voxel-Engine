// #include "file_handler.h"
//
// char *_concat(const char *s1, const char *s2)
// {
//     size_t len1 = strlen(s1);
//     size_t len2 = strlen(s2);
//     char *res = malloc(len1 + len2 + 1); // Null terminated
//     if (!res)
//     {
//         fprintf(stderr, "Malloc failed in concat.\n");
//         exit(1);
//     }
//
//     memcpy(res, s1, len1);
//     memcpy(res + len1, s2, len2 + 1);
//     return res;
// }
//
// char *_concat3(const char *s1, const char *s2, const char *s3)
// {
//     size_t len1 = strlen(s1);
//     size_t len2 = strlen(s2);
//     size_t len3 = strlen(s3);
//     char *res = malloc(len1 + len2 + len3 + 1); // Null terminated
//     if (!res)
//     {
//         fprintf(stderr, "Malloc failed in concat.\n");
//         exit(1);
//     }
//
//     memcpy(res, s1, len1);
//     memcpy(res + len1, s2, len2 + 1);
//     memcpy(res + len1 + len2, s3, len3 + 1);
//     return res;
// }
//
// int _rm_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
// {
//     (void) sb;
//     (void) typeflag;
//     (void) ftwbuf;
//
//     return remove(fpath);
// }
//
// bool file_make_world_dir(char *world_name)
// {
//     char *dir_name = _concat(WORLD_DATA_DIR, world_name);
//     // printf("dir name: %s\n", dir_name);
//
//     #ifdef _WIN32
//         /* TODO: Use WIN API for dir. */
//         fprintf("Windows not supported yet.");
//         exit(1);
//     #else // linux
//         DIR *d_ptr = opendir(dir_name);
//         if (d_ptr)
//         {
//             closedir(d_ptr);
//             return false;
//         } 
//         else if (ENOENT == errno)
//         {
//             int32_t res = mkdir(dir_name, 0755);
//             if (res != 0)
//             {
//                 fprintf(stderr, "Failed to create world folder.\n");
//                 exit(1);
//             }
//         }
//         else
//         {
//             fprintf(stderr, "Failed to create world folder.\n");
//             exit(1);
//         }
//     #endif // linux
//
//     return true;
// }
//
// // void file_write_chunk(char *world_name, chunk_t *c)
// // {
// //     /* Create new or overwrite old chunk file. */
// //     char chunk_file_name[MAX_CHUNK_FILE_LEN] = {0};
// //     #ifdef _WIN32 
// //         sprintf(chunk_file_name, "\\%i_%i.mrc", c->x, c->z);
// //     #else // linux 
// //         sprintf(chunk_file_name, "/%i_%i.mrc", c->x, c->z);
// //     #endif
// //     char *chunk_file = _concat3(WORLD_DATA_DIR, world_name, chunk_file_name);
// //
// //     FILE *f_ptr = fopen(chunk_file, "wb");
// //     if (!f_ptr)
// //     {
// //         fprintf(stderr, "Failed to open chunk file for writing.\n");
// //         exit(1);
// //     }
// //
// //     /* Write chunk flags. */
// //     uint8_t flags = 0;
// //     fprintf(f_ptr, "%hhu", flags);
// //
// //     /* Write block data. */
// //     uint8_t *ptr = (uint8_t *) c->blocks->types;
// //     for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
// //     {
// //         uint8_t type = *ptr;
// //         fprintf(f_ptr, "%hhu", type);
// //         ptr++;
// //     }
// //
// //     fclose(f_ptr);
// //     free(chunk_file);
// // }
//
// // chunk_t *file_get_chunk(char *world_name, int32_t x, int32_t z)
// // {
// //     char *chunk_file_name = malloc(sizeof(char) * MAX_CHUNK_FILE_LEN);
// //     #ifdef _WIN32 
// //         sprintf(chunk_file_name, "\\%i_%i.mrc", x, z);
// //     #else // linux 
// //         sprintf(chunk_file_name, "/%i_%i.mrc", x, z);
// //     #endif
// //     char *chunk_file = _concat3(WORLD_DATA_DIR, world_name, chunk_file_name);
// //     FILE *f_ptr = fopen(chunk_file, "rb");
// //     if (!f_ptr)
// //     {
// //         free(chunk_file);
// //         free(chunk_file_name);
// //         return NULL; // No chunk file for this coordinate.
// //     }
// //
// //     chunk_t *c = malloc(sizeof(chunk_t));
// //     c->blocks = malloc(sizeof(chunk_data_t));
// //
// //     /* Extract flag data. */
// //     uint8_t flags = fgetc(f_ptr);
// //
// //     /* Extract block data. */
// //     for (size_t i = 0; i < CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE; i++)
// //     {
// //         const uint8_t x = i / (CHUNK_HEIGHT * CHUNK_SIZE);
// //         const uint8_t y = (i / CHUNK_SIZE) % CHUNK_HEIGHT;
// //         const uint8_t z = i % CHUNK_SIZE;
// //
// //         c->blocks->types[x][y][z] = fgetc(f_ptr);
// //     }
// //
// //     c->x = x;
// //     c->z = z;
// //     c->dirty = false;
// //     c->soft_delete = false;
// //     c->mesh = NULL;
// //     c->generation = 0;
// //     c->offsets.v_ofst = 0;
// //     c->offsets.i_ofst = 0;
// //
// //     fclose(f_ptr);
// //     free(chunk_file_name);
// //     free(chunk_file);
// //
// //     return c;
// // }
//
// char **file_get_world_names(size_t *len)
// {
//     size_t num_files = 64;
//     size_t res_len = 0;
//     char **res = malloc(num_files * sizeof(char *));
//
//     #ifdef _WIN32 
//         /* TODO: Use WIN API for dir. */
//         fprintf("Windows not supported yet.");
//         exit(1);
//     #else // linux
//         DIR *d_ptr = opendir(WORLD_DATA_DIR);
//         if (d_ptr)
//         {
//             struct dirent *de;
//             while ((de = readdir(d_ptr)) != NULL)
//             {
//                 char *name = de->d_name;
//                 if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
//                     continue;
//
//                 res[res_len] = calloc(MAX_WORLD_NAME_LEN, sizeof(char));
//                 strncpy(res[res_len], name, MAX_WORLD_NAME_LEN);
//                 res_len++;
//
//                 if (res_len == num_files)
//                 {
//                     num_files *= 1.5;
//                     res = realloc(res, num_files * sizeof(char *));
//                 }
//             }
//
//             closedir(d_ptr);
//         }
//     #endif // linux
//
//     res = realloc(res, res_len * sizeof(char *));
//     *len = res_len;
//     return res;
// }
//
// void file_rename_world_dir(char *world_name, char *new_name)
// {
//     #ifdef _WIN32
//         /* TODO: Use WIN API for dir. */
//         fprintf(stderr, "Windows not supported yet.");
//         exit(1);
//     #else // linux 
//         char *old_dir_name = _concat(WORLD_DATA_DIR, world_name);
//         char *new_dir_name = _concat(WORLD_DATA_DIR, new_name);
//
//         int res = rename(old_dir_name, new_dir_name);
//
//         free(old_dir_name);
//         free(new_dir_name);
//         if (res != 0)
//         {
//             fprintf(stderr, "Failed to rename world dir.\n");
//             exit(1);
//         }
//     #endif // linux
// }
//
// void file_delete_world_dir(char *world_name)
// {
//     #ifdef _WIN32 
//         /* TODO: Use WIN API for dir. */
//         fprintf(stderr, "Windows not supported yet.");
//         exit(1);
//     #else // linux
//         char *dir = _concat(WORLD_DATA_DIR, world_name);
//         int res = nftw(dir, _rm_cb, FTW_D, FTW_DEPTH | FTW_PHYS);
//         free(dir);
//         if (res != 0)
//         {
//             fprintf(stderr, "Failed to delete world dir.\n");
//             exit(1);
//         }
//     #endif // linux
// }
//
// void file_make_world_desc(game_desc_t desc)
// {
//     #ifdef _WIN32 
//         char *desc_name = "\\desc";
//     #else // linux 
//         char *desc_name = "/desc";
//     #endif
//
//     char *dir = _concat3(WORLD_DATA_DIR, desc.selected_str, desc_name);
//     FILE *f_ptr = fopen(dir, "w");
//     if (!f_ptr)
//     {
//         fprintf(stderr, "Failed to open chunk file for writing.\n");
//         exit(1);
//     }
//
//     // TODO: Add any local settings to this desc file.
//     fprintf(f_ptr, "%u\n", desc.seed);
//
//     fclose(f_ptr);
//     free(dir);
// }
//
// load_data_t *file_get_world_data(char *world_name)
// {
//     #ifdef _WIN32 
//         char *desc_name = "\\desc";
//     #else // linux 
//         char *desc_name = "/desc";
//     #endif
//
//     char *dir = _concat3(WORLD_DATA_DIR, world_name, desc_name);
//     FILE *f_ptr = fopen(dir, "r");
//     if (!f_ptr)
//     {
//         fprintf(stderr, "Failed to open chunk file for writing.\n");
//         exit(1);
//     }
//
//     load_data_t *res = malloc(sizeof(load_data_t));
//
//     char buf[11] = {0};
//     fgets(buf, 11, f_ptr);
//     res->seed = strtoul(buf, NULL, 10);
//
//     fclose(f_ptr);
//     free(dir);
//
//     return res;
// }
