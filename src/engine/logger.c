#include "logger.h"

char *_get_time(void)
{
    time_t tmp = time(NULL);
    char *full_time = asctime(localtime(&tmp));
    /* Split at spaces an move forward until we reach the hour:minute:time token. */
    strtok(full_time, " "); strtok(NULL, " "); strtok(NULL, " ");
    char *res = strtok(NULL, " ");
    return res;
}

