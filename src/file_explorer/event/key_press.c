/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void handle_file_explorer_keypress(file_explorer_t *explorer, sfKeyCode key) {
    if (!explorer || explorer->mode != FILE_EXPLORER_SAVE)
        return;

    int base_len = strlen(explorer->base_filename);
    int total_len = strlen(explorer->filename_input);
    int max_cursor = explorer->extension_editable ? total_len : base_len;

    switch (key) {
    case sfKeyLeft:
        if (explorer->input_cursor_pos > 0)
            explorer->input_cursor_pos--;
        break;

    case sfKeyRight:
        if (explorer->input_cursor_pos < max_cursor)
            explorer->input_cursor_pos++;
        break;

    case sfKeyHome:
        explorer->input_cursor_pos = 0;
        break;

    case sfKeyEnd:
        explorer->input_cursor_pos = max_cursor;
        break;

    case sfKeyDelete:
        handle_file_explorer_text(explorer, 127);
        break;

    default:
        break;
    }
}
