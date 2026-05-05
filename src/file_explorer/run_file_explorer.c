/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void render_file_explorer(sfRenderWindow *window, file_explorer_t *explorer) {
    if (!explorer || explorer->mode == FILE_EXPLORER_CLOSED)
        return;

    // Draw overlay
    sfRenderWindow_drawRectangleShape(window, explorer->overlay_bg, NULL);

    // Draw main window
    sfRenderWindow_drawRectangleShape(window, explorer->explorer_bg, NULL);

    // Draw title
    sfRenderWindow_drawText(window, explorer->title_text, NULL);

    // Draw path bar
    sfRenderWindow_drawRectangleShape(window, explorer->path_bar, NULL);
    sfText_setString(explorer->path_text, explorer->current_path);
    sfRenderWindow_drawText(window, explorer->path_text, NULL);

    // Draw file list
    int visible_count = (explorer->entry_count - explorer->scroll_offset > 15) ? 15 : (explorer->entry_count - explorer->scroll_offset);

    for (int i = 0; i < visible_count; i++) {
        int entry_idx = i + explorer->scroll_offset;
        if (entry_idx >= explorer->entry_count)
            break;

        file_entry_t *entry = &explorer->entries[entry_idx];

        // Highlight selected entry
        if (entry_idx == explorer->selected_index)
            sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){80, 120, 160, 255});
        else
            sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){50, 50, 55, 255});

        sfRenderWindow_drawRectangleShape(window, explorer->file_buttons[i], NULL);

        // Draw icon
        sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->file_buttons[i]);
        sfSprite *icon_to_draw = NULL;

        if (entry->type == FILE_TYPE_DIRECTORY || entry->type == FILE_TYPE_PARENT)
            icon_to_draw = explorer->folder_icon_sprite;
        else if (entry->is_image)
            icon_to_draw = explorer->image_icon_sprite;
        else
            icon_to_draw = explorer->file_icon_sprite;

        if (icon_to_draw) {
            sfSprite_setPosition(icon_to_draw, (sfVector2f){btn_pos.x + 5, btn_pos.y + 5});
            sfRenderWindow_drawSprite(window, icon_to_draw, NULL);
        }

        // Draw filename (offset by icon width)
        sfText_setString(explorer->file_labels[i], entry->name);
        sfText_setPosition(explorer->file_labels[i], (sfVector2f){btn_pos.x + explorer->icon_size + 10, btn_pos.y + 5});
        sfRenderWindow_drawText(window, explorer->file_labels[i], NULL);
    }

    // Draw scrollbar if needed
    if (explorer->entry_count > 15) {
        sfRenderWindow_drawRectangleShape(window, explorer->scrollbar, NULL);

        // Calculate thumb position
        float thumb_ratio = 15.0f / explorer->entry_count;
        float thumb_height = thumb_ratio * 480.0f;
        if (thumb_height < 30)
            thumb_height = 30;

        float scroll_ratio = (float)explorer->scroll_offset / (explorer->entry_count - 15);
        float thumb_y = 256 + scroll_ratio * (480 - thumb_height);

        sfRectangleShape_setSize(explorer->scrollbar_thumb, (sfVector2f){13, thumb_height});
        sfRectangleShape_setPosition(explorer->scrollbar_thumb, (sfVector2f){1026, thumb_y});

        sfRenderWindow_drawRectangleShape(window, explorer->scrollbar_thumb, NULL);
    }

    // Draw filename input box (only in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE) {
        sfRenderWindow_drawRectangleShape(window, explorer->filename_input_box, NULL);
        sfText_setString(explorer->filename_input_text, explorer->filename_input);
        sfRenderWindow_drawText(window, explorer->filename_input_text, NULL);

        // Draw cursor - calculate position using findCharacterPos
        // Blink every 0.5 seconds (show/hide)
        sfTime elapsed = sfClock_getElapsedTime(explorer->cursor_blink_clock);
        float seconds = sfTime_asSeconds(elapsed);
        int show_cursor = ((int)(seconds * 2)) % 2 == 0; // Blink every 0.5s

        if (show_cursor) {
            sfText *temp_text = sfText_create(sfText_getFont(explorer->filename_input_text));
            sfText_setString(temp_text, explorer->filename_input);
            sfText_setCharacterSize(temp_text, 16);
            sfText_setPosition(temp_text, (sfVector2f){370, 678});

            sfVector2f cursor_pos = sfText_findCharacterPos(temp_text, explorer->input_cursor_pos);
            sfText_destroy(temp_text);

            sfText_setPosition(explorer->cursor_indicator, (sfVector2f){cursor_pos.x, 678});
            sfRenderWindow_drawText(window, explorer->cursor_indicator, NULL);
        }

        // Draw format button
        sfRenderWindow_drawRectangleShape(window, explorer->format_button, NULL);
        sfRenderWindow_drawText(window, explorer->format_button_label, NULL);

        // Draw format dropdown if open
        if (explorer->format_dropdown_open) {
            sfRenderWindow_drawRectangleShape(window, explorer->format_dropdown_bg, NULL);

            for (int i = 0; i < explorer->format_count; i++) {
                // Highlight selected format
                if (i == explorer->selected_format_index) {
                    sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){70, 120, 200, 255});
                } else {
                    sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){50, 50, 55, 255});
                }

                sfRenderWindow_drawRectangleShape(window, explorer->format_dropdown_buttons[i], NULL);
                sfRenderWindow_drawText(window, explorer->format_dropdown_labels[i], NULL);
            }
        }
    }

    // Draw buttons
    sfRenderWindow_drawRectangleShape(window, explorer->ok_button, NULL);
    sfRenderWindow_drawText(window, explorer->ok_label, NULL);
    sfRenderWindow_drawRectangleShape(window, explorer->cancel_button, NULL);
    sfRenderWindow_drawText(window, explorer->cancel_label, NULL);
}