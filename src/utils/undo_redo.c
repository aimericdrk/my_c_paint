/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Mouse utility functions for handling scaled mouse positions
*/

#include "paint.h"

void save_undo_state(paint_state_t *paint) {
    // Clear any redo states after current position
    for (int i = paint->undo_position + 1; i < paint->undo_count; i++) {
        for (int j = 0; j < paint->undo_stack[i].layer_count; j++) {
            if (paint->undo_stack[i].layer_textures[j])
                sfRenderTexture_destroy(paint->undo_stack[i].layer_textures[j]);
            paint->undo_stack[i].layer_textures[j] = NULL;
        }
    }

    // Move to next position
    paint->undo_position++;

    // If we exceed the max, shift everything down
    if (paint->undo_position >= MAX_UNDO) {
        for (int j = 0; j < paint->undo_stack[0].layer_count; j++) {
            if (paint->undo_stack[0].layer_textures[j])
                sfRenderTexture_destroy(paint->undo_stack[0].layer_textures[j]);
        }

        for (int i = 0; i < MAX_UNDO - 1; i++) {
            paint->undo_stack[i] = paint->undo_stack[i + 1];
        }
        paint->undo_position = MAX_UNDO - 1;
    }

    // Save current state - save all layer textures
    undo_state_t *state = &paint->undo_stack[paint->undo_position];
    state->layer_count = paint->layer_count;
    state->current_layer = paint->current_layer;

    for (int i = 0; i < paint->layer_count; i++) {
        state->layer_textures[i] = sfRenderTexture_create((sfVector2u){paint->canvas_width, paint->canvas_height}, NULL);

        // Copy layer texture
        const sfTexture *layer_tex = sfRenderTexture_getTexture(paint->layers[i].texture);
        sfSprite *temp_sprite = sfSprite_create(layer_tex);

        sfRenderTexture_clear(state->layer_textures[i], sfTransparent);
        sfRenderTexture_drawSprite(state->layer_textures[i], temp_sprite, NULL);
        sfRenderTexture_display(state->layer_textures[i]);

        sfSprite_destroy(temp_sprite);
    }

    paint->undo_count = paint->undo_position + 1;
}

void undo(paint_state_t *paint) {
    if (paint->undo_position <= 0)
        return;

    paint->undo_position--;

    // Restore layer states from undo stack
    undo_state_t *state = &paint->undo_stack[paint->undo_position];

    for (int i = 0; i < state->layer_count && i < paint->layer_count; i++) {
        const sfTexture *layer_tex = sfRenderTexture_getTexture(state->layer_textures[i]);
        sfSprite *restore_sprite = sfSprite_create(layer_tex);

        sfRenderTexture_clear(paint->layers[i].texture, sfTransparent);
        sfRenderTexture_drawSprite(paint->layers[i].texture, restore_sprite, NULL);
        sfRenderTexture_display(paint->layers[i].texture);

        sfSprite_destroy(restore_sprite);
    }

    paint->current_layer = state->current_layer;

    // Recomposite all layers
    composite_layers(paint);
}

void redo(paint_state_t *paint) {
    if (paint->undo_position >= paint->undo_count - 1)
        return;

    paint->undo_position++;

    // Restore layer states from undo stack
    undo_state_t *state = &paint->undo_stack[paint->undo_position];

    for (int i = 0; i < state->layer_count && i < paint->layer_count; i++) {
        const sfTexture *layer_tex = sfRenderTexture_getTexture(state->layer_textures[i]);
        sfSprite *restore_sprite = sfSprite_create(layer_tex);

        sfRenderTexture_clear(paint->layers[i].texture, sfTransparent);
        sfRenderTexture_drawSprite(paint->layers[i].texture, restore_sprite, NULL);
        sfRenderTexture_display(paint->layers[i].texture);

        sfSprite_destroy(restore_sprite);
    }

    paint->current_layer = state->current_layer;

    // Recomposite all layers
    composite_layers(paint);
}