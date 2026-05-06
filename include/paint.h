#ifndef PAINT_H
#define PAINT_H

#include "config.h"
#include <CSFML/Graphics.h>
#include <CSFML/System.h>
#include <CSFML/Window.h>
#include <dirent.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_UNDO 50

#define MAX_RECENT_COLORS 10
#define MAX_POLYGON_POINTS 20

#define MAX_FILE_ENTRIES 100
#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256

#define MAX_FILL_STACK 50000

#define MAX_LAYERS 50
#define MAX_LAYER_NAME 64

#define MAX_AI_MESSAGES 100
#define MAX_AI_MESSAGE_LENGTH 2048
#define MAX_AI_COMMANDS 100

typedef enum {
    TOOL_PEN,
    TOOL_ERASER,
    TOOL_BRUSH,
    TOOL_SPRAY,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_FILL,
    TOOL_TEXT,
    TOOL_GRADIENT,
    TOOL_POLYGON,
    TOOL_STAR,
    TOOL_EYEDROPPER,
    TOOL_SELECT_RECT
} tool_type_t;

typedef enum { SYMMETRY_NONE, SYMMETRY_HORIZONTAL, SYMMETRY_VERTICAL, SYMMETRY_BOTH, SYMMETRY_RADIAL_4, SYMMETRY_RADIAL_8 } symmetry_mode_t;

typedef enum { TAB_PEN, TAB_COLOR, TAB_LAYER, TAB_IA } toolbar_tab_t;

// File explorer enums and structures
typedef enum { FILE_EXPLORER_CLOSED, FILE_EXPLORER_OPEN, FILE_EXPLORER_SAVE } file_explorer_mode_t;

typedef enum { FILE_TYPE_FILE, FILE_TYPE_DIRECTORY, FILE_TYPE_PARENT } file_type_t;

typedef struct {
    sfVector2i start;
    sfVector2i end;
    int active;
} shape_preview_t;

typedef struct {
    sfColor color;
    int size;
    tool_type_t tool;
} stroke_data_t;

typedef struct {
    sfRenderTexture *texture;
    sfSprite *sprite;
} undo_state_t;

typedef struct {
    char name[MAX_LAYER_NAME];
    sfRenderTexture *texture;
    sfSprite *sprite;
    uint8_t opacity;
    int visible;
} layer_t;

typedef struct {
    sfRenderTexture *canvas;
    sfSprite *canvas_sprite;
    sfColor current_color;
    uint8_t alpha;
    int brush_size;
    tool_type_t current_tool;
    sfVector2i last_pos;
    int is_drawing;
    shape_preview_t shape_preview;

    // Canvas dimensions
    int canvas_width;
    int canvas_height;
    int canvas_x;
    int canvas_y;

    // Undo/Redo
    undo_state_t undo_stack[MAX_UNDO];
    int undo_count;
    int undo_position;

    // Recent colors
    sfColor recent_colors[MAX_RECENT_COLORS];
    int recent_color_count;

    // Symmetry
    symmetry_mode_t symmetry_mode;

    // Polygon tool
    sfVector2i polygon_points[MAX_POLYGON_POINTS];
    int polygon_point_count;

    // Text tool
    char text_buffer[256];
    int text_input_active;

    // Zoom and pan
    float zoom_level;
    sfVector2i pan_offset;

    // Grid
    int show_grid;

    // Layers
    layer_t layers[MAX_LAYERS];
    int layer_count;
    int current_layer;
    int grid_size;
    int snap_to_grid;

    // Debug
    int show_debug;
    sfClock *fps_clock;
    int frame_count;
    float fps;
} paint_state_t;

typedef enum { MESSAGE_USER, MESSAGE_AI } message_type_t;

typedef struct {
    message_type_t type;
    char content[MAX_AI_MESSAGE_LENGTH];
    char timestamp[32];
} ai_message_t;

typedef struct {
    char command[256];
    float progress;  // 0.0 to 1.0 for animation
    int is_animated; // Whether this command should be animated
} ai_command_t;

typedef struct {
    ai_message_t messages[MAX_AI_MESSAGES];
    int message_count;
    int is_processing;
    char api_key[256];

    // Command queue for animated execution
    ai_command_t command_queue[MAX_AI_COMMANDS];
    int command_count;
    int current_command;
    sfClock *command_timer;
    sfVector2i draw_cursor_pos;
    int show_draw_cursor;

    // Threading support
    pthread_t request_thread;
    pthread_mutex_t mutex;
    int thread_active;
} ai_chat_t;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    file_type_t type;
    int is_image;
} file_entry_t;

