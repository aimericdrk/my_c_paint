/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Layer management functions
*/

#include "paint.h"

void init_layers(paint_state_t *paint) {
    paint->layer_count = 0;
    paint->current_layer = 0;

    // Create default layer
    create_layer(paint, "Background");
}
