#ifndef LOGGER_H
#define LOGGER_H

#include "base.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#ifdef DEBUG 
#define DO_DEBUG 1
#else 
#define DO_DEBUG 0
#endif

extern char *_get_time(void);

#define _LOG(prefix, col, fmt, ...) fprintf(stderr, "%s:%d:%s(): \n        " col "[%s][" prefix "]: " fmt COL_RST "\n", __FILE__, __LINE__, __func__, _get_time(), __VA_ARGS__)
#define _ASSERT(prefix, expr, msg) if (!(expr)) { _LOG(prefix, COL_ERR, "Assertion failed: " QUOTE(expr) ".\n        " msg, NULL); raise(SIGABRT); }
// #define _ASSERT(prefix, expr, msg) if (expr) { fprintf(stderr, "%s:%d:%s(): \n        " COL_ERR "[%s][" prefix "]: Assertion failed: " QUOTE(expr) ".\n         " msg COL_RST, __FILE__, __LINE__, __func__, _get_time()); raise(SIGABRT); }

#if DO_DEBUG
#define ENGINE_LOG_ERROR(fmt, ...) _LOG("ENGINE", COL_ERR, fmt, __VA_ARGS__)
#define ENGINE_LOG_WARN(fmt, ...) _LOG("ENGINE", COL_WRN, fmt, __VA_ARGS__)
#define ENGINE_LOG_OK(fmt, ...) _LOG("ENGINE", COL_OKK, fmt, __VA_ARGS__)
#define ENGINE_ASSERT(expr, msg) _ASSERT("ENGINE", expr, msg)
#define APP_LOG_ERROR(fmt, ...) _LOG("APP", COL_ERR, fmt, __VA_ARGS__)
#define APP_LOG_WARN(fmt, ...) _LOG("APP", COL_WRN, fmt, __VA_ARGS__)
#define APP_LOG_OK(fmt, ...) _LOG("APP", COL_OKK, fmt, __VA_ARGS__)
#define APP_ASSERT(expr, msg) _ASSERT("APP", expr, msg)
#else 
#define ENGINE_LOG_ERROR(fmt, ...)
#define ENGINE_LOG_WARN(fmt, ...)
#define ENGINE_LOG_OK(fmt, ...)
#define ENGINE_ASSERT(expr, msg)
#define APP_LOG_ERROR(fmt, ...)
#define APP_LOG_WARN(fmt, ...)
#define APP_LOG_OK(fmt, ...)
#define APP_ASSERT(expr, msg)
#endif

#endif
