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

void cleanup_layers(paint_state_t *paint) {
    for (int i = 0; i < paint->layer_count; i++) {
        if (paint->layers[i].texture) {
            sfRenderTexture_destroy(paint->layers[i].texture);
            paint->layers[i].texture = NULL;
        }
        if (paint->layers[i].sprite) {
            sfSprite_destroy(paint->layers[i].sprite);
            paint->layers[i].sprite = NULL;
        }
    }
    paint->layer_count = 0;
}

int create_layer(paint_state_t *paint, const char *name) {
    if (paint->layer_count >= MAX_LAYERS) {
        printf("Maximum number of layers reached!\n");
        return -1;
    }

    int index = paint->layer_count;
    layer_t *layer = &paint->layers[index];

    // Set layer name
    if (name && strlen(name) > 0) {
        strncpy(layer->name, name, MAX_LAYER_NAME - 1);
        layer->name[MAX_LAYER_NAME - 1] = '\0';
    } else {
        snprintf(layer->name, MAX_LAYER_NAME, "Layer %d", index + 1);
    }

    // Create layer texture
    layer->texture = sfRenderTexture_create((sfVector2u){paint->canvas_width, paint->canvas_height}, NULL);
    if (!layer->texture) {
        printf("Failed to create layer texture!\n");
        return -1;
    }

    // Clear layer to transparent
    sfRenderTexture_clear(layer->texture, (sfColor){0, 0, 0, 0});
    sfRenderTexture_display(layer->texture);

    // Create sprite
    layer->sprite = sfSprite_create(sfRenderTexture_getTexture(layer->texture));
    if (!layer->sprite) {
        sfRenderTexture_destroy(layer->texture);
        printf("Failed to create layer sprite!\n");
        return -1;
    }

    sfSprite_setPosition(layer->sprite, (sfVector2f){paint->canvas_x, paint->canvas_y});

    // Set default properties
    layer->opacity = 255;
    layer->visible = 1;

    paint->layer_count++;

    printf("✨ Created layer: %s (index %d)\n", layer->name, index);

    return index;
}

void delete_layer(paint_state_t *paint, int layer_index) {
    if (layer_index < 0 || layer_index >= paint->layer_count) {
        printf("Invalid layer index!\n");
        return;
    }

    // Can't delete the last layer
    if (paint->layer_count <= 1) {
        printf("Cannot delete the last layer!\n");
        return;
    }

    printf("🗑️ Deleting layer: %s\n", paint->layers[layer_index].name);

    // Cleanup layer resources
    if (paint->layers[layer_index].texture) {
        sfRenderTexture_destroy(paint->layers[layer_index].texture);
    }
    if (paint->layers[layer_index].sprite) {
        sfSprite_destroy(paint->layers[layer_index].sprite);
    }

    // Shift layers down
    for (int i = layer_index; i < paint->layer_count - 1; i++) {
        paint->layers[i] = paint->layers[i + 1];
    }

    paint->layer_count--;

    // Adjust current layer if needed
    if (paint->current_layer >= paint->layer_count) {
        paint->current_layer = paint->layer_count - 1;
    } else if (paint->current_layer > layer_index) {
        paint->current_layer--;
    }

    composite_layers(paint);
}

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
