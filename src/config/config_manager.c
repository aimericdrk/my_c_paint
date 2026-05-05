/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

ui_config_t *load_ui_config(const char *config_path) {
    ui_config_t *config = malloc(sizeof(ui_config_t));
    if (!config)
        return NULL;

    // Read file
    FILE *file = fopen(config_path, "r");
    if (!file) {
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
    if (!content) {
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

    if (!config->root) {
        printf("JSON parse error: %s\n", cJSON_GetErrorPtr());
        free(config);
        return NULL;
    }

    config->file_path = strdup(config_path);
    printf("Config loaded from %s\n", config_path);
    return config;
}

void free_ui_config(ui_config_t *config) {
    if (!config)
        return;

    if (config->root)
        cJSON_Delete(config->root);
    if (config->file_path)
        free(config->file_path);
    free(config);
}