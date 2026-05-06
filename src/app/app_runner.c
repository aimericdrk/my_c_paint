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

    // Process AI command queue for animated drawing
    process_ai_command_queue(app);

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

    // Handle layer opacity dragging
    if (app->ui->dragging_layer_opacity >= 0 && app->ui->dragging_layer_opacity < app->paint->layer_count) {
        ui_config_t *config = app->config;
        cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
        cJSON *layers_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "layers") : NULL;
        int layer_panel_x = app->toolbar_x + config_get_int(layers_cfg, "panel_x", 15);
        int layer_panel_y = config_get_int(layers_cfg, "panel_y", 115);
        int layer_item_height = config_get_int(layers_cfg, "item_height", 50);
        int layer_item_spacing = config_get_int(layers_cfg, "item_spacing", 5);
        int max_visible = config_get_int(layers_cfg, "max_visible_items", 10);

        int visible_idx = app->ui->dragging_layer_opacity - app->ui->layer_panel_scroll_offset;
        if (visible_idx >= 0 && visible_idx < max_visible) {
            sfVector2f opacity_bar_pos = {layer_panel_x + 40, layer_panel_y + visible_idx * (layer_item_height + layer_item_spacing) + 30};
            float percent = (mouse_pos.x - opacity_bar_pos.x) / 180.0f;
            if (percent < 0)
                percent = 0;
            if (percent > 1)
                percent = 1;
            uint8_t opacity = (uint8_t)(percent * 255);
            set_layer_opacity(app->paint, app->ui->dragging_layer_opacity, opacity);
        }
    }

    // Handle layer scrollbar dragging
    if (app->dragging_layer_scrollbar) {
        ui_config_t *config = app->config;
        cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
        cJSON *layers_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "layers") : NULL;
        int layer_panel_y = config_get_int(layers_cfg, "panel_y", 115);
        int layer_item_height = config_get_int(layers_cfg, "item_height", 50);
        int layer_item_spacing = config_get_int(layers_cfg, "item_spacing", 5);
        int max_visible = config_get_int(layers_cfg, "max_visible_items", 10);
        int scrollbar_height = max_visible * (layer_item_height + layer_item_spacing);

        float percent = (mouse_pos.y - layer_panel_y) / (float)scrollbar_height;
        app->ui->layer_panel_scroll_offset = (int)(percent * (app->paint->layer_count - max_visible));
        if (app->ui->layer_panel_scroll_offset < 0)
            app->ui->layer_panel_scroll_offset = 0;
        if (app->ui->layer_panel_scroll_offset > app->paint->layer_count - max_visible)
            app->ui->layer_panel_scroll_offset = app->paint->layer_count - max_visible;
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
