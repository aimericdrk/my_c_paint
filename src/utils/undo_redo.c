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
        if (paint->undo_stack[i].texture)
            sfRenderTexture_destroy(paint->undo_stack[i].texture);
        if (paint->undo_stack[i].sprite)
            sfSprite_destroy(paint->undo_stack[i].sprite);
        paint->undo_stack[i].texture = NULL;
        paint->undo_stack[i].sprite = NULL;
    }

    // Move to next position
    paint->undo_position++;

    // If we exceed the max, shift everything down
    if (paint->undo_position >= MAX_UNDO) {
        if (paint->undo_stack[0].texture)
            sfRenderTexture_destroy(paint->undo_stack[0].texture);
        if (paint->undo_stack[0].sprite)
            sfSprite_destroy(paint->undo_stack[0].sprite);

        for (int i = 0; i < MAX_UNDO - 1; i++) {
            paint->undo_stack[i] = paint->undo_stack[i + 1];
        }
        paint->undo_position = MAX_UNDO - 1;
    }

    // Save current state
    paint->undo_stack[paint->undo_position].texture = sfRenderTexture_create((sfVector2u){paint->canvas_width, paint->canvas_height}, NULL);

    // Copy current canvas to undo state
    const sfTexture *canvas_tex = sfRenderTexture_getTexture(paint->canvas);
    sfSprite *temp_sprite = sfSprite_create(canvas_tex);

    sfRenderTexture_clear(paint->undo_stack[paint->undo_position].texture, sfTransparent);
    sfRenderTexture_drawSprite(paint->undo_stack[paint->undo_position].texture, temp_sprite, NULL);
    sfRenderTexture_display(paint->undo_stack[paint->undo_position].texture);

    sfSprite_destroy(temp_sprite);

    paint->undo_count = paint->undo_position + 1;
}

void undo(paint_state_t *paint) {
    if (paint->undo_position <= 0)
        return;

    paint->undo_position--;

    // Restore from undo stack
    const sfTexture *undo_tex = sfRenderTexture_getTexture(paint->undo_stack[paint->undo_position].texture);
    sfSprite *restore_sprite = sfSprite_create(undo_tex);

    sfRenderTexture_clear(paint->canvas, sfWhite);
    sfRenderTexture_drawSprite(paint->canvas, restore_sprite, NULL);
    sfRenderTexture_display(paint->canvas);

    sfSprite_destroy(restore_sprite);
}

void redo(paint_state_t *paint) {
    if (paint->undo_position >= paint->undo_count - 1)
        return;

    paint->undo_position++;

    // Restore from undo stack
    const sfTexture *undo_tex = sfRenderTexture_getTexture(paint->undo_stack[paint->undo_position].texture);
    sfSprite *restore_sprite = sfSprite_create(undo_tex);

    sfRenderTexture_clear(paint->canvas, sfWhite);
    sfRenderTexture_drawSprite(paint->canvas, restore_sprite, NULL);
    sfRenderTexture_display(paint->canvas);

    sfSprite_destroy(restore_sprite);
}