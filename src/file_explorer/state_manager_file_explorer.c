/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void open_file_explorer(file_explorer_t *explorer, file_explorer_mode_t mode) {
    if (!explorer)
        return;

    explorer->mode = mode;
    explorer->selected_index = -1;
    explorer->scroll_offset = 0;
    explorer->format_dropdown_open = 0;
    explorer->extension_editable = 0;
    explorer->selected_format_index = 0;

    if (mode == FILE_EXPLORER_SAVE) {
        sfText_setString(explorer->title_text, "Save File");
        strcpy(explorer->base_filename, "untitled");
        strcpy(explorer->extension, ".png");
        snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        explorer->input_cursor_pos = strlen(explorer->base_filename);
        sfText_setString(explorer->format_button_label, explorer->extension);
    } else if (mode == FILE_EXPLORER_OPEN) {
        sfText_setString(explorer->title_text, "Open File");
        explorer->filename_input[0] = '\0';
        explorer->base_filename[0] = '\0';
        explorer->input_cursor_pos = 0;
    }

    refresh_file_list(explorer);
}

void close_file_explorer(file_explorer_t *explorer) {
    if (!explorer)
        return;

    explorer->mode = FILE_EXPLORER_CLOSED;
}