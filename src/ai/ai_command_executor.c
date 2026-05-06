/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI command queue processor for animated drawing
*/

#include "paint.h"

void execute_drawing_command(app_t *app, const char *command, float progress) {
    if (!command || !app || !app->paint)
        return;

    char cmd_type[32];
    sscanf(command, "%s", cmd_type);

    if (progress >= 1.0f) {
        printf("Executing: %s (layer %d/%d) [FINAL]\n", cmd_type, app->paint->current_layer, app->paint->layer_count);
    }

    if (strcmp(cmd_type, "DRAW_LINE") == 0) {
        int x1, y1, x2, y2, r, g, b;
        if (sscanf(command, "DRAW_LINE %d %d %d %d %d %d %d", &x1, &y1, &x2, &y2, &r, &g, &b) == 7) {
            // Interpolate the line endpoint based on progress
            int cur_x = x1 + (int)((x2 - x1) * progress);
            int cur_y = y1 + (int)((y2 - y1) * progress);

            // Update cursor position to follow the drawing
            app->ai_chat->draw_cursor_pos = (sfVector2i){cur_x + app->canvas_x, cur_y + app->canvas_y};
            app->ai_chat->show_draw_cursor = (progress < 1.0f);

            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                draw_line(app->paint->layers[app->paint->current_layer].texture, (sfVector2i){x1, y1}, (sfVector2i){cur_x, cur_y}, color, app->paint->brush_size);
                sfRenderTexture_display(app->paint->layers[app->paint->current_layer].texture);
                composite_layers(app->paint);
                if (progress >= 1.0f) {
                    printf("Line drawn from (%d,%d) to (%d,%d)\n", x1, y1, x2, y2);
                }
            }
        }
    } else if (strcmp(cmd_type, "DRAW_CIRCLE") == 0) {
        int x, y, radius, r, g, b;
        if (sscanf(command, "DRAW_CIRCLE %d %d %d %d %d %d", &x, &y, &radius, &r, &g, &b) == 6) {
            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                // Draw arc based on progress (0 to 2*PI)
                float max_angle = 2 * M_PI * progress;
                for (float angle = 0; angle < max_angle; angle += 0.01f) {
                    sfVector2i point = {x + radius * cos(angle), y + radius * sin(angle)};
                    draw_circle_point(app->paint->layers[app->paint->current_layer].texture, point, color, app->paint->brush_size);
                }

                // Update cursor to follow the arc
                float cur_angle = max_angle;
                int cur_x = x + radius * cos(cur_angle);
                int cur_y = y + radius * sin(cur_angle);
                app->ai_chat->draw_cursor_pos = (sfVector2i){cur_x + app->canvas_x, cur_y + app->canvas_y};
                app->ai_chat->show_draw_cursor = (progress < 1.0f);

                sfRenderTexture_display(app->paint->layers[app->paint->current_layer].texture);
                composite_layers(app->paint);
                if (progress >= 1.0f) {
                    printf("Circle drawn at (%d,%d) radius %d\n", x, y, radius);
                }
            }
        }
    } else if (strcmp(cmd_type, "DRAW_FILLED_CIRCLE") == 0) {
        int x, y, radius, r, g, b;
        if (sscanf(command, "DRAW_FILLED_CIRCLE %d %d %d %d %d %d", &x, &y, &radius, &r, &g, &b) == 6) {
            // Animate by growing the circle from 0 to full radius
            int animated_radius = (int)(radius * progress);
            app->ai_chat->draw_cursor_pos = (sfVector2i){x + app->canvas_x, y + app->canvas_y};
            app->ai_chat->show_draw_cursor = (progress < 1.0f);

            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;

                // Draw filled circle using sfCircleShape for better performance
                sfCircleShape *circle = sfCircleShape_create();
                sfCircleShape_setRadius(circle, animated_radius);
                sfCircleShape_setPosition(circle, (sfVector2f){x - animated_radius, y - animated_radius});
                sfCircleShape_setFillColor(circle, color);
                sfRenderTexture_drawCircleShape(target, circle, NULL);
                sfCircleShape_destroy(circle);

                sfRenderTexture_display(target);
                composite_layers(app->paint);
                if (progress >= 1.0f) {
                    printf("Filled circle drawn at (%d,%d) radius %d\n", x, y, radius);
                }
            }
        }
    } else if (strcmp(cmd_type, "DRAW_RECT") == 0) {
        int x, y, width, height, r, g, b;
        if (sscanf(command, "DRAW_RECT %d %d %d %d %d %d %d", &x, &y, &width, &height, &r, &g, &b) == 7) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){x + app->canvas_x, y + app->canvas_y};
            app->ai_chat->show_draw_cursor = (progress < 1.0f);

            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count && progress >= 1.0f) {
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;
                draw_line(target, (sfVector2i){x, y}, (sfVector2i){x + width, y}, color, app->paint->brush_size);
                draw_line(target, (sfVector2i){x + width, y}, (sfVector2i){x + width, y + height}, color, app->paint->brush_size);
                draw_line(target, (sfVector2i){x + width, y + height}, (sfVector2i){x, y + height}, color, app->paint->brush_size);
                draw_line(target, (sfVector2i){x, y + height}, (sfVector2i){x, y}, color, app->paint->brush_size);
                sfRenderTexture_display(target);
                composite_layers(app->paint);
                printf("Rectangle drawn at (%d,%d) size %dx%d\n", x, y, width, height);
            }
        }
    } else if (strcmp(cmd_type, "DRAW_FILLED_RECT") == 0 || strcmp(cmd_type, "FILL_RECT") == 0) {
        int x, y, width, height, r, g, b;
        if (sscanf(command, "DRAW_FILLED_RECT %d %d %d %d %d %d %d", &x, &y, &width, &height, &r, &g, &b) == 7 ||
            sscanf(command, "FILL_RECT %d %d %d %d %d %d %d", &x, &y, &width, &height, &r, &g, &b) == 7) {
            // Animate by growing from center
            int animated_width = (int)(width * progress);
            int animated_height = (int)(height * progress);
            int offset_x = (width - animated_width) / 2;
            int offset_y = (height - animated_height) / 2;

            app->ai_chat->draw_cursor_pos = (sfVector2i){x + width / 2 + app->canvas_x, y + height / 2 + app->canvas_y};
            app->ai_chat->show_draw_cursor = (progress < 1.0f);

            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;

                // Draw filled rectangle using sfRectangleShape for better performance
                sfRectangleShape *rect = sfRectangleShape_create();
                sfRectangleShape_setPosition(rect, (sfVector2f){x + offset_x, y + offset_y});
                sfRectangleShape_setSize(rect, (sfVector2f){animated_width, animated_height});
                sfRectangleShape_setFillColor(rect, color);
                sfRenderTexture_drawRectangleShape(target, rect, NULL);
                sfRectangleShape_destroy(rect);

                sfRenderTexture_display(target);
                composite_layers(app->paint);
                if (progress >= 1.0f) {
                    printf("Filled rectangle drawn at (%d,%d) size %dx%d\n", x, y, width, height);
                }
            }
        }
    } else if (strcmp(cmd_type, "DRAW_POINT") == 0) {
        int x, y, size, r, g, b;
        if (sscanf(command, "DRAW_POINT %d %d %d %d %d %d", &x, &y, &size, &r, &g, &b) == 6) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){x + app->canvas_x, y + app->canvas_y};
            app->ai_chat->show_draw_cursor = (progress < 1.0f);

            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count && progress >= 1.0f) {
                draw_circle_point(app->paint->layers[app->paint->current_layer].texture, (sfVector2i){x, y}, color, size);
                sfRenderTexture_display(app->paint->layers[app->paint->current_layer].texture);
                composite_layers(app->paint);
                printf("Point drawn at (%d,%d)\n", x, y);
            }
        }
    } else if (strcmp(cmd_type, "DRAW_TRIANGLE") == 0) {
        int x1, y1, x2, y2, x3, y3, r, g, b;
        if (sscanf(command, "DRAW_TRIANGLE %d %d %d %d %d %d %d %d %d", &x1, &y1, &x2, &y2, &x3, &y3, &r, &g, &b) == 9) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){x1 + app->canvas_x, y1 + app->canvas_y};
            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                printf("Drawing triangle (%d,%d) (%d,%d) (%d,%d)\n", x1, y1, x2, y2, x3, y3);
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;
                draw_line(target, (sfVector2i){x1, y1}, (sfVector2i){x2, y2}, color, app->paint->brush_size);
                draw_line(target, (sfVector2i){x2, y2}, (sfVector2i){x3, y3}, color, app->paint->brush_size);
                draw_line(target, (sfVector2i){x3, y3}, (sfVector2i){x1, y1}, color, app->paint->brush_size);
                sfRenderTexture_display(target);
                composite_layers(app->paint);
                printf("Triangle drawn and composited\n");
            }
        }
    } else if (strcmp(cmd_type, "DRAW_FILLED_TRIANGLE") == 0) {
        int x1, y1, x2, y2, x3, y3, r, g, b;
        if (sscanf(command, "DRAW_FILLED_TRIANGLE %d %d %d %d %d %d %d %d %d", &x1, &y1, &x2, &y2, &x3, &y3, &r, &g, &b) == 9) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){x1 + app->canvas_x, y1 + app->canvas_y};
            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                printf("Drawing filled triangle (%d,%d) (%d,%d) (%d,%d) RGB(%d,%d,%d)\n", x1, y1, x2, y2, x3, y3, r, g, b);
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;

                // Sort vertices by y coordinate
                if (y2 < y1) {
                    int tx = x1, ty = y1;
                    x1 = x2;
                    y1 = y2;
                    x2 = tx;
                    y2 = ty;
                }
                if (y3 < y1) {
                    int tx = x1, ty = y1;
                    x1 = x3;
                    y1 = y3;
                    x3 = tx;
                    y3 = ty;
                }
                if (y3 < y2) {
                    int tx = x2, ty = y2;
                    x2 = x3;
                    y2 = y3;
                    x3 = tx;
                    y3 = ty;
                }

                // Helper function to draw scanline
                for (int y = y1; y <= y3; y++) {
                    int x_left = x1, x_right = x1;

                    // Calculate x on left and right edges
                    if (y <= y2) {
                        // Upper part: interpolate between v1-v2 and v1-v3
                        if (y2 - y1 != 0)
                            x_left = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                        else
                            x_left = x1;

                        if (y3 - y1 != 0)
                            x_right = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
                        else
                            x_right = x1;
                    } else {
                        // Lower part: interpolate between v2-v3 and v1-v3
                        if (y3 - y2 != 0)
                            x_left = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
                        else
                            x_left = x2;

                        if (y3 - y1 != 0)
                            x_right = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
                        else
                            x_right = x1;
                    }

                    // Ensure x_left is on the left
                    if (x_left > x_right) {
                        int temp = x_left;
                        x_left = x_right;
                        x_right = temp;
                    }

                    // Draw horizontal line
                    if (x_right >= x_left) {
                        draw_line(target, (sfVector2i){x_left, y}, (sfVector2i){x_right, y}, color, 1);
                    }
                }

                sfRenderTexture_display(target);
                composite_layers(app->paint);
                printf("Filled triangle drawn and composited\n");
            }
        }
    } else if (strcmp(cmd_type, "DRAW_ELLIPSE") == 0) {
        int cx, cy, rx, ry, r, g, b;
        if (sscanf(command, "DRAW_ELLIPSE %d %d %d %d %d %d %d", &cx, &cy, &rx, &ry, &r, &g, &b) == 7) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){cx + app->canvas_x, cy + app->canvas_y};
            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                printf("Drawing ellipse at (%d,%d) radii %dx%d\n", cx, cy, rx, ry);
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;
                for (float angle = 0; angle < 2 * M_PI; angle += 0.01f) {
                    int x = cx + rx * cos(angle);
                    int y = cy + ry * sin(angle);
                    draw_circle_point(target, (sfVector2i){x, y}, color, app->paint->brush_size);
                }
                sfRenderTexture_display(target);
                composite_layers(app->paint);
                printf("Ellipse drawn and composited\n");
            }
        }
    } else if (strcmp(cmd_type, "DRAW_CURVE") == 0) {
        int x1, y1, x2, y2, x3, y3, x4, y4, r, g, b;
        if (sscanf(command, "DRAW_CURVE %d %d %d %d %d %d %d %d %d %d %d", &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4, &r, &g, &b) == 11) {
            app->ai_chat->draw_cursor_pos = (sfVector2i){x1 + app->canvas_x, y1 + app->canvas_y};
            sfColor color = {r, g, b, 255};
            if (app->paint->current_layer >= 0 && app->paint->current_layer < app->paint->layer_count) {
                printf("Drawing bezier curve from (%d,%d) to (%d,%d)\n", x1, y1, x4, y4);
                sfRenderTexture *target = app->paint->layers[app->paint->current_layer].texture;
                // Draw bezier curve using parametric form
                int prev_x = x1, prev_y = y1;
                for (float t = 0; t <= 1.0f; t += 0.01f) {
                    float t2 = t * t;
                    float t3 = t2 * t;
                    float mt = 1 - t;
                    float mt2 = mt * mt;
                    float mt3 = mt2 * mt;
                    int x = mt3 * x1 + 3 * mt2 * t * x2 + 3 * mt * t2 * x3 + t3 * x4;
                    int y = mt3 * y1 + 3 * mt2 * t * y2 + 3 * mt * t2 * y3 + t3 * y4;
                    draw_line(target, (sfVector2i){prev_x, prev_y}, (sfVector2i){x, y}, color, app->paint->brush_size);
                    prev_x = x;
                    prev_y = y;
                }
                sfRenderTexture_display(target);
                composite_layers(app->paint);
                printf("Curve drawn and composited\n");
            }
        }
    } else if (strcmp(cmd_type, "SET_BRUSH_SIZE") == 0) {
        int size;
        if (sscanf(command, "SET_BRUSH_SIZE %d", &size) == 1) {
            if (size >= 1 && size <= 50) {
                app->paint->brush_size = size;
                printf("Brush size set to %d\n", size);
            }
        }
    } else if (strcmp(cmd_type, "CLEAR_CANVAS") == 0) {
        printf("Clearing canvas\n");
        save_undo_state(app->paint);
        clear_canvas(app->paint);
        printf("Canvas cleared\n");
    } else {
        printf("ERROR, Unknown command: %s\n", cmd_type);
    }
}
