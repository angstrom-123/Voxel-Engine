#ifndef FILE_META_H
#define FILE_META_H

typedef enum file_usage {
    USAGE_NONE,
    USAGE_READ,
    USAGE_READ_BIN,
    USAGE_WRITE,
    USAGE_WRITE_BIN
} file_usage_e;

typedef enum file_flags {
    FILEFLAG_OPEN   = 1,
    FILEFLAG_BINARY = 2,
    FILEFLAG_READ   = 4,
    FILEFLAG_WRITE  = 8,
    FILEFLAG_DIR    = 16
} file_flags_e;

#endif
