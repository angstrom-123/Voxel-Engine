#include "base.h"

void multicat(char *res, size_t n, ...)
{
    va_list args;
    va_start(args, n);
    for (size_t i = 0; i < n; i++)
        strcat(res, va_arg(args, const char *));
    strcat(res, "\0");
}
