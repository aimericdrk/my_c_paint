#include "../include/paint.h"

// Helper function to scale mouse position based on actual window size
sfVector2i get_scaled_mouse_pos(app_t *app) {
    sfVector2i raw_pos = sfMouse_getPositionRenderWindow(app->window);
    sfVector2u window_size = sfRenderWindow_getSize(app->window);

    // Scale coordinates based on actual window size vs expected size
    sfVector2i scaled_pos;
    scaled_pos.x = (int)(raw_pos.x * (float)app->window_width / window_size.x);
    scaled_pos.y = (int)(raw_pos.y * (float)app->window_height / window_size.y);

    return scaled_pos;
}

app_t *init_app(void) {
    app_t *app = malloc(sizeof(app_t));
    if (!app)
        return NULL;

    // Load UI configuration
    app->config = load_ui_config("config/ui_config.json");
    if (!app->config) {
        printf("Failed to load config, using defaults\n");
        // Continue with defaults
    }

    // Get window configuration
    cJSON *window_cfg = app->config ? config_get_section(app->config, "window") : NULL;
    app->window_width = config_get_int(window_cfg, "width", 1400);
    app->window_height = config_get_int(window_cfg, "height", 920);
    const char *window_title = config_get_string(window_cfg, "title", "C-Paint - Professional Edition");
    int framerate = config_get_int(window_cfg, "framerate", 60);

    // Get canvas configuration
    cJSON *canvas_cfg = app->config ? config_get_section(app->config, "canvas") : NULL;
    app->canvas_width = config_get_int(canvas_cfg, "width", 1100);
    app->canvas_height = config_get_int(canvas_cfg, "height", 730);
    app->canvas_x = config_get_int(canvas_cfg, "x", 50);
    app->canvas_y = config_get_int(canvas_cfg, "y", 90);

    // Get toolbar configuration
    cJSON *toolbar_cfg = app->config ? config_get_section(app->config, "toolbar") : NULL;
    app->toolbar_x = config_get_int(toolbar_cfg, "x", 1150);

    // Get topbar configuration
    cJSON *topbar_cfg = app->config ? config_get_section(app->config, "topbar") : NULL;
    app->topbar_height = config_get_int(topbar_cfg, "height", 40);

    sfVideoMode mode = {{app->window_width, app->window_height}, 32};
    app->window = sfRenderWindow_create(mode, window_title, sfResize | sfClose, sfWindowed, NULL);
    if (!app->window) {
        free_ui_config(app->config);
        free(app);
        return NULL;
    }

    sfRenderWindow_setFramerateLimit(app->window, framerate);
    sfRenderWindow_setMouseCursorVisible(app->window, false);

    app->font = sfFont_createFromFile("/System/Library/Fonts/Helvetica.ttc");
    if (!app->font) {
        app->font = sfFont_createFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

    app->paint = init_paint_state(app->canvas_width, app->canvas_height, app->canvas_x, app->canvas_y);
    app->ui = init_ui(app->font, app->config);
    app->dragging_slider = -1;
    app->dropdown_open = 0;
    app->options_dropdown_open = 0;

    if (!app->paint || !app->ui) {
        cleanup_app(app);
        return NULL;
    }

    return app;
}

void cleanup_app(app_t *app) {
    if (!app)
        return;

    if (app->paint)
        cleanup_paint_state(app->paint);
    if (app->ui)
        cleanup_ui(app->ui);
    if (app->font)
        sfFont_destroy(app->font);
    if (app->config)
        free_ui_config(app->config);
    if (app->window)
        sfRenderWindow_destroy(app->window);

    free(app);
}

void handle_events(app_t *app) {
    sfEvent event;

    while (sfRenderWindow_pollEvent(app->window, &event)) {
        if (event.type == sfEvtClosed) {
            sfRenderWindow_close(app->window);
        }

        // Handle text input for file explorer
        if (event.type == sfEvtTextEntered) {
            if (app->ui->file_explorer && app->ui->file_explorer->mode == FILE_EXPLORER_SAVE) {
                handle_file_explorer_text(app->ui->file_explorer, event.text.unicode);
            }
        }

        // Handle mouse wheel scrolling for file explorer
        if (event.type == sfEvtMouseWheelScrolled) {
            if (app->ui->file_explorer && app->ui->file_explorer->mode != FILE_EXPLORER_CLOSED) {
                int max_scroll = app->ui->file_explorer->entry_count - 15;
                if (max_scroll < 0)
                    max_scroll = 0;

                // Scroll up
                if (event.mouseWheelScroll.delta > 0) {
                    app->ui->file_explorer->scroll_offset -= 3;
                    if (app->ui->file_explorer->scroll_offset < 0)
                        app->ui->file_explorer->scroll_offset = 0;
                }
                // Scroll down
                else if (event.mouseWheelScroll.delta < 0) {
                    app->ui->file_explorer->scroll_offset += 3;
                    if (app->ui->file_explorer->scroll_offset > max_scroll)
                        app->ui->file_explorer->scroll_offset = max_scroll;
                }
            }
        }

        if (event.type == sfEvtKeyPressed) {
            // Handle file explorer keyboard navigation
            if (app->ui->file_explorer && app->ui->file_explorer->mode == FILE_EXPLORER_SAVE) {
                handle_file_explorer_keypress(app->ui->file_explorer, event.key.code);
            }

            // Toggle debug
            if (event.key.code == sfKeyF1) {
                app->paint->show_debug = !app->paint->show_debug;
            }

            // Undo/Redo
            if (event.key.control && event.key.code == sfKeyZ) {
                undo(app->paint);
                printf("Undo performed\n");
            }
            if (event.key.control && event.key.code == sfKeyY) {
                redo(app->paint);
                printf("Redo performed\n");
            }

            // Clear canvas
            if (event.key.control && event.key.code == sfKeyN) {
                save_undo_state(app->paint);
                clear_canvas(app->paint);
                printf("Canvas cleared\n");
            }

            // Save
            if (event.key.control && event.key.code == sfKeyS) {
                save_to_file(app->paint, "output.png");
                printf("Saved to output.png\n");
            }

            // Open
            if (event.key.control && event.key.code == sfKeyO) {
                load_from_file(app->paint, "output.png");
                printf("Loaded from output.png\n");
            }

            // Symmetry modes (M key to cycle)
            if (event.key.code == sfKeyM) {
                app->paint->symmetry_mode = (app->paint->symmetry_mode + 1) % 6;
                const char *modes[] = {"None", "Horizontal", "Vertical", "Both", "Radial 4", "Radial 8"};
                printf("Symmetry mode: %s\n", modes[app->paint->symmetry_mode]);
            }

            // Flip canvas horizontally (H key)
            if (event.key.code == sfKeyH) {
                save_undo_state(app->paint);
                flip_canvas_horizontal(app->paint);
            }

            // Flip canvas vertically (V key)
            if (event.key.code == sfKeyV) {
                save_undo_state(app->paint);
                flip_canvas_vertical(app->paint);
            }

            // Toggle grid (G key)
            if (event.key.code == sfKeyG) {
                app->paint->show_grid = !app->paint->show_grid;
                printf("Grid %s\n", app->paint->show_grid ? "ON" : "OFF");
            }

            // Toggle snap to grid (Shift+G)
            if (event.key.shift && event.key.code == sfKeyG) {
                app->paint->snap_to_grid = !app->paint->snap_to_grid;
                printf("Snap to grid %s\n", app->paint->snap_to_grid ? "ON" : "OFF");
            }
        }

        if (event.type == sfEvtMouseButtonPressed) {
            sfVector2i mouse_pos = get_scaled_mouse_pos(app);

            if (event.mouseButton.button == sfMouseLeft) {
                // Check if file explorer is open - handle its clicks first
                if (app->ui->file_explorer && app->ui->file_explorer->mode != FILE_EXPLORER_CLOSED) {
                    handle_ui_click(app, mouse_pos);
                }
                // Check if clicking on top bar (File menu area)
                else if (mouse_pos.y < app->topbar_height) {
                    handle_ui_click(app, mouse_pos);
                }
                // Check if clicking on file dropdown menu (when open)
                else if (app->dropdown_open && mouse_pos.x >= 10 && mouse_pos.x <= 160 && mouse_pos.y >= 40 && mouse_pos.y <= 145) {
                    handle_ui_click(app, mouse_pos);
                }
                // Check if clicking on options dropdown menu (when open)
                else if (app->options_dropdown_open && mouse_pos.x >= 100 && mouse_pos.x <= 320 && mouse_pos.y >= 40 && mouse_pos.y <= 250) {
                    handle_ui_click(app, mouse_pos);
                }
                // Check if clicking on UI toolbar
                else if (mouse_pos.x >= app->toolbar_x) {
                    handle_ui_click(app, mouse_pos);
                } else if (mouse_pos.x >= app->canvas_x && mouse_pos.x < app->canvas_x + app->canvas_width && mouse_pos.y >= app->canvas_y && mouse_pos.y < app->canvas_y + app->canvas_height) {
                    // Close dropdowns if open
                    if (app->dropdown_open || app->options_dropdown_open) {
                        app->dropdown_open = 0;
                        app->options_dropdown_open = 0;
                    } else {
                        // Start drawing on canvas
                        save_undo_state(app->paint);
                        app->paint->is_drawing = 1;
                        app->paint->last_pos = (sfVector2i){mouse_pos.x - app->canvas_x, mouse_pos.y - app->canvas_y};

                        if (app->paint->current_tool == TOOL_LINE || app->paint->current_tool == TOOL_RECTANGLE || app->paint->current_tool == TOOL_CIRCLE) {
                            app->paint->shape_preview.start = app->paint->last_pos;
                            app->paint->shape_preview.active = 1;
                        } else if (app->paint->current_tool == TOOL_FILL) {
                            fill_canvas(app->paint, app->paint->last_pos,
                                        get_pixel_color(app->paint->canvas, app->paint->last_pos.x, app->paint->last_pos.y, app->paint->canvas_width, app->paint->canvas_height));
                        } else {
                            draw_with_tool(app->paint, app->paint->last_pos);
                        }
                    }
                } else {
                    // Click elsewhere, close dropdowns
                    if (app->dropdown_open || app->options_dropdown_open) {
                        app->dropdown_open = 0;
                        app->options_dropdown_open = 0;
                    }
                }
            } else if (event.mouseButton.button == sfMouseRight) {
                // Color picker from canvas
                if (mouse_pos.x >= app->canvas_x && mouse_pos.x < app->canvas_x + app->canvas_width && mouse_pos.y >= app->canvas_y && mouse_pos.y < app->canvas_y + app->canvas_height) {
                    sfVector2i canvas_pos = {mouse_pos.x - app->canvas_x, mouse_pos.y - app->canvas_y};
                    app->paint->current_color = get_pixel_color(app->paint->canvas, canvas_pos.x, canvas_pos.y, app->paint->canvas_width, app->paint->canvas_height);
                    printf("Picked color: R=%d G=%d B=%d\n", app->paint->current_color.r, app->paint->current_color.g, app->paint->current_color.b);
                }
            }
        }

        if (event.type == sfEvtMouseButtonReleased) {
            if (event.mouseButton.button == sfMouseLeft) {
                app->paint->is_drawing = 0;
                app->dragging_slider = -1;

                // Stop dragging scrollbar
                if (app->ui->file_explorer && app->ui->file_explorer->dragging_scrollbar) {
                    app->ui->file_explorer->dragging_scrollbar = 0;
                }

                if (app->paint->shape_preview.active) {
                    finish_shape(app->paint);
                    app->paint->shape_preview.active = 0;
                }
            }
        }
    }
}

void update(app_t *app) {
    sfVector2i mouse_pos = get_scaled_mouse_pos(app);

    // Handle drawing
    if (app->paint->is_drawing && mouse_pos.x >= app->canvas_x && mouse_pos.x < app->canvas_x + app->canvas_width && mouse_pos.y >= app->canvas_y && mouse_pos.y < app->canvas_y + app->canvas_height) {

        sfVector2i canvas_pos = {mouse_pos.x - app->canvas_x, mouse_pos.y - app->canvas_y};

        if (app->paint->current_tool == TOOL_LINE || app->paint->current_tool == TOOL_RECTANGLE || app->paint->current_tool == TOOL_CIRCLE) {
            app->paint->shape_preview.end = canvas_pos;
        } else {
            handle_drawing(app->paint, canvas_pos);
        }
    }

    // Handle slider dragging
    if (app->dragging_slider >= 0) {
        update_color_slider(app, mouse_pos);
    }

    // Handle file explorer scrollbar dragging
    if (app->ui->file_explorer && app->ui->file_explorer->dragging_scrollbar) {
        file_explorer_t *explorer = app->ui->file_explorer;
        if (explorer->entry_count > 15) {
            // Calculate scroll position based on mouse Y position
            sfVector2f scrollbar_pos = sfRectangleShape_getPosition(explorer->scrollbar);
            sfVector2f scrollbar_size = sfRectangleShape_getSize(explorer->scrollbar);

            float thumb_ratio = 15.0f / explorer->entry_count;
            float thumb_height = thumb_ratio * scrollbar_size.y;
            if (thumb_height < 30)
                thumb_height = 30;

            float available_height = scrollbar_size.y - thumb_height;
            float mouse_offset = mouse_pos.y - scrollbar_pos.y - (thumb_height / 2);

            if (mouse_offset < 0)
                mouse_offset = 0;
            if (mouse_offset > available_height)
                mouse_offset = available_height;

            float scroll_ratio = mouse_offset / available_height;
            int max_scroll = explorer->entry_count - 15;
            explorer->scroll_offset = (int)(scroll_ratio * max_scroll);

            if (explorer->scroll_offset < 0)
                explorer->scroll_offset = 0;
            if (explorer->scroll_offset > max_scroll)
                explorer->scroll_offset = max_scroll;
        }
    }

    // Update FPS
    app->paint->frame_count++;
    float elapsed = sfTime_asSeconds(sfClock_getElapsedTime(app->paint->fps_clock));
    if (elapsed >= 1.0f) {
        app->paint->fps = app->paint->frame_count / elapsed;
        app->paint->frame_count = 0;
        sfClock_restart(app->paint->fps_clock);
    }
}

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

void run_app(app_t *app) {
    while (sfRenderWindow_isOpen(app->window)) {
        handle_events(app);
        update(app);
        render(app);
    }
}
