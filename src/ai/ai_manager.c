/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI Chat initialization
*/

#include "paint.h"

void cleanup_ai_chat(ai_chat_t *chat) {
    if (chat) {
        // Wait for thread to finish if active
        if (chat->thread_active) {
            pthread_join(chat->request_thread, NULL);
        }
        pthread_mutex_destroy(&chat->mutex);
        if (chat->command_timer)
            sfClock_destroy(chat->command_timer);
        free(chat);
    }
}

void add_ai_message(ai_chat_t *chat, message_type_t type, const char *content) {
    if (!chat || chat->message_count >= MAX_AI_MESSAGES)
        return;

    ai_message_t *msg = &chat->messages[chat->message_count];
    msg->type = type;
    strncpy(msg->content, content, MAX_AI_MESSAGE_LENGTH - 1);
    msg->content[MAX_AI_MESSAGE_LENGTH - 1] = '\0';

    // Add timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(msg->timestamp, sizeof(msg->timestamp), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

    chat->message_count++;
}
