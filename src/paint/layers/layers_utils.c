/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Layer management functions
*/

#include "paint.h"

void merge_layer_down(paint_state_t *paint, int layer_index) {
    if (layer_index <= 0 || layer_index >= paint->layer_count) {
        printf("Cannot merge this layer!\n");
        return;
    }

    layer_t *upper = &paint->layers[layer_index];
    layer_t *lower = &paint->layers[layer_index - 1];

    printf("🔗 Merging %s into %s\n", upper->name, lower->name);

    // Draw upper layer onto lower layer with opacity
    sfColor blend_color = (sfColor){255, 255, 255, upper->opacity};
    sfSprite_setColor(upper->sprite, blend_color);
    sfSprite_setPosition(upper->sprite, (sfVector2f){0, 0});
    sfRenderTexture_drawSprite(lower->texture, upper->sprite, NULL);
    sfRenderTexture_display(lower->texture);

    // Reset sprite position
    sfSprite_setPosition(upper->sprite, (sfVector2f){paint->canvas_x, paint->canvas_y});
    sfSprite_setColor(upper->sprite, sfWhite);

    // Delete the upper layer
    delete_layer(paint, layer_index);
}

void set_layer_opacity(paint_state_t *paint, int layer_index, uint8_t opacity) {
    if (layer_index < 0 || layer_index >= paint->layer_count) {
        return;
    }

    paint->layers[layer_index].opacity = opacity;
    composite_layers(paint);
}

void toggle_layer_visibility(paint_state_t *paint, int layer_index) {
    if (layer_index < 0 || layer_index >= paint->layer_count) {
        return;
    }

    paint->layers[layer_index].visible = !paint->layers[layer_index].visible;
    printf("👁️ Layer %s visibility: %s\n", paint->layers[layer_index].name, paint->layers[layer_index].visible ? "ON" : "OFF");
    composite_layers(paint);
}

void composite_layers(paint_state_t *paint) {
    // Clear the main canvas
    sfRenderTexture_clear(paint->canvas, (sfColor){255, 255, 255, 255});

    // Draw all visible layers from bottom to top
    for (int i = paint->layer_count - 1; i >= 0; i--) {
        layer_t *layer = &paint->layers[i];

        if (!layer->visible) {
            continue;
        }

        // Set sprite properties
        sfSprite_setPosition(layer->sprite, (sfVector2f){0, 0});
        sfColor blend_color = (sfColor){255, 255, 255, layer->opacity};
        sfSprite_setColor(layer->sprite, blend_color);

        // Draw layer to canvas
        sfRenderTexture_drawSprite(paint->canvas, layer->sprite, NULL);

        // Reset sprite
        sfSprite_setPosition(layer->sprite, (sfVector2f){paint->canvas_x, paint->canvas_y});
    }

    sfRenderTexture_display(paint->canvas);
}
