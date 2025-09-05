#ifndef MENU_H
#define MENU_H

#ifndef NK_NUKLEAR_H_
#undef NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#endif // NK_NUKLEAR_H_

#include "state.h"
#include "file_handler.h"

extern void menu_draw(menu_desc_t *menu_desc, struct nk_context *ctx);

#endif
