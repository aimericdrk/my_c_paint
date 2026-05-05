/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void handle_file_explorer_click(app_t *app, sfVector2i mouse_pos) {
    file_explorer_t *explorer = app->ui->file_explorer;
    if (!explorer || explorer->mode == FILE_EXPLORER_CLOSED)
        return;

    // Check format dropdown clicks (if open and in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE && explorer->format_dropdown_open) {
        for (int i = 0; i < explorer->format_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->format_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(explorer->format_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                explorer->selected_format_index = i;

                if (i == 4) {
                    // "personalised" selected - enable extension editing
                    explorer->extension_editable = 1;
                    sfText_setString(explorer->format_button_label, "custom");
                    // Set cursor to end of full filename for editing
                    explorer->input_cursor_pos = strlen(explorer->filename_input);
                } else {
                    // Specific format selected
                    explorer->extension_editable = 0;
                    strcpy(explorer->extension, explorer->format_options[i]);
                    sfText_setString(explorer->format_button_label, explorer->extension);
                    snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
                    explorer->input_cursor_pos = strlen(explorer->base_filename);
                }

                explorer->format_dropdown_open = 0;
                return;
            }
        }

        // Click outside dropdown - close it
        explorer->format_dropdown_open = 0;
        return;
    }

    // Check format button click (in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE) {
        sfVector2f format_btn_pos = sfRectangleShape_getPosition(explorer->format_button);
        sfVector2f format_btn_size = sfRectangleShape_getSize(explorer->format_button);

        if (is_point_in_rect(mouse_pos, format_btn_pos, format_btn_size)) {
            explorer->format_dropdown_open = !explorer->format_dropdown_open;
            return;
        }
    }

    // Check Cancel button
    sfVector2f cancel_pos = sfRectangleShape_getPosition(explorer->cancel_button);
    sfVector2f cancel_size = sfRectangleShape_getSize(explorer->cancel_button);
    if (is_point_in_rect(mouse_pos, cancel_pos, cancel_size)) {
        close_file_explorer(explorer);
        return;
    }

    // Check OK button
    sfVector2f ok_pos = sfRectangleShape_getPosition(explorer->ok_button);
    sfVector2f ok_size = sfRectangleShape_getSize(explorer->ok_button);
    if (is_point_in_rect(mouse_pos, ok_pos, ok_size)) {
        if (explorer->mode == FILE_EXPLORER_SAVE) {
            // Save file
            char full_path[MAX_PATH_LENGTH + MAX_FILENAME_LENGTH];
            if (strcmp(explorer->current_path, "/") == 0)
                snprintf(full_path, sizeof(full_path), "/%s", explorer->filename_input);
            else
                snprintf(full_path, sizeof(full_path), "%s/%s", explorer->current_path, explorer->filename_input);

            save_to_file(app->paint, full_path);
            printf("Saved to: %s\n", full_path);
        } else if (explorer->mode == FILE_EXPLORER_OPEN) {
            // Open selected file
            if (explorer->selected_index >= 0 && explorer->selected_index < explorer->entry_count) {
                file_entry_t *entry = &explorer->entries[explorer->selected_index];
                if (entry->type == FILE_TYPE_FILE && entry->is_image) {
                    char full_path[MAX_PATH_LENGTH + MAX_FILENAME_LENGTH];
                    if (strcmp(explorer->current_path, "/") == 0)
                        snprintf(full_path, sizeof(full_path), "/%s", entry->name);
                    else
                        snprintf(full_path, sizeof(full_path), "%s/%s", explorer->current_path, entry->name);

                    load_from_file(app->paint, full_path);
                    printf("Opened: %s\n", full_path);
                }
            }
        }

        close_file_explorer(explorer);
        return;
    }

    // Check file list clicks
    for (int i = 0; i < 15; i++) {
        int entry_idx = i + explorer->scroll_offset;
        if (entry_idx >= explorer->entry_count)
            break;

        sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->file_buttons[i]);
        sfVector2f btn_size = sfRectangleShape_getSize(explorer->file_buttons[i]);

        if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
            file_entry_t *entry = &explorer->entries[entry_idx];

            if (entry->type == FILE_TYPE_DIRECTORY || entry->type == FILE_TYPE_PARENT) {
                // Navigate to directory
                navigate_to_directory(explorer, entry->name);
            } else {
                // Select file
                explorer->selected_index = entry_idx;

                // In save mode, populate filename input
                if (explorer->mode == FILE_EXPLORER_SAVE && entry->is_image) {
                    // Separate filename and extension
                    const char *dot = strrchr(entry->name, '.');
                    if (dot && !explorer->extension_editable) {
                        // Copy base filename (without extension)
                        int base_len = dot - entry->name;
                        strncpy(explorer->base_filename, entry->name, base_len);
                        explorer->base_filename[base_len] = '\0';

                        // Copy extension
                        strcpy(explorer->extension, dot);

                        // Update full filename
                        snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);

                        // Set cursor to end of base filename
                        explorer->input_cursor_pos = strlen(explorer->base_filename);

                        // Update format button label
                        sfText_setString(explorer->format_button_label, explorer->extension);
                    } else {
                        strcpy(explorer->filename_input, entry->name);
                        strcpy(explorer->base_filename, entry->name);
                        explorer->input_cursor_pos = strlen(explorer->filename_input);
                    }
                }
            }
            return;
        }
    }

    // Check scrollbar thumb click (only if there are more items than can be displayed)
    if (explorer->entry_count > 15) {
        sfVector2f thumb_pos = sfRectangleShape_getPosition(explorer->scrollbar_thumb);
        sfVector2f thumb_size = sfRectangleShape_getSize(explorer->scrollbar_thumb);

        if (is_point_in_rect(mouse_pos, thumb_pos, thumb_size)) {
            explorer->dragging_scrollbar = 1;
            return;
        }

        // Check scrollbar track click (jump to position)
        sfVector2f scrollbar_pos = sfRectangleShape_getPosition(explorer->scrollbar);
        sfVector2f scrollbar_size = sfRectangleShape_getSize(explorer->scrollbar);

        if (is_point_in_rect(mouse_pos, scrollbar_pos, scrollbar_size)) {
            // Calculate new scroll position based on click position
            float click_ratio = (mouse_pos.y - scrollbar_pos.y) / scrollbar_size.y;
            int max_scroll = explorer->entry_count - 15;
            explorer->scroll_offset = (int)(click_ratio * max_scroll);

            if (explorer->scroll_offset < 0)
                explorer->scroll_offset = 0;
            if (explorer->scroll_offset > max_scroll)
                explorer->scroll_offset = max_scroll;

            return;
        }
    }
}
