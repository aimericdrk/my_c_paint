/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void clear_canvas(paint_state_t *paint) {
    // Clear current layer
    if (paint->current_layer >= 0 && paint->current_layer < paint->layer_count) {
        sfRenderTexture_clear(paint->layers[paint->current_layer].texture, (sfColor){0, 0, 0, 0});
        sfRenderTexture_display(paint->layers[paint->current_layer].texture);
        composite_layers(paint);
    }
}

int colors_match(sfColor c1, sfColor c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

void add_recent_color(paint_state_t *paint, sfColor color) {
    // Check if color already exists in recent colors
    for (int i = 0; i < paint->recent_color_count; i++) {
        if (paint->recent_colors[i].r == color.r && paint->recent_colors[i].g == color.g && paint->recent_colors[i].b == color.b && paint->recent_colors[i].a == color.a) {
            return; // Already in recent colors
        }
    }

    // Shift colors down and add new one at front
    for (int i = MAX_RECENT_COLORS - 1; i > 0; i--) {
        paint->recent_colors[i] = paint->recent_colors[i - 1];
    }
    paint->recent_colors[0] = color;

    if (paint->recent_color_count < MAX_RECENT_COLORS) {
        paint->recent_color_count++;
    }
}