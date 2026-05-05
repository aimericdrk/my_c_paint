/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void flip_canvas_horizontal(paint_state_t *paint) {
    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image)
        return;

    for (int y = 0; y < paint->canvas_height; y++) {
        for (int x = 0; x < paint->canvas_width / 2; x++) {
            sfColor left = sfImage_getPixel(image, (sfVector2u){x, y});
            sfColor right = sfImage_getPixel(image, (sfVector2u){paint->canvas_width - x - 1, y});
            sfImage_setPixel(image, (sfVector2u){x, y}, right);
            sfImage_setPixel(image, (sfVector2u){paint->canvas_width - x - 1, y}, left);
        }
    }

    sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
    if (new_texture) {
        sfSprite *sprite = sfSprite_create(new_texture);
        sfRenderTexture_clear(paint->canvas, sfWhite);
        sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
        sfRenderTexture_display(paint->canvas);
        sfSprite_destroy(sprite);
        sfTexture_destroy(new_texture);
    }

    sfImage_destroy(image);
    printf("Canvas flipped horizontally\n");
}

void flip_canvas_vertical(paint_state_t *paint) {
    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image)
        return;

    for (int y = 0; y < paint->canvas_height / 2; y++) {
        for (int x = 0; x < paint->canvas_width; x++) {
            sfColor top = sfImage_getPixel(image, (sfVector2u){x, y});
            sfColor bottom = sfImage_getPixel(image, (sfVector2u){x, paint->canvas_height - y - 1});
            sfImage_setPixel(image, (sfVector2u){x, y}, bottom);
            sfImage_setPixel(image, (sfVector2u){x, paint->canvas_height - y - 1}, top);
        }
    }

    sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
    if (new_texture) {
        sfSprite *sprite = sfSprite_create(new_texture);
        sfRenderTexture_clear(paint->canvas, sfWhite);
        sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
        sfRenderTexture_display(paint->canvas);
        sfSprite_destroy(sprite);
        sfTexture_destroy(new_texture);
    }

    sfImage_destroy(image);
    printf("Canvas flipped vertically\n");
}
