/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void cleanup_file_explorer(file_explorer_t *explorer) {
    if (!explorer)
        return;

    sfRectangleShape_destroy(explorer->overlay_bg);
    sfRectangleShape_destroy(explorer->explorer_bg);
    sfRectangleShape_destroy(explorer->path_bar);
    sfText_destroy(explorer->path_text);
    sfRectangleShape_destroy(explorer->filename_input_box);
    sfText_destroy(explorer->filename_input_text);
    sfText_destroy(explorer->cursor_indicator);
    sfRectangleShape_destroy(explorer->scrollbar);
    sfRectangleShape_destroy(explorer->scrollbar_thumb);
    sfRectangleShape_destroy(explorer->ok_button);
    sfText_destroy(explorer->ok_label);
    sfRectangleShape_destroy(explorer->cancel_button);
    sfText_destroy(explorer->cancel_label);
    sfText_destroy(explorer->title_text);

    // Cleanup cursor blink clock
    if (explorer->cursor_blink_clock)
        sfClock_destroy(explorer->cursor_blink_clock);

    // Format dropdown cleanup
    sfRectangleShape_destroy(explorer->format_button);
    sfText_destroy(explorer->format_button_label);
    sfRectangleShape_destroy(explorer->format_dropdown_bg);

    for (int i = 0; i < 5; i++) {
        sfRectangleShape_destroy(explorer->format_dropdown_buttons[i]);
        sfText_destroy(explorer->format_dropdown_labels[i]);
    }
    free(explorer->format_dropdown_buttons);
    free(explorer->format_dropdown_labels);

    for (int i = 0; i < 15; i++) {
        sfRectangleShape_destroy(explorer->file_buttons[i]);
        sfText_destroy(explorer->file_labels[i]);
    }

    free(explorer->file_buttons);
    free(explorer->file_labels);

    // Cleanup icon resources
    if (explorer->folder_icon_sprite)
        sfSprite_destroy(explorer->folder_icon_sprite);
    if (explorer->folder_icon_texture)
        sfTexture_destroy(explorer->folder_icon_texture);
    if (explorer->image_icon_sprite)
        sfSprite_destroy(explorer->image_icon_sprite);
    if (explorer->image_icon_texture)
        sfTexture_destroy(explorer->image_icon_texture);
    if (explorer->file_icon_sprite)
        sfSprite_destroy(explorer->file_icon_sprite);
    if (explorer->file_icon_texture)
        sfTexture_destroy(explorer->file_icon_texture);

    free(explorer);
}