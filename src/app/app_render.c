/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Application rendering functions
*/

#include "paint.h"

void render(app_t *app) {
    sfRenderWindow_clear(app->window, (sfColor){40, 40, 45, 255});

    // Render canvas
    sfRenderWindow_drawSprite(app->window, app->paint->canvas_sprite, NULL);

    // Render shape preview
    if (app->paint->shape_preview.active) {
        sfVector2i start = app->paint->shape_preview.start;
        sfVector2i end = app->paint->shape_preview.end;

        if (app->paint->current_tool == TOOL_LINE) {
            sfVertex line[2] = {{{start.x + app->canvas_x, start.y + app->canvas_y}, app->paint->current_color, {0, 0}},
                                {{end.x + app->canvas_x, end.y + app->canvas_y}, app->paint->current_color, {0, 0}}};
            sfRenderWindow_drawPrimitives(app->window, line, 2, sfLines, NULL);
        } else if (app->paint->current_tool == TOOL_RECTANGLE) {
            sfRectangleShape *rect = sfRectangleShape_create();
            sfRectangleShape_setPosition(rect, (sfVector2f){fmin(start.x, end.x) + app->canvas_x, fmin(start.y, end.y) + app->canvas_y});
            sfRectangleShape_setSize(rect, (sfVector2f){abs(end.x - start.x), abs(end.y - start.y)});
            sfRectangleShape_setOutlineColor(rect, app->paint->current_color);
            sfRectangleShape_setOutlineThickness(rect, app->paint->brush_size);
            sfRectangleShape_setFillColor(rect, sfTransparent);
            sfRenderWindow_drawRectangleShape(app->window, rect, NULL);
            sfRectangleShape_destroy(rect);
        } else if (app->paint->current_tool == TOOL_CIRCLE) {
            int dx = end.x - start.x;
            int dy = end.y - start.y;
            float radius = sqrt(dx * dx + dy * dy);
            sfCircleShape *circle = sfCircleShape_create();
            sfCircleShape_setRadius(circle, radius);
            sfCircleShape_setPosition(circle, (sfVector2f){start.x + app->canvas_x - radius, start.y + app->canvas_y - radius});
            sfCircleShape_setOutlineColor(circle, app->paint->current_color);
            sfCircleShape_setOutlineThickness(circle, app->paint->brush_size);
            sfCircleShape_setFillColor(circle, sfTransparent);
            sfRenderWindow_drawCircleShape(app->window, circle, NULL);
            sfCircleShape_destroy(circle);
        } else if (app->paint->current_tool == TOOL_FILLED_CIRCLE) {
            int dx = end.x - start.x;
            int dy = end.y - start.y;
            float radius = sqrt(dx * dx + dy * dy);
            sfCircleShape *circle = sfCircleShape_create();
            sfCircleShape_setRadius(circle, radius);
            sfCircleShape_setPosition(circle, (sfVector2f){start.x + app->canvas_x - radius, start.y + app->canvas_y - radius});
            sfColor preview_color = app->paint->current_color;
            preview_color.a = 128; // Semi-transparent preview
            sfCircleShape_setFillColor(circle, preview_color);
            sfCircleShape_setOutlineColor(circle, app->paint->current_color);
            sfCircleShape_setOutlineThickness(circle, 1);
            sfRenderWindow_drawCircleShape(app->window, circle, NULL);
            sfCircleShape_destroy(circle);
        } else if (app->paint->current_tool == TOOL_FILLED_RECTANGLE) {
            sfRectangleShape *rect = sfRectangleShape_create();
            sfRectangleShape_setPosition(rect, (sfVector2f){fmin(start.x, end.x) + app->canvas_x, fmin(start.y, end.y) + app->canvas_y});
            sfRectangleShape_setSize(rect, (sfVector2f){abs(end.x - start.x), abs(end.y - start.y)});
            sfColor preview_color = app->paint->current_color;
            preview_color.a = 128; // Semi-transparent preview
            sfRectangleShape_setFillColor(rect, preview_color);
            sfRectangleShape_setOutlineColor(rect, app->paint->current_color);
            sfRectangleShape_setOutlineThickness(rect, 1);
            sfRenderWindow_drawRectangleShape(app->window, rect, NULL);
            sfRectangleShape_destroy(rect);
        } else if (app->paint->current_tool == TOOL_TEXT && strlen(app->paint->text_buffer) > 0) {
            // Preview text at start position
            sfText *text = sfText_create(app->font);
            sfText_setString(text, app->paint->text_buffer);
            sfText_setCharacterSize(text, app->paint->brush_size * 3);
            sfColor preview_color = app->paint->current_color;
            preview_color.a = 180; // Semi-transparent preview
            sfText_setFillColor(text, preview_color);
            sfText_setPosition(text, (sfVector2f){start.x + app->canvas_x, start.y + app->canvas_y});
            sfRenderWindow_drawText(app->window, text, NULL);
            sfText_destroy(text);
        } else if (app->paint->current_tool == TOOL_GRADIENT) {
            // Preview gradient line
            sfVertexArray *line = sfVertexArray_create();
            sfVertexArray_setPrimitiveType(line, sfLineStrip);
            int dx = end.x - start.x;
            int dy = end.y - start.y;
            float length = sqrt(dx * dx + dy * dy);
            int steps = (int)length;
            for (int i = 0; i <= fmin(steps, 100); i++) {
                float t = (float)i / fmin(steps, 100);
                sfVertex vertex;
                vertex.position = (sfVector2f){start.x + dx * t + app->canvas_x, start.y + dy * t + app->canvas_y};
                sfColor gradient_color = app->paint->current_color;
                gradient_color.a = (uint8_t)(app->paint->current_color.a * (0.1f - (t * (app->paint->brush_size * app->paint->brush_size))));
                vertex.color = gradient_color;
                sfVertexArray_append(line, vertex);
            }
            sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
            sfRenderWindow_drawVertexArray(app->window, line, &states);
            sfVertexArray_destroy(line);
        } else if (app->paint->current_tool == TOOL_STAR) {
            // Preview star shape
            int dx = end.x - start.x;
            int dy = end.y - start.y;
            float outer_radius = sqrt(dx * dx + dy * dy);
            float inner_radius = outer_radius * 0.4f;
            int points = 5;
            sfVertexArray *star = sfVertexArray_create();
            sfVertexArray_setPrimitiveType(star, sfLineStrip);
            for (int i = 0; i <= points * 2; i++) {
                float angle = (float)(i % (points * 2)) * M_PI / points - M_PI / 2.0f;
                float radius = (i % 2 == 0) ? outer_radius : inner_radius;
                sfVertex vertex;
                vertex.position = (sfVector2f){start.x + radius * cos(angle) + app->canvas_x, start.y + radius * sin(angle) + app->canvas_y};
                vertex.color = app->paint->current_color;
                sfVertexArray_append(star, vertex);
            }
            sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
            sfRenderWindow_drawVertexArray(app->window, star, &states);
            sfVertexArray_destroy(star);
        } else if (app->paint->current_tool == TOOL_SELECT_RECT) {
            // Preview selection rectangle with dashed outline
            sfRectangleShape *rect = sfRectangleShape_create();
            sfRectangleShape_setPosition(rect, (sfVector2f){fmin(start.x, end.x) + app->canvas_x, fmin(start.y, end.y) + app->canvas_y});
            sfRectangleShape_setSize(rect, (sfVector2f){abs(end.x - start.x), abs(end.y - start.y)});
            sfRectangleShape_setFillColor(rect, sfTransparent);
            sfRectangleShape_setOutlineColor(rect, (sfColor){100, 150, 255, 255});
            sfRectangleShape_setOutlineThickness(rect, 2);
            sfRenderWindow_drawRectangleShape(app->window, rect, NULL);
            sfRectangleShape_destroy(rect);
        }
    }

    // Render polygon points if polygon tool is active
    if (app->paint->current_tool == TOOL_POLYGON && app->paint->polygon_point_count > 0) {
        for (int i = 0; i < app->paint->polygon_point_count; i++) {
            sfCircleShape *point = sfCircleShape_create();
            sfCircleShape_setRadius(point, 4);
            sfCircleShape_setPosition(point, (sfVector2f){app->paint->polygon_points[i].x + app->canvas_x - 4, app->paint->polygon_points[i].y + app->canvas_y - 4});
            sfCircleShape_setFillColor(point, app->paint->current_color);
            sfCircleShape_setOutlineColor(point, sfWhite);
            sfCircleShape_setOutlineThickness(point, 1);
            sfRenderWindow_drawCircleShape(app->window, point, NULL);
            sfCircleShape_destroy(point);

            // Draw lines between points
            if (i > 0) {
                sfVertexArray *line = sfVertexArray_create();
                sfVertexArray_setPrimitiveType(line, sfLines);
                sfVertex v1 = {{app->paint->polygon_points[i - 1].x + app->canvas_x, app->paint->polygon_points[i - 1].y + app->canvas_y}, app->paint->current_color, {0, 0}};
                sfVertex v2 = {{app->paint->polygon_points[i].x + app->canvas_x, app->paint->polygon_points[i].y + app->canvas_y}, app->paint->current_color, {0, 0}};
                sfVertexArray_append(line, v1);
                sfVertexArray_append(line, v2);
                sfRenderStates states = {.blendMode = sfBlendAlpha, .transform = sfTransform_Identity, .texture = NULL, .shader = NULL};
                sfRenderWindow_drawVertexArray(app->window, line, &states);
                sfVertexArray_destroy(line);
            }
        }
    }

    // Render cursor preview on canvas
    sfVector2i mouse_pos = get_scaled_mouse_pos(app);
    if (mouse_pos.x >= app->canvas_x && mouse_pos.x < app->canvas_x + app->canvas_width && mouse_pos.y >= app->canvas_y && mouse_pos.y < app->canvas_y + app->canvas_height) {
        sfCircleShape *cursor_preview = sfCircleShape_create();
        float preview_radius = app->paint->brush_size / 2.0f;
        sfCircleShape_setRadius(cursor_preview, preview_radius);
        sfCircleShape_setPosition(cursor_preview, (sfVector2f){mouse_pos.x - preview_radius, mouse_pos.y - preview_radius});
        sfCircleShape_setFillColor(cursor_preview, sfTransparent);
        sfCircleShape_setOutlineColor(cursor_preview, app->paint->current_color);
        sfCircleShape_setOutlineThickness(cursor_preview, 1);
        sfRenderWindow_drawCircleShape(app->window, cursor_preview, NULL);
        sfCircleShape_destroy(cursor_preview);
    }

    // Render grid if enabled
    if (app->paint->show_grid) {
        sfColor grid_color = {100, 100, 100, 100};
        int grid_size = app->paint->grid_size;

        // Vertical lines
        for (int x = 0; x <= app->canvas_width; x += grid_size) {
            sfVertex line[2] = {{{app->canvas_x + x, app->canvas_y}, grid_color, {0, 0}}, {{app->canvas_x + x, app->canvas_y + app->canvas_height}, grid_color, {0, 0}}};
            sfRenderWindow_drawPrimitives(app->window, line, 2, sfLines, NULL);
        }

        // Horizontal lines
        for (int y = 0; y <= app->canvas_height; y += grid_size) {
            sfVertex line[2] = {{{app->canvas_x, app->canvas_y + y}, grid_color, {0, 0}}, {{app->canvas_x + app->canvas_width, app->canvas_y + y}, grid_color, {0, 0}}};
            sfRenderWindow_drawPrimitives(app->window, line, 2, sfLines, NULL);
        }
    }

    // Render AI drawing cursor
    if (app->ai_chat && app->ai_chat->show_draw_cursor) {
        sfCircleShape *ai_cursor = sfCircleShape_create();
        sfCircleShape_setRadius(ai_cursor, 8);
        sfCircleShape_setPosition(ai_cursor, (sfVector2f){app->ai_chat->draw_cursor_pos.x - 8, app->ai_chat->draw_cursor_pos.y - 8});
        sfCircleShape_setFillColor(ai_cursor, (sfColor){255, 100, 0, 150});
        sfCircleShape_setOutlineColor(ai_cursor, (sfColor){255, 150, 0, 255});
        sfCircleShape_setOutlineThickness(ai_cursor, 2);
        sfRenderWindow_drawCircleShape(app->window, ai_cursor, NULL);
        sfCircleShape_destroy(ai_cursor);
    }

    // Render UI
    render_ui(app->window, app);

    // Render debug info
    if (app->paint->show_debug) {
        sfVector2i mouse_pos = get_scaled_mouse_pos(app);
        render_debug_info(app->window, app->paint, app->font, mouse_pos);
    }

    // Render custom cursor (always visible)
    sfVector2i cursor_pos = get_scaled_mouse_pos(app);
    sfCircleShape *custom_cursor = sfCircleShape_create();
    sfCircleShape_setRadius(custom_cursor, 4);
    sfCircleShape_setPosition(custom_cursor, (sfVector2f){cursor_pos.x - 4, cursor_pos.y - 4});
    sfCircleShape_setFillColor(custom_cursor, (sfColor){255, 255, 255, 200});
    sfCircleShape_setOutlineColor(custom_cursor, (sfColor){0, 0, 0, 255});
    sfCircleShape_setOutlineThickness(custom_cursor, 2);
    sfRenderWindow_drawCircleShape(app->window, custom_cursor, NULL);
    sfCircleShape_destroy(custom_cursor);

    sfRenderWindow_display(app->window);
}
