/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

void draw_with_symmetry(paint_state_t *paint, sfVector2i pos) {
    // Draw at original position
    draw_at_position(paint, pos);

    // Apply symmetry
    if (paint->symmetry_mode == SYMMETRY_HORIZONTAL || paint->symmetry_mode == SYMMETRY_BOTH) {
        sfVector2i sym_pos = {pos.x, paint->canvas_height - pos.y - 1};
        draw_at_position(paint, sym_pos);
    }

    if (paint->symmetry_mode == SYMMETRY_VERTICAL || paint->symmetry_mode == SYMMETRY_BOTH) {
        sfVector2i sym_pos = {paint->canvas_width - pos.x - 1, pos.y};
        draw_at_position(paint, sym_pos);
    }

    if (paint->symmetry_mode == SYMMETRY_BOTH) {
        sfVector2i sym_pos = {paint->canvas_width - pos.x - 1, paint->canvas_height - pos.y - 1};
        draw_at_position(paint, sym_pos);
    }

    if (paint->symmetry_mode == SYMMETRY_RADIAL_4) {
        // 4-way radial symmetry
        sfVector2i positions[4] = {
            {pos.x, pos.y}, {paint->canvas_width - pos.x - 1, pos.y}, {pos.x, paint->canvas_height - pos.y - 1}, {paint->canvas_width - pos.x - 1, paint->canvas_height - pos.y - 1}};
        for (int i = 1; i < 4; i++) {
            draw_at_position(paint, positions[i]);
        }
    }

    if (paint->symmetry_mode == SYMMETRY_RADIAL_8) {
        // 8-way radial symmetry (includes diagonals)
        int center_x = paint->canvas_width / 2;
        int center_y = paint->canvas_height / 2;
        int dx = pos.x - center_x;
        int dy = pos.y - center_y;

        sfVector2i positions[8] = {
            {pos.x, pos.y},                 // Original
            {center_x - dx, center_y + dy}, // Horizontal flip
            {center_x + dx, center_y - dy}, // Vertical flip
            {center_x - dx, center_y - dy}, // Both flips
            {center_x + dy, center_y + dx}, // Diagonal swap
            {center_x - dy, center_y + dx}, // Diagonal + H flip
            {center_x + dy, center_y - dx}, // Diagonal + V flip
            {center_x - dy, center_y - dx}  // Diagonal + both flips
        };

        for (int i = 1; i < 8; i++) {
            draw_at_position(paint, positions[i]);
        }
    }

    // For spray tool, display the canvas after drawing all particles
    if (paint->current_tool == TOOL_SPRAY) {
        sfRenderTexture_display(paint->canvas);
    }
}