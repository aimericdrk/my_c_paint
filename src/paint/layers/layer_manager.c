/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Layer management functions
*/

#include "paint.h"

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
