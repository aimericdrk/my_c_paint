/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void draw_with_tool(paint_state_t *paint, sfVector2i pos) {
    // Get current layer
    if (paint->current_layer < 0 || paint->current_layer >= paint->layer_count)
        return;
    sfRenderTexture *target = paint->layers[paint->current_layer].texture;

    // Use symmetry if enabled
    if (paint->symmetry_mode != SYMMETRY_NONE) {
        draw_with_symmetry(paint, pos);
        return;
    }

    // Apply alpha to color
    sfColor draw_color = paint->current_color;

    switch (paint->current_tool) {
    case TOOL_PEN:
        draw_circle_point(target, pos, draw_color, paint->brush_size);
        sfRenderTexture_display(target);
        composite_layers(paint);
        break;

    case TOOL_FOUNTAIN_PEN: {
        // Calligraphy/fountain pen effect - stroke width varies with angle
        // Calculate angle of movement
        int dx = pos.x - paint->last_pos.x;
        int dy = pos.y - paint->last_pos.y;
        float angle = atan2f(dy, dx);

        // Calculate width variation based on angle
        // Thin when perpendicular to 45° angle, thick when parallel
        float angle_variation = fabsf(sinf(2.0f * (angle - M_PI / 4.0f)));
        float min_width = paint->brush_size * 0.3f;
        float max_width = paint->brush_size * 1.5f;
        float width = min_width + (max_width - min_width) * angle_variation;

        // Draw ellipse at angle
        float perpendicular_angle = angle + M_PI / 2.0f;
        int steps = (int)(width * 2);
        for (int i = -steps; i <= steps; i++) {
            float offset = (float)i / (steps * 2.0f) * width;
            sfVector2i draw_pos = {pos.x + (int)(offset * cosf(perpendicular_angle)), pos.y + (int)(offset * sinf(perpendicular_angle))};
            if (draw_pos.x >= 0 && draw_pos.x < paint->canvas_width && draw_pos.y >= 0 && draw_pos.y < paint->canvas_height) {
                draw_circle_point(target, draw_pos, draw_color, paint->brush_size * 0.4f);
            }
        }
        sfRenderTexture_display(target);
        composite_layers(paint);
        break;
    }

    case TOOL_ERASER:
        draw_circle_point(target, pos, (sfColor){0, 0, 0, 0}, paint->brush_size * 2);
        sfRenderTexture_display(target);
        composite_layers(paint);
        break;

    case TOOL_BRUSH:
        // Soft brush with multiple circles
        for (int i = 0; i < 3; i++) {
            sfColor soft_color = draw_color;
            soft_color.a = (paint->current_color.a * (i * 10)) / 255;
            draw_circle_point(target, pos, soft_color, paint->brush_size + i * 2);
        }
        sfRenderTexture_display(target);
        composite_layers(paint);
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
                draw_circle_point(target, spray_pos, spray_color, 1 + (rand() % 2));
            }
        }
        sfRenderTexture_display(target);
        composite_layers(paint);
        break;

    default:
        break;
    }
}
