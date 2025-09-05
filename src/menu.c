#include "menu.h"

static const float PADDING_AMOUNT = 20;
static const float MARGIN_AMOUNT = 0;

void _apply_styles(struct nk_context *ctx)
{
    const struct nk_color COL_PRIMARY   = nk_rgb(92, 103, 132);
    const struct nk_color COL_SECONDARY = nk_rgb(76, 185, 99);
    const struct nk_color COL_TERTIARY  = nk_rgb(21, 127, 31);
    const struct nk_color COL_ACCENT    = nk_rgb(160, 234, 222);
    const struct nk_color COL_DARK      = nk_rgb(29, 38, 59);

    const struct nk_vec2 MARGIN         = nk_vec2(MARGIN_AMOUNT, MARGIN_AMOUNT);
    const struct nk_vec2 PADDING        = nk_vec2(PADDING_AMOUNT, PADDING_AMOUNT);

    ctx->style.window.padding = MARGIN;
    ctx->style.window.group_padding = MARGIN;
    ctx->style.window.spacing = PADDING;
    ctx->style.window.fixed_background = nk_style_item_hide();

    ctx->style.text.color = COL_ACCENT;
    // ctx->style.text.padding = PADDING;

    ctx->style.button.normal = nk_style_item_color(COL_PRIMARY);
    ctx->style.button.hover = nk_style_item_color(COL_PRIMARY);
    ctx->style.button.active = nk_style_item_color(COL_DARK);
    ctx->style.button.text_background = COL_ACCENT;
    ctx->style.button.text_normal = COL_ACCENT;
    ctx->style.button.text_hover = COL_ACCENT;
    ctx->style.button.text_active = COL_ACCENT;

    ctx->style.edit.normal = nk_style_item_color(COL_DARK);
    ctx->style.edit.hover = nk_style_item_color(COL_DARK);
    ctx->style.edit.active = nk_style_item_color(COL_DARK);
    ctx->style.edit.cursor_normal = COL_PRIMARY;
    ctx->style.edit.cursor_hover = COL_PRIMARY;
    ctx->style.edit.cursor_text_hover = COL_PRIMARY;
    ctx->style.edit.cursor_text_normal = COL_PRIMARY;
    ctx->style.edit.text_normal = COL_ACCENT;
    ctx->style.edit.text_hover = COL_ACCENT;
    ctx->style.edit.text_active = COL_ACCENT;

    ctx->style.scrollv.normal = nk_style_item_color(COL_PRIMARY);
    ctx->style.scrollv.hover = nk_style_item_color(COL_PRIMARY);
    ctx->style.scrollv.active = nk_style_item_color(COL_PRIMARY);

    ctx->style.scrollv.cursor_normal = nk_style_item_color(COL_DARK);
    ctx->style.scrollv.cursor_hover = nk_style_item_color(COL_DARK);
    ctx->style.scrollv.cursor_active = nk_style_item_color(COL_DARK);
}

