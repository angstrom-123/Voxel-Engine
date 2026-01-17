#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "logger.h"

#define MAX_ACTION_LENGTH 16
#define MAX_TACK_LENGTH 16
#define MAX_PARAM_LENGTH 64
#define MAX_PARAM_COUNT 4

typedef enum arg_type {
    ARGTYPE_ACTION,
    ARGTYPE_TACK,
    ARGTYPE_PARAM
} arg_type_e;

typedef struct args {
    int argc;
    char **argv;
} args_t;

// Padded to leave space for null terminator (unlikely to get that long anyway).
typedef struct parsed_args {
    char action[MAX_ACTION_LENGTH + 1];
    char tacks[MAX_PARAM_COUNT + 1][MAX_TACK_LENGTH + 1];
    char params[MAX_PARAM_COUNT + 1][MAX_TACK_LENGTH + 1];
} parsed_args_t;

extern bool process_args(const args_t *args, const char *format, parsed_args_t *res);
extern bool args_match(const parsed_args_t *pargs, const arg_type_e type, const char *val);
extern const char *arg_at(const parsed_args_t *pargs, const arg_type_e type, const int32_t ix);
extern int32_t args_index_of(const parsed_args_t *pargs, const arg_type_e type, const char *val);

#endif
