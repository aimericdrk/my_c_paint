/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

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

            // // Symmetry modes (M key to cycle)
            // if (event.key.code == sfKeyM) {
            //     app->paint->symmetry_mode = (app->paint->symmetry_mode + 1) % 6;
            //     const char *modes[] = {"None", "Horizontal", "Vertical", "Both", "Radial 4", "Radial 8"};
            //     printf("Symmetry mode: %s\n", modes[app->paint->symmetry_mode]);
            // }

            // // Flip canvas horizontally (H key)
            // if (event.key.code == sfKeyH) {
            //     save_undo_state(app->paint);
            //     flip_canvas_horizontal(app->paint);
            // }

            // // Flip canvas vertically (V key)
            // if (event.key.code == sfKeyV) {
            //     save_undo_state(app->paint);
            //     flip_canvas_vertical(app->paint);
            // }

            // // Toggle grid (G key)
            // if (event.key.code == sfKeyG) {
            //     app->paint->show_grid = !app->paint->show_grid;
            //     printf("Grid %s\n", app->paint->show_grid ? "ON" : "OFF");
            // }

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