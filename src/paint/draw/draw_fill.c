/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void fill_canvas_optimized(paint_state_t *paint, int x, int y, sfColor target, sfColor replacement, sfImage *image, int *stack_x, int *stack_y) {
    (void)paint; // Unused parameter
    int stack_size = 0;

    stack_x[stack_size] = x;
    stack_y[stack_size] = y;
    stack_size++;

    while (stack_size > 0 && stack_size < MAX_FILL_STACK) {
        stack_size--;
        int cx = stack_x[stack_size];
        int cy = stack_y[stack_size];

        if (cx < 0 || cx >= paint->canvas_width || cy < 0 || cy >= paint->canvas_height)
            continue;

        sfColor current = sfImage_getPixel(image, (sfVector2u){cx, cy});

        if (!colors_match(current, target) || colors_match(current, replacement))
            continue;

        // Fill the scanline
        int left = cx;
        int right = cx;

        // Expand left
        while (left > 0) {
            sfColor c = sfImage_getPixel(image, (sfVector2u){left - 1, cy});
            if (!colors_match(c, target))
                break;
            left--;
        }

        // Expand right
        while (right < paint->canvas_width - 1) {
            sfColor c = sfImage_getPixel(image, (sfVector2u){right + 1, cy});
            if (!colors_match(c, target))
                break;
            right++;
        }

        // Fill the line
        for (int i = left; i <= right; i++) {
            sfImage_setPixel(image, (sfVector2u){i, cy}, replacement);
        }

        // Add pixels above and below to stack
        for (int i = left; i <= right; i++) {
            // Check above
            if (cy > 0 && stack_size < MAX_FILL_STACK - 1) {
                sfColor c = sfImage_getPixel(image, (sfVector2u){i, cy - 1});
                if (colors_match(c, target)) {
                    stack_x[stack_size] = i;
                    stack_y[stack_size] = cy - 1;
                    stack_size++;
                }
            }

            // Check below
            if (cy < paint->canvas_height - 1 && stack_size < MAX_FILL_STACK - 1) {
                sfColor c = sfImage_getPixel(image, (sfVector2u){i, cy + 1});
                if (colors_match(c, target)) {
                    stack_x[stack_size] = i;
                    stack_y[stack_size] = cy + 1;
                    stack_size++;
                }
            }
        }
    }

    if (stack_size >= MAX_FILL_STACK) {
        printf("Fill area too large, stopped at limit\n");
    }
}

void fill_canvas(paint_state_t *paint, sfVector2i pos, sfColor target) {
    if (colors_match(target, paint->current_color))
        return;

    // Get current layer
    if (paint->current_layer < 0 || paint->current_layer >= paint->layer_count)
        return;

    printf("Filling area on layer %d...\n", paint->current_layer);

    // Get the current layer's texture as an image
    const sfTexture *texture = sfRenderTexture_getTexture(paint->layers[paint->current_layer].texture);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image) {
        printf("Failed to get layer image!\n");
        return;
    }

    // Allocate smaller stack for optimized flood fill
    int *stack_x = malloc(sizeof(int) * MAX_FILL_STACK);
    int *stack_y = malloc(sizeof(int) * MAX_FILL_STACK);

    if (stack_x && stack_y) {
        fill_canvas_optimized(paint, pos.x, pos.y, target, paint->current_color, image, stack_x, stack_y);

        // Create a texture from the modified image and draw it to current layer
        sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
        if (new_texture) {
            sfSprite *sprite = sfSprite_create(new_texture);
            if (sprite) {
                // Clear current layer and redraw with filled content
                sfRenderTexture_clear(paint->layers[paint->current_layer].texture, (sfColor){0, 0, 0, 0});
                sfRenderTexture_drawSprite(paint->layers[paint->current_layer].texture, sprite, NULL);
                sfRenderTexture_display(paint->layers[paint->current_layer].texture);
                sfSprite_destroy(sprite);
            }
            sfTexture_destroy(new_texture);
        }

        // Update composite display
        composite_layers(paint);

        printf("Fill complete!\n");
    } else {
        printf("Memory allocation failed!\n");
    }

    if (stack_x)
        free(stack_x);
    if (stack_y)
        free(stack_y);
    if (image)
        sfImage_destroy(image);
}