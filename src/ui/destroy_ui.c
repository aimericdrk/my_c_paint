/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void cleanup_ui(ui_elements_t *ui) {
    if (!ui)
        return;

    if (ui->bg)
        sfRectangleShape_destroy(ui->bg);

    // Tab buttons cleanup
    if (ui->tab_buttons) {
        for (int i = 0; i < ui->tab_count; i++) {
            if (ui->tab_buttons[i])
                sfRectangleShape_destroy(ui->tab_buttons[i]);
            if (ui->tab_labels[i])
                sfText_destroy(ui->tab_labels[i]);
        }
        free(ui->tab_buttons);
        free(ui->tab_labels);
    }

    if (ui->buttons) {
        for (int i = 0; i < ui->button_count; i++) {
            if (ui->buttons[i])
                sfRectangleShape_destroy(ui->buttons[i]);
            if (ui->labels[i])
                sfText_destroy(ui->labels[i]);
            if (ui->tool_icons[i])
                sfCircleShape_destroy(ui->tool_icons[i]);
        }
        free(ui->buttons);
        free(ui->labels);
        free(ui->tool_icons);
    }

    if (ui->color_preview)
        sfRectangleShape_destroy(ui->color_preview);
    if (ui->r_slider)
        sfRectangleShape_destroy(ui->r_slider);
    if (ui->g_slider)
        sfRectangleShape_destroy(ui->g_slider);
    if (ui->b_slider)
        sfRectangleShape_destroy(ui->b_slider);
    if (ui->a_slider)
        sfRectangleShape_destroy(ui->a_slider);
    if (ui->r_bar)
        sfRectangleShape_destroy(ui->r_bar);
    if (ui->g_bar)
        sfRectangleShape_destroy(ui->g_bar);
    if (ui->b_bar)
        sfRectangleShape_destroy(ui->b_bar);
    if (ui->a_bar)
        sfRectangleShape_destroy(ui->a_bar);
    if (ui->size_slider)
        sfRectangleShape_destroy(ui->size_slider);
    if (ui->size_bar)
        sfRectangleShape_destroy(ui->size_bar);

    // Top bar cleanup
    if (ui->topbar)
        sfRectangleShape_destroy(ui->topbar);
    if (ui->file_button)
        sfRectangleShape_destroy(ui->file_button);
    if (ui->file_label)
        sfText_destroy(ui->file_label);

    // Dropdown cleanup
    if (ui->dropdown_bg)
        sfRectangleShape_destroy(ui->dropdown_bg);

    for (int i = 0; i < ui->dropdown_count; i++) {
        if (ui->dropdown_buttons[i])
            sfRectangleShape_destroy(ui->dropdown_buttons[i]);
        if (ui->dropdown_labels[i])
            sfText_destroy(ui->dropdown_labels[i]);
    }

    free(ui->dropdown_buttons);
    free(ui->dropdown_labels);

    // Options dropdown cleanup
    if (ui->options_button)
        sfRectangleShape_destroy(ui->options_button);
    if (ui->options_label)
        sfText_destroy(ui->options_label);
    if (ui->options_dropdown_bg)
        sfRectangleShape_destroy(ui->options_dropdown_bg);

    for (int i = 0; i < ui->options_dropdown_count; i++) {
        if (ui->options_dropdown_buttons[i])
            sfRectangleShape_destroy(ui->options_dropdown_buttons[i]);
        if (ui->options_dropdown_labels[i])
            sfText_destroy(ui->options_dropdown_labels[i]);
        if (ui->options_checkboxes[i])
            sfRectangleShape_destroy(ui->options_checkboxes[i]);
    }

    free(ui->options_dropdown_buttons);
    free(ui->options_dropdown_labels);
    free(ui->options_checkboxes);

    // Layer panel cleanup
    if (ui->layer_items) {
        for (int i = 0; i < MAX_LAYERS; i++) {
            if (ui->layer_items[i])
                sfRectangleShape_destroy(ui->layer_items[i]);
            if (ui->layer_names[i])
                sfText_destroy(ui->layer_names[i]);
            if (ui->layer_visibility_buttons[i])
                sfRectangleShape_destroy(ui->layer_visibility_buttons[i]);
            if (ui->layer_opacity_bars[i])
                sfRectangleShape_destroy(ui->layer_opacity_bars[i]);
            if (ui->layer_opacity_sliders[i])
                sfRectangleShape_destroy(ui->layer_opacity_sliders[i]);
        }
        free(ui->layer_items);
        free(ui->layer_names);
        free(ui->layer_visibility_buttons);
        free(ui->layer_opacity_bars);
        free(ui->layer_opacity_sliders);
    }

    if (ui->layer_scrollbar)
        sfRectangleShape_destroy(ui->layer_scrollbar);
    if (ui->layer_scrollbar_thumb)
        sfRectangleShape_destroy(ui->layer_scrollbar_thumb);

    if (ui->layer_new_button)
        sfRectangleShape_destroy(ui->layer_new_button);
    if (ui->layer_new_label)
        sfText_destroy(ui->layer_new_label);
    if (ui->layer_delete_button)
        sfRectangleShape_destroy(ui->layer_delete_button);
    if (ui->layer_delete_label)
        sfText_destroy(ui->layer_delete_label);
    if (ui->layer_merge_button)
        sfRectangleShape_destroy(ui->layer_merge_button);
    if (ui->layer_merge_label)
        sfText_destroy(ui->layer_merge_label);
    if (ui->layer_up_button)
        sfRectangleShape_destroy(ui->layer_up_button);
    if (ui->layer_up_label)
        sfText_destroy(ui->layer_up_label);
    if (ui->layer_down_button)
        sfRectangleShape_destroy(ui->layer_down_button);
    if (ui->layer_down_label)
        sfText_destroy(ui->layer_down_label);

    // File explorer cleanup
    if (ui->file_explorer)
        cleanup_file_explorer(ui->file_explorer);

    // Color wheel cleanup
    if (ui->color_wheel)
        sfCircleShape_destroy(ui->color_wheel);
    if (ui->color_wheel_cursor)
        sfCircleShape_destroy(ui->color_wheel_cursor);
    if (ui->color_wheel_texture)
        sfTexture_destroy(ui->color_wheel_texture);
    if (ui->color_wheel_image)
        sfImage_destroy(ui->color_wheel_image);

    free(ui);
}