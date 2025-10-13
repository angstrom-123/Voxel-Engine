#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#ifndef NK_NUKLEAR_H_
#undef NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#endif // NK_NUKLEAR_H_

#include "engine/state.h"
#include "engine/constants.h"
#include "engine/file_handler.h"

extern void menu_draw(game_desc_t *game_desc, void (*init_cb)(bool), void (*save_cb)(void),
                      struct nk_context *ctx);

#endif
