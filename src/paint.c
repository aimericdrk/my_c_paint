#include "../include/paint.h"

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

void cleanup_paint_state(paint_state_t *paint) {
    if (!paint)
        return;

    if (paint->canvas)
        sfRenderTexture_destroy(paint->canvas);
    if (paint->canvas_sprite)
        sfSprite_destroy(paint->canvas_sprite);
    if (paint->fps_clock)
        sfClock_destroy(paint->fps_clock);

    // Cleanup undo stack
    for (int i = 0; i < MAX_UNDO; i++) {
        if (paint->undo_stack[i].texture)
            sfRenderTexture_destroy(paint->undo_stack[i].texture);
        if (paint->undo_stack[i].sprite)
            sfSprite_destroy(paint->undo_stack[i].sprite);
    }

    free(paint);
}

void draw_circle_point(sfRenderTexture *texture, sfVector2i pos, sfColor color, int size) {
    sfCircleShape *circle = sfCircleShape_create();
    sfCircleShape_setRadius(circle, size / 2.0f);
    sfCircleShape_setPosition(circle, (sfVector2f){pos.x - size / 2.0f, pos.y - size / 2.0f});
    sfCircleShape_setFillColor(circle, color);
    sfRenderTexture_drawCircleShape(texture, circle, NULL);
    sfCircleShape_destroy(circle);
}

