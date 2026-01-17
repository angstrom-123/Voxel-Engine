#include "world_creation.h"

void world_new(engine_t *e, const char *name, const uint32_t seed)
{
    char wd_path[STD_BUFLEN]; multicat(wd_path, 2, WORLD_DATA_DIR, name);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = name
    };

    char mf_path[STD_BUFLEN]; multicat(mf_path, 4, WORLD_DATA_DIR, name, SEP, WORLD_META_FILE);
    file_t mf = {
        .base = WORLD_DATA_DIR,
        .path = mf_path,
        .name = WORLD_META_FILE
    };

    RUNTIME_ASSERT(!file_exists(&wd), "World already exists with this name");
    RUNTIME_ASSERT(file_create(&wd), "Failed to create world dir");

    RUNTIME_ASSERT(file_create(&mf), "Failed to create meta file");

    char meta[4][STD_BUFLEN];
    snprintf(meta[0], STD_BUFLEN, "%.5f,%.5f,%.5f", 0.0, 100.0, 0.0);
    snprintf(meta[1], STD_BUFLEN, "%.5f,%.5f,%.5f,%.5f", 0.0, 0.0, 0.0, 0.0);
    snprintf(meta[2], STD_BUFLEN, "%u", seed);
    snprintf(meta[3], STD_BUFLEN, "%lu", 0l);

    RUNTIME_ASSERT(file_write_lines(&mf, 4, STD_BUFLEN, meta), "Failed to write to meta file");
    RUNTIME_ASSERT(file_close(&mf), "Failed to close meta file");

    e->api.start_running(e, &(engine_run_desc_t) {
        .world_name = name,
        .seed = seed,
        .time = 0,
        .cam_pos = VEC3(8.5, 100.0, 8.5),
        .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
    });
}

void world_load(engine_t *e, const char *name)
{
    char wd_path[STD_BUFLEN] = {0}; multicat(wd_path, 2, WORLD_DATA_DIR, name);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = name
    };

    char mf_path[STD_BUFLEN] = {0}; multicat(mf_path, 4, WORLD_DATA_DIR, name, SEP, WORLD_META_FILE);
    file_t mf = {
        .base = WORLD_DATA_DIR,
        .path = mf_path,
        .name = WORLD_META_FILE
    };

    RUNTIME_ASSERT(file_exists(&wd), "World dir not found");
    RUNTIME_ASSERT(file_exists(&mf), "Meta file not found");
    RUNTIME_ASSERT(file_open(&mf, USAGE_READ), "Failed to open meta file");

    size_t cnt = 4;
    char lines[cnt][STD_BUFLEN];
    RUNTIME_ASSERT(file_read_lines(&mf, &cnt, STD_BUFLEN, lines), "Failed to read from meta file");
    RUNTIME_ASSERT(file_close(&mf), "Failed to close meta file");
    RUNTIME_ASSERT(cnt == 4, "World meta file must contain all fields");

    vec3 player_pos;
    quat player_rot;
    uint32_t seed;
    uint64_t time;

    // Read in values from the strings extracted from the meta file.
    char *tok;
    tok = strtok(lines[0], ",");
    for (size_t i = 0; i < 3; i++)
    {
        RUNTIME_ASSERT(tok != NULL, "Player position must contain all components");
        player_pos.elements[i] = strtof(tok, NULL);
        tok = strtok(NULL, ",");
    }

    tok = strtok(lines[1], ",");
    for (size_t i = 0; i < 4; i++)
    {
        RUNTIME_ASSERT(tok != NULL, "Player rotation must contain all components");
        player_rot.elements[i] = strtof(tok, NULL);
        tok = strtok(NULL, ",");
    }

    seed = (uint32_t) strtoul(lines[2], NULL, 10);
    time = strtoul(lines[3], NULL, 10);

    e->api.start_running(e, &(engine_run_desc_t) {
        .world_name = name,
        .seed = seed,
        .time = time,
        .cam_pos = player_pos,
        .cam_rot = player_rot
    });
}

void world_rename(engine_t *e, const char *name, const char *new_name)
{
    (void) e;
    char wd_path[STD_BUFLEN]; multicat(wd_path, 2, WORLD_DATA_DIR, name);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = name
    };

    RUNTIME_ASSERT(file_exists(&wd), "World dir not found");
    RUNTIME_ASSERT(file_rename(&wd, new_name), "Failed to rename world dir");
}

void world_delete(engine_t *e, const char *name)
{
    (void) e;
    char wd_path[STD_BUFLEN]; multicat(wd_path, 2, WORLD_DATA_DIR, name);
    file_t wd = {
        .flags = FILEFLAG_DIR,
        .base = WORLD_DATA_DIR,
        .path = wd_path,
        .name = name
    };

    RUNTIME_ASSERT(file_dir_exists(&wd), "World dir not found");
    RUNTIME_ASSERT(file_dir_delete(&wd), "Failed to delete world dir");
}

void world_print_help(void)
{
    _LOG("HELP", COL_NRM, 
         "\n"
         "%s\n"
         "%s:\n"
         "    %s\n"
         "        %s\n\n"
         "    %s\n"
         "        %s\n\n"
         "%s:\n"
         "    %s\n"
         "        %s\n\n"
         "%s:\n"
         "    %s\n"
         "        %s\n\n"
         "    %s\n"
         "        %s\n\n"
         "%s:\n"
         "    %s\n"
         "        %s\n\n",
         "To create a temporary world for testing, simply run the binary as-is",
         "new", 
         "-n, --name", "Name of the world to create",
         "-s, --seed", "Seed of the world to create",
         "load",
         "-n, --name", "Name of the world to load",
         "rename",
         "-n, --name", "Name of the world to rename",
         "-nn, --newname", "Name to rename the world to",
         "delete",
         "-n, --name", "Name of the world to delete");
}
