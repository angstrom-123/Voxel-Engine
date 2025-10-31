#ifndef EM_RANDOM_H
#define EM_RANDOM_H

/*
 * Romu Pseudorandom Number Generators
 *
 * Copyright 2020 Mark A. Overton
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------------
 *
 * Website: romu-random.org
 * Paper:   http://arxiv.org/abs/2002.11331
 */

#ifndef EM_RANDOM_INCLUDED
#define EM_RANDOM_INCLUDED

#include "em_global.h"

#include <stdint.h>

extern void em_romu_duo_init(uint32_t seed);
extern uint64_t em_romu_duo_random();

extern void em_romu_mono32_init(uint32_t seed);
extern uint16_t em_romu_mono32_random();

#endif // EM_RANDOM_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_RANDOM_IMPL
#define ROTL(x, n) ((x << (n)) | (x >> (8 * sizeof(x) - (n))))

/* Romu-Duo. */
#define DUO_MAX UINT64_MAX

static uint64_t state_duo_x;
static uint64_t state_duo_y;

void em_romu_duo_init(uint32_t seed)
{
    state_duo_x = (seed >> 16) + 4182391820u;
    state_duo_y = (seed & 0xFF) + 7401739501u;
}

uint64_t em_romu_duo_random() {
   uint64_t xp = state_duo_x;
   state_duo_x = 15241094284759029579u * state_duo_y;
   state_duo_y = ROTL(state_duo_y,36) + ROTL(state_duo_y,15) - xp;
   return xp;
}

/* Romu-Mono32. */
#define MONO32_MAX UINT16_MAX

static uint32_t state_mono32;

void em_romu_mono32_init(uint32_t seed) 
{
    /* 29 seed bits used. */
    state_mono32 = (seed & 0x1FFFFFFF) + 1156979152u; 
}

uint16_t em_romu_mono32_random()
{
    uint16_t res = state_mono32 >> 16;
    state_mono32 *= 3611795771u;
    state_mono32 = ROTL(state_mono32, 12);
    return res;
}

#endif // EM_RANDOM_IMPL

#endif
