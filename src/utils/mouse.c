/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Mouse utility functions for handling scaled mouse positions
*/

#include "paint.h"

// Helper function to scale mouse position based on actual window size
sfVector2i get_scaled_mouse_pos(app_t *app) {
    sfVector2i raw_pos = sfMouse_getPositionRenderWindow(app->window);
    sfVector2u window_size = sfRenderWindow_getSize(app->window);

    // Scale coordinates based on actual window size vs expected size
    sfVector2i scaled_pos;
    scaled_pos.x = (int)(raw_pos.x * (float)app->window_width / window_size.x);
    scaled_pos.y = (int)(raw_pos.y * (float)app->window_height / window_size.y);

    return scaled_pos;
}