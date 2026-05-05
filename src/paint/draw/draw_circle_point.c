/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void draw_circle_point(sfRenderTexture *texture, sfVector2i pos, sfColor color, int size) {
    sfCircleShape *circle = sfCircleShape_create();
    sfCircleShape_setRadius(circle, size / 2.0f);
    sfCircleShape_setPosition(circle, (sfVector2f){pos.x - size / 2.0f, pos.y - size / 2.0f});
    sfCircleShape_setFillColor(circle, color);
    sfRenderTexture_drawCircleShape(texture, circle, NULL);
    sfCircleShape_destroy(circle);
}