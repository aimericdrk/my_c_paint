/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void handle_ui_click(app_t *app, sfVector2i mouse_pos) {
    // Check if file explorer is open - handle its clicks first (it's on top)
    if (app->ui->file_explorer && app->ui->file_explorer->mode != FILE_EXPLORER_CLOSED) {
        handle_file_explorer_click(app, mouse_pos);
        return;
    }

    // Check if clicking on File button
    sfVector2f file_pos = sfRectangleShape_getPosition(app->ui->file_button);
    sfVector2f file_size = sfRectangleShape_getSize(app->ui->file_button);

    if (is_point_in_rect(mouse_pos, file_pos, file_size)) {
        app->dropdown_open = !app->dropdown_open;
        app->options_dropdown_open = 0; // Close options menu
        printf("File menu %s\n", app->dropdown_open ? "opened" : "closed");
        return;
    }

    // Check if clicking on Options button
    sfVector2f options_pos = sfRectangleShape_getPosition(app->ui->options_button);
    sfVector2f options_size = sfRectangleShape_getSize(app->ui->options_button);

    if (is_point_in_rect(mouse_pos, options_pos, options_size)) {
        app->options_dropdown_open = !app->options_dropdown_open;
        app->dropdown_open = 0; // Close file menu
        printf("Options menu %s\n", app->options_dropdown_open ? "opened" : "closed");
        return;
    }

    // Check if clicking on file dropdown menu items
    if (app->dropdown_open) {
        for (int i = 0; i < app->ui->dropdown_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(app->ui->dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                const char *actions[] = {"New", "Open", "Save"};
                printf("%s clicked\n", actions[i]);

                if (i == 0) { // New
                    save_undo_state(app->paint);
                    clear_canvas(app->paint);
                    printf("New canvas created\n");
                } else if (i == 1) { // Open
                    if (app->ui->file_explorer)
                        open_file_explorer(app->ui->file_explorer, FILE_EXPLORER_OPEN);
                } else if (i == 2) { // Save
                    if (app->ui->file_explorer)
                        open_file_explorer(app->ui->file_explorer, FILE_EXPLORER_SAVE);
                }

                app->dropdown_open = 0;
                return;
            }
        }
    }

    // Check if clicking on options dropdown menu items
    if (app->options_dropdown_open) {
        for (int i = 0; i < app->ui->options_dropdown_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->options_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(app->ui->options_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                if (i == 0) { // Toggle Show Grid
                    app->paint->show_grid = !app->paint->show_grid;
                    printf("Grid %s\n", app->paint->show_grid ? "ON" : "OFF");
                } else if (i == 1) { // Toggle Snap to Grid
                    app->paint->snap_to_grid = !app->paint->snap_to_grid;
                    printf("Snap to grid %s\n", app->paint->snap_to_grid ? "ON" : "OFF");
                } else if (i == 2) { // Cycle Symmetry Mode
                    app->paint->symmetry_mode = (app->paint->symmetry_mode + 1) % 6;
                    const char *modes[] = {"None", "Horizontal", "Vertical", "Both", "Radial 4", "Radial 8"};
                    printf("Symmetry mode: %s\n", modes[app->paint->symmetry_mode]);
                } else if (i == 3) { // Toggle Show Debug
                    app->paint->show_debug = !app->paint->show_debug;
                    printf("Debug info %s\n", app->paint->show_debug ? "ON" : "OFF");
                } else if (i == 4) { // Flip Horizontal
                    save_undo_state(app->paint);
                    flip_canvas_horizontal(app->paint);
                } else if (i == 5) { // Flip Vertical
                    save_undo_state(app->paint);
                    flip_canvas_vertical(app->paint);
                }

                return; // Keep menu open for toggles
            }
        }
    }

    // Close dropdowns if clicking elsewhere
    if (app->dropdown_open || app->options_dropdown_open) {
        app->dropdown_open = 0;
        app->options_dropdown_open = 0;
        return;
    }

    // Check tool buttons
    for (int i = 0; i < app->ui->button_count; i++) {
        sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->buttons[i]);
        sfVector2f btn_size = sfRectangleShape_getSize(app->ui->buttons[i]);

        if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
            app->paint->current_tool = (tool_type_t)i;
            const char *tool_names[] = {"Pen", "Eraser", "Brush", "Spray", "Line", "Rectangle", "Circle", "Fill"};
            printf("🔧 Tool changed to: %s\n", tool_names[i]);
            return;
        }
    }

    // Check color sliders
    sfVector2f r_pos = sfRectangleShape_getPosition(app->ui->r_bar);
    sfVector2f slider_size = {200, 20};

    if (is_point_in_rect(mouse_pos, r_pos, slider_size)) {
        app->dragging_slider = 0;
        update_color_slider(app, mouse_pos);
        return;
    }

    sfVector2f g_pos = sfRectangleShape_getPosition(app->ui->g_bar);
    if (is_point_in_rect(mouse_pos, g_pos, slider_size)) {
        app->dragging_slider = 1;
        update_color_slider(app, mouse_pos);
        return;
    }

    sfVector2f b_pos = sfRectangleShape_getPosition(app->ui->b_bar);
    if (is_point_in_rect(mouse_pos, b_pos, slider_size)) {
        app->dragging_slider = 2;
        update_color_slider(app, mouse_pos);
        return;
    }

    sfVector2f a_pos = sfRectangleShape_getPosition(app->ui->a_bar);
    if (is_point_in_rect(mouse_pos, a_pos, slider_size)) {
        app->dragging_slider = 3;
        update_color_slider(app, mouse_pos);
        return;
    }

    // Check size slider
    sfVector2f size_pos = sfRectangleShape_getPosition(app->ui->size_bar);
    if (is_point_in_rect(mouse_pos, size_pos, slider_size)) {
        app->dragging_slider = 4;
        update_color_slider(app, mouse_pos);
        return;
    }
}