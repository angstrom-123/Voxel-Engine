#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <stdint.h>
#include <stdlib.h>

typedef struct gen_block {
	float vertex_buf[168];
	uint16_t index_buf[36];
} gen_block_t;

typedef struct gen_chunk {
	size_t block_len;
	gen_block_t *blocks;
} gen_chunk_t;

#endif
