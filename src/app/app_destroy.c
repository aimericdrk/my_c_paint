/*
** CCU PROJECT, 2026
** my c paint
** File description:
** Cleanup functions for freeing app resources
*/

#include "paint.h"

void cleanup_app(app_t *app) {
    if (!app)
        return;

    if (app->paint)
        cleanup_paint_state(app->paint);
    if (app->ui)
        cleanup_ui(app->ui);
    if (app->ai_chat)
        cleanup_ai_chat(app->ai_chat);
    if (app->cursor_blink_clock)
        sfClock_destroy(app->cursor_blink_clock);
    if (app->font)
        sfFont_destroy(app->font);
    if (app->config)
        free_ui_config(app->config);
    if (app->window)
        sfRenderWindow_destroy(app->window);

    free(app);
}