typedef struct {
    file_explorer_mode_t mode;
    char current_path[MAX_PATH_LENGTH];
    file_entry_t entries[MAX_FILE_ENTRIES];
    int entry_count;
    int scroll_offset;
    int selected_index;
    char filename_input[MAX_FILENAME_LENGTH];
    int input_cursor_pos;
    int dragging_scrollbar;

    // Format dropdown
    int format_dropdown_open;
    int selected_format_index;
    char format_options[5][16]; // .png, .jpg, .bmp, .tga, personalised
    int format_count;
    char base_filename[MAX_FILENAME_LENGTH];
    char extension[16];
    int extension_editable;

    // UI elements
    sfRectangleShape *overlay_bg;
    sfRectangleShape *explorer_bg;
    sfRectangleShape *path_bar;
    sfText *path_text;
    sfRectangleShape *filename_input_box;
    sfText *filename_input_text;
    sfText *cursor_indicator;
    sfRectangleShape **file_buttons;
    sfText **file_labels;
    sfRectangleShape *scrollbar;
    sfRectangleShape *scrollbar_thumb;
    sfRectangleShape *ok_button;
    sfText *ok_label;
    sfRectangleShape *cancel_button;
    sfText *cancel_label;
    sfText *title_text;

    // Format dropdown UI
    sfRectangleShape *format_button;
    sfText *format_button_label;
    sfRectangleShape *format_dropdown_bg;
    sfRectangleShape **format_dropdown_buttons;
    sfText **format_dropdown_labels;

    // Icon sprites
    sfTexture *folder_icon_texture;
    sfSprite *folder_icon_sprite;
    sfTexture *image_icon_texture;
    sfSprite *image_icon_sprite;
    sfTexture *file_icon_texture;
    sfSprite *file_icon_sprite;
    int icon_size;

    // Cursor blinking
    sfClock *cursor_blink_clock;
} file_explorer_t;

typedef struct {
    sfRectangleShape *bg;
    sfRectangleShape **buttons;
    sfText **labels;
    int button_count;

    // Toolbar tabs
    sfRectangleShape **tab_buttons;
    sfText **tab_labels;
    int tab_count;

    // Color picker
    sfRectangleShape *color_preview;
    sfRectangleShape *r_slider;
    sfRectangleShape *g_slider;
    sfRectangleShape *b_slider;
    sfRectangleShape *a_slider;
    sfRectangleShape *r_bar;
    sfRectangleShape *g_bar;
    sfRectangleShape *b_bar;
    sfRectangleShape *a_bar;

    // Size slider
    sfRectangleShape *size_slider;
    sfRectangleShape *size_bar;

    // Color wheel
    sfCircleShape *color_wheel;
    sfImage *color_wheel_image;
    sfTexture *color_wheel_texture;
    sfCircleShape *color_wheel_cursor;

    // Tool buttons
    sfCircleShape **tool_icons;

    // Recent colors
    sfRectangleShape **recent_color_boxes;

    // Top bar
    sfRectangleShape *topbar;
    sfRectangleShape *file_button;
    sfText *file_label;
    sfRectangleShape *options_button;
    sfText *options_label;

    // File dropdown menu
    sfRectangleShape *dropdown_bg;
    sfRectangleShape **dropdown_buttons;
    sfText **dropdown_labels;
    int dropdown_count;

    // Options dropdown menu
    sfRectangleShape *options_dropdown_bg;
    sfRectangleShape **options_dropdown_buttons;
    sfText **options_dropdown_labels;
    sfRectangleShape **options_checkboxes;
    int options_dropdown_count;

    // Font reference
    sfFont *font;

    // File explorer overlay
    file_explorer_t *file_explorer;

    // Layer panel
    sfRectangleShape **layer_items;
    sfText **layer_names;
    sfRectangleShape **layer_visibility_buttons;
    sfRectangleShape **layer_opacity_bars;
    sfRectangleShape **layer_opacity_sliders;
    int layer_panel_scroll_offset;
    sfRectangleShape *layer_scrollbar;
    sfRectangleShape *layer_scrollbar_thumb;

    // Layer control buttons
    sfRectangleShape *layer_new_button;
    sfText *layer_new_label;
    sfRectangleShape *layer_delete_button;
    sfText *layer_delete_label;
    sfRectangleShape *layer_merge_button;
    sfText *layer_merge_label;
    sfRectangleShape *layer_up_button;
    sfText *layer_up_label;
    sfRectangleShape *layer_down_button;
    sfText *layer_down_label;

    int dragging_layer_opacity;

    // AI Chat panel
    sfRectangleShape *ai_chat_bg;
    sfRectangleShape *ai_input_box;
    sfText *ai_input_text;
    sfRectangleShape *ai_send_button;
    sfText *ai_send_label;
    sfRectangleShape *ai_chat_area;
    sfRectangleShape *ai_scrollbar;
    sfRectangleShape *ai_scrollbar_thumb;
    int ai_chat_scroll_offset;
    char ai_input_buffer[512];
    int ai_input_cursor;
    int ai_input_scroll_offset;
    int ai_is_typing;
    int ai_input_display_lines;
} ui_elements_t;

