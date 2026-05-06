/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void cleanup_paint_state(paint_state_t *paint) {
    if (!paint)
        return;

    // Cleanup layers
    cleanup_layers(paint);

    if (paint->canvas)
        sfRenderTexture_destroy(paint->canvas);
    if (paint->canvas_sprite)
        sfSprite_destroy(paint->canvas_sprite);
    if (paint->fps_clock)
        sfClock_destroy(paint->fps_clock);

    // Cleanup undo stack
    for (int i = 0; i < MAX_UNDO; i++) {
        for (int j = 0; j < paint->undo_stack[i].layer_count; j++) {
            if (paint->undo_stack[i].layer_textures[j])
                sfRenderTexture_destroy(paint->undo_stack[i].layer_textures[j]);
        }
    }

    free(paint);
}