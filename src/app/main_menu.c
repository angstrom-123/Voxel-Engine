// #include "main_menu.h"
//
// static const struct nk_color COL_PRIMARY = {92, 103, 132, 255};
// static const struct nk_color COL_SECONDARY = {76, 185, 99, 255};
// static const struct nk_color COL_TERTIARY = {21, 127, 31, 255};
// static const struct nk_color COL_ACCENT = {160, 234, 222, 255};
// static const struct nk_color COL_DARK = {29, 38, 59, 255};
//
// static const float PADDING_AMOUNT = 20;
// static const float MARGIN_AMOUNT = 50;
//
// static const float TOP_HEIGHT = 160;
// static const float LABEL_HEIGHT = 20;
// static const float EDIT_HEIGHT = 60;
// static const float BUTTON_HEIGHT = 100;
//
// void _apply_normal_button_style(struct nk_context *ctx)
// {
//     ctx->style.button.normal = nk_style_item_color(COL_PRIMARY);
//     ctx->style.button.hover = nk_style_item_color(COL_PRIMARY);
//     ctx->style.button.active = nk_style_item_color(COL_DARK);
//     ctx->style.button.text_background = COL_ACCENT;
//     ctx->style.button.text_normal = COL_ACCENT;
//     ctx->style.button.text_hover = COL_ACCENT;
//     ctx->style.button.text_active = COL_ACCENT;
// }
//
// void _apply_highlight_button_style(struct nk_context *ctx)
// {
//     ctx->style.button.normal = nk_style_item_color(COL_DARK);
//     ctx->style.button.hover = nk_style_item_color(COL_DARK);
//     ctx->style.button.active = nk_style_item_color(COL_DARK);
//     ctx->style.button.text_background = COL_ACCENT;
//     ctx->style.button.text_normal = COL_ACCENT;
//     ctx->style.button.text_hover = COL_ACCENT;
//     ctx->style.button.text_active = COL_ACCENT;
// }
//
// void _apply_styles(struct nk_context *ctx)
// {
//     const struct nk_vec2 MARGIN = nk_vec2(MARGIN_AMOUNT, MARGIN_AMOUNT);
//     const struct nk_vec2 PADDING = nk_vec2(PADDING_AMOUNT, PADDING_AMOUNT);
//
//     ctx->style.window.spacing = PADDING;
//     ctx->style.window.popup_padding = MARGIN;
//     ctx->style.window.fixed_background = nk_style_item_hide();
//
//     ctx->style.text.color = COL_ACCENT;
//     // ctx->style.text.padding = PADDING;
//
//     ctx->style.edit.normal = nk_style_item_color(COL_DARK);
//     ctx->style.edit.hover = nk_style_item_color(COL_DARK);
//     ctx->style.edit.active = nk_style_item_color(COL_DARK);
//     ctx->style.edit.cursor_normal = COL_PRIMARY;
//     ctx->style.edit.cursor_hover = COL_PRIMARY;
//     ctx->style.edit.cursor_text_hover = COL_PRIMARY;
//     ctx->style.edit.cursor_text_normal = COL_PRIMARY;
//     ctx->style.edit.text_normal = COL_ACCENT;
//     ctx->style.edit.text_hover = COL_ACCENT;
//     ctx->style.edit.text_active = COL_ACCENT;
//
//     ctx->style.scrollv.normal = nk_style_item_color(COL_PRIMARY);
//     ctx->style.scrollv.hover = nk_style_item_color(COL_PRIMARY);
//     ctx->style.scrollv.active = nk_style_item_color(COL_PRIMARY);
//
//     ctx->style.scrollv.cursor_normal = nk_style_item_color(COL_DARK);
//     ctx->style.scrollv.cursor_hover = nk_style_item_color(COL_DARK);
//     ctx->style.scrollv.cursor_active = nk_style_item_color(COL_DARK);
//
//     _apply_normal_button_style(ctx);
// }
//
// extern void menu_draw(game_desc_t *desc, void (*init_cb)(bool), void (*save_cb)(void),
//                       struct nk_context *ctx)
// {
//     nk_style_hide_cursor(ctx);
//
//     _apply_styles(ctx);
//
//     switch (desc->game_state) {
//     case GAME_MENUMAIN: 
//     {
//         memset(desc->selected_str, '\0', sizeof(desc->selected_str));
//
//         if (nk_begin(ctx, "Top", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT, 
//                      desc->window.x - 2 * MARGIN_AMOUNT, 
//                      TOP_HEIGHT),
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
//             nk_label(ctx, "Minecraft Remake", NK_TEXT_ALIGN_CENTERED);
//         }
//         nk_end(ctx);
//
//         if (nk_begin(ctx, "Left", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT + TOP_HEIGHT, 
//                      desc->window.x / 2.0 - MARGIN_AMOUNT, 
//                      desc->window.y - TOP_HEIGHT - 2 * MARGIN_AMOUNT), 
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (nk_button_label(ctx, "New World"))
//                 desc->game_state = GAME_MENUNEW;
//             if (nk_button_label(ctx, "Load World"))
//                 desc->game_state = GAME_MENULOAD;
//             if (nk_button_label(ctx, "Options"))
//                 desc->game_state = GAME_MENUOPT;
//             if (nk_button_label(ctx, "Quit"))
//                 exit(0);
//         }
//         nk_end(ctx);
//         break;
//     }
//     case GAME_MENUNEW:
//     {
//         memset(desc->selected_str, '\0', sizeof(desc->selected_str));
//
//         if (nk_begin(ctx, "Top", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT, 
//                      desc->window.x - 2 * MARGIN_AMOUNT, 
//                      TOP_HEIGHT),
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
//             nk_label(ctx, "New World", NK_TEXT_ALIGN_CENTERED);
//         }
//         nk_end(ctx);
//
//         if (nk_begin(ctx, "Left", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT + TOP_HEIGHT, 
//                      desc->window.x / 2.0 - MARGIN_AMOUNT, 
//                      desc->window.y - TOP_HEIGHT - 2 * MARGIN_AMOUNT), 
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, EDIT_HEIGHT + LABEL_HEIGHT + PADDING_AMOUNT, 1);
//             if (nk_group_begin(ctx, "Name Group", NK_WINDOW_NO_SCROLLBAR))
//             {
//                 nk_layout_row_dynamic(ctx, LABEL_HEIGHT, 1);
//                 nk_label(ctx, "World Name:", NK_TEXT_ALIGN_LEFT);
//
//                 nk_layout_row_dynamic(ctx, EDIT_HEIGHT, 1);
//
//                 nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, desc->world_str, 
//                                                MAX_WORLD_NAME_LEN, nk_filter_ascii);
//             }
//             nk_group_end(ctx);
//
//             nk_layout_row_dynamic(ctx, EDIT_HEIGHT + LABEL_HEIGHT + PADDING_AMOUNT, 1);
//             if (nk_group_begin(ctx, "Seed Group", NK_WINDOW_NO_SCROLLBAR))
//             {
//                 nk_layout_row_dynamic(ctx, LABEL_HEIGHT, 1);
//                 nk_label(ctx, "Seed:", NK_TEXT_ALIGN_LEFT);
//
//                 nk_layout_row_dynamic(ctx, EDIT_HEIGHT, 1);
//                 nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, desc->seed_str, 
//                                                MAX_SEED_LEN, nk_filter_hex);
//             }
//             nk_group_end(ctx);
//
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (nk_button_label(ctx, "Create"))
//             {
//                 if (file_make_world_dir(desc->world_str))
//                 {
//                     strcpy(desc->selected_str, desc->world_str);
//                     init_cb(false);
//                 }
//                 else 
//                 {
//                     fprintf(stderr, "World with this name already exists.\n");
//                 }
//             }
//
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (nk_button_label(ctx, "Back"))
//                 desc->game_state = GAME_MENUMAIN;
//         }
//         nk_end(ctx);
//         break;
//     }
//     case GAME_MENULOAD:
//     {
//         if (nk_begin(ctx, "Top", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT, 
//                      desc->window.x - 2 * MARGIN_AMOUNT, 
//                      TOP_HEIGHT),
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
//             nk_label(ctx, "Load World", NK_TEXT_ALIGN_CENTERED);
//         }
//         nk_end(ctx);
//
//         if (nk_begin(ctx, "Left", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT + TOP_HEIGHT, 
//                      desc->window.x / 2.0 - MARGIN_AMOUNT, 
//                      desc->window.y - TOP_HEIGHT - 2 * MARGIN_AMOUNT), 
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (desc->selected_str[0] != '\0')
//             {
//                 if (nk_button_label(ctx, "Play"))
//                     init_cb(true);
//                 if (nk_button_label(ctx, "Delete"))
//                     desc->popup = POPUP_CONFIRM;
//                 if (nk_button_label(ctx, "Rename"))
//                     desc->popup = POPUP_EDIT_CONFIRM;
//             }
//             else 
//             {
//                 nk_spacing(ctx, 1);
//                 nk_spacing(ctx, 1);
//                 nk_spacing(ctx, 1);
//             }
//
//             if (nk_button_label(ctx, "Back"))
//                 desc->game_state = GAME_MENUMAIN;
//         }
//         nk_end(ctx);
//
//         if (nk_begin(ctx, "Right", 
//                      nk_rect(desc->window.x / 2.0 + MARGIN_AMOUNT, MARGIN_AMOUNT + TOP_HEIGHT, 
//                      desc->window.x / 2.0 - 2 * MARGIN_AMOUNT,
//                      desc->window.y - TOP_HEIGHT - 2 * MARGIN_AMOUNT), 
//                      0))
//         {
//             size_t cnt;
//             char **worlds = file_get_world_names(&cnt);
//             for (size_t i = 0; i < cnt; i++)
//             {
//                 if (strcmp(desc->selected_str, worlds[i]) == 0)
//                     _apply_highlight_button_style(ctx);
//                 else 
//                     _apply_normal_button_style(ctx);
//
//                 nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//                 if (nk_button_label(ctx, worlds[i]))
//                     strncpy(desc->selected_str, worlds[i], MAX_WORLD_NAME_LEN);
//             }
//         }
//         nk_end(ctx);
//         _apply_normal_button_style(ctx);
//
//         switch (desc->popup) {
//         case POPUP_CONFIRM:
//         {
//             if (nk_begin(ctx, "Popup", nk_rect(300, 150, 
//                          desc->window.x - 600, desc->window.y - 520), 
//                          0))
//             {
//                 ctx->style.window.fixed_background = nk_style_item_color(COL_SECONDARY);
//                 if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Confirm", 
//                     NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR, 
//                     nk_rect(0, 0, desc->window.x - 600, desc->window.y - 520)))
//                 {
//                     nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 2);
//                     if (nk_button_label(ctx, "Cancel"))
//                         desc->popup = POPUP_NONE;
//
//                     if (nk_button_label(ctx, "Confirm"))
//                     {
//                         file_delete_world_dir(desc->selected_str);
//                         desc->popup = POPUP_NONE;
//                         memset(desc->selected_str, '\0', sizeof(desc->selected_str));
//                     }
//
//                     nk_popup_end(ctx);
//                 } else desc->popup = POPUP_NONE;
//                 ctx->style.window.fixed_background = nk_style_item_hide();
//             }
//             nk_end(ctx);
//
//             break;
//         }
//         case POPUP_EDIT_CONFIRM:
//         {
//             if (nk_begin(ctx, "Popup", nk_rect(300, 150, 
//                          desc->window.x - 600, desc->window.y - 450), 
//                          0))
//             {
//                 ctx->style.window.fixed_background = nk_style_item_color(COL_SECONDARY);
//                 if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Rename", 
//                     NK_WINDOW_BACKGROUND | NK_WINDOW_NO_SCROLLBAR, 
//                     nk_rect(0, 0, desc->window.x - 600, desc->window.y - 450)))
//                 {
//                     nk_layout_row_dynamic(ctx, EDIT_HEIGHT, 1);
//                     nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, desc->world_str, 
//                                                    MAX_WORLD_NAME_LEN, nk_filter_ascii);
//                     nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 2);
//
//                     if (nk_button_label(ctx, "Cancel"))
//                         desc->popup = POPUP_NONE;
//
//                     if (nk_button_label(ctx, "Rename"))
//                     {
//                         file_rename_world_dir(desc->selected_str, desc->world_str);
//                         strncpy(desc->selected_str, desc->world_str, MAX_WORLD_NAME_LEN);
//                         desc->popup = POPUP_NONE;
//                     }
//                     nk_popup_end(ctx);
//                 } else desc->popup = POPUP_NONE;
//                 ctx->style.window.fixed_background = nk_style_item_hide();
//             }
//             nk_end(ctx);
//
//             break;
//         }
//         default:
//             break;
//         };
//
//         break;
//     }
//     case GAME_PAUSE:
//     {
//         /*
//          * TODO:
//          *      - Resolution 
//          *
//          *      - LOD Settings 
//          *      - Toggle Fog 
//          *      - Sensitivity 
//          *      - FOV 
//          *      - Keybinds
//          */
//
//         if (nk_begin(ctx, "Left", nk_rect(MARGIN_AMOUNT, MARGIN_AMOUNT, 
//                      desc->window.x / 2.0 - MARGIN_AMOUNT, 
//                      desc->window.y - 2 * MARGIN_AMOUNT), 
//                      NK_WINDOW_NO_SCROLLBAR))
//         {
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (nk_button_label(ctx, "Resume"))
//                 desc->game_state = GAME_RUN;
//         }
//         nk_end(ctx);
//
//         if (nk_begin(ctx, "Right", 
//                      nk_rect(desc->window.x / 2.0 + MARGIN_AMOUNT, MARGIN_AMOUNT, 
//                      desc->window.x / 2.0 - 2 * MARGIN_AMOUNT,
//                      desc->window.y - 2 * MARGIN_AMOUNT), 
//                      0))
//         {
//             nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
//             if (nk_button_label(ctx, "Save"))
//             {
//                 save_cb();
//                 desc->game_state = GAME_RUN;
//             }
//         }
//         nk_end(ctx);
//         break;
//
//     }
//     case GAME_MENUOPT:
//     {
//         /*
//          * TODO:
//          *      - Resolution
//          *      - LOD Settings
//          *      - Toggle Fog 
//          */
//         break;
//     }
//     default:
//         break;
//     };
// }
