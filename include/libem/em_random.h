#ifndef EM_RANDOM_H
#define EM_RANDOM_H

/*
 * The following code is adapted from:
 *
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

typedef struct em_romu_duo_state {
    ULONG x;
    ULONG y;
} em_romu_duo_state_t;

#define EM_ROMU_DUO_MAX 18446744073709551615u
#define EM_ROMU_DUO_MAXF 18446744073709551615.0
extern void em_romu_duo_init(em_romu_duo_state_t *state, UINT seed);
extern ULONG em_romu_duo_random(em_romu_duo_state_t *state);

typedef struct em_romu_mono32_state {
    UINT x;
} em_romu_mono32_state_t;

#define EM_ROMU_MONO32_MAX 65536u
#define EM_ROMU_MONO32_MAXF 65536.0
extern void em_romu_mono32_init(em_romu_mono32_state_t *state, UINT seed);
extern USHORT em_romu_mono32_random(em_romu_mono32_state_t *state);

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
void em_romu_duo_init(em_romu_duo_state_t *state, UINT seed)
{
    state->x = (seed >> 16) + 4182391820u;
    state->y = (seed & 0xFF) + 7401739501u;
}

ULONG em_romu_duo_random(em_romu_duo_state_t *state) {
   ULONG xp = state->x;
   state->x = 15241094284759029579u * state->y;
   state->y = ROTL(state->y, 36) + ROTL(state->y, 15) - xp;
   return xp;
}

/* Romu-Mono32. */
void em_romu_mono32_init(em_romu_mono32_state_t *state, UINT seed) 
{
    /* 29 seed bits used. */
    state->x = (seed & 0x1FFFFFFF) + 1156979152u; 
}

USHORT em_romu_mono32_random(em_romu_mono32_state_t *state)
{
    USHORT res = state->x >> 16;
    state->x *= 3611795771u;
    state->x = ROTL(state->x, 12);
    return res;
}

#endif // EM_RANDOM_IMPL

#endif
