/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void handle_file_explorer_text(file_explorer_t *explorer, uint32_t unicode) {
    if (!explorer || explorer->mode != FILE_EXPLORER_SAVE)
        return;

    int base_len = strlen(explorer->base_filename);

    if (unicode == 8) // Backspace
    {
        if (explorer->input_cursor_pos > 0) {
            if (explorer->input_cursor_pos <= base_len) {
                // Remove character before cursor in base_filename
                memmove(&explorer->base_filename[explorer->input_cursor_pos - 1], &explorer->base_filename[explorer->input_cursor_pos], base_len - explorer->input_cursor_pos + 1);
                explorer->input_cursor_pos--;
            } else if (explorer->extension_editable) {
                // Remove character before cursor in extension
                int ext_pos = explorer->input_cursor_pos - base_len;
                int ext_len = strlen(explorer->extension);
                if (ext_pos > 0) {
                    memmove(&explorer->extension[ext_pos - 1], &explorer->extension[ext_pos], ext_len - ext_pos + 1);
                    explorer->input_cursor_pos--;
                }
            }
            // Update full filename
            snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        }
    } else if (unicode == 127) // Delete key
    {
        int total_len = strlen(explorer->filename_input);
        if (explorer->input_cursor_pos < total_len) {
            if (explorer->input_cursor_pos < base_len) {
                // Remove character at cursor position in base_filename
                memmove(&explorer->base_filename[explorer->input_cursor_pos], &explorer->base_filename[explorer->input_cursor_pos + 1], base_len - explorer->input_cursor_pos);
            } else if (explorer->extension_editable) {
                // Remove character at cursor position in extension
                int ext_pos = explorer->input_cursor_pos - base_len;
                int ext_len = strlen(explorer->extension);
                if (ext_pos < ext_len) {
                    memmove(&explorer->extension[ext_pos], &explorer->extension[ext_pos + 1], ext_len - ext_pos);
                }
            }
            // Update full filename
            snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        }
    } else if (unicode >= 32 && unicode < 127) // Printable characters
    {
        // Check if we're in personalised mode and cursor is in extension area
        if (explorer->extension_editable && explorer->input_cursor_pos >= base_len) {
            int ext_pos = explorer->input_cursor_pos - base_len;
            int ext_len = strlen(explorer->extension);
            if (ext_len < 15) {
                // Insert into extension
                memmove(&explorer->extension[ext_pos + 1], &explorer->extension[ext_pos], ext_len - ext_pos + 1);
                explorer->extension[ext_pos] = (char)unicode;
                explorer->input_cursor_pos++;
                snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
            }
        } else if (explorer->input_cursor_pos <= base_len) {
            // Insert into base filename
            if (base_len < MAX_FILENAME_LENGTH - 16) {
                memmove(&explorer->base_filename[explorer->input_cursor_pos + 1], &explorer->base_filename[explorer->input_cursor_pos], base_len - explorer->input_cursor_pos + 1);
                explorer->base_filename[explorer->input_cursor_pos] = (char)unicode;
                explorer->input_cursor_pos++;
                // Update full filename
                snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
            }
        }
    }
}
