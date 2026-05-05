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

    // File explorer cleanup
    if (ui->file_explorer)
        cleanup_file_explorer(ui->file_explorer);

    free(ui);
}