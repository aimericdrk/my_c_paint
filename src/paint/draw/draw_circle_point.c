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

    // Use special blend mode for erasing (transparent color)
    if (color.r == 0 && color.g == 0 && color.b == 0 && color.a == 0) {
        sfRenderStates states;
        states.blendMode = sfBlendNone;
        states.transform = sfTransform_Identity;
        states.texture = NULL;
        states.shader = NULL;
        sfRenderTexture_drawCircleShape(texture, circle, &states);
    } else {
        sfRenderTexture_drawCircleShape(texture, circle, NULL);
    }

    sfCircleShape_destroy(circle);
}
