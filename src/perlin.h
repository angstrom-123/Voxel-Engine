#ifndef PERLIN_H
#define PERLIN_H

// TODO: Implement simplex noise because apparently it's better in every way

#include <stdint.h>
#include <stddef.h> // size_t
// #include <threads.h> // TODO: make the noise multithreaded for performance :)

#include "extra_math.h" // vectors
#include "romu_random.h" // fast random

extern int32_t perlin_hash(int32_t x);
extern int32_t perlin_pair(int32_t a, int32_t b);

extern float perlin_2d(float x, float y, float frequency);
extern float perlin_octave_2d(float x, float y, uint8_t num_octaves);

extern float perlin_3d(float x, float y, float z, float frequency);
extern float perlin_octave_3d(float x, float y, float z, uint8_t num_octaves);

#endif

