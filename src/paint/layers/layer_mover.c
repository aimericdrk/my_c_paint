/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Layer management functions
*/

#include "paint.h"

void move_layer_up(paint_state_t *paint, int layer_index) {
    if (layer_index <= 0 || layer_index >= paint->layer_count) {
        return; // Already at top or invalid
    }

    // Swap layers
    layer_t temp = paint->layers[layer_index];
    paint->layers[layer_index] = paint->layers[layer_index - 1];
    paint->layers[layer_index - 1] = temp;

    // Update current layer
    if (paint->current_layer == layer_index) {
        paint->current_layer--;
    } else if (paint->current_layer == layer_index - 1) {
        paint->current_layer++;
    }

    printf("⬆️ Moved layer up: %s\n", temp.name);
    composite_layers(paint);
}

void move_layer_down(paint_state_t *paint, int layer_index) {
    if (layer_index < 0 || layer_index >= paint->layer_count - 1) {
        return; // Already at bottom or invalid
    }

    // Swap layers
    layer_t temp = paint->layers[layer_index];
    paint->layers[layer_index] = paint->layers[layer_index + 1];
    paint->layers[layer_index + 1] = temp;

    // Update current layer
    if (paint->current_layer == layer_index) {
        paint->current_layer++;
    } else if (paint->current_layer == layer_index + 1) {
        paint->current_layer--;
    }

    printf("⬇️ Moved layer down: %s\n", temp.name);
    composite_layers(paint);
}