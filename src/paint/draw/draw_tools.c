/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void draw_with_tool(paint_state_t *paint, sfVector2i pos) {
    // Use symmetry if enabled
    if (paint->symmetry_mode != SYMMETRY_NONE) {
        draw_with_symmetry(paint, pos);
        return;
    }

    // Apply alpha to color
    sfColor draw_color = paint->current_color;

    switch (paint->current_tool) {
    case TOOL_PEN:
        draw_circle_point(paint->canvas, pos, draw_color, paint->brush_size);
        break;

    case TOOL_ERASER:
        draw_circle_point(paint->canvas, pos, sfWhite, paint->brush_size * 2);
        break;

    case TOOL_BRUSH:
        // Soft brush with multiple circles
        for (int i = 0; i < 3; i++) {
            sfColor soft_color = draw_color;
            soft_color.a = (paint->current_color.a * (i * 10)) / 255;
            draw_circle_point(paint->canvas, pos, soft_color, paint->brush_size + i * 2);
        }
        break;

    case TOOL_SPRAY:
        // Spray effect with random particles
        for (int i = 0; i < 30; i++) {
            int radius = paint->brush_size * 2;
            float angle = (float)(rand() % 360) * M_PI / 180.0f;
            float distance = (float)(rand() % (radius * 100)) / 100.0f;
            int offset_x = (int)(distance * cos(angle));
            int offset_y = (int)(distance * sin(angle));
            sfVector2i spray_pos = {pos.x + offset_x, pos.y + offset_y};
            if (spray_pos.x >= 0 && spray_pos.x < paint->canvas_width && spray_pos.y >= 0 && spray_pos.y < paint->canvas_height) {
                sfColor spray_color = draw_color;
                spray_color.a = (paint->current_color.a * (80 + (rand() % 80))) / 255;
                draw_circle_point(paint->canvas, spray_pos, spray_color, 1 + (rand() % 2));
            }
        }
        sfRenderTexture_display(paint->canvas);
        break;

    default:
        break;
    }
}
