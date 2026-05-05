/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void render_debug_info(sfRenderWindow *window, paint_state_t *paint, sfFont *font, sfVector2i mouse_pos) {
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
             paint->fps, mouse_pos.x, mouse_pos.y, mouse_pos.x - paint->canvas_x, mouse_pos.y - paint->canvas_y, paint->current_tool, paint->current_color.r, paint->current_color.g,
             paint->current_color.b, paint->brush_size, paint->is_drawing ? "YES" : "NO", paint->undo_count, MAX_UNDO, paint->undo_position, paint->shape_preview.active ? "ACTIVE" : "INACTIVE");

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
