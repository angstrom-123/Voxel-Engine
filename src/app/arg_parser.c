#include "arg_parser.h"

bool process_args(const args_t *args, const char *format, parsed_args_t *res)
{
    if (strlen(format) != (size_t) args->argc - 1) return false;

    for (size_t i = 1; i < (size_t) args->argc; i++)
    {
        const char *arg = args->argv[i];
        char c = format[i - 1];
        APP_LOG_WARN("Comparing arg: %s with format: %c\n", arg, c);

        switch (c) {
        case 'a':
            strncpy(res->action, arg, MAX_ACTION_LENGTH);
            break;
        case 't': {
            int32_t insert = -1;
            while ((res->tacks[++insert][0]));
            strncpy(res->tacks[insert], arg, MAX_TACK_LENGTH);
            break;
        }
        case 'p': {
            int32_t insert = -1;
            while ((res->params[++insert][0]));
            strncpy(res->params[insert], arg, MAX_PARAM_LENGTH);
            break;
        }
        default:
            return false;
        };
    }

    return true;
}

bool args_match(const parsed_args_t *pargs, const arg_type_e type, const char *val)
{
    switch (type) {
    case ARGTYPE_ACTION:
        return strcmp(pargs->action, val) == 0;
    case ARGTYPE_TACK: {
        for (size_t i = 0; i < MAX_PARAM_COUNT && (pargs->tacks[i]); i++)
            if (strcmp(pargs->tacks[i], val) == 0) return true;
        return false;
    }
    case ARGTYPE_PARAM: {
        for (size_t i = 0; i < MAX_PARAM_COUNT && (pargs->params[i]); i++)
            if (strcmp(pargs->params[i], val) == 0) return true;
        return false;
    }
    default:
        return false;
    };
}

const char *arg_at(const parsed_args_t *pargs, const arg_type_e type, const int32_t ix)
{
    if (ix < 0) return NULL;
    switch (type) {
    case ARGTYPE_TACK:
        return pargs->tacks[ix];
    case ARGTYPE_PARAM:
        return pargs->params[ix];
    default:
        return NULL;
    };
}

int32_t args_index_of(const parsed_args_t *pargs, const arg_type_e type, const char *val)
{
    switch (type) {
    case ARGTYPE_ACTION:
        return -1;
    case ARGTYPE_TACK: {
        for (size_t i = 0; i < MAX_PARAM_COUNT && (pargs->tacks[i]); i++)
            if (strcmp(pargs->tacks[i], val) == 0) return i;
        return -1;
    }
    case ARGTYPE_PARAM: {
        for (size_t i = 0; i < MAX_PARAM_COUNT && (pargs->params[i]); i++)
            if (strcmp(pargs->params[i], val) == 0) return i;
        return -1;
    }
    default:
        return false;
    };
}
