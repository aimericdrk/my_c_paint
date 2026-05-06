/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

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
    app->ai_chat = init_ai_chat();
    app->cursor_blink_clock = sfClock_create();
    app->dragging_slider = -1;
    app->dropdown_open = 0;
    app->options_dropdown_open = 0;
    app->active_tab = TAB_PEN;
    app->dragging_layer_scrollbar = 0;

    if (!app->paint || !app->ui || !app->ai_chat || !app->cursor_blink_clock) {
        cleanup_app(app);
        return NULL;
    }

    return app;
}