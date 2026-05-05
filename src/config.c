#include "../include/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ui_config_t *load_ui_config(const char *config_path)
{
    ui_config_t *config = malloc(sizeof(ui_config_t));
    if (!config)
        return NULL;

    // Read file
    FILE *file = fopen(config_path, "r");
    if (!file)
    {
        printf("Could not open config file: %s\n", config_path);
        free(config);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file content
    char *content = malloc(file_size + 1);
    if (!content)
    {
        fclose(file);
        free(config);
        return NULL;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);

    // Parse JSON
    config->root = cJSON_Parse(content);
    free(content);

    if (!config->root)
    {
        printf("JSON parse error: %s\n", cJSON_GetErrorPtr());
        free(config);
        return NULL;
    }

    config->file_path = strdup(config_path);
    printf("Config loaded from %s\n", config_path);
    return config;
}

void free_ui_config(ui_config_t *config)
{
    if (!config)
        return;

    if (config->root)
        cJSON_Delete(config->root);
    if (config->file_path)
        free(config->file_path);
    free(config);
}

int config_get_int(cJSON *parent, const char *key, int default_value)
{
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsNumber(item))
        return default_value;

    return item->valueint;
}

float config_get_float(cJSON *parent, const char *key, float default_value)
{
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsNumber(item))
        return default_value;

    return (float)item->valuedouble;
}

const char *config_get_string(cJSON *parent, const char *key, const char *default_value)
{
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsString(item))
        return default_value;

    return item->valuestring;
}

sfColor config_get_color(cJSON *parent, const char *key, sfColor default_color)
{
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

cJSON *config_get_section(ui_config_t *config, const char *path)
{
    if (!config || !config->root)
        return NULL;

    // Split path by '.' and traverse
    char *path_copy = strdup(path);
    char *token = strtok(path_copy, ".");
    cJSON *current = config->root;

    while (token && current)
    {
        current = cJSON_GetObjectItem(current, token);
        token = strtok(NULL, ".");
    }

    free(path_copy);
    return current;
}
