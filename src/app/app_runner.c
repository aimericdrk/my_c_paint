/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Application runner functions
*/

#include "paint.h"

void run_app(app_t *app) {
    while (sfRenderWindow_isOpen(app->window)) {
        handle_events(app);
        update(app);
        render(app);
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