void draw_line(sfRenderTexture *texture, sfVector2i p1, sfVector2i p2, sfColor color, int size) {
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx - dy;
    int x = p1.x;
    int y = p1.y;

    while (1) {
        draw_circle_point(texture, (sfVector2i){x, y}, color, size);

        if (x == p2.x && y == p2.y)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void draw_with_tool(paint_state_t *paint, sfVector2i pos) {
    // Use symmetry if enabled
    if (paint->symmetry_mode != SYMMETRY_NONE) {
        draw_with_symmetry(paint, pos);
        return;
    }

    // Apply alpha to color
    sfColor draw_color = paint->current_color;

    switch (paint->current_tool) {
    case TOOL_PEN:
        draw_circle_point(paint->canvas, pos, draw_color, paint->brush_size);
        break;

    case TOOL_ERASER:
        draw_circle_point(paint->canvas, pos, sfWhite, paint->brush_size * 2);
        break;

    case TOOL_BRUSH:
        // Soft brush with multiple circles
        for (int i = 0; i < 3; i++) {
            sfColor soft_color = draw_color;
            soft_color.a = (paint->current_color.a * (i * 10)) / 255;
            draw_circle_point(paint->canvas, pos, soft_color, paint->brush_size + i * 2);
        }
        break;

    case TOOL_SPRAY:
        // Spray effect with random particles
        for (int i = 0; i < 30; i++) {
            int radius = paint->brush_size * 2;
            float angle = (float)(rand() % 360) * M_PI / 180.0f;
            float distance = (float)(rand() % (radius * 100)) / 100.0f;
            int offset_x = (int)(distance * cos(angle));
            int offset_y = (int)(distance * sin(angle));
            sfVector2i spray_pos = {pos.x + offset_x, pos.y + offset_y};
            if (spray_pos.x >= 0 && spray_pos.x < paint->canvas_width && spray_pos.y >= 0 && spray_pos.y < paint->canvas_height) {
                sfColor spray_color = draw_color;
                spray_color.a = (paint->current_color.a * (80 + (rand() % 80))) / 255;
                draw_circle_point(paint->canvas, spray_pos, spray_color, 1 + (rand() % 2));
            }
        }
        sfRenderTexture_display(paint->canvas);
        break;

    default:
        break;
    }
}

void handle_drawing(paint_state_t *paint, sfVector2i mouse_pos) {
    if (paint->current_tool == TOOL_LINE || paint->current_tool == TOOL_RECTANGLE || paint->current_tool == TOOL_CIRCLE || paint->current_tool == TOOL_FILL) {
        return; // These tools don't draw continuously
    }

    // Draw line from last position to current position for smooth drawing
    if (paint->last_pos.x != mouse_pos.x || paint->last_pos.y != mouse_pos.y) {
        int steps = fmax(abs(mouse_pos.x - paint->last_pos.x), abs(mouse_pos.y - paint->last_pos.y));

        for (int i = 0; i <= steps; i++) {
            sfVector2i interpolated = {paint->last_pos.x + (mouse_pos.x - paint->last_pos.x) * i / steps, paint->last_pos.y + (mouse_pos.y - paint->last_pos.y) * i / steps};
            draw_with_tool(paint, interpolated);
        }
    }

    paint->last_pos = mouse_pos;
    sfRenderTexture_display(paint->canvas);
}

void finish_shape(paint_state_t *paint) {
    sfVector2i start = paint->shape_preview.start;
    sfVector2i end = paint->shape_preview.end;

    if (paint->current_tool == TOOL_LINE) {
        draw_line(paint->canvas, start, end, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_RECTANGLE) {
        // Draw rectangle outline
        int x1 = fmin(start.x, end.x);
        int y1 = fmin(start.y, end.y);
        int x2 = fmax(start.x, end.x);
        int y2 = fmax(start.y, end.y);

        draw_line(paint->canvas, (sfVector2i){x1, y1}, (sfVector2i){x2, y1}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x2, y1}, (sfVector2i){x2, y2}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x2, y2}, (sfVector2i){x1, y2}, paint->current_color, paint->brush_size);
        draw_line(paint->canvas, (sfVector2i){x1, y2}, (sfVector2i){x1, y1}, paint->current_color, paint->brush_size);
    } else if (paint->current_tool == TOOL_CIRCLE) {
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        float radius = sqrt(dx * dx + dy * dy);

        // Draw circle using parametric equation
        for (float angle = 0; angle < 2 * M_PI; angle += 0.01f) {
            sfVector2i point = {start.x + radius * cos(angle), start.y + radius * sin(angle)};
            draw_circle_point(paint->canvas, point, paint->current_color, paint->brush_size);
        }
    }

    sfRenderTexture_display(paint->canvas);
}

void clear_canvas(paint_state_t *paint) {
    sfRenderTexture_clear(paint->canvas, sfWhite);
    sfRenderTexture_display(paint->canvas);
}

#define MAX_FILL_STACK 50000

static int colors_match(sfColor c1, sfColor c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

void fill_canvas_optimized(paint_state_t *paint, int x, int y, sfColor target, sfColor replacement, sfImage *image, int *stack_x, int *stack_y) {
    (void)paint; // Unused parameter
    int stack_size = 0;

    stack_x[stack_size] = x;
    stack_y[stack_size] = y;
    stack_size++;

    while (stack_size > 0 && stack_size < MAX_FILL_STACK) {
        stack_size--;
        int cx = stack_x[stack_size];
        int cy = stack_y[stack_size];

        if (cx < 0 || cx >= paint->canvas_width || cy < 0 || cy >= paint->canvas_height)
            continue;

        sfColor current = sfImage_getPixel(image, (sfVector2u){cx, cy});

        if (!colors_match(current, target) || colors_match(current, replacement))
            continue;

        // Fill the scanline
        int left = cx;
        int right = cx;

        // Expand left
        while (left > 0) {
            sfColor c = sfImage_getPixel(image, (sfVector2u){left - 1, cy});
            if (!colors_match(c, target))
                break;
            left--;
        }

        // Expand right
        while (right < paint->canvas_width - 1) {
            sfColor c = sfImage_getPixel(image, (sfVector2u){right + 1, cy});
            if (!colors_match(c, target))
                break;
            right++;
        }

        // Fill the line
        for (int i = left; i <= right; i++) {
            sfImage_setPixel(image, (sfVector2u){i, cy}, replacement);
        }

        // Add pixels above and below to stack
        for (int i = left; i <= right; i++) {
            // Check above
            if (cy > 0 && stack_size < MAX_FILL_STACK - 1) {
                sfColor c = sfImage_getPixel(image, (sfVector2u){i, cy - 1});
                if (colors_match(c, target)) {
                    stack_x[stack_size] = i;
                    stack_y[stack_size] = cy - 1;
                    stack_size++;
                }
            }

            // Check below
            if (cy < paint->canvas_height - 1 && stack_size < MAX_FILL_STACK - 1) {
                sfColor c = sfImage_getPixel(image, (sfVector2u){i, cy + 1});
                if (colors_match(c, target)) {
                    stack_x[stack_size] = i;
                    stack_y[stack_size] = cy + 1;
                    stack_size++;
                }
            }
        }
    }

    if (stack_size >= MAX_FILL_STACK) {
        printf("Fill area too large, stopped at limit\n");
    }
}

void fill_canvas(paint_state_t *paint, sfVector2i pos, sfColor target) {
    if (colors_match(target, paint->current_color))
        return;

    printf("Filling area...\n");

    // Get the current canvas as an image
    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image) {
        printf("Failed to get canvas image!\n");
        return;
    }

    // Allocate smaller stack for optimized flood fill
    int *stack_x = malloc(sizeof(int) * MAX_FILL_STACK);
    int *stack_y = malloc(sizeof(int) * MAX_FILL_STACK);

    if (stack_x && stack_y) {
        fill_canvas_optimized(paint, pos.x, pos.y, target, paint->current_color, image, stack_x, stack_y);

        // Create a texture from the modified image and draw it to canvas
        sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
        if (new_texture) {
            sfSprite *sprite = sfSprite_create(new_texture);
            if (sprite) {
                sfRenderTexture_clear(paint->canvas, sfWhite);
                sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
                sfRenderTexture_display(paint->canvas);
                sfSprite_destroy(sprite);
            }
            sfTexture_destroy(new_texture);
        }

        printf("Fill complete!\n");
    } else {
        printf("Memory allocation failed!\n");
    }

    if (stack_x)
        free(stack_x);
    if (stack_y)
        free(stack_y);
    if (image)
        sfImage_destroy(image);
}

// Helper function to add color to recent colors
void add_recent_color(paint_state_t *paint, sfColor color) {
    // Check if color already exists in recent colors
    for (int i = 0; i < paint->recent_color_count; i++) {
        if (paint->recent_colors[i].r == color.r && paint->recent_colors[i].g == color.g && paint->recent_colors[i].b == color.b && paint->recent_colors[i].a == color.a) {
            return; // Already in recent colors
        }
    }

    // Shift colors down and add new one at front
    for (int i = MAX_RECENT_COLORS - 1; i > 0; i--) {
        paint->recent_colors[i] = paint->recent_colors[i - 1];
    }
    paint->recent_colors[0] = color;

    if (paint->recent_color_count < MAX_RECENT_COLORS) {
        paint->recent_color_count++;
    }
}

// Helper function to draw at a position with current tool
static void draw_at_position(paint_state_t *paint, sfVector2i pos) {
    sfColor draw_color = paint->current_color;

    switch (paint->current_tool) {
    case TOOL_PEN:
        draw_circle_point(paint->canvas, pos, draw_color, paint->brush_size);
        break;
    case TOOL_ERASER:
        draw_circle_point(paint->canvas, pos, sfWhite, paint->brush_size * 2);
        break;
    case TOOL_BRUSH:
        for (int i = 0; i < 3; i++) {
            sfColor soft_color = draw_color;
            soft_color.a = (paint->current_color.a * (100 - i * 20)) / 255;
            draw_circle_point(paint->canvas, pos, soft_color, paint->brush_size + i * 2);
        }
        break;
    case TOOL_SPRAY:
        // Spray effect with random particles
        for (int i = 0; i < 30; i++) {
            int radius = paint->brush_size * 2;
            float angle = (float)(rand() % 360) * M_PI / 180.0f;
            float distance = (float)(rand() % (radius * 100)) / 100.0f;
            int offset_x = (int)(distance * cos(angle));
            int offset_y = (int)(distance * sin(angle));
            sfVector2i spray_pos = {pos.x + offset_x, pos.y + offset_y};
            if (spray_pos.x >= 0 && spray_pos.x < paint->canvas_width && spray_pos.y >= 0 && spray_pos.y < paint->canvas_height) {
                sfColor spray_color = draw_color;
                spray_color.a = (paint->current_color.a * (80 + (rand() % 80))) / 255;
                draw_circle_point(paint->canvas, spray_pos, spray_color, 1 + (rand() % 2));
            }
        }
        break;
    default:
        draw_circle_point(paint->canvas, pos, draw_color, paint->brush_size);
        break;
    }
}

// Apply symmetry to drawing position
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

// Canvas flip operations
void flip_canvas_horizontal(paint_state_t *paint) {
    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image)
        return;

    for (int y = 0; y < paint->canvas_height; y++) {
        for (int x = 0; x < paint->canvas_width / 2; x++) {
            sfColor left = sfImage_getPixel(image, (sfVector2u){x, y});
            sfColor right = sfImage_getPixel(image, (sfVector2u){paint->canvas_width - x - 1, y});
            sfImage_setPixel(image, (sfVector2u){x, y}, right);
            sfImage_setPixel(image, (sfVector2u){paint->canvas_width - x - 1, y}, left);
        }
    }

    sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
    if (new_texture) {
        sfSprite *sprite = sfSprite_create(new_texture);
        sfRenderTexture_clear(paint->canvas, sfWhite);
        sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
        sfRenderTexture_display(paint->canvas);
        sfSprite_destroy(sprite);
        sfTexture_destroy(new_texture);
    }

    sfImage_destroy(image);
    printf("Canvas flipped horizontally\n");
}

