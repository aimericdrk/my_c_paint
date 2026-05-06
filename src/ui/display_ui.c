/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void render_ui(sfRenderWindow *window, app_t *app) {
    ui_elements_t *ui = app->ui;
    paint_state_t *paint = app->paint;

    // Draw background
    sfRenderWindow_drawRectangleShape(window, ui->bg, NULL);

    // Get tab configuration
    ui_config_t *config = app->config;
    cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
    cJSON *tabs_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "tabs") : NULL;
    sfColor tab_btn_color = config_get_color(tabs_cfg, "button_color", (sfColor){60, 60, 65, 255});
    sfColor tab_btn_selected = config_get_color(tabs_cfg, "button_selected_color", (sfColor){100, 150, 200, 255});

    // Draw tab buttons
    for (int i = 0; i < ui->tab_count; i++) {
        // Highlight selected tab
        if (i == (int)app->active_tab) {
            sfRectangleShape_setFillColor(ui->tab_buttons[i], tab_btn_selected);
        } else {
            sfRectangleShape_setFillColor(ui->tab_buttons[i], tab_btn_color);
        }

        sfRenderWindow_drawRectangleShape(window, ui->tab_buttons[i], NULL);
        sfRenderWindow_drawText(window, ui->tab_labels[i], NULL);
    }

    // Draw content based on active tab
    if (app->active_tab == TAB_PEN) {
        // Draw tool buttons
        for (int i = 0; i < ui->button_count; i++) {
            // Highlight selected tool
            if (i == (int)paint->current_tool) {
                sfRectangleShape_setFillColor(ui->buttons[i], (sfColor){100, 150, 200, 255});
            } else {
                sfRectangleShape_setFillColor(ui->buttons[i], (sfColor){70, 70, 75, 255});
            }

            sfRenderWindow_drawRectangleShape(window, ui->buttons[i], NULL);
            sfRenderWindow_drawCircleShape(window, ui->tool_icons[i], NULL);
            sfRenderWindow_drawText(window, ui->labels[i], NULL);
        }

        // Display text input buffer if text tool is active
        if (paint->text_input_active && paint->current_tool == TOOL_TEXT) {
            // Draw text input box
            sfRectangleShape *text_box = sfRectangleShape_create();
            sfRectangleShape_setPosition(text_box, (sfVector2f){app->toolbar_x + 15, 600});
            sfRectangleShape_setSize(text_box, (sfVector2f){270, 80});
            sfRectangleShape_setFillColor(text_box, (sfColor){30, 30, 35, 255});
            sfRectangleShape_setOutlineColor(text_box, (sfColor){100, 150, 200, 255});
            sfRectangleShape_setOutlineThickness(text_box, 2);
            sfRenderWindow_drawRectangleShape(window, text_box, NULL);

            // Draw label
            sfText *label = sfText_create(ui->font);
            sfText_setString(label, "Text Input:");
            sfText_setCharacterSize(label, 14);
            sfText_setFillColor(label, (sfColor){200, 200, 200, 255});
            sfText_setPosition(label, (sfVector2f){app->toolbar_x + 25, 610});
            sfRenderWindow_drawText(window, label, NULL);

            // Draw text buffer
            sfText *text = sfText_create(ui->font);
            sfText_setString(text, strlen(paint->text_buffer) > 0 ? paint->text_buffer : "(type text...)");
            sfText_setCharacterSize(text, 16);
            sfText_setFillColor(text, strlen(paint->text_buffer) > 0 ? (sfColor){255, 255, 255, 255} : (sfColor){150, 150, 150, 255});
            sfText_setPosition(text, (sfVector2f){app->toolbar_x + 25, 640});
            sfRenderWindow_drawText(window, text, NULL);

            sfText_destroy(label);
            sfText_destroy(text);
            sfRectangleShape_destroy(text_box);
        }
    } else if (app->active_tab == TAB_COLOR) {
        // Draw color preview
        sfRectangleShape_setFillColor(ui->color_preview, paint->current_color);
        sfRenderWindow_drawRectangleShape(window, ui->color_preview, NULL);

        // Draw color sliders
        sfRenderWindow_drawRectangleShape(window, ui->r_bar, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->g_bar, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->b_bar, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->a_bar, NULL);

        // Update slider positions based on color - get config sections
        cJSON *sliders_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "sliders") : NULL;
        cJSON *r_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "r_slider") : NULL;
        cJSON *g_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "g_slider") : NULL;
        cJSON *b_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "b_slider") : NULL;
        cJSON *a_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "a_slider") : NULL;
        cJSON *size_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "size_slider") : NULL;

        int slider_base_x = app->toolbar_x + config_get_int(sliders_cfg, "x", 25);
        int slider_range = config_get_int(sliders_cfg, "slider_range", 190);
        int r_slider_y = config_get_int(r_slider_cfg, "y", 185);
        int g_slider_y = config_get_int(g_slider_cfg, "y", 235);
        int b_slider_y = config_get_int(b_slider_cfg, "y", 285);
        int a_slider_y = config_get_int(a_slider_cfg, "y", 335);
        int size_slider_y = config_get_int(size_slider_cfg, "y", 420);
        int handle_y_offset = config_get_int(r_slider_cfg, "handle_y_offset", -5);

        sfRectangleShape_setPosition(ui->r_slider, (sfVector2f){slider_base_x + (paint->current_color.r / 255.0f) * slider_range, r_slider_y + handle_y_offset});
        sfRectangleShape_setPosition(ui->g_slider, (sfVector2f){slider_base_x + (paint->current_color.g / 255.0f) * slider_range, g_slider_y + handle_y_offset});
        sfRectangleShape_setPosition(ui->b_slider, (sfVector2f){slider_base_x + (paint->current_color.b / 255.0f) * slider_range, b_slider_y + handle_y_offset});
        sfRectangleShape_setPosition(ui->a_slider, (sfVector2f){slider_base_x + (paint->current_color.a / 255.0f) * slider_range, a_slider_y + handle_y_offset});

        sfRenderWindow_drawRectangleShape(window, ui->r_slider, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->g_slider, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->b_slider, NULL);
        sfRenderWindow_drawRectangleShape(window, ui->a_slider, NULL);

        // Draw size slider
        sfRenderWindow_drawRectangleShape(window, ui->size_bar, NULL);

        // Update size slider position
        int size_min = config_get_int(size_slider_cfg, "min", 1);
        int size_max = config_get_int(size_slider_cfg, "max", 50);
        float size_percent = (paint->brush_size - size_min) / (float)(size_max - size_min);
        sfRectangleShape_setPosition(ui->size_slider, (sfVector2f){slider_base_x + size_percent * slider_range, size_slider_y + handle_y_offset});
        sfRenderWindow_drawRectangleShape(window, ui->size_slider, NULL);

        // Draw labels from config
        cJSON *color_preview_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "color_preview") : NULL;

        sfText *text = sfText_create(ui->font);
        int label_font_size = config_get_int(color_preview_cfg, "label_font_size", 16);
        sfText_setCharacterSize(text, label_font_size);
        sfText_setFillColor(text, sfWhite);

        // Color label
        const char *color_label = config_get_string(color_preview_cfg, "label_text", "Color:");
        int color_label_x = config_get_int(color_preview_cfg, "label_x", 15);
        int color_label_y = config_get_int(color_preview_cfg, "label_y", 115);
        sfText_setString(text, color_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + color_label_x, color_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // R label
        const char *r_label = config_get_string(r_slider_cfg, "label", "R");
        int r_label_x = config_get_int(r_slider_cfg, "label_x", 230);
        int r_label_y = config_get_int(r_slider_cfg, "label_y", 185);
        sfText_setString(text, r_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + r_label_x, r_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // G label
        const char *g_label = config_get_string(g_slider_cfg, "label", "G");
        int g_label_x = config_get_int(g_slider_cfg, "label_x", 230);
        int g_label_y = config_get_int(g_slider_cfg, "label_y", 235);
        sfText_setString(text, g_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + g_label_x, g_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // B label
        const char *b_label = config_get_string(b_slider_cfg, "label", "B");
        int b_label_x = config_get_int(b_slider_cfg, "label_x", 230);
        int b_label_y = config_get_int(b_slider_cfg, "label_y", 285);
        sfText_setString(text, b_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + b_label_x, b_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // A label
        const char *a_label = config_get_string(a_slider_cfg, "label", "A");
        int a_label_x = config_get_int(a_slider_cfg, "label_x", 230);
        int a_label_y = config_get_int(a_slider_cfg, "label_y", 335);
        sfText_setString(text, a_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + a_label_x, a_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // Size label
        const char *size_label = config_get_string(size_slider_cfg, "label", "Size:");
        int size_label_x = config_get_int(size_slider_cfg, "label_x", 15);
        int size_label_y = config_get_int(size_slider_cfg, "label_y", 390);
        sfText_setString(text, size_label);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + size_label_x, size_label_y});
        sfRenderWindow_drawText(window, text, NULL);

        // Size value label
        char size_str[32];
        sprintf(size_str, "%d px", paint->brush_size);
        int size_value_x = config_get_int(size_slider_cfg, "value_label_x", 70);
        int size_value_y = config_get_int(size_slider_cfg, "value_label_y", 390);
        sfText_setString(text, size_str);
        sfText_setPosition(text, (sfVector2f){app->toolbar_x + size_value_x, size_value_y});
        sfRenderWindow_drawText(window, text, NULL);

        // Draw color wheel
        cJSON *color_wheel_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "color_wheel") : NULL;
        int wheel_x = config_get_int(color_wheel_cfg, "x", 75);
        int wheel_y = config_get_int(color_wheel_cfg, "y", 520);

        sfCircleShape_setPosition(ui->color_wheel, (sfVector2f){app->toolbar_x + wheel_x, wheel_y});
        sfRenderWindow_drawCircleShape(window, ui->color_wheel, NULL);

        // Draw color wheel cursor (optional - shows current color position)
        // Note: This would require calculating the HSV position from current RGB color
        // For now, we'll skip it or implement a simple version

        sfText_destroy(text);
    } else if (app->active_tab == TAB_LAYER) {
        // Draw layer panel
        cJSON *layers_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "layers") : NULL;
        int layer_panel_x = app->toolbar_x + config_get_int(layers_cfg, "panel_x", 15);
        int layer_panel_y = config_get_int(layers_cfg, "panel_y", 115);
        int layer_item_height = config_get_int(layers_cfg, "item_height", 50);
        int layer_item_spacing = config_get_int(layers_cfg, "item_spacing", 5);
        int max_visible = config_get_int(layers_cfg, "max_visible_items", 10);
        sfColor selected_color = config_get_color(layers_cfg, "item_selected_color", (sfColor){100, 150, 200, 255});
        sfColor item_color = config_get_color(layers_cfg, "item_color", (sfColor){60, 60, 65, 255});

        int visible_count = paint->layer_count < max_visible ? paint->layer_count : max_visible;

        // Draw visible layers
        for (int i = 0; i < visible_count; i++) {
            int layer_idx = i + ui->layer_panel_scroll_offset;
            if (layer_idx >= paint->layer_count)
                break;

            layer_t *layer = &paint->layers[layer_idx];

            // Update position
            sfRectangleShape_setPosition(ui->layer_items[i], (sfVector2f){layer_panel_x, layer_panel_y + i * (layer_item_height + layer_item_spacing)});

            // Highlight selected layer
            if (layer_idx == paint->current_layer) {
                sfRectangleShape_setFillColor(ui->layer_items[i], selected_color);
            } else {
                sfRectangleShape_setFillColor(ui->layer_items[i], item_color);
            }

            // Draw layer item
            sfRenderWindow_drawRectangleShape(window, ui->layer_items[i], NULL);

            // Draw layer name
            sfText_setString(ui->layer_names[i], layer->name);
            sfText_setPosition(ui->layer_names[i], (sfVector2f){layer_panel_x + 40, layer_panel_y + i * (layer_item_height + layer_item_spacing) + 5});
            sfRenderWindow_drawText(window, ui->layer_names[i], NULL);

            // Draw visibility button
            sfVector2f vis_pos = {layer_panel_x + 10, layer_panel_y + i * (layer_item_height + layer_item_spacing) + 5};
            sfRectangleShape_setPosition(ui->layer_visibility_buttons[i], vis_pos);
            if (layer->visible) {
                sfRectangleShape_setFillColor(ui->layer_visibility_buttons[i], (sfColor){100, 200, 100, 255});
            } else {
                sfRectangleShape_setFillColor(ui->layer_visibility_buttons[i], (sfColor){200, 100, 100, 255});
            }
            sfRenderWindow_drawRectangleShape(window, ui->layer_visibility_buttons[i], NULL);

            // Draw opacity bar and slider
            sfVector2f opacity_bar_pos = {layer_panel_x + 40, layer_panel_y + i * (layer_item_height + layer_item_spacing) + 30};
            sfRectangleShape_setPosition(ui->layer_opacity_bars[i], opacity_bar_pos);
            sfRenderWindow_drawRectangleShape(window, ui->layer_opacity_bars[i], NULL);

            // Position opacity slider
            float opacity_percent = layer->opacity / 255.0f;
            sfVector2f slider_pos = {opacity_bar_pos.x + opacity_percent * 172, opacity_bar_pos.y - 2};
            sfRectangleShape_setPosition(ui->layer_opacity_sliders[i], slider_pos);
            sfRenderWindow_drawRectangleShape(window, ui->layer_opacity_sliders[i], NULL);

            // Draw opacity value
            char opacity_str[16];
            snprintf(opacity_str, sizeof(opacity_str), "%d%%", (int)(opacity_percent * 100));
            sfText *opacity_text = sfText_create(ui->font);
            sfText_setString(opacity_text, opacity_str);
            sfText_setCharacterSize(opacity_text, 10);
            sfText_setFillColor(opacity_text, sfWhite);
            sfText_setPosition(opacity_text, (sfVector2f){layer_panel_x + 225, opacity_bar_pos.y - 2});
            sfRenderWindow_drawText(window, opacity_text, NULL);
            sfText_destroy(opacity_text);
        }

        // Draw scrollbar if needed
        if (paint->layer_count > max_visible) {
            int scrollbar_x = layer_panel_x + 275;
            int scrollbar_height = max_visible * (layer_item_height + layer_item_spacing);
            sfRectangleShape_setSize(ui->layer_scrollbar, (sfVector2f){10, scrollbar_height});
            sfRectangleShape_setPosition(ui->layer_scrollbar, (sfVector2f){scrollbar_x, layer_panel_y});
            sfRenderWindow_drawRectangleShape(window, ui->layer_scrollbar, NULL);

            // Calculate thumb size and position
            float thumb_height = (float)max_visible / paint->layer_count * scrollbar_height;
            float thumb_y = layer_panel_y + ((float)ui->layer_panel_scroll_offset / paint->layer_count * scrollbar_height);
            sfRectangleShape_setSize(ui->layer_scrollbar_thumb, (sfVector2f){10, thumb_height});
            sfRectangleShape_setPosition(ui->layer_scrollbar_thumb, (sfVector2f){scrollbar_x, thumb_y});
            sfRenderWindow_drawRectangleShape(window, ui->layer_scrollbar_thumb, NULL);
        }

        // Draw control buttons
        sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(window);
        sfColor button_hover = config_get_color(layers_cfg, "button_hover_color", (sfColor){100, 150, 200, 255});
        sfColor button_normal = config_get_color(layers_cfg, "button_color", (sfColor){70, 70, 75, 255});

        sfRectangleShape **buttons[] = {&ui->layer_new_button, &ui->layer_delete_button, &ui->layer_merge_button, &ui->layer_up_button, &ui->layer_down_button};
        sfText **labels[] = {&ui->layer_new_label, &ui->layer_delete_label, &ui->layer_merge_label, &ui->layer_up_label, &ui->layer_down_label};

        for (int i = 0; i < 5; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(*buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(*buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                sfRectangleShape_setFillColor(*buttons[i], button_hover);
            } else {
                sfRectangleShape_setFillColor(*buttons[i], button_normal);
            }

            sfRenderWindow_drawRectangleShape(window, *buttons[i], NULL);
            sfRenderWindow_drawText(window, *labels[i], NULL);
        }
    } else if (app->active_tab == TAB_IA) {
        // Draw AI Chat panel
        cJSON *ai_chat_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "ai_chat") : NULL;
        int ai_panel_x = app->toolbar_x + config_get_int(ai_chat_cfg, "panel_x", 15);
        int ai_panel_y = config_get_int(ai_chat_cfg, "panel_y", 115);
        int ai_chat_area_height = config_get_int(ai_chat_cfg, "chat_area_height", 420);
        int ai_message_padding = config_get_int(ai_chat_cfg, "message_padding", 10);
        int ai_message_spacing = config_get_int(ai_chat_cfg, "message_spacing", 5);
        int ai_font_size = config_get_int(ai_chat_cfg, "font_size", 12);

        // Draw chat background
        sfRenderWindow_drawRectangleShape(window, ui->ai_chat_bg, NULL);

        // Draw messages with word wrapping
        if (app->ai_chat && app->ai_chat->message_count > 0) {
            sfText *msg_text = sfText_create(ui->font);
            sfText_setCharacterSize(msg_text, ai_font_size);

            int y_offset = ai_panel_y + ai_message_padding - (ui->ai_chat_scroll_offset * 60);
            int panel_width = config_get_int(ai_chat_cfg, "panel_width", 270);
            float max_text_width = panel_width - (2 * ai_message_padding);
            float line_height = ai_font_size + 4;

            for (int i = 0; i < app->ai_chat->message_count && i < MAX_AI_MESSAGES; i++) {
                ai_message_t *msg = &app->ai_chat->messages[i];

                // Set color based on message type
                if (msg->type == MESSAGE_USER) {
                    sfText_setFillColor(msg_text, (sfColor){100, 180, 255, 255});
                } else {
                    sfText_setFillColor(msg_text, (sfColor){200, 200, 200, 255});
                }

                // Word wrap the message content
                char line_buffer[MAX_AI_MESSAGE_LENGTH];
                const char *text = msg->content;
                int text_len = strlen(text);
                int start = 0;
                int message_line_count = 0;

                while (start < text_len) {
                    int end = start;
                    int last_space = -1;

                    // Find how much text fits on this line
                    while (end < text_len) {
                        if (text[end] == ' ' || text[end] == '\n') {
                            last_space = end;
                        }
                        if (text[end] == '\n') {
                            end++;
                            break;
                        }

                        // Copy substring to test width
                        int len = end - start + 1;
                        if (len >= MAX_AI_MESSAGE_LENGTH)
                            len = MAX_AI_MESSAGE_LENGTH - 1;
                        strncpy(line_buffer, text + start, len);
                        line_buffer[len] = '\0';

                        sfText_setString(msg_text, line_buffer);
                        sfFloatRect bounds = sfText_getGlobalBounds(msg_text);

                        if (bounds.size.x > max_text_width) {
                            // Line is too long, break at last space
                            if (last_space > start) {
                                end = last_space;
                            } else if (end > start) {
                                // No space found, break at current position
                                end--;
                            }
                            break;
                        }
                        end++;
                    }

                    // Draw this line
                    if (end > start) {
                        int len = end - start;
                        if (text[end - 1] == '\n')
                            len--;
                        if (len >= MAX_AI_MESSAGE_LENGTH)
                            len = MAX_AI_MESSAGE_LENGTH - 1;
                        strncpy(line_buffer, text + start, len);
                        line_buffer[len] = '\0';

                        // Trim trailing spaces
                        while (len > 0 && line_buffer[len - 1] == ' ') {
                            line_buffer[--len] = '\0';
                        }

                        // Safely set string and render, skip on error
                        if (line_buffer[0] != '\0') {
                            sfText_setString(msg_text, line_buffer);
                            float line_y = y_offset + (message_line_count * line_height);
                            sfText_setPosition(msg_text, (sfVector2f){ai_panel_x + ai_message_padding, line_y});

                            if (line_y >= ai_panel_y && line_y < ai_panel_y + ai_chat_area_height) {
                                sfRenderWindow_drawText(window, msg_text, NULL);
                            }
                        }

                        message_line_count++;
                    }

                    // Move to next line
                    start = end;
                    while (start < text_len && (text[start] == ' ' || text[start] == '\n')) {
                        start++;
                    }
                }

                // Update y_offset for next message (with minimum height)
                int message_height = message_line_count * line_height;
                if (message_height < 20)
                    message_height = 20;
                y_offset += message_height + ai_message_spacing;
            }

            sfText_destroy(msg_text);
        }

        // Draw input box
        sfRenderWindow_drawRectangleShape(window, ui->ai_input_box, NULL);

        // Handle text scrolling and cursor rendering
        sfVector2f input_pos = sfRectangleShape_getPosition(ui->ai_input_box);
        sfVector2f input_size = sfRectangleShape_getSize(ui->ai_input_box);
        float max_width = input_size.x - 10; // padding

        sfText *temp_text = sfText_create(ui->font);
        sfText_setCharacterSize(temp_text, 14);

        // Calculate full text width
        sfText_setString(temp_text, ui->ai_input_buffer);
        sfFloatRect text_bounds = sfText_getGlobalBounds(temp_text);

        // Check if we need multi-line display
        if (text_bounds.size.x > max_width) {
            // Multi-line rendering
            char line_buffer[512];
            int line_start = 0;
            int line_count = 0;
            float line_height = 18;
            int len = strlen(ui->ai_input_buffer);

            for (int i = 0; i <= len; i++) {
                if (i > line_start) {
                    strncpy(line_buffer, ui->ai_input_buffer + line_start, i - line_start);
                    line_buffer[i - line_start] = '\0';

                    sfText_setString(temp_text, line_buffer);
                    sfFloatRect line_bounds = sfText_getGlobalBounds(temp_text);

                    if (line_bounds.size.x > max_width || i == len) {
                        // Draw this line
                        if (line_bounds.size.x > max_width && i > line_start + 1) {
                            i--; // Back up one character
                            strncpy(line_buffer, ui->ai_input_buffer + line_start, i - line_start);
                            line_buffer[i - line_start] = '\0';
                        }

                        sfText_setString(ui->ai_input_text, line_buffer);
                        sfText_setPosition(ui->ai_input_text, (sfVector2f){input_pos.x + 5, input_pos.y + 10 + line_count * line_height});
                        sfRenderWindow_drawText(window, ui->ai_input_text, NULL);

                        // Draw cursor if in this line
                        if (ui->ai_is_typing && ui->ai_input_cursor >= line_start && ui->ai_input_cursor <= i) {
                            char before_cursor_line[512];
                            int cursor_in_line = ui->ai_input_cursor - line_start;
                            strncpy(before_cursor_line, line_buffer, cursor_in_line);
                            before_cursor_line[cursor_in_line] = '\0';
                            sfText_setString(temp_text, before_cursor_line);
                            sfFloatRect cursor_line_bounds = sfText_getGlobalBounds(temp_text);

                            float cursor_blink = sfTime_asSeconds(sfClock_getElapsedTime(app->cursor_blink_clock));
                            if (fmodf(cursor_blink, 1.0f) < 0.5f) {
                                sfRectangleShape *cursor = sfRectangleShape_create();
                                sfRectangleShape_setSize(cursor, (sfVector2f){2, 16});
                                sfRectangleShape_setPosition(cursor, (sfVector2f){input_pos.x + 5 + cursor_line_bounds.size.x, input_pos.y + 10 + line_count * line_height});
                                sfRectangleShape_setFillColor(cursor, sfWhite);
                                sfRenderWindow_drawRectangleShape(window, cursor, NULL);
                                sfRectangleShape_destroy(cursor);
                            }
                        }

                        line_count++;
                        line_start = i + 1;
                    }
                }
            }
            ui->ai_input_display_lines = line_count;
        } else {
            // Single line rendering with horizontal scroll
            ui->ai_input_display_lines = 1;

            // Calculate cursor position in pixels
            char before_cursor[512];
            strncpy(before_cursor, ui->ai_input_buffer, ui->ai_input_cursor);
            before_cursor[ui->ai_input_cursor] = '\0';
            sfText_setString(temp_text, before_cursor);
            sfFloatRect cursor_bounds = sfText_getGlobalBounds(temp_text);
            float cursor_x = cursor_bounds.size.x;

            // Adjust scroll to keep cursor visible
            if (cursor_x - ui->ai_input_scroll_offset < 0) {
                ui->ai_input_scroll_offset = cursor_x;
            } else if (cursor_x - ui->ai_input_scroll_offset > max_width - 10) {
                ui->ai_input_scroll_offset = cursor_x - max_width + 10;
            }

            // Don't scroll past the end
            if (text_bounds.size.x - ui->ai_input_scroll_offset < max_width) {
                ui->ai_input_scroll_offset = text_bounds.size.x - max_width;
                if (ui->ai_input_scroll_offset < 0) {
                    ui->ai_input_scroll_offset = 0;
                }
            }

            // Render text with scroll offset
            sfText_setString(ui->ai_input_text, ui->ai_input_buffer);
            sfText_setPosition(ui->ai_input_text, (sfVector2f){input_pos.x + 5 - ui->ai_input_scroll_offset, input_pos.y + 10});
            sfRenderWindow_drawText(window, ui->ai_input_text, NULL);

            // Draw blinking cursor if typing
            if (ui->ai_is_typing) {
                float cursor_blink = sfTime_asSeconds(sfClock_getElapsedTime(app->cursor_blink_clock));
                if (fmodf(cursor_blink, 1.0f) < 0.5f) {
                    sfRectangleShape *cursor = sfRectangleShape_create();
                    sfRectangleShape_setSize(cursor, (sfVector2f){2, 16});
                    sfRectangleShape_setPosition(cursor, (sfVector2f){input_pos.x + 5 + cursor_x - ui->ai_input_scroll_offset, input_pos.y + 10});
                    sfRectangleShape_setFillColor(cursor, sfWhite);
                    sfRenderWindow_drawRectangleShape(window, cursor, NULL);
                    sfRectangleShape_destroy(cursor);
                }
            }
        }

        sfText_destroy(temp_text);

        // Draw send button
        sfRenderWindow_drawRectangleShape(window, ui->ai_send_button, NULL);
        sfRenderWindow_drawText(window, ui->ai_send_label, NULL);

        // Draw clear button
        sfRenderWindow_drawRectangleShape(window, ui->ai_clear_button, NULL);
        sfRenderWindow_drawText(window, ui->ai_clear_label, NULL);
    }

    // Draw top bar (always visible)
    sfRenderWindow_drawRectangleShape(window, ui->topbar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->file_button, NULL);
    sfRenderWindow_drawText(window, ui->file_label, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->options_button, NULL);
    sfRenderWindow_drawText(window, ui->options_label, NULL);

    // Draw file dropdown menu if open
    if (app->dropdown_open) {
        sfRenderWindow_drawRectangleShape(window, ui->dropdown_bg, NULL);

        for (int i = 0; i < ui->dropdown_count; i++) {
            // Highlight on hover
            sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(window);
            sfVector2f btn_pos = sfRectangleShape_getPosition(ui->dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(ui->dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                sfRectangleShape_setFillColor(ui->dropdown_buttons[i], (sfColor){70, 100, 130, 255});
            } else {
                sfRectangleShape_setFillColor(ui->dropdown_buttons[i], (sfColor){50, 50, 55, 255});
            }

            sfRenderWindow_drawRectangleShape(window, ui->dropdown_buttons[i], NULL);
            sfRenderWindow_drawText(window, ui->dropdown_labels[i], NULL);
        }
    }

    // Draw options dropdown menu if open
    if (app->options_dropdown_open) {
        sfRenderWindow_drawRectangleShape(window, ui->options_dropdown_bg, NULL);

        for (int i = 0; i < ui->options_dropdown_count; i++) {
            // Highlight on hover
            sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(window);
            sfVector2f btn_pos = sfRectangleShape_getPosition(ui->options_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(ui->options_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                sfRectangleShape_setFillColor(ui->options_dropdown_buttons[i], (sfColor){70, 100, 130, 255});
            } else {
                sfRectangleShape_setFillColor(ui->options_dropdown_buttons[i], (sfColor){50, 50, 55, 255});
            }

            sfRenderWindow_drawRectangleShape(window, ui->options_dropdown_buttons[i], NULL);

            // Draw checkbox (not for symmetry option - index 2)
            if (i != 2 && i != 4 && i != 5) {
                sfRectangleShape_setFillColor(ui->options_checkboxes[i], (sfColor){30, 30, 35, 255});

                // Determine if checkbox should be checked
                bool is_checked = false;
                if (i == 0)
                    is_checked = paint->show_grid;
                else if (i == 1)
                    is_checked = paint->snap_to_grid;
                else if (i == 3)
                    is_checked = paint->show_debug;

                // Draw checkmark if enabled
                if (is_checked) {
                    sfRectangleShape_setFillColor(ui->options_checkboxes[i], (sfColor){50, 200, 50, 255});
                }

                sfRenderWindow_drawRectangleShape(window, ui->options_checkboxes[i], NULL);
            }

            // Update symmetry label dynamically
            if (i == 2) {
                const char *sym_modes[] = {"Symmetry: None", "Symmetry: Horizontal", "Symmetry: Vertical", "Symmetry: Both", "Symmetry: Radial 4", "Symmetry: Radial 8"};
                sfText_setString(ui->options_dropdown_labels[i], sym_modes[paint->symmetry_mode]);
            }

            sfRenderWindow_drawText(window, ui->options_dropdown_labels[i], NULL);
        }
    }

    // Draw file explorer overlay (always last so it's on top)
    if (ui->file_explorer) {
        render_file_explorer(window, ui->file_explorer);
    }
}