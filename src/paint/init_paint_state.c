/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

paint_state_t *init_paint_state(int canvas_width, int canvas_height, int canvas_x, int canvas_y) {
    paint_state_t *paint = malloc(sizeof(paint_state_t));
    if (!paint)
        return NULL;

    paint->canvas_width = canvas_width;
    paint->canvas_height = canvas_height;
    paint->canvas_x = canvas_x;
    paint->canvas_y = canvas_y;

    // Create canvas render texture
    paint->canvas = sfRenderTexture_create((sfVector2u){canvas_width, canvas_height}, NULL);
    if (!paint->canvas) {
        free(paint);
        return NULL;
    }

    // Clear canvas to white
    sfRenderTexture_clear(paint->canvas, sfWhite);
    sfRenderTexture_display(paint->canvas);

    // Create sprite for canvas with texture
    paint->canvas_sprite = sfSprite_create(sfRenderTexture_getTexture(paint->canvas));
    sfSprite_setPosition(paint->canvas_sprite, (sfVector2f){canvas_x, canvas_y});

    // Initialize state
    paint->current_color = (sfColor){0, 0, 0, 255}; // Black with full opacity
    paint->alpha = 255;
    paint->brush_size = 5;
    paint->current_tool = TOOL_PEN;
    paint->is_drawing = 0;
    paint->shape_preview.active = 0;
    paint->show_debug = 0;

    // Initialize recent colors
    paint->recent_color_count = 0;
    for (int i = 0; i < MAX_RECENT_COLORS; i++) {
        paint->recent_colors[i] = sfBlack;
    }

    // Initialize symmetry
    paint->symmetry_mode = SYMMETRY_NONE;

    // Initialize polygon
    paint->polygon_point_count = 0;

    // Initialize text tool
    paint->text_input_active = 0;
    memset(paint->text_buffer, 0, sizeof(paint->text_buffer));

    // Initialize zoom and pan
    paint->zoom_level = 1.0f;
    paint->pan_offset = (sfVector2i){0, 0};

    // Initialize grid
    paint->show_grid = 0;
    paint->grid_size = 20;
    paint->snap_to_grid = 0;

    // Initialize undo system
    paint->undo_count = 0;
    paint->undo_position = -1;
    for (int i = 0; i < MAX_UNDO; i++) {
        paint->undo_stack[i].texture = NULL;
        paint->undo_stack[i].sprite = NULL;
    }

    // Initialize FPS counter
    paint->fps_clock = sfClock_create();
    paint->frame_count = 0;
    paint->fps = 0.0f;

    return paint;
}