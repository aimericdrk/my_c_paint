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
}

void finish_shape(paint_state_t *paint, sfFont *font) {
    // Get current layer
    if (paint->current_layer < 0 || paint->current_layer >= paint->layer_count)
        return;
    sfRenderTexture *target = paint->layers[paint->current_layer].texture;

    sfVector2i start = paint->shape_preview.start;
    sfVector2i end = paint->shape_preview.end;

    if (paint->current_tool == TOOL_LINE) {
        draw_line(target, start, end, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_RECTANGLE) {
        // Draw rectangle outline
        int x1 = fmin(start.x, end.x);
        int y1 = fmin(start.y, end.y);
        int x2 = fmax(start.x, end.x);
        int y2 = fmax(start.y, end.y);

        draw_line(target, (sfVector2i){x1, y1}, (sfVector2i){x2, y1}, paint->current_color, paint->brush_size);
        draw_line(target, (sfVector2i){x2, y1}, (sfVector2i){x2, y2}, paint->current_color, paint->brush_size);
        draw_line(target, (sfVector2i){x2, y2}, (sfVector2i){x1, y2}, paint->current_color, paint->brush_size);
        draw_line(target, (sfVector2i){x1, y2}, (sfVector2i){x1, y1}, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_CIRCLE) {
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float radius = sqrt(dx * dx + dy * dy);

        // Draw circle using parametric equation
        for (float angle = 0; angle < 2 * M_PI; angle += 0.01f) {
            sfVector2i point = {start.x + radius * cos(angle), start.y + radius * sin(angle)};
            draw_circle_point(target, point, paint->current_color, paint->brush_size);
        }
    } else if (paint->current_tool == TOOL_FILLED_CIRCLE) {
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float radius = sqrt(dx * dx + dy * dy);

        // Draw filled circle using sfCircleShape
        sfCircleShape *circle = sfCircleShape_create();
        sfCircleShape_setRadius(circle, radius);
        sfCircleShape_setPosition(circle, (sfVector2f){start.x - radius, start.y - radius});
        sfCircleShape_setFillColor(circle, paint->current_color);
        sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
        sfRenderTexture_drawCircleShape(target, circle, &states);
        sfCircleShape_destroy(circle);
    } else if (paint->current_tool == TOOL_FILLED_RECTANGLE) {
        // Draw filled rectangle
        int x1 = fmin(start.x, end.x);
        int y1 = fmin(start.y, end.y);
        int x2 = fmax(start.x, end.x);
        int y2 = fmax(start.y, end.y);
        int width = x2 - x1;
        int height = y2 - y1;

        sfRectangleShape *rect = sfRectangleShape_create();
        sfRectangleShape_setPosition(rect, (sfVector2f){x1, y1});
        sfRectangleShape_setSize(rect, (sfVector2f){width, height});
        sfRectangleShape_setFillColor(rect, paint->current_color);
        sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
        sfRenderTexture_drawRectangleShape(target, rect, &states);
        sfRectangleShape_destroy(rect);
    } else if (paint->current_tool == TOOL_TEXT) {
        // Draw text at the starting position
        if (strlen(paint->text_buffer) > 0 && font) {
            // Create text object
            sfText *text = sfText_create(font);
            sfText_setString(text, paint->text_buffer);
            sfText_setCharacterSize(text, paint->brush_size * 3);
            sfText_setFillColor(text, paint->current_color);
            sfText_setPosition(text, (sfVector2f){start.x, start.y});

            sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
            sfRenderTexture_drawText(target, text, &states);
            sfText_destroy(text);

            // Clear text buffer and deactivate input
            paint->text_buffer[0] = '\0';
            paint->text_input_active = 0;
            printf("Text drawn: deactivated input\n");
        }
    } else if (paint->current_tool == TOOL_GRADIENT) {
        // Draw gradient line from start to end
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float length = sqrt(dx * dx + dy * dy);
        int steps = (int)length;

        for (int i = 0; i <= steps; i++) {
            float t = (float)i / steps;
            sfVector2i pos = {start.x + (int)(dx * t), start.y + (int)(dy * t)};

            // Interpolate color from current color to transparent/black
            sfColor gradient_color = paint->current_color;
            gradient_color.a = (uint8_t)(paint->current_color.a * (1.0f - t));

            draw_circle_point(target, pos, gradient_color, paint->brush_size);
        }
    } else if (paint->current_tool == TOOL_STAR) {
        // Draw star shape with 5 points
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float outer_radius = sqrt(dx * dx + dy * dy);
        float inner_radius = outer_radius * 0.4f;
        int points = 5;

        sfVector2i star_points[10];
        for (int i = 0; i < points * 2; i++) {
            float angle = (float)i * M_PI / points - M_PI / 2.0f;
            float radius = (i % 2 == 0) ? outer_radius : inner_radius;
            star_points[i].x = start.x + (int)(radius * cos(angle));
            star_points[i].y = start.y + (int)(radius * sin(angle));
        }

        // Draw lines between consecutive points
        for (int i = 0; i < points * 2; i++) {
            int next = (i + 1) % (points * 2);
            draw_line(target, star_points[i], star_points[next], paint->current_color, paint->brush_size);
        }
    } else if (paint->current_tool == TOOL_SELECT_RECT) {
        // Draw selection rectangle with dashed outline
        int x1 = fmin(start.x, end.x);
        int y1 = fmin(start.y, end.y);
        int x2 = fmax(start.x, end.x);
        int y2 = fmax(start.y, end.y);

        sfColor select_color = {100, 150, 255, 255}; // Blue selection color

        // Draw dashed lines
        int dash_length = 5;
        for (int x = x1; x <= x2; x += dash_length * 2) {
            int end_x = fmin(x + dash_length, x2);
            draw_line(target, (sfVector2i){x, y1}, (sfVector2i){end_x, y1}, select_color, 1);
            draw_line(target, (sfVector2i){x, y2}, (sfVector2i){end_x, y2}, select_color, 1);
        }
        for (int y = y1; y <= y2; y += dash_length * 2) {
            int end_y = fmin(y + dash_length, y2);
            draw_line(target, (sfVector2i){x1, y}, (sfVector2i){x1, end_y}, select_color, 1);
            draw_line(target, (sfVector2i){x2, y}, (sfVector2i){x2, end_y}, select_color, 1);
        }
    }

    sfRenderTexture_display(target);
    composite_layers(paint);
}