/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI Chat initialization
*/

#include "paint.h"

// Load .env file
static void load_env_file(const char *filename, ai_chat_t *chat) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Warning: .env file not found\n");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\r\n")] = '\0';

        // Skip empty lines and comments
        if (line[0] == '\0' || line[0] == '#')
            continue;

        // Parse KEY=VALUE
        char *equals = strchr(line, '=');
        if (equals) {
            *equals = '\0';
            const char *key = line;
            const char *value = equals + 1;

            if (strcmp(key, "MISTRAL_API_KEY") == 0) {
                strncpy(chat->api_key, value, sizeof(chat->api_key) - 1);
                chat->api_key[sizeof(chat->api_key) - 1] = '\0';
                printf("Loaded MISTRAL_API_KEY from .env\n");
            }
        }
    }

    fclose(file);
}

ai_chat_t *init_ai_chat(void) {
    ai_chat_t *chat = malloc(sizeof(ai_chat_t));
    if (!chat)
        return NULL;

    chat->message_count = 0;
    chat->is_processing = 0;
    chat->api_key[0] = '\0';

    // Initialize command queue
    chat->command_count = 0;
    chat->current_command = 0;
    chat->command_timer = sfClock_create();
    chat->draw_cursor_pos = (sfVector2i){0, 0};
    chat->show_draw_cursor = 0;
    
    // Initialize threading
    pthread_mutex_init(&chat->mutex, NULL);
    chat->thread_active = 0;
    
    // Initialize command queue with animation flags
    for (int i = 0; i < MAX_AI_COMMANDS; i++) {
        chat->command_queue[i].progress = 0.0f;
        chat->command_queue[i].is_animated = 1;
    }

    // Try to load from .env file first
    load_env_file(".env", chat);

    // If not found in .env, try environment variable
    if (chat->api_key[0] == '\0') {
        const char *api_key = getenv("MISTRAL_API_KEY");
        if (api_key) {
            strncpy(chat->api_key, api_key, sizeof(chat->api_key) - 1);
            chat->api_key[sizeof(chat->api_key) - 1] = '\0';
            printf("Loaded MISTRAL_API_KEY from environment\n");
        } else {
            printf("Warning: MISTRAL_API_KEY not found in .env or environment\n");
        }
    }

    // Add welcome message
    add_ai_message(chat, MESSAGE_AI, "Hello! I'm your AI drawing assistant. I can help you create drawings on the canvas. Try asking me to draw something!");

    return chat;
}
