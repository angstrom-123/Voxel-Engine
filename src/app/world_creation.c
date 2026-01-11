#include "world_creation.h"

static const size_t BUF_LEN = 128;

world_create_err_e world_new(engine_t *e, const char *name, const uint32_t seed)
{
    char world_dir_path[BUF_LEN];
    char meta_file_path[BUF_LEN];

    snprintf(world_dir_path, BUF_LEN, WORLD_DATA_DIR "%s", name);
    snprintf(meta_file_path, BUF_LEN, WORLD_DATA_DIR "%s" SEP WORLD_META_FILE, name);

    file_t world_dir = {
        .flags = FILEFLAG_DIR,
        .path = world_dir_path,
        .name = name
    };

    file_t meta_file = {
        .path = meta_file_path,
        .name = WORLD_META_FILE
    };

    if (file_exists(&world_dir)) return ERR_DUPLICATE_NAME;
    if (!file_create(&world_dir)) return ERR_GENERIC;

    if (!file_create(&meta_file)) return ERR_GENERIC;
    if (!file_open(&meta_file, USAGE_WRITE)) return ERR_GENERIC;

    char meta_lines[4][BUF_LEN];
    snprintf(meta_lines[0], BUF_LEN, "%.5f,%.5f,%.5f", 0.0, 0.0, 0.0);
    snprintf(meta_lines[1], BUF_LEN, "%.5f,%.5f,%.5f,%.5f", 0.0, 0.0, 0.0, 0.0);
    snprintf(meta_lines[2], BUF_LEN, "%u", seed);
    snprintf(meta_lines[3], BUF_LEN, "%lu", 0l);

    if (!file_write_lines(&meta_file, 4, BUF_LEN, meta_lines)) return ERR_GENERIC;

    e->api.start_running(e, &(engine_run_desc_t) {
        .world_name = name,
        .seed = seed,
        .time = 0,
        .cam_pos = VEC3(0.0, 100.0, 0.0),
        .cam_rot = QUAT(0.0, 0.0, 0.0, 1.0)
    });

    return ERR_NONE;
}

world_create_err_e world_load(engine_t *e, const char *name)
{
    char path_buf[BUF_LEN];
    snprintf(path_buf, BUF_LEN, WORLD_DATA_DIR "%s", name);

    file_t f = {
        .flags = FILEFLAG_DIR,
        .path = path_buf,
        .name = name
    };

    if (!file_exists(&f)) return ERR_NOT_EXISTS;
    if (!file_open(&f, USAGE_READ)) return ERR_GENERIC;

    size_t num_lines = 4;
    char lines[num_lines][BUF_LEN];
    if (!file_read_lines(&f, &num_lines, BUF_LEN, lines)) return ERR_GENERIC;
    _ASSERT("RUNTIME", num_lines == 4, "World meta file must contain all fields");

    vec3 player_pos;
    quat player_rot;
    uint32_t seed;
    uint64_t time;

    // Read in values from the strings extracted from the meta file.
    for (size_t i = 0; i < 3; i++)
    {
        char *tok = strtok(lines[0], ",");
        _ASSERT("RUNTIME", tok != NULL, "Player position must contain all components");
        player_pos.elements[i] = strtof(tok, NULL);
    }
    for (size_t i = 0; i < 4; i++)
    {
        char *tok = strtok(lines[1], ",");
        _ASSERT("RUNTIME", tok != NULL, "Player direction must contain all components");
        player_rot.elements[i] = strtof(tok, NULL);
    }
    seed = (uint32_t) strtoul(lines[2], NULL, 10);
    time = strtoul(lines[3], NULL, 10);

    e->api.start_running(e, &(engine_run_desc_t) {
        .seed = seed,
        .world_name = name,
        .time = time,
        .cam_pos = player_pos,
        .cam_rot = player_rot
    });

    return ERR_NONE;
}

world_create_err_e world_rename(engine_t *e, const char *name, const char *new_name)
{
    char path_buf[BUF_LEN];
    snprintf(path_buf, BUF_LEN, WORLD_DATA_DIR "%s", name);

    file_t f = {
        .flags = FILEFLAG_DIR,
        .path = path_buf,
        .name = name
    };

    if (!file_exists(&f)) return ERR_NOT_EXISTS;
    if (!file_rename(&f, new_name)) return ERR_GENERIC;

    e->meta.world.name = new_name;

    return ERR_NONE;
}

world_create_err_e world_delete(engine_t *e, const char *name)
{
    if (strcmp(e->meta.world.name, name))
    {
        e->meta.world.name = NULL;
        e->meta.world.seed = 0;
        e->_chunk_sys.seed = 0;
    }

    char path_buf[BUF_LEN];
    snprintf(path_buf, BUF_LEN, WORLD_DATA_DIR "%s", name);

    file_t f = {
        .flags = FILEFLAG_DIR,
        .path = path_buf,
        .name = name
    };

    if (!file_exists(&f)) return ERR_NOT_EXISTS;
    if (file_dir_delete(&f)) return ERR_GENERIC;

    return ERR_NONE;
}

void world_print_help(void)
{
    _LOG("HELP", COL_OKK, 
         "                    \n"
         "                    %s\n"
         "                    %s:\n"
         "                        %s\n"
         "                            %s\n\n"
         "                        %s\n"
         "                            %s\n\n"
         "                    %s:\n"
         "                        %s\n"
         "                            %s\n\n"
         "                    %s:\n"
         "                        %s\n"
         "                            %s\n\n"
         "                        %s\n"
         "                            %s\n\n"
         "                    %s:\n"
         "                        %s\n"
         "                            %s\n\n",
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