typedef struct {
    sfRenderWindow *window;
    paint_state_t *paint;
    ui_elements_t *ui;
    toolbar_tab_t active_tab;
    sfFont *font;
    ui_config_t *config;
    int dragging_slider;
    int dropdown_open;
    int options_dropdown_open;
    int dragging_layer_scrollbar;

    // AI Chat
    ai_chat_t *ai_chat;
    sfClock *cursor_blink_clock;

    // Dimensions loaded from config
    int window_width;
    int window_height;
    int canvas_x;
    int canvas_y;
    int canvas_width;
    int canvas_height;
    int topbar_height;
    int toolbar_x;
} app_t;

// Main functions
app_t *init_app(void);
void run_app(app_t *app);
void cleanup_app(app_t *app);
void render(app_t *app);
void update(app_t *app);
void handle_events(app_t *app);

// Config functions
void free_ui_config(ui_config_t *config);
int config_get_int(cJSON *parent, const char *key, int default_value);
float config_get_float(cJSON *parent, const char *key, float default_value);
sfColor config_get_color(cJSON *parent, const char *key, sfColor default_color);

// Paint functions
paint_state_t *init_paint_state(int canvas_width, int canvas_height, int canvas_x, int canvas_y);
void cleanup_paint_state(paint_state_t *paint);
void handle_drawing(paint_state_t *paint, sfVector2i mouse_pos);
void draw_with_tool(paint_state_t *paint, sfVector2i pos);
void draw_with_symmetry(paint_state_t *paint, sfVector2i pos);
void finish_shape(paint_state_t *paint);
void clear_canvas(paint_state_t *paint);
void fill_canvas(paint_state_t *paint, sfVector2i pos, sfColor target);
void add_recent_color(paint_state_t *paint, sfColor color);
void flip_canvas_horizontal(paint_state_t *paint);
void flip_canvas_vertical(paint_state_t *paint);
void draw_at_position(paint_state_t *paint, sfVector2i pos);
int colors_match(sfColor c1, sfColor c2);

// UI functions
ui_elements_t *init_ui(sfFont *font, ui_config_t *config);
void cleanup_ui(ui_elements_t *ui);
void render_ui(sfRenderWindow *window, app_t *app);
void handle_ui_click(app_t *app, sfVector2i mouse_pos);
void update_color_slider(app_t *app, sfVector2i mouse_pos);

// Undo/Redo
void save_undo_state(paint_state_t *paint);
void undo(paint_state_t *paint);
void redo(paint_state_t *paint);

// Layer functions
void init_layers(paint_state_t *paint);
void cleanup_layers(paint_state_t *paint);
int create_layer(paint_state_t *paint, const char *name);
void delete_layer(paint_state_t *paint, int layer_index);
void move_layer_up(paint_state_t *paint, int layer_index);
void move_layer_down(paint_state_t *paint, int layer_index);
void merge_layer_down(paint_state_t *paint, int layer_index);
void set_layer_opacity(paint_state_t *paint, int layer_index, uint8_t opacity);
void toggle_layer_visibility(paint_state_t *paint, int layer_index);
void composite_layers(paint_state_t *paint);

// File operations
void save_to_file(paint_state_t *paint, const char *filename);
void load_from_file(paint_state_t *paint, const char *filename);

// File explorer
file_explorer_t *init_file_explorer(sfFont *font, ui_config_t *config);
void cleanup_file_explorer(file_explorer_t *explorer);
void open_file_explorer(file_explorer_t *explorer, file_explorer_mode_t mode);
void close_file_explorer(file_explorer_t *explorer);
void refresh_file_list(file_explorer_t *explorer);
void navigate_to_directory(file_explorer_t *explorer, const char *path);
void handle_file_explorer_click(app_t *app, sfVector2i mouse_pos);
void handle_file_explorer_text(file_explorer_t *explorer, uint32_t unicode);
void handle_file_explorer_keypress(file_explorer_t *explorer, sfKeyCode key);
void render_file_explorer(sfRenderWindow *window, file_explorer_t *explorer);

// Utility functions
int is_point_in_rect(sfVector2i point, sfVector2f pos, sfVector2f size);
sfColor get_pixel_color(sfRenderTexture *texture, int x, int y, int width, int height);
void draw_line(sfRenderTexture *texture, sfVector2i p1, sfVector2i p2, sfColor color, int size);
void draw_circle_point(sfRenderTexture *texture, sfVector2i pos, sfColor color, int size);
sfVector2i get_scaled_mouse_pos(app_t *app);

// Debug
void render_debug_info(sfRenderWindow *window, paint_state_t *paint, sfFont *font, sfVector2i mouse_pos);

// AI Chat functions
ai_chat_t *init_ai_chat(void);
void cleanup_ai_chat(ai_chat_t *chat);
void add_ai_message(ai_chat_t *chat, message_type_t type, const char *content);
void send_ai_request(app_t *app, const char *user_message);
void parse_and_execute_ai_response(app_t *app, const char *response);
void execute_drawing_command(app_t *app, const char *command, float progress);
void process_ai_command_queue(app_t *app);

#endif
