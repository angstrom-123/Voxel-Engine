#ifndef EM_BINARY_H
#define EM_BINARY_H

#ifndef EM_BINARY_INCLUDED
#define EM_BINARY_INCLUDED 

#include "em_global.h"
#include <stdio.h> // FILE

/* macros assume LITTLE ENDIAN (least significant byte at lowest memory address). */
#define TO_U16(b1, b2) ((USHORT) ((b1) | ((b2) << 8)))
#define TO_U32(b1, b2, b3, b4) ((UINT) ((b1) | ((b2) << 8) | ((b3) << 16) | ((b4) << 24)))

#define READ_U16_MOVE(mem, i) (TO_U16(mem[i], mem[(i) + 1])); ((i) += 2)
#define READ_U32_MOVE(mem, i) (TO_U32(mem[i], mem[(i) + 1], mem[(i) + 2], mem[(i) + 3])); ((i) += 4)

#define WRITE_U16_MOVE(u16, mem, i) (mem[(i)++] = (u16) & 0xFF); (mem[(i)++] = (u16) >> 8)

extern bool binary_read(FILE *f_ptr, SIZE offset, SIZE *len, UBYTE buf[*len]);

#endif // EM_BINARY_INCLUDED

/*     ______  _______  __    ________  __________   ___________  ______________  _   __
 *    /   /  |/  / __ \/ /   / ____/  |/  / ____/ | / /_  __/   |/_  __/   / __ \/ | / /
 *    / // /|_/ / /_/ / /   / __/ / /|_/ / __/ /  |/ / / / / /| | / /  / // / / /  |/ /
 *   / // /  / / ____/ /___/ /___/ /  / / /___/ /|  / / / / ___ |/ /  / // /_/ / /|  /
 * /___/_/  /_/_/   /_____/_____/_/  /_/_____/_/ |_/ /_/ /_/  |_/_/ /___/\____/_/ |_/
 *
 */

#ifdef EM_BINARY_IMPL 

bool binary_read(FILE *f_ptr, SIZE offset, SIZE *len, UBYTE buf[*len])
{
    fseek(f_ptr, offset, SEEK_SET);

    for (SIZE i = 0; i < *len; i++)
    {
        INT read = fgetc(f_ptr);
        if (read == EOF) 
        {
            *len = i;
            return false;
        }

        buf[i] = (UBYTE) read;
    }

    return true;
}

#endif // EM_BINARY_IMPL

#endif