extern void menu_draw(menu_desc_t *md, struct nk_context *ctx)
{
    nk_style_hide_cursor(ctx);

    _apply_styles(ctx);

    const float TOP_HEIGHT = 160;
    const float LABEL_HEIGHT = 20;
    const float EDIT_HEIGHT = 60;
    const float BUTTON_HEIGHT = 100;
    const float MARGIN = 50;

    switch (md->state) {
    case MENU_MAIN: 
    {
        memset(md->selected_name, '\0', sizeof(md->selected_name));

        if (nk_begin(ctx, "Top", nk_rect(MARGIN, MARGIN, 
                     md->win_width - 2 * MARGIN, 
                     TOP_HEIGHT),
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
            nk_label(ctx, "Minecraft Remake", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Left", nk_rect(MARGIN, MARGIN + TOP_HEIGHT, 
                     md->win_width / 2.0 - MARGIN, 
                     md->win_height - TOP_HEIGHT - 2 * MARGIN), 
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
            if (nk_button_label(ctx, "New World"))
                md->state = MENU_NEW;
            if (nk_button_label(ctx, "Load World"))
                md->state = MENU_LOAD;
            if (nk_button_label(ctx, "Options"))
                md->state = MENU_OPT;
            if (nk_button_label(ctx, "Quit"))
                exit(0);
        }
        nk_end(ctx);
        break;
    }
    case MENU_NEW:
    {
        memset(md->selected_name, '\0', sizeof(md->selected_name));

        if (nk_begin(ctx, "Top", nk_rect(MARGIN, MARGIN, 
                     md->win_width - 2 * MARGIN, 
                     TOP_HEIGHT),
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
            nk_label(ctx, "New World", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Left", nk_rect(MARGIN, MARGIN + TOP_HEIGHT, 
                     md->win_width / 2.0 - MARGIN, 
                     md->win_height - TOP_HEIGHT - 2 * MARGIN), 
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, EDIT_HEIGHT + LABEL_HEIGHT + PADDING_AMOUNT, 1);
            if (nk_group_begin(ctx, "Name Group", NK_WINDOW_NO_SCROLLBAR))
            {
                nk_layout_row_dynamic(ctx, LABEL_HEIGHT, 1);
                nk_label(ctx, "World Name:", NK_TEXT_ALIGN_LEFT);

                nk_layout_row_dynamic(ctx, EDIT_HEIGHT, 1);
                
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, md->name_buf, 
                                               24, nk_filter_ascii);
            }
            nk_group_end(ctx);

            nk_layout_row_dynamic(ctx, EDIT_HEIGHT + LABEL_HEIGHT + PADDING_AMOUNT, 1);
            if (nk_group_begin(ctx, "Seed Group", NK_WINDOW_NO_SCROLLBAR))
            {
                nk_layout_row_dynamic(ctx, LABEL_HEIGHT, 1);
                nk_label(ctx, "Seed:", NK_TEXT_ALIGN_LEFT);

                nk_layout_row_dynamic(ctx, EDIT_HEIGHT, 1);
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, md->seed_buf, 
                                               32, nk_filter_hex);
            }
            nk_group_end(ctx);

            nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
            if (nk_button_label(ctx, "Create"))
            {
                char *dir_name = file_make_world_dir(md->name_buf);
                if (dir_name)
                {
                    strcpy(md->selected_name, md->name_buf);
                    md->state = MENU_PLAY;
                }
                else 
                {
                    fprintf(stderr, "World with this name already exists.\n");
                }
            }

            nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
            if (nk_button_label(ctx, "Back"))
                md->state = MENU_MAIN;
        }
        nk_end(ctx);
        break;
    }
    case MENU_LOAD:
    {
        if (nk_begin(ctx, "Top", nk_rect(MARGIN, MARGIN, 
                     md->win_height - 2 * MARGIN, 
                     TOP_HEIGHT),
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, TOP_HEIGHT, 1);
            nk_label(ctx, "Load World", NK_TEXT_ALIGN_CENTERED);
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Left", nk_rect(MARGIN, MARGIN + TOP_HEIGHT, 
                     md->win_width / 2.0 - MARGIN, 
                     md->win_height - TOP_HEIGHT - 2 * MARGIN), 
                     NK_WINDOW_NO_SCROLLBAR))
        {
            nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
            if (md->selected_name[0] != '\0')
            {
                if (nk_button_label(ctx, "Play"))
                    md->state = MENU_PLAY;
                if (nk_button_label(ctx, "Rename"))
                    printf("Rename world %s\n", md->selected_name);
                if (nk_button_label(ctx, "Delete"))
                    printf("Delete world %s\n", md->selected_name);
            }
            else 
            {
                nk_spacing(ctx, 1);
                nk_spacing(ctx, 1);
                nk_spacing(ctx, 1);
            }

            if (nk_button_label(ctx, "Back"))
                md->state = MENU_MAIN;
        }
        nk_end(ctx);

        if (nk_begin(ctx, "Right", nk_rect(md->win_width / 2.0 + MARGIN, MARGIN + TOP_HEIGHT, 
                     md->win_width / 2.0 - 2 * MARGIN,
                     md->win_height - TOP_HEIGHT - 2 * MARGIN), 
                     0))
        {
            size_t cnt;
            char **worlds = file_get_filenames("data", &cnt);
            
            for (size_t i = 0; i < cnt; i++)
            {
                nk_layout_row_dynamic(ctx, BUTTON_HEIGHT, 1);
                if (nk_button_label(ctx, worlds[i]))
                    strncpy(md->selected_name, worlds[i], 24);
            }
        }
        nk_end(ctx);
        break;
    }
    case MENU_OPT:
    {
        /*
         * TODO:
         *      - Resolution
         *      - LOD Settings
         *      - Toggle Fog 
         */
        break;
    }
    default:
        break;
    };
}
