/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI command queue processor for animated drawing
*/

#include "paint.h"

// Process AI command queue - animate drawing with smooth progress
void process_ai_command_queue(app_t *app) {
    if (!app || !app->ai_chat)
        return;

    // Check if there are commands to execute
    if (app->ai_chat->current_command >= app->ai_chat->command_count) {
        if (app->ai_chat->show_draw_cursor) {
            app->ai_chat->show_draw_cursor = 0;
        }
        return;
    }

    // Get current command
    ai_command_t *cmd = &app->ai_chat->command_queue[app->ai_chat->current_command];

    // Update animation progress
    float elapsed = sfTime_asSeconds(sfClock_getElapsedTime(app->ai_chat->command_timer));
    float animation_speed = 0.5f; // 0.5 seconds per command for smooth animation

    if (cmd->is_animated) {
        // Smooth animation progress
        cmd->progress = fminf(elapsed / animation_speed, 1.0f);

        // Execute with partial progress for animation
        if (cmd->progress < 1.0f) {
            execute_drawing_command(app, cmd->command, cmd->progress);
            app->ai_chat->show_draw_cursor = 1; // Show cursor during animation
            return;                             // Keep animating current command
        }
    }

    // Command complete - execute final state if not already done
    if (cmd->progress >= 1.0f) {
        cmd->progress = 1.0f;
        execute_drawing_command(app, cmd->command, cmd->progress);

        printf("Completed command %d/%d: %s\n", app->ai_chat->current_command + 1, app->ai_chat->command_count, cmd->command);

        // Move to next command
        app->ai_chat->current_command++;

        // Reset timer and progress for next command
        if (app->ai_chat->current_command < app->ai_chat->command_count) {
            app->ai_chat->command_queue[app->ai_chat->current_command].progress = 0.0f;
            sfClock_restart(app->ai_chat->command_timer);
        }
    }
}
