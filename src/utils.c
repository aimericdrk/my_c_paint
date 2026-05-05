#include "../include/paint.h"

int is_point_in_rect(sfVector2i point, sfVector2f pos, sfVector2f size)
{
    return point.x >= pos.x && point.x <= pos.x + size.x &&
           point.y >= pos.y && point.y <= pos.y + size.y;
}

sfColor get_pixel_color(sfRenderTexture *texture, int x, int y, int width, int height)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return sfWhite;

    const sfTexture *tex = sfRenderTexture_getTexture(texture);
    sfImage *image = sfTexture_copyToImage(tex);
    sfColor color = sfImage_getPixel(image, (sfVector2u){x, y});
    sfImage_destroy(image);

    return color;
}

void save_undo_state(paint_state_t *paint)
{
    // Clear any redo states after current position
    for (int i = paint->undo_position + 1; i < paint->undo_count; i++)
    {
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
    if (paint->undo_position >= MAX_UNDO)
    {
        if (paint->undo_stack[0].texture)
            sfRenderTexture_destroy(paint->undo_stack[0].texture);
        if (paint->undo_stack[0].sprite)
            sfSprite_destroy(paint->undo_stack[0].sprite);

        for (int i = 0; i < MAX_UNDO - 1; i++)
        {
            paint->undo_stack[i] = paint->undo_stack[i + 1];
        }
        paint->undo_position = MAX_UNDO - 1;
    }

    // Save current state
    paint->undo_stack[paint->undo_position].texture =
        sfRenderTexture_create((sfVector2u){paint->canvas_width, paint->canvas_height}, NULL);

    // Copy current canvas to undo state
    const sfTexture *canvas_tex = sfRenderTexture_getTexture(paint->canvas);
    sfSprite *temp_sprite = sfSprite_create(canvas_tex);

    sfRenderTexture_clear(paint->undo_stack[paint->undo_position].texture,
                          sfTransparent);
    sfRenderTexture_drawSprite(paint->undo_stack[paint->undo_position].texture,
                               temp_sprite, NULL);
    sfRenderTexture_display(paint->undo_stack[paint->undo_position].texture);

    sfSprite_destroy(temp_sprite);

    paint->undo_count = paint->undo_position + 1;
}

void undo(paint_state_t *paint)
{
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

void redo(paint_state_t *paint)
{
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

void save_to_file(paint_state_t *paint, const char *filename)
{
    printf("Saving canvas to %s...\n", filename);

    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (sfImage_saveToFile(image, filename))
    {
        printf("Successfully saved!\n");
    }
    else
    {
        printf("Failed to save file!\n");
    }

    sfImage_destroy(image);
}

void load_from_file(paint_state_t *paint, const char *filename)
{
    printf("Loading canvas from %s...\n", filename);

    sfTexture *texture = sfTexture_createFromFile(filename, NULL);
    if (!texture)
    {
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

void render_debug_info(sfRenderWindow *window, paint_state_t *paint,
                       sfFont *font, sfVector2i mouse_pos)
{
    sfText *debug_text = sfText_create(font);
    sfText_setCharacterSize(debug_text, 14);
    sfText_setFillColor(debug_text, sfYellow);

    char debug_str[512];
    snprintf(debug_str, sizeof(debug_str),
             "═══════════════ DEBUG INFO ═══════════════\n"
             "FPS: %.1f\n"
             "Mouse: (%d, %d)\n"
             "Canvas Pos: (%d, %d)\n"
             "Tool: %d\n"
             "Color: RGB(%d, %d, %d)\n"
             "Brush Size: %d\n"
             "Drawing: %s\n"
             "Undo Stack: %d/%d (pos: %d)\n"
             "Shape Preview: %s",
             paint->fps,
             mouse_pos.x, mouse_pos.y,
             mouse_pos.x - paint->canvas_x, mouse_pos.y - paint->canvas_y,
             paint->current_tool,
             paint->current_color.r, paint->current_color.g, paint->current_color.b,
             paint->brush_size,
             paint->is_drawing ? "YES" : "NO",
             paint->undo_count, MAX_UNDO, paint->undo_position,
             paint->shape_preview.active ? "ACTIVE" : "INACTIVE");

    sfText_setString(debug_text, debug_str);
    sfText_setPosition(debug_text, (sfVector2f){10, 10});

    // Draw semi-transparent background for debug text
    sfRectangleShape *debug_bg = sfRectangleShape_create();
    sfFloatRect bounds = sfText_getLocalBounds(debug_text);
    sfRectangleShape_setSize(debug_bg, (sfVector2f){bounds.size.x + 20, bounds.size.y + 30});
    sfRectangleShape_setPosition(debug_bg, (sfVector2f){5, 5});
    sfRectangleShape_setFillColor(debug_bg, (sfColor){0, 0, 0, 200});

    sfRenderWindow_drawRectangleShape(window, debug_bg, NULL);
    sfRenderWindow_drawText(window, debug_text, NULL);

    sfRectangleShape_destroy(debug_bg);
    sfText_destroy(debug_text);
}
