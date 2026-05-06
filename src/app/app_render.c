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
