/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

int config_get_int(cJSON *parent, const char *key, int default_value) {
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsNumber(item))
        return default_value;

    return item->valueint;
}

float config_get_float(cJSON *parent, const char *key, float default_value) {
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsNumber(item))
        return default_value;

    return (float)item->valuedouble;
}

const char *config_get_string(cJSON *parent, const char *key, const char *default_value) {
    if (!parent)
        return default_value;

    cJSON *item = cJSON_GetObjectItem(parent, key);
    if (!item || !cJSON_IsString(item))
        return default_value;

    return item->valuestring;
}
