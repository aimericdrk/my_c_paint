#ifndef CONFIG_H
#define CONFIG_H

#include "cJSON.h"
#include <CSFML/Graphics.h>

typedef struct
{
    cJSON *root;
    char *file_path;
} ui_config_t;

// Initialize and load config
ui_config_t *load_ui_config(const char *config_path);
void free_ui_config(ui_config_t *config);

// Helper functions to get values
int config_get_int(cJSON *parent, const char *key, int default_value);
float config_get_float(cJSON *parent, const char *key, float default_value);
const char *config_get_string(cJSON *parent, const char *key, const char *default_value);
sfColor config_get_color(cJSON *parent, const char *key, sfColor default_color);

// Get specific sections
cJSON *config_get_section(ui_config_t *config, const char *path);

#endif
