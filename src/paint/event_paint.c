/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void handle_drawing(paint_state_t *paint, sfVector2i mouse_pos) {
    if (paint->current_tool == TOOL_LINE || paint->current_tool == TOOL_RECTANGLE || paint->current_tool == TOOL_CIRCLE || paint->current_tool == TOOL_FILL) {
        return; // These tools don't draw continuously
    }

    // Draw line from last position to current position for smooth drawing
    if (paint->last_pos.x != mouse_pos.x || paint->last_pos.y != mouse_pos.y) {
        int steps = fmax(abs(mouse_pos.x - paint->last_pos.x), abs(mouse_pos.y - paint->last_pos.y));

        for (int i = 0; i <= steps; i++) {
            sfVector2i interpolated = {paint->last_pos.x + (mouse_pos.x - paint->last_pos.x) * i / steps, paint->last_pos.y + (mouse_pos.y - paint->last_pos.y) * i / steps};
            draw_with_tool(paint, interpolated);
        }
    }

    paint->last_pos = mouse_pos;
    sfRenderTexture_display(paint->canvas);
}

void finish_shape(paint_state_t *paint) {
    sfVector2i start = paint->shape_preview.start;
    sfVector2i end = paint->shape_preview.end;

    if (paint->current_tool == TOOL_LINE) {
        draw_line(paint->canvas, start, end, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_RECTANGLE) {
        // Draw rectangle outline
        int x1 = fmin(start.x, end.x);
        int y1 = fmin(start.y, end.y);
        int x2 = fmax(start.x, end.x);
        int y2 = fmax(start.y, end.y);

        draw_line(paint->canvas, (sfVector2i){x1, y1}, (sfVector2i){x2, y1}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x2, y1}, (sfVector2i){x2, y2}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x2, y2}, (sfVector2i){x1, y2}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x1, y2}, (sfVector2i){x1, y1}, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_CIRCLE) {
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float radius = sqrt(dx * dx + dy * dy);

        // Draw circle using parametric equation
        for (float angle = 0; angle < 2 * M_PI; angle += 0.01f) {
            sfVector2i point = {start.x + radius * cos(angle), start.y + radius * sin(angle)};
            draw_circle_point(paint->canvas, point, paint->current_color, paint->brush_size);
        }
    }

    sfRenderTexture_display(paint->canvas);
}