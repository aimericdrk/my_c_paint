/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

int is_point_in_rect(sfVector2i point, sfVector2f pos, sfVector2f size) {
    return point.x >= pos.x && point.x <= pos.x + size.x && point.y >= pos.y && point.y <= pos.y + size.y;
}

sfColor get_pixel_color(sfRenderTexture *texture, int x, int y, int width, int height) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return sfWhite;

    const sfTexture *tex = sfRenderTexture_getTexture(texture);
    sfImage *image = sfTexture_copyToImage(tex);
    sfColor color = sfImage_getPixel(image, (sfVector2u){x, y});
    sfImage_destroy(image);

    return color;
}