/*
** CCU PROJECT, 2026
** my c paint
** File description:
** AI HTTP requests to Mistral API
*/

#include "paint.h"
#include <curl/curl.h>

// Thread data structure for AI requests
typedef struct {
    app_t *app;
    char user_message[MAX_AI_MESSAGE_LENGTH];
} ai_request_thread_data_t;

// System prompt for Mistral AI
static const char *SYSTEM_PROMPT = "You are an AI assistant integrated into a painting application. "
                                   "You can help users draw on a canvas by providing drawing commands. "
                                   "The canvas is 1050x750 pixels. Origin (0,0) is top-left. "
                                   "Available commands:\n"
                                   "- DRAW_LINE x1 y1 x2 y2 r g b (draw line from point to point)\n"
                                   "- DRAW_CIRCLE x y radius r g b (draw circle outline)\n"
                                   "- DRAW_FILLED_CIRCLE x y radius r g b (draw filled circle)\n"
                                   "- DRAW_RECT x y width height r g b (draw rectangle outline)\n"
                                   "- DRAW_FILLED_RECT x y width height r g b (draw filled rectangle)\n"
                                   "- DRAW_ELLIPSE x y rx ry r g b (draw ellipse outline)\n"
                                   "- DRAW_TRIANGLE x1 y1 x2 y2 x3 y3 r g b (draw triangle outline)\n"
                                   "- DRAW_FILLED_TRIANGLE x1 y1 x2 y2 x3 y3 r g b (draw filled triangle)\n"
                                   "- DRAW_POINT x y size r g b (draw single point/dot)\n"
                                   "- DRAW_CURVE x1 y1 x2 y2 x3 y3 x4 y4 r g b (draw bezier curve)\n"
                                   "- SET_BRUSH_SIZE size (1-50)\n"
                                   "- CLEAR_CANVAS\n"
                                   "Always respond with a brief explanation followed by the commands in a JSON array format:\n"
                                   "{\"explanation\": \"text\", \"commands\": [\"DRAW_LINE 100 100 200 200 255 0 0\", ...]}\n"
                                   "Use multiple commands to create complex drawings. Keep responses concise.";

// Callback function for libcurl to write response data
struct response_data {
    char *data;
    size_t size;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct response_data *mem = (struct response_data *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

// Thread function for AI request
void *ai_request_thread(void *arg) {
    ai_request_thread_data_t *data = (ai_request_thread_data_t *)arg;
    app_t *app = data->app;
    const char *user_message = data->user_message;

    CURL *curl;
    CURLcode res;
    struct response_data chunk;

    chunk.data = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if (!curl) {
        pthread_mutex_lock(&app->ai_chat->mutex);
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Failed to initialize HTTP client");
        app->ai_chat->is_processing = 0;
        app->ai_chat->thread_active = 0;
        pthread_mutex_unlock(&app->ai_chat->mutex);
        free(data);
        return NULL;
    }

    // Build JSON request body using cJSON to properly escape strings
    cJSON *request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "model", "mistral-small-latest");

    cJSON *messages = cJSON_CreateArray();

    // Add system message
    cJSON *system_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(system_msg, "role", "system");
    cJSON_AddStringToObject(system_msg, "content", SYSTEM_PROMPT);
    cJSON_AddItemToArray(messages, system_msg);

    // Add user message
    cJSON *user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");
    cJSON_AddStringToObject(user_msg, "content", user_message);
    cJSON_AddItemToArray(messages, user_msg);

    cJSON_AddItemToObject(request, "messages", messages);
    cJSON_AddNumberToObject(request, "temperature", 0.7);
    cJSON_AddNumberToObject(request, "max_tokens", 1000);

    // Convert to string
    char *json_body = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);

    if (!json_body) {
        pthread_mutex_lock(&app->ai_chat->mutex);
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Failed to build request");
        app->ai_chat->is_processing = 0;
        app->ai_chat->thread_active = 0;
        pthread_mutex_unlock(&app->ai_chat->mutex);
        curl_easy_cleanup(curl);
        free(chunk.data);
        free(data);
        return NULL;
    }

    printf("📤 Sending request: %s\n", json_body);

    // Set up headers
    struct curl_slist *headers = NULL;
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", app->ai_chat->api_key);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    // Configure curl
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.mistral.ai/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Perform request
    res = curl_easy_perform(curl);

    pthread_mutex_lock(&app->ai_chat->mutex);
    if (res != CURLE_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Error: %s", curl_easy_strerror(res));
        add_ai_message(app->ai_chat, MESSAGE_AI, error_msg);
        printf("❌ CURL Error: %s\n", curl_easy_strerror(res));
    } else {
        // Log raw response for debugging
        printf("\n📡 Raw API Response:\n%s\n\n", chunk.data);

        // Parse response
        pthread_mutex_unlock(&app->ai_chat->mutex);
        parse_and_execute_ai_response(app, chunk.data);
        pthread_mutex_lock(&app->ai_chat->mutex);
    }

    app->ai_chat->is_processing = 0;
    app->ai_chat->thread_active = 0;
    pthread_mutex_unlock(&app->ai_chat->mutex);

    // Cleanup
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    free(json_body);
    free(chunk.data);
    free(data);

    return NULL;
}

void send_ai_request(app_t *app, const char *user_message) {
    if (!app->ai_chat || !user_message || strlen(user_message) == 0)
        return;

    if (app->ai_chat->api_key[0] == '\0') {
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: API key not configured. Please set MISTRAL_API_KEY environment variable.");
        return;
    }

    // Check if a request is already in progress
    pthread_mutex_lock(&app->ai_chat->mutex);
    if (app->ai_chat->thread_active) {
        add_ai_message(app->ai_chat, MESSAGE_AI, "Please wait for the current request to complete...");
        pthread_mutex_unlock(&app->ai_chat->mutex);
        return;
    }

    // Create thread data
    ai_request_thread_data_t *thread_data = malloc(sizeof(ai_request_thread_data_t));
    if (!thread_data) {
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Memory allocation failed");
        pthread_mutex_unlock(&app->ai_chat->mutex);
        return;
    }

    thread_data->app = app;
    strncpy(thread_data->user_message, user_message, MAX_AI_MESSAGE_LENGTH - 1);
    thread_data->user_message[MAX_AI_MESSAGE_LENGTH - 1] = '\0';

    // Set processing flags
    app->ai_chat->is_processing = 1;
    app->ai_chat->thread_active = 1;

    // Create thread
    if (pthread_create(&app->ai_chat->request_thread, NULL, ai_request_thread, thread_data) != 0) {
        add_ai_message(app->ai_chat, MESSAGE_AI, "Error: Failed to create request thread");
        app->ai_chat->is_processing = 0;
        app->ai_chat->thread_active = 0;
        free(thread_data);
        pthread_mutex_unlock(&app->ai_chat->mutex);
        return;
    }

    // Detach thread so it cleans up automatically
    pthread_detach(app->ai_chat->request_thread);
    pthread_mutex_unlock(&app->ai_chat->mutex);

    printf("🚀 AI request started in background thread\n");
}
