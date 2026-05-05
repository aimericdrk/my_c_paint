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

    // Draw color preview
    sfRectangleShape_setFillColor(ui->color_preview, paint->current_color);
    sfRenderWindow_drawRectangleShape(window, ui->color_preview, NULL);

    // Draw color sliders
    sfRenderWindow_drawRectangleShape(window, ui->r_bar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->g_bar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->b_bar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->a_bar, NULL);

    // Update slider positions based on color - get config sections
    ui_config_t *config = app->config;
    cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
    cJSON *sliders_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "sliders") : NULL;
    cJSON *r_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "r_slider") : NULL;
    cJSON *g_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "g_slider") : NULL;
    cJSON *b_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "b_slider") : NULL;
    cJSON *a_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "a_slider") : NULL;
    cJSON *size_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "size_slider") : NULL;

    int slider_base_x = app->toolbar_x + config_get_int(sliders_cfg, "x", 25);
    int slider_range = config_get_int(sliders_cfg, "slider_range", 190);
    int r_slider_y = config_get_int(r_slider_cfg, "y", 520);
    int g_slider_y = config_get_int(g_slider_cfg, "y", 570);
    int b_slider_y = config_get_int(b_slider_cfg, "y", 620);
    int a_slider_y = config_get_int(a_slider_cfg, "y", 670);
    int size_slider_y = config_get_int(size_slider_cfg, "y", 685);
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
    int color_label_y = config_get_int(color_preview_cfg, "label_y", 535);
    sfText_setString(text, color_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + color_label_x, color_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // R label
    const char *r_label = config_get_string(r_slider_cfg, "label", "R");
    int r_label_x = config_get_int(r_slider_cfg, "label_x", 230);
    int r_label_y = config_get_int(r_slider_cfg, "label_y", 640);
    sfText_setString(text, r_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + r_label_x, r_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // G label
    const char *g_label = config_get_string(g_slider_cfg, "label", "G");
    int g_label_x = config_get_int(g_slider_cfg, "label_x", 230);
    int g_label_y = config_get_int(g_slider_cfg, "label_y", 690);
    sfText_setString(text, g_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + g_label_x, g_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // B label
    const char *b_label = config_get_string(b_slider_cfg, "label", "B");
    int b_label_x = config_get_int(b_slider_cfg, "label_x", 230);
    int b_label_y = config_get_int(b_slider_cfg, "label_y", 740);
    sfText_setString(text, b_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + b_label_x, b_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // A label
    const char *a_label = config_get_string(a_slider_cfg, "label", "A");
    int a_label_x = config_get_int(a_slider_cfg, "label_x", 230);
    int a_label_y = config_get_int(a_slider_cfg, "label_y", 670);
    sfText_setString(text, a_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + a_label_x, a_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // Size label
    const char *size_label = config_get_string(size_slider_cfg, "label", "Size:");
    int size_label_x = config_get_int(size_slider_cfg, "label_x", 15);
    int size_label_y = config_get_int(size_slider_cfg, "label_y", 770);
    sfText_setString(text, size_label);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + size_label_x, size_label_y});
    sfRenderWindow_drawText(window, text, NULL);

    // Size value label
    char size_str[32];
    sprintf(size_str, "%d px", paint->brush_size);
    int size_value_x = config_get_int(size_slider_cfg, "value_label_x", 230);
    int size_value_y = config_get_int(size_slider_cfg, "value_label_y", 810);
    sfText_setString(text, size_str);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + size_value_x, size_value_y});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_destroy(text);

    // Draw top bar
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