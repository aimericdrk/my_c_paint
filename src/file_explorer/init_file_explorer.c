/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

file_explorer_t *init_file_explorer(sfFont *font, ui_config_t *config) {
    file_explorer_t *explorer = malloc(sizeof(file_explorer_t));
    if (!explorer)
        return NULL;

    explorer->mode = FILE_EXPLORER_CLOSED;
    explorer->entry_count = 0;
    explorer->scroll_offset = 0;
    explorer->selected_index = -1;
    explorer->input_cursor_pos = 0;
    explorer->dragging_scrollbar = 0;
    strcpy(explorer->filename_input, "untitled.png");

    // Load icon configuration
    cJSON *fe_cfg = config ? config_get_section(config, "file_explorer.icons") : NULL;
    explorer->icon_size = config_get_int(fe_cfg, "icon_size", 20);
    const char *folder_icon_path = config_get_string(fe_cfg, "folder", "assets/icons/folder.png");
    const char *image_icon_path = config_get_string(fe_cfg, "image", "assets/icons/image.png");
    const char *file_icon_path = config_get_string(fe_cfg, "file", "assets/icons/file.png");

    // Load icon textures
    explorer->folder_icon_texture = sfTexture_createFromFile(folder_icon_path, NULL);
    explorer->image_icon_texture = sfTexture_createFromFile(image_icon_path, NULL);
    explorer->file_icon_texture = sfTexture_createFromFile(file_icon_path, NULL);

    // Create icon sprites
    if (explorer->folder_icon_texture) {
        explorer->folder_icon_sprite = sfSprite_create(explorer->folder_icon_texture);
        sfSprite_setScale(explorer->folder_icon_sprite, (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->folder_icon_texture).x,
                                                                     (float)explorer->icon_size / sfTexture_getSize(explorer->folder_icon_texture).y});
    } else {
        explorer->folder_icon_sprite = NULL;
    }

    if (explorer->image_icon_texture) {
        explorer->image_icon_sprite = sfSprite_create(explorer->image_icon_texture);
        sfSprite_setScale(explorer->image_icon_sprite,
                          (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->image_icon_texture).x, (float)explorer->icon_size / sfTexture_getSize(explorer->image_icon_texture).y});
    } else {
        explorer->image_icon_sprite = NULL;
    }

    if (explorer->file_icon_texture) {
        explorer->file_icon_sprite = sfSprite_create(explorer->file_icon_texture);
        sfSprite_setScale(explorer->file_icon_sprite,
                          (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->file_icon_texture).x, (float)explorer->icon_size / sfTexture_getSize(explorer->file_icon_texture).y});
    } else {
        explorer->file_icon_sprite = NULL;
    }

    // Get home directory
    const char *home = getenv("HOME");
    if (home)
        strncpy(explorer->current_path, home, MAX_PATH_LENGTH - 1);
    else
        strcpy(explorer->current_path, "/");
    explorer->current_path[MAX_PATH_LENGTH - 1] = '\0';

    // Create overlay background (semi-transparent)
    explorer->overlay_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->overlay_bg, (sfVector2f){1400, 920});
    sfRectangleShape_setPosition(explorer->overlay_bg, (sfVector2f){0, 0});
    sfRectangleShape_setFillColor(explorer->overlay_bg, (sfColor){0, 0, 0, 180});

    // Create main explorer window
    explorer->explorer_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->explorer_bg, (sfVector2f){700, 600});
    sfRectangleShape_setPosition(explorer->explorer_bg, (sfVector2f){350, 160});
    sfRectangleShape_setFillColor(explorer->explorer_bg, (sfColor){40, 40, 45, 255});
    sfRectangleShape_setOutlineColor(explorer->explorer_bg, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->explorer_bg, 2);

    // Title text
    explorer->title_text = sfText_create(font);
    sfText_setString(explorer->title_text, "Open File");
    sfText_setFont(explorer->title_text, font);
    sfText_setCharacterSize(explorer->title_text, 24);
    sfText_setFillColor(explorer->title_text, sfWhite);
    sfText_setPosition(explorer->title_text, (sfVector2f){370, 170});

    // Path bar
    explorer->path_bar = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->path_bar, (sfVector2f){680, 35});
    sfRectangleShape_setPosition(explorer->path_bar, (sfVector2f){360, 210});
    sfRectangleShape_setFillColor(explorer->path_bar, (sfColor){30, 30, 35, 255});
    sfRectangleShape_setOutlineColor(explorer->path_bar, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineThickness(explorer->path_bar, 1);

    explorer->path_text = sfText_create(font);
    sfText_setFont(explorer->path_text, font);
    sfText_setCharacterSize(explorer->path_text, 16);
    sfText_setFillColor(explorer->path_text, sfWhite);
    sfText_setPosition(explorer->path_text, (sfVector2f){370, 218});

    // File list area - create buttons for file entries
    explorer->file_buttons = malloc(sizeof(sfRectangleShape *) * 15); // Show 15 entries at a time
    explorer->file_labels = malloc(sizeof(sfText *) * 15);

    for (int i = 0; i < 15; i++) {
        explorer->file_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(explorer->file_buttons[i], (sfVector2f){650, 30});
        sfRectangleShape_setPosition(explorer->file_buttons[i], (sfVector2f){365, 255 + i * 32});
        sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){50, 50, 55, 255});

        explorer->file_labels[i] = sfText_create(font);
        sfText_setFont(explorer->file_labels[i], font);
        sfText_setCharacterSize(explorer->file_labels[i], 16);
        sfText_setFillColor(explorer->file_labels[i], sfWhite);
        sfText_setPosition(explorer->file_labels[i], (sfVector2f){375, 258 + i * 32});
    }

    // Scrollbar
    explorer->scrollbar = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->scrollbar, (sfVector2f){15, 480});
    sfRectangleShape_setPosition(explorer->scrollbar, (sfVector2f){1025, 255});
    sfRectangleShape_setFillColor(explorer->scrollbar, (sfColor){30, 30, 35, 255});

    explorer->scrollbar_thumb = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->scrollbar_thumb, (sfVector2f){13, 60});
    sfRectangleShape_setPosition(explorer->scrollbar_thumb, (sfVector2f){1026, 256});
    sfRectangleShape_setFillColor(explorer->scrollbar_thumb, (sfColor){100, 100, 105, 255});

    // Filename input box (for save mode)
    explorer->filename_input_box = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->filename_input_box, (sfVector2f){550, 35});
    sfRectangleShape_setPosition(explorer->filename_input_box, (sfVector2f){360, 670});
    sfRectangleShape_setFillColor(explorer->filename_input_box, (sfColor){30, 30, 35, 255});
    sfRectangleShape_setOutlineColor(explorer->filename_input_box, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineThickness(explorer->filename_input_box, 1);

    explorer->filename_input_text = sfText_create(font);
    sfText_setFont(explorer->filename_input_text, font);
    sfText_setCharacterSize(explorer->filename_input_text, 16);
    sfText_setFillColor(explorer->filename_input_text, sfWhite);
    sfText_setPosition(explorer->filename_input_text, (sfVector2f){370, 678});

    // Cursor indicator
    explorer->cursor_indicator = sfText_create(font);
    sfText_setString(explorer->cursor_indicator, "|");
    sfText_setFont(explorer->cursor_indicator, font);
    sfText_setCharacterSize(explorer->cursor_indicator, 16);
    sfText_setFillColor(explorer->cursor_indicator, sfWhite);

    // Initialize format options
    explorer->format_count = 5;
    strcpy(explorer->format_options[0], ".png");
    strcpy(explorer->format_options[1], ".jpg");
    strcpy(explorer->format_options[2], ".bmp");
    strcpy(explorer->format_options[3], ".tga");
    strcpy(explorer->format_options[4], "personalised");
    explorer->selected_format_index = 0;
    strcpy(explorer->extension, ".png");
    explorer->format_dropdown_open = 0;
    explorer->extension_editable = 0;
    explorer->base_filename[0] = '\0';

    // Format button
    explorer->format_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->format_button, (sfVector2f){120, 35});
    sfRectangleShape_setPosition(explorer->format_button, (sfVector2f){700, 715});
    sfRectangleShape_setFillColor(explorer->format_button, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineColor(explorer->format_button, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->format_button, 1);

    explorer->format_button_label = sfText_create(font);
    sfText_setString(explorer->format_button_label, ".png");
    sfText_setFont(explorer->format_button_label, font);
    sfText_setCharacterSize(explorer->format_button_label, 16);
    sfText_setFillColor(explorer->format_button_label, sfWhite);
    sfText_setPosition(explorer->format_button_label, (sfVector2f){735, 720});

    // Format dropdown background
    explorer->format_dropdown_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->format_dropdown_bg, (sfVector2f){120, 175});
    sfRectangleShape_setPosition(explorer->format_dropdown_bg, (sfVector2f){700, 540});
    sfRectangleShape_setFillColor(explorer->format_dropdown_bg, (sfColor){40, 40, 45, 255});
    sfRectangleShape_setOutlineColor(explorer->format_dropdown_bg, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->format_dropdown_bg, 2);

    // Format dropdown items
    explorer->format_dropdown_buttons = malloc(sizeof(sfRectangleShape *) * 5);
    explorer->format_dropdown_labels = malloc(sizeof(sfText *) * 5);

    for (int i = 0; i < 5; i++) {
        explorer->format_dropdown_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(explorer->format_dropdown_buttons[i], (sfVector2f){116, 33});
        sfRectangleShape_setPosition(explorer->format_dropdown_buttons[i], (sfVector2f){702, 542 + i * 35});
        sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){50, 50, 55, 255});

        explorer->format_dropdown_labels[i] = sfText_create(font);
        sfText_setString(explorer->format_dropdown_labels[i], explorer->format_options[i]);
        sfText_setFont(explorer->format_dropdown_labels[i], font);
        sfText_setCharacterSize(explorer->format_dropdown_labels[i], 14);
        sfText_setFillColor(explorer->format_dropdown_labels[i], sfWhite);
        sfText_setPosition(explorer->format_dropdown_labels[i], (sfVector2f){710, 548 + i * 35});
    }

    // OK button
    explorer->ok_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->ok_button, (sfVector2f){100, 35});
    sfRectangleShape_setPosition(explorer->ok_button, (sfVector2f){830, 715});
    sfRectangleShape_setFillColor(explorer->ok_button, (sfColor){70, 120, 200, 255});
    sfRectangleShape_setOutlineColor(explorer->ok_button, (sfColor){90, 140, 220, 255});
    sfRectangleShape_setOutlineThickness(explorer->ok_button, 1);

    explorer->ok_label = sfText_create(font);
    sfText_setString(explorer->ok_label, "OK");
    sfText_setFont(explorer->ok_label, font);
    sfText_setCharacterSize(explorer->ok_label, 18);
    sfText_setFillColor(explorer->ok_label, sfWhite);
    sfText_setPosition(explorer->ok_label, (sfVector2f){867, 720});

    // Cancel button
    explorer->cancel_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->cancel_button, (sfVector2f){100, 35});
    sfRectangleShape_setPosition(explorer->cancel_button, (sfVector2f){940, 715});
    sfRectangleShape_setFillColor(explorer->cancel_button, (sfColor){100, 100, 105, 255});
    sfRectangleShape_setOutlineColor(explorer->cancel_button, (sfColor){120, 120, 125, 255});
    sfRectangleShape_setOutlineThickness(explorer->cancel_button, 1);

    explorer->cancel_label = sfText_create(font);
    sfText_setString(explorer->cancel_label, "Cancel");
    sfText_setFont(explorer->cancel_label, font);
    sfText_setCharacterSize(explorer->cancel_label, 18);
    sfText_setFillColor(explorer->cancel_label, sfWhite);
    sfText_setPosition(explorer->cancel_label, (sfVector2f){960, 720});

    // Initialize cursor blink clock
    explorer->cursor_blink_clock = sfClock_create();

    return explorer;
}