/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Layer management functions
*/

#include "paint.h"

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
