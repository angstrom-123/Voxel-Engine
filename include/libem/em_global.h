#ifndef EM_GLOBAL_H
#define EM_GLOBAL_H

#include <malloc.h>
#include <memory.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef EM_ENABLE_LOGGING
#define LOG 1
#else 
#define LOG 0
#endif

#define EM_LOG(fmt, ...) do { if (LOG) (fprintf(stderr, "%s:%d:%s(): [LIBEM] " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__)); } while (0)

typedef enum em_flags {
    EM_FLAG_NONE = 0,
    EM_FLAG_NO_RESIZE = 1,
    EM_FLAG_AL_FAST_REMOVE = 2,
    EM_FLAG_NO_DESTROY_ENTRIES = 4
} em_flags_t;

typedef struct em_notype {
    char _padding; // Padding 1 Byte to avoid undefined behaviour of 0 size struct.
} em_notype_t;

#define EM_NOTYPE_DUMMY (em_notype_t) {0}

typedef void (*void_cln_func)(void *);
typedef bool (*void_cmp_func)(const void *, const void *);
typedef void_cmp_func void_fltr_func;

#endif
