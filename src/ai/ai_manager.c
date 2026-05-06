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

// Sanitize text to remove/replace problematic UTF-8 characters
static void sanitize_text(char *dest, const char *src, size_t max_len) {
    size_t src_idx = 0, dst_idx = 0;
    size_t src_len = strlen(src);

    while (src_idx < src_len && dst_idx < max_len - 1) {
        unsigned char c = (unsigned char)src[src_idx];

        // Handle ASCII printable characters and common whitespace
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            dest[dst_idx++] = src[src_idx++];
        }
        // Handle UTF-8 multibyte sequences - try to convert or skip
        else if (c >= 0x80) {
            // Skip UTF-8 multibyte character by detecting sequence length
            if ((c & 0xE0) == 0xC0) {
                // 2-byte sequence - replace with '?'
                dest[dst_idx++] = '?';
                src_idx += 2;
            } else if ((c & 0xF0) == 0xE0) {
                // 3-byte sequence - replace with '?'
                dest[dst_idx++] = '?';
                src_idx += 3;
            } else if ((c & 0xF8) == 0xF0) {
                // 4-byte sequence - replace with '?'
                dest[dst_idx++] = '?';
                src_idx += 4;
            } else {
                // Invalid UTF-8 or continuation byte - skip
                src_idx++;
            }
        } else {
            // Skip other non-printable characters
            src_idx++;
        }
    }
    dest[dst_idx] = '\0';
}

void add_ai_message(ai_chat_t *chat, message_type_t type, const char *content) {
    if (!chat || !content)
        return;

    // If we've reached the message limit, shift messages to make room
    if (chat->message_count >= MAX_AI_MESSAGES) {
        // Shift all messages down by 1 (remove oldest message)
        for (int i = 1; i < MAX_AI_MESSAGES; i++) {
            chat->messages[i - 1] = chat->messages[i];
        }
        chat->message_count = MAX_AI_MESSAGES - 1;
    }

    ai_message_t *msg = &chat->messages[chat->message_count];
    msg->type = type;

    // Sanitize content to handle UTF-8 issues
    sanitize_text(msg->content, content, MAX_AI_MESSAGE_LENGTH);

    // Add timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(msg->timestamp, sizeof(msg->timestamp), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

    chat->message_count++;
}

void clear_ai_chat(ai_chat_t *chat) {
    if (!chat)
        return;

    chat->message_count = 0;

    // Add a welcome message after clearing
    add_ai_message(chat, MESSAGE_AI, "Chat cleared. How can I help you?");
}
