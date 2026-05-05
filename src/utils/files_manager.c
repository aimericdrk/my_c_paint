/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void save_to_file(paint_state_t *paint, const char *filename) {
    printf("Saving canvas to %s...\n", filename);

    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (sfImage_saveToFile(image, filename)) {
        printf("Successfully saved!\n");
    } else {
        printf("Failed to save file!\n");
    }

    sfImage_destroy(image);
}

void load_from_file(paint_state_t *paint, const char *filename) {
    printf("Loading canvas from %s...\n", filename);

    sfTexture *texture = sfTexture_createFromFile(filename, NULL);
    if (!texture) {
        printf("Failed to load file!\n");
        return;
    }

    save_undo_state(paint);

    sfSprite *sprite = sfSprite_create(texture);

    sfRenderTexture_clear(paint->canvas, sfWhite);
    sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
    sfRenderTexture_display(paint->canvas);

    sfSprite_destroy(sprite);
    sfTexture_destroy(texture);

    printf("Successfully loaded!\n");
}