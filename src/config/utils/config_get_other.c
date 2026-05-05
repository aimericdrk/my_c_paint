/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

sfColor config_get_color(cJSON *parent, const char *key, sfColor default_color) {
    if (!parent)
        return default_color;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsArray(item) || cJSON_GetArraySize(item) != 4)
        return default_color;

    sfColor color;
    color.r = (uint8_t)cJSON_GetArrayItem(item, 0)->valueint;
    color.g = (uint8_t)cJSON_GetArrayItem(item, 1)->valueint;
    color.b = (uint8_t)cJSON_GetArrayItem(item, 2)->valueint;
    color.a = (uint8_t)cJSON_GetArrayItem(item, 3)->valueint;

    return color;
}

cJSON *config_get_section(ui_config_t *config, const char *path) {
    if (!config || !config->root)
        return NULL;

    // Split path by '.' and traverse
    char *path_copy = strdup(path);
    char *token = strtok(path_copy, ".");
    cJSON *current = config->root;

    while (token && current) {
        current = cJSON_GetObjectItem(current, token);
        token = strtok(NULL, ".");
    }

    free(path_copy);
    return current;
}
