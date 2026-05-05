/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void update_color_slider(app_t *app, sfVector2i mouse_pos) {
    if (app->dragging_slider == 0) {
        // Red slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->r_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.r = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 1) {
        // Green slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->g_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.g = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 2) {
        // Blue slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->b_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.b = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 3) {
        // Alpha slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->a_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.a = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 4) {
        // Size slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->size_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->brush_size = 1 + (int)(percent * 49); // Range 1-50
    }
}