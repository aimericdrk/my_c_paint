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
                const char *actions[] = {"New / Clear", "Open", "Save"};
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

    // Check tab buttons
    for (int i = 0; i < app->ui->tab_count; i++) {
        sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->tab_buttons[i]);
        sfVector2f btn_size = sfRectangleShape_getSize(app->ui->tab_buttons[i]);

        if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
            app->active_tab = (toolbar_tab_t)i;
            const char *tab_names[] = {"Pen", "Color", "Layer", "IA"};
            printf("Switched to %s tab\n", tab_names[i]);
            return;
        }
    }

    // Check content based on active tab
    if (app->active_tab == TAB_PEN) {
        // Check tool buttons
        for (int i = 0; i < app->ui->button_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(app->ui->buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                app->paint->current_tool = (tool_type_t)i;
                const char *tool_names[] = {"Pen",  "Eraser",        "Brush",       "Fountain Pen", "Spray",    "Line",    "Rectangle", "Circle",
                                            "Fill", "Filled Circle", "Filled Rect", "Text",         "Gradient", "Polygon", "Star",      "Select"};
                printf("Tool changed to: %s\n", tool_names[i]);

                // If text tool is selected, activate text input
                if (app->paint->current_tool == TOOL_TEXT) {
                    app->paint->text_input_active = 1;
                    app->paint->text_buffer[0] = '\0';
                    printf("Text input activated\n");
                }
                // Reset polygon points when switching to polygon tool
                if (app->paint->current_tool == TOOL_POLYGON) {
                    app->paint->polygon_point_count = 0;
                    printf("Polygon tool: Click to add points, right-click to finish\n");
                }

                return;
            }
        }
    } else if (app->active_tab == TAB_COLOR) {
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

        // Check color wheel
        ui_config_t *config = app->config;
        cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
        cJSON *color_wheel_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "color_wheel") : NULL;
        int wheel_x = app->toolbar_x + config_get_int(color_wheel_cfg, "x", 75);
        int wheel_y = config_get_int(color_wheel_cfg, "y", 520);
        int wheel_radius = config_get_int(color_wheel_cfg, "radius", 100);

        // Calculate distance from wheel center
        float dx = mouse_pos.x - (wheel_x + wheel_radius);
        float dy = mouse_pos.y - (wheel_y + wheel_radius);
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance <= wheel_radius) {
            // Click is inside color wheel
            // Calculate hue from angle
            float angle = atan2f(dy, dx);
            float hue = (angle + M_PI) / (2.0f * M_PI); // Normalize to 0-1

            // Calculate saturation from distance
            float saturation = distance / wheel_radius;
            float value = 1.0f; // Full brightness

            // Convert HSV to RGB
            float c = value * saturation;
            float h_prime = hue * 6.0f;
            float x_val = c * (1.0f - fabsf(fmodf(h_prime, 2.0f) - 1.0f));
            float m = value - c;

            float r, g, b;
            if (h_prime < 1.0f) {
                r = c;
                g = x_val;
                b = 0;
            } else if (h_prime < 2.0f) {
                r = x_val;
                g = c;
                b = 0;
            } else if (h_prime < 3.0f) {
                r = 0;
                g = c;
                b = x_val;
            } else if (h_prime < 4.0f) {
                r = 0;
                g = x_val;
                b = c;
            } else if (h_prime < 5.0f) {
                r = x_val;
                g = 0;
                b = c;
            } else {
                r = c;
                g = 0;
                b = x_val;
            }

            app->paint->current_color.r = (uint8_t)((r + m) * 255);
            app->paint->current_color.g = (uint8_t)((g + m) * 255);
            app->paint->current_color.b = (uint8_t)((b + m) * 255);
            // Keep current alpha value

            printf("Color picked from wheel: R=%d G=%d B=%d\n", app->paint->current_color.r, app->paint->current_color.g, app->paint->current_color.b);
            return;
        }
    } else if (app->active_tab == TAB_LAYER) {
        // Get layer config
        ui_config_t *config = app->config;
        cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
        cJSON *layers_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "layers") : NULL;
        int layer_panel_x = app->toolbar_x + config_get_int(layers_cfg, "panel_x", 15);
        int layer_panel_y = config_get_int(layers_cfg, "panel_y", 115);
        int layer_item_height = config_get_int(layers_cfg, "item_height", 50);
        int layer_item_spacing = config_get_int(layers_cfg, "item_spacing", 5);
        int max_visible = config_get_int(layers_cfg, "max_visible_items", 10);
        int layer_panel_width = config_get_int(layers_cfg, "panel_width", 270);

        // Check control buttons
        sfVector2f new_pos = sfRectangleShape_getPosition(app->ui->layer_new_button);
        sfVector2f new_size = sfRectangleShape_getSize(app->ui->layer_new_button);
        if (is_point_in_rect(mouse_pos, new_pos, new_size)) {
            save_undo_state(app->paint);
            create_layer(app->paint, NULL);
            composite_layers(app->paint);
            printf("New layer created\n");
            return;
        }

        sfVector2f del_pos = sfRectangleShape_getPosition(app->ui->layer_delete_button);
        sfVector2f del_size = sfRectangleShape_getSize(app->ui->layer_delete_button);
        if (is_point_in_rect(mouse_pos, del_pos, del_size)) {
            if (app->paint->layer_count > 1) {
                save_undo_state(app->paint);
                delete_layer(app->paint, app->paint->current_layer);
            }
            return;
        }

        sfVector2f merge_pos = sfRectangleShape_getPosition(app->ui->layer_merge_button);
        sfVector2f merge_size = sfRectangleShape_getSize(app->ui->layer_merge_button);
        if (is_point_in_rect(mouse_pos, merge_pos, merge_size)) {
            if (app->paint->current_layer > 0) {
                save_undo_state(app->paint);
                merge_layer_down(app->paint, app->paint->current_layer);
            }
            return;
        }

        sfVector2f up_pos = sfRectangleShape_getPosition(app->ui->layer_up_button);
        sfVector2f up_size = sfRectangleShape_getSize(app->ui->layer_up_button);
        if (is_point_in_rect(mouse_pos, up_pos, up_size)) {
            move_layer_up(app->paint, app->paint->current_layer);
            return;
        }

        sfVector2f down_pos = sfRectangleShape_getPosition(app->ui->layer_down_button);
        sfVector2f down_size = sfRectangleShape_getSize(app->ui->layer_down_button);
        if (is_point_in_rect(mouse_pos, down_pos, down_size)) {
            move_layer_down(app->paint, app->paint->current_layer);
            return;
        }

        // Check layer items
        int visible_count = app->paint->layer_count < max_visible ? app->paint->layer_count : max_visible;
        for (int i = 0; i < visible_count; i++) {
            int layer_idx = i + app->ui->layer_panel_scroll_offset;
            if (layer_idx >= app->paint->layer_count)
                break;

            sfVector2f item_pos = {layer_panel_x, layer_panel_y + i * (layer_item_height + layer_item_spacing)};
            sfVector2f item_size = {layer_panel_width, layer_item_height};

            if (is_point_in_rect(mouse_pos, item_pos, item_size)) {
                // Check if clicking visibility button
                sfVector2f vis_pos = {layer_panel_x + 10, layer_panel_y + i * (layer_item_height + layer_item_spacing) + 5};
                sfVector2f vis_size = {20, 20};

                if (is_point_in_rect(mouse_pos, vis_pos, vis_size)) {
                    toggle_layer_visibility(app->paint, layer_idx);
                    return;
                }

                // Check if clicking opacity slider
                sfVector2f opacity_bar_pos = {layer_panel_x + 40, layer_panel_y + i * (layer_item_height + layer_item_spacing) + 30};
                sfVector2f opacity_bar_size = {180, 10};

                if (is_point_in_rect(mouse_pos, opacity_bar_pos, opacity_bar_size)) {
                    app->ui->dragging_layer_opacity = layer_idx;
                    // Update opacity immediately
                    float percent = (mouse_pos.x - opacity_bar_pos.x) / opacity_bar_size.x;
                    if (percent < 0)
                        percent = 0;
                    if (percent > 1)
                        percent = 1;
                    uint8_t opacity = (uint8_t)(percent * 255);
                    set_layer_opacity(app->paint, layer_idx, opacity);
                    return;
                }

                // Otherwise select the layer
                app->paint->current_layer = layer_idx;
                printf("Selected layer: %s\n", app->paint->layers[layer_idx].name);
                return;
            }
        }

        // Check scrollbar
        if (app->paint->layer_count > max_visible) {
            int scrollbar_x = layer_panel_x + 275;
            int scrollbar_height = max_visible * (layer_item_height + layer_item_spacing);
            sfVector2f scrollbar_pos = {scrollbar_x, layer_panel_y};
            sfVector2f scrollbar_size = {10, scrollbar_height};

            if (is_point_in_rect(mouse_pos, scrollbar_pos, scrollbar_size)) {
                app->dragging_layer_scrollbar = 1;
                // Calculate scroll offset from click position
                float percent = (mouse_pos.y - layer_panel_y) / (float)scrollbar_height;
                app->ui->layer_panel_scroll_offset = (int)(percent * (app->paint->layer_count - max_visible));
                if (app->ui->layer_panel_scroll_offset < 0)
                    app->ui->layer_panel_scroll_offset = 0;
                if (app->ui->layer_panel_scroll_offset > app->paint->layer_count - max_visible)
                    app->ui->layer_panel_scroll_offset = app->paint->layer_count - max_visible;
                return;
            }
        }
    } else if (app->active_tab == TAB_IA) {
        // Check AI Chat send button
        sfVector2f send_pos = sfRectangleShape_getPosition(app->ui->ai_send_button);
        sfVector2f send_size = sfRectangleShape_getSize(app->ui->ai_send_button);

        if (is_point_in_rect(mouse_pos, send_pos, send_size)) {
            if (strlen(app->ui->ai_input_buffer) > 0) {
                // Add user message to chat
                add_ai_message(app->ai_chat, MESSAGE_USER, app->ui->ai_input_buffer);

                // Send request to AI
                send_ai_request(app, app->ui->ai_input_buffer);

                // Clear input
                app->ui->ai_input_buffer[0] = '\0';
                app->ui->ai_input_cursor = 0;
            }
            return;
        }

        // Check AI Chat clear button
        sfVector2f clear_pos = sfRectangleShape_getPosition(app->ui->ai_clear_button);
        sfVector2f clear_size = sfRectangleShape_getSize(app->ui->ai_clear_button);

        if (is_point_in_rect(mouse_pos, clear_pos, clear_size)) {
            clear_ai_chat(app->ai_chat);
            app->ui->ai_chat_scroll_offset = 0;
            return;
        }

        // Check AI input box click (to enable typing)
        sfVector2f input_pos = sfRectangleShape_getPosition(app->ui->ai_input_box);
        sfVector2f input_size = sfRectangleShape_getSize(app->ui->ai_input_box);

        if (is_point_in_rect(mouse_pos, input_pos, input_size)) {
            app->ui->ai_is_typing = 1;
            return;
        }
    }
}