#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG 
#define DO_DEBUG 1
#else 
#define DO_DEBUG 0
#endif

#ifdef PLAT_LINUX
#define COL_NRM "\x1B[0m"
#define COL_ERR "\x1B[31m"
#define COL_OKK "\x1B[32m"
#define COL_WRN "\x1B[33m"
#define COL_RST "\x1B[0m"
#else
#define COL_NRM ""
#define COL_ERR ""
#define COL_OKK ""
#define COL_WRN ""
#define COL_RST ""
#endif

#define _LOG(prefix, col, fmt, ...) do { if (DO_DEBUG) (fprintf(stderr, "%s:%d:%s(): \n        " col "[" prefix "]: " fmt COL_RST "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)); } while (0)

#define ENGINE_LOG_ERROR(fmt, ...) _LOG("ENGINE", COL_ERR, fmt, __VA_ARGS__)
#define ENGINE_LOG_WARN(fmt, ...) _LOG("ENGINE", COL_WRN, fmt, __VA_ARGS__)
#define ENGINE_LOG_OK(fmt, ...) _LOG("ENGINE", COL_OKK, fmt, __VA_ARGS__)

#define APP_LOG_ERROR(fmt, ...) _LOG("APP", COL_ERR, fmt, __VA_ARGS__)
#define APP_LOG_WARN(fmt, ...) _LOG("APP", COL_WRN, fmt, __VA_ARGS__)
#define APP_LOG_OK(fmt, ...) _LOG("APP", COL_OKK, fmt, __VA_ARGS__)

#endif
