/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI response parser and command executor
*/

#include "paint.h"
#include <math.h>

// Parse JSON response from Mistral AI and extract drawing commands
void parse_and_execute_ai_response(app_t *app, const char *response) {
    if (!response || !app || !app->ai_chat)
        return;

    // Try to parse JSON response
    cJSON *json = cJSON_Parse(response);
    if (!json) {
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Failed to parse JSON response");
        printf("ERROR, JSON Parse Error\n");
        return;
    }

    printf("OK, Parsed JSON successfully\n");

    // Check for error in response
    cJSON *error = cJSON_GetObjectItem(json, "error");
    if (error) {
        cJSON *message = cJSON_GetObjectItem(error, "message");
        if (message && cJSON_IsString(message)) {
            char error_msg[512];
            snprintf(error_msg, sizeof(error_msg), "API Error: %s", message->valuestring);
            add_ai_message(app->ai_chat, MESSAGE_AI, error_msg);
            printf("ERROR, API Error: %s\n", message->valuestring);
        } else {
            add_ai_message(app->ai_chat, MESSAGE_AI, "API Error: Unknown error");
        }
        cJSON_Delete(json);
        return;
    }

    // Extract the AI's message from the response
    cJSON *choices = cJSON_GetObjectItem(json, "choices");
    if (!choices || !cJSON_IsArray(choices) || cJSON_GetArraySize(choices) == 0) {
        // Print the actual JSON structure for debugging
        char *json_str = cJSON_Print(json);
        printf("ERROR, Invalid response format. Received JSON:\n%s\n", json_str);
        free(json_str);
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Invalid response format (no choices array)");
        cJSON_Delete(json);
        return;
    }

    printf("OK, Found choices array with %d items\n", cJSON_GetArraySize(choices));

    cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
    cJSON *message = cJSON_GetObjectItem(first_choice, "message");
    cJSON *content = cJSON_GetObjectItem(message, "content");

    if (!content || !cJSON_IsString(content)) {
        printf("ERROR, No content in message object\n");
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: No content in response");
        cJSON_Delete(json);
        return;
    }

    const char *ai_message = content->valuestring;
    printf("OK, AI Message: %s\n", ai_message);

    // Strip markdown code blocks if present (```json ... ``` or ``` ... ```)
    char json_buffer[MAX_AI_MESSAGE_LENGTH];
    const char *json_start = ai_message;
    const char *json_end = ai_message + strlen(ai_message);

    // Check for markdown code block start
    if (strncmp(json_start, "```json", 7) == 0) {
        json_start = strchr(json_start + 7, '\n');
        if (json_start)
            json_start++; // Skip the newline
    } else if (strncmp(json_start, "```", 3) == 0) {
        json_start = strchr(json_start + 3, '\n');
        if (json_start)
            json_start++; // Skip the newline
    }

    // Check for markdown code block end
    const char *end_marker = strstr(json_start, "```");
    if (end_marker) {
        json_end = end_marker;
    }

    // Copy the extracted JSON
    size_t json_len = json_end - json_start;
    if (json_len >= sizeof(json_buffer)) {
        json_len = sizeof(json_buffer) - 1;
    }
    strncpy(json_buffer, json_start, json_len);
    json_buffer[json_len] = '\0';

    // Trim whitespace from beginning
    char *trimmed = json_buffer;
    while (*trimmed == ' ' || *trimmed == '\t' || *trimmed == '\n' || *trimmed == '\r') {
        trimmed++;
    }

    printf("Extracted JSON: %s\n", trimmed);

    // Remove JavaScript-style comments from JSON
    char cleaned_json[MAX_AI_MESSAGE_LENGTH];
    int src = 0, dst = 0;
    int in_string = 0;
    int len = strlen(trimmed);

    while (src < len && dst < MAX_AI_MESSAGE_LENGTH - 1) {
        // Track if we're inside a string
        if (trimmed[src] == '"' && (src == 0 || trimmed[src - 1] != '\\')) {
            in_string = !in_string;
            cleaned_json[dst++] = trimmed[src++];
        }
        // Skip // comments only outside strings
        else if (!in_string && trimmed[src] == '/' && src + 1 < len && trimmed[src + 1] == '/') {
            // Skip until end of line
            while (src < len && trimmed[src] != '\n') {
                src++;
            }
            // Skip the newline too
            if (src < len)
                src++;
        } else {
            cleaned_json[dst++] = trimmed[src++];
        }
    }
    cleaned_json[dst] = '\0';

    printf("Cleaned JSON (first 500 chars): %.500s\n", cleaned_json);

    // Try to parse drawing commands from the AI's response
    cJSON *commands_json = cJSON_Parse(cleaned_json);
    if (!commands_json) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("ERROR, JSON Parse Error before: %.80s\n", error_ptr);
        } else {
            printf("ERROR, JSON Parse Error (no error pointer)\n");
        }
    }
    if (commands_json) {
        printf("OK, AI message contains JSON commands\n");
        cJSON *explanation = cJSON_GetObjectItem(commands_json, "explanation");
        cJSON *commands = cJSON_GetObjectItem(commands_json, "commands");

        // Add explanation to chat (safely handle encoding issues)
        if (explanation && cJSON_IsString(explanation) && explanation->valuestring) {
            // Skip if explanation is empty or too long
            size_t exp_len = strlen(explanation->valuestring);
            if (exp_len > 0 && exp_len < MAX_AI_MESSAGE_LENGTH) {
                add_ai_message(app->ai_chat, MESSAGE_AI, explanation->valuestring);
            }
        }

        // Add commands to queue instead of executing immediately
        if (commands && cJSON_IsArray(commands)) {
            int command_count = cJSON_GetArraySize(commands);
            printf("OK, Adding %d drawing commands to queue\n", command_count);

            // Reset queue
            app->ai_chat->command_count = 0;
            app->ai_chat->current_command = 0;
            sfClock_restart(app->ai_chat->command_timer);

            for (int i = 0; i < command_count && i < MAX_AI_COMMANDS; i++) {
                cJSON *cmd = cJSON_GetArrayItem(commands, i);
                if (cJSON_IsString(cmd)) {
                    strncpy(app->ai_chat->command_queue[i].command, cmd->valuestring, 255);
                    app->ai_chat->command_queue[i].command[255] = '\0';
                    app->ai_chat->command_count++;
                    printf("Command %d: %s\n", i + 1, cmd->valuestring);
                }
            }

            app->ai_chat->show_draw_cursor = 1;
        }

        cJSON_Delete(commands_json);
    } else {
        // No structured commands, just add the message as-is
        printf("AI message is plain text (no JSON commands)\n");

        // Safely add message, checking length first
        if (ai_message && strlen(ai_message) > 0 && strlen(ai_message) < MAX_AI_MESSAGE_LENGTH) {
            add_ai_message(app->ai_chat, MESSAGE_AI, ai_message);
        }
    }

    cJSON_Delete(json);
}