void flip_canvas_vertical(paint_state_t *paint) {
    const sfTexture *texture = sfRenderTexture_getTexture(paint->canvas);
    sfImage *image = sfTexture_copyToImage(texture);

    if (!image)
        return;

    for (int y = 0; y < paint->canvas_height / 2; y++) {
        for (int x = 0; x < paint->canvas_width; x++) {
            sfColor top = sfImage_getPixel(image, (sfVector2u){x, y});
            sfColor bottom = sfImage_getPixel(image, (sfVector2u){x, paint->canvas_height - y - 1});
            sfImage_setPixel(image, (sfVector2u){x, y}, bottom);
            sfImage_setPixel(image, (sfVector2u){x, paint->canvas_height - y - 1}, top);
        }
    }

    sfTexture *new_texture = sfTexture_createFromImage(image, NULL);
    if (new_texture) {
        sfSprite *sprite = sfSprite_create(new_texture);
        sfRenderTexture_clear(paint->canvas, sfWhite);
        sfRenderTexture_drawSprite(paint->canvas, sprite, NULL);
        sfRenderTexture_display(paint->canvas);
        sfSprite_destroy(sprite);
        sfTexture_destroy(new_texture);
    }

    sfImage_destroy(image);
    printf("Canvas flipped vertically\n");
}
