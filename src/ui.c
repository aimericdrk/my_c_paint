#include "../include/paint.h"

ui_elements_t *init_ui(sfFont *font, ui_config_t *config) {
    ui_elements_t *ui = malloc(sizeof(ui_elements_t));
    if (!ui)
        return NULL;

    // Store font reference
    ui->font = font;

    // Get configuration sections
    cJSON *toolbar_cfg = config ? config_get_section(config, "toolbar") : NULL;
    cJSON *tools_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "tools") : NULL;
    cJSON *topbar_cfg = config ? config_get_section(config, "topbar") : NULL;
    cJSON *sliders_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "sliders") : NULL;
    cJSON *color_preview_cfg = toolbar_cfg ? cJSON_GetObjectItem(toolbar_cfg, "color_preview") : NULL;
    cJSON *file_dropdown_cfg = config ? config_get_section(config, "dropdowns.file_menu") : NULL;
    cJSON *options_dropdown_cfg = config ? config_get_section(config, "dropdowns.options_menu") : NULL;

    // Suppress unused warnings for configs we'll use later
    (void)tools_cfg;
    (void)topbar_cfg;
    (void)sliders_cfg;
    (void)color_preview_cfg;
    (void)file_dropdown_cfg;
    (void)options_dropdown_cfg;

    // Background
    ui->bg = sfRectangleShape_create();
    int toolbar_x = config_get_int(toolbar_cfg, "x", 1150);
    int toolbar_width = config_get_int(toolbar_cfg, "width", 250);
    sfColor toolbar_bg_color = config_get_color(toolbar_cfg, "background_color", (sfColor){50, 50, 55, 255});

    sfRectangleShape_setSize(ui->bg, (sfVector2f){toolbar_width, 920});
    sfRectangleShape_setPosition(ui->bg, (sfVector2f){toolbar_x, 0});
    sfRectangleShape_setFillColor(ui->bg, toolbar_bg_color);

    // Tool buttons
    ui->button_count = 8;
    ui->buttons = malloc(sizeof(sfRectangleShape *) * ui->button_count);
    ui->labels = malloc(sizeof(sfText *) * ui->button_count);
    ui->tool_icons = malloc(sizeof(sfCircleShape *) * ui->button_count);

    // Load tool button config
    int tool_start_y = config_get_int(tools_cfg, "start_y", 60);
    int tool_btn_width = config_get_int(tools_cfg, "button_width", 220);
    int tool_btn_height = config_get_int(tools_cfg, "button_height", 50);
    int tool_spacing = config_get_int(tools_cfg, "spacing", 60);
    int tool_x_offset = config_get_int(tools_cfg, "x_offset", 15);
    sfColor tool_btn_color = config_get_color(tools_cfg, "button_color", (sfColor){70, 70, 75, 255});
    sfColor tool_btn_outline = config_get_color(tools_cfg, "button_outline_color", (sfColor){100, 100, 105, 255});
    sfColor tool_btn_selected = config_get_color(tools_cfg, "button_selected_color", (sfColor){100, 150, 200, 255});
    (void)tool_btn_selected; // Used in render function
    int font_size = config_get_int(tools_cfg, "font_size", 20);

    const char *tool_names[] = {"Pen", "Eraser", "Brush", "Spray", "Line", "Rectangle", "Circle", "Fill"};

    for (int i = 0; i < ui->button_count; i++) {
        ui->buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ui->buttons[i], (sfVector2f){tool_btn_width, tool_btn_height});
        sfRectangleShape_setPosition(ui->buttons[i], (sfVector2f){toolbar_x + tool_x_offset, tool_start_y + i * tool_spacing});
        sfRectangleShape_setFillColor(ui->buttons[i], tool_btn_color);
        sfRectangleShape_setOutlineColor(ui->buttons[i], tool_btn_outline);
        sfRectangleShape_setOutlineThickness(ui->buttons[i], 2);

        ui->labels[i] = sfText_create(font);
        sfText_setString(ui->labels[i], tool_names[i]);
        sfText_setFont(ui->labels[i], font);
        sfText_setCharacterSize(ui->labels[i], font_size);
        sfText_setFillColor(ui->labels[i], sfWhite);
        sfText_setPosition(ui->labels[i], (sfVector2f){toolbar_x + tool_x_offset + 45, tool_start_y + i * tool_spacing + 10});

        // Create simple icon
        ui->tool_icons[i] = sfCircleShape_create();
        sfCircleShape_setRadius(ui->tool_icons[i], 8);
        sfCircleShape_setPosition(ui->tool_icons[i], (sfVector2f){toolbar_x + tool_x_offset + 15, tool_start_y + i * tool_spacing + 15});
        sfCircleShape_setFillColor(ui->tool_icons[i], (sfColor){200, 200, 205, 255});
    }

    // Color preview
    int preview_x = toolbar_x + config_get_int(color_preview_cfg, "x", 25);
    int preview_y = config_get_int(color_preview_cfg, "y", 565);
    int preview_width = config_get_int(color_preview_cfg, "width", 200);
    int preview_height = config_get_int(color_preview_cfg, "height", 50);
    sfColor preview_outline = config_get_color(color_preview_cfg, "outline_color", sfWhite);
    int preview_outline_thickness = config_get_int(color_preview_cfg, "outline_thickness", 3);

    ui->color_preview = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->color_preview, (sfVector2f){preview_width, preview_height});
    sfRectangleShape_setPosition(ui->color_preview, (sfVector2f){preview_x, preview_y});
    sfRectangleShape_setFillColor(ui->color_preview, sfBlack);
    sfRectangleShape_setOutlineColor(ui->color_preview, preview_outline);
    sfRectangleShape_setOutlineThickness(ui->color_preview, preview_outline_thickness);

    // Color sliders (R, G, B)
    cJSON *r_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "r") : NULL;
    cJSON *g_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "g") : NULL;
    cJSON *b_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "b") : NULL;

    int slider_x = toolbar_x + 25;
    float slider_y[] = {config_get_int(r_slider_cfg, "y", 640), config_get_int(g_slider_cfg, "y", 690), config_get_int(b_slider_cfg, "y", 740)};

    sfColor slider_colors[] = {config_get_color(r_slider_cfg, "bar_color", (sfColor){255, 100, 100, 255}), config_get_color(g_slider_cfg, "bar_color", (sfColor){100, 255, 100, 255}),
                               config_get_color(b_slider_cfg, "bar_color", (sfColor){100, 100, 255, 255})};

    int slider_width = config_get_int(r_slider_cfg, "bar_width", 200);
    int slider_height = config_get_int(r_slider_cfg, "bar_height", 20);

    ui->r_bar = sfRectangleShape_create();
    ui->g_bar = sfRectangleShape_create();
    ui->b_bar = sfRectangleShape_create();
    sfRectangleShape **bars[] = {&ui->r_bar, &ui->g_bar, &ui->b_bar};

    for (int i = 0; i < 3; i++) {
        sfRectangleShape_setSize(*bars[i], (sfVector2f){slider_width, slider_height});
        sfRectangleShape_setPosition(*bars[i], (sfVector2f){slider_x, slider_y[i]});
        sfRectangleShape_setFillColor(*bars[i], slider_colors[i]);
    }

    int handle_width = config_get_int(r_slider_cfg, "handle_width", 10);
    int handle_height = config_get_int(r_slider_cfg, "handle_height", 30);

    ui->r_slider = sfRectangleShape_create();
    ui->g_slider = sfRectangleShape_create();
    ui->b_slider = sfRectangleShape_create();
    sfRectangleShape **sliders[] = {&ui->r_slider, &ui->g_slider, &ui->b_slider};

    for (int i = 0; i < 3; i++) {
        sfRectangleShape_setSize(*sliders[i], (sfVector2f){handle_width, handle_height});
        sfRectangleShape_setPosition(*sliders[i], (sfVector2f){slider_x, slider_y[i] - 5});
        sfRectangleShape_setFillColor(*sliders[i], sfWhite);
        sfRectangleShape_setOutlineColor(*sliders[i], sfBlack);
        sfRectangleShape_setOutlineThickness(*sliders[i], 2);
    }

    // Size slider
    cJSON *size_slider_cfg = sliders_cfg ? cJSON_GetObjectItem(sliders_cfg, "size") : NULL;
    int size_slider_y = config_get_int(size_slider_cfg, "y", 805);
    sfColor size_slider_color = config_get_color(size_slider_cfg, "bar_color", (sfColor){150, 150, 150, 255});

    ui->size_bar = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->size_bar, (sfVector2f){slider_width, slider_height});
    sfRectangleShape_setPosition(ui->size_bar, (sfVector2f){slider_x, size_slider_y});
    sfRectangleShape_setFillColor(ui->size_bar, size_slider_color);

    ui->size_slider = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->size_slider, (sfVector2f){handle_width, handle_height});
    sfRectangleShape_setPosition(ui->size_slider, (sfVector2f){slider_x + 20, size_slider_y - 5});
    sfRectangleShape_setFillColor(ui->size_slider, sfWhite);
    sfRectangleShape_setOutlineColor(ui->size_slider, sfBlack);
    sfRectangleShape_setOutlineThickness(ui->size_slider, 2);

    // Top bar
    int topbar_height = config_get_int(topbar_cfg, "height", 40);
    sfColor topbar_color = config_get_color(topbar_cfg, "color", (sfColor){35, 35, 40, 255});

    ui->topbar = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->topbar, (sfVector2f){1400, topbar_height});
    sfRectangleShape_setPosition(ui->topbar, (sfVector2f){0, 0});
    sfRectangleShape_setFillColor(ui->topbar, topbar_color);

    // File button
    cJSON *file_btn_cfg = topbar_cfg ? cJSON_GetObjectItem(topbar_cfg, "buttons") : NULL;
    file_btn_cfg = file_btn_cfg ? cJSON_GetObjectItem(file_btn_cfg, "file") : NULL;

    int file_btn_x = config_get_int(file_btn_cfg, "x", 10);
    int file_btn_y = config_get_int(file_btn_cfg, "y", 5);
    int file_btn_width = config_get_int(file_btn_cfg, "width", 80);
    int file_btn_height = config_get_int(file_btn_cfg, "height", 30);
    const char *file_btn_text = config_get_string(file_btn_cfg, "text", "File");
    int file_btn_font_size = config_get_int(file_btn_cfg, "font_size", 18);
    sfColor file_btn_color = config_get_color(file_btn_cfg, "background_color", (sfColor){50, 50, 55, 255});
    sfColor file_btn_outline = config_get_color(file_btn_cfg, "outline_color", (sfColor){80, 80, 85, 255});

    ui->file_button = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->file_button, (sfVector2f){file_btn_width, file_btn_height});
    sfRectangleShape_setPosition(ui->file_button, (sfVector2f){file_btn_x, file_btn_y});
    sfRectangleShape_setFillColor(ui->file_button, file_btn_color);
    sfRectangleShape_setOutlineColor(ui->file_button, file_btn_outline);
    sfRectangleShape_setOutlineThickness(ui->file_button, 1);

    ui->file_label = sfText_create(font);
    sfText_setString(ui->file_label, file_btn_text);
    sfText_setCharacterSize(ui->file_label, file_btn_font_size);
    sfText_setFillColor(ui->file_label, sfWhite);
    sfText_setPosition(ui->file_label, (sfVector2f){file_btn_x + 18, file_btn_y + 5});

    // Dropdown menu
    int dropdown_x = config_get_int(file_dropdown_cfg, "x", 10);
    int dropdown_y = config_get_int(file_dropdown_cfg, "y", 40);
    int dropdown_width = config_get_int(file_dropdown_cfg, "width", 150);
    int dropdown_item_height = config_get_int(file_dropdown_cfg, "item_height", 35);
    sfColor dropdown_bg_color = config_get_color(file_dropdown_cfg, "background_color", (sfColor){45, 45, 50, 255});
    sfColor dropdown_outline = config_get_color(file_dropdown_cfg, "outline_color", (sfColor){80, 80, 85, 255});
    int dropdown_outline_thickness = config_get_int(file_dropdown_cfg, "outline_thickness", 2);

    ui->dropdown_count = 3;
    ui->dropdown_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->dropdown_bg, (sfVector2f){dropdown_width, ui->dropdown_count * dropdown_item_height});
    sfRectangleShape_setPosition(ui->dropdown_bg, (sfVector2f){dropdown_x, dropdown_y});
    sfRectangleShape_setFillColor(ui->dropdown_bg, dropdown_bg_color);
    sfRectangleShape_setOutlineColor(ui->dropdown_bg, dropdown_outline);
    sfRectangleShape_setOutlineThickness(ui->dropdown_bg, dropdown_outline_thickness);

    ui->dropdown_buttons = malloc(sizeof(sfRectangleShape *) * ui->dropdown_count);
    ui->dropdown_labels = malloc(sizeof(sfText *) * ui->dropdown_count);

    const char *dropdown_names[] = {"New", "Open", "Save"};

    for (int i = 0; i < ui->dropdown_count; i++) {
        ui->dropdown_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ui->dropdown_buttons[i], (sfVector2f){dropdown_width - 4, dropdown_item_height - 3});
        sfRectangleShape_setPosition(ui->dropdown_buttons[i], (sfVector2f){dropdown_x + 2, dropdown_y + 1 + i * dropdown_item_height});
        sfRectangleShape_setFillColor(ui->dropdown_buttons[i], (sfColor){50, 50, 55, 255});

        ui->dropdown_labels[i] = sfText_create(font);
        sfText_setString(ui->dropdown_labels[i], dropdown_names[i]);
        sfText_setCharacterSize(ui->dropdown_labels[i], 16);
        sfText_setFillColor(ui->dropdown_labels[i], sfWhite);
        sfText_setPosition(ui->dropdown_labels[i], (sfVector2f){dropdown_x + 10, dropdown_y + 7 + i * dropdown_item_height});
    }

    // Options button
    cJSON *options_btn_cfg = topbar_cfg ? cJSON_GetObjectItem(topbar_cfg, "buttons") : NULL;
    options_btn_cfg = options_btn_cfg ? cJSON_GetObjectItem(options_btn_cfg, "options") : NULL;

    int options_btn_x = config_get_int(options_btn_cfg, "x", 100);
    int options_btn_y = config_get_int(options_btn_cfg, "y", 5);
    int options_btn_width = config_get_int(options_btn_cfg, "width", 100);
    int options_btn_height = config_get_int(options_btn_cfg, "height", 30);
    const char *options_btn_text = config_get_string(options_btn_cfg, "text", "Options");
    int options_btn_font_size = config_get_int(options_btn_cfg, "font_size", 18);
    sfColor options_btn_color = config_get_color(options_btn_cfg, "background_color", (sfColor){50, 50, 55, 255});
    sfColor options_btn_outline = config_get_color(options_btn_cfg, "outline_color", (sfColor){80, 80, 85, 255});

    ui->options_button = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->options_button, (sfVector2f){options_btn_width, options_btn_height});
    sfRectangleShape_setPosition(ui->options_button, (sfVector2f){options_btn_x, options_btn_y});
    sfRectangleShape_setFillColor(ui->options_button, options_btn_color);
    sfRectangleShape_setOutlineColor(ui->options_button, options_btn_outline);
    sfRectangleShape_setOutlineThickness(ui->options_button, 1);

    ui->options_label = sfText_create(font);
    sfText_setString(ui->options_label, options_btn_text);
    sfText_setCharacterSize(ui->options_label, options_btn_font_size);
    sfText_setFillColor(ui->options_label, sfWhite);
    sfText_setPosition(ui->options_label, (sfVector2f){options_btn_x + 13, options_btn_y + 5});

    // Options dropdown menu
    int options_dropdown_x = config_get_int(options_dropdown_cfg, "x", 100);
    int options_dropdown_y = config_get_int(options_dropdown_cfg, "y", 40);
    int options_dropdown_width = config_get_int(options_dropdown_cfg, "width", 220);
    int options_item_height = config_get_int(options_dropdown_cfg, "item_height", 35);
    sfColor options_dropdown_bg = config_get_color(options_dropdown_cfg, "background_color", (sfColor){45, 45, 50, 255});
    sfColor options_dropdown_outline = config_get_color(options_dropdown_cfg, "outline_color", (sfColor){80, 80, 85, 255});
    int options_outline_thickness = config_get_int(options_dropdown_cfg, "outline_thickness", 2);
    int checkbox_size = config_get_int(options_dropdown_cfg, "checkbox_size", 16);

    ui->options_dropdown_count = 6;
    ui->options_dropdown_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(ui->options_dropdown_bg, (sfVector2f){options_dropdown_width, ui->options_dropdown_count * options_item_height});
    sfRectangleShape_setPosition(ui->options_dropdown_bg, (sfVector2f){options_dropdown_x, options_dropdown_y});
    sfRectangleShape_setFillColor(ui->options_dropdown_bg, options_dropdown_bg);
    sfRectangleShape_setOutlineColor(ui->options_dropdown_bg, options_dropdown_outline);
    sfRectangleShape_setOutlineThickness(ui->options_dropdown_bg, options_outline_thickness);

    ui->options_dropdown_buttons = malloc(sizeof(sfRectangleShape *) * ui->options_dropdown_count);
    ui->options_dropdown_labels = malloc(sizeof(sfText *) * ui->options_dropdown_count);
    ui->options_checkboxes = malloc(sizeof(sfRectangleShape *) * ui->options_dropdown_count);

    const char *options_names[] = {"Show Grid", "Snap to Grid", "Symmetry: None", "Show Debug", "Flip Horizontal", "Flip Vertical"};

    for (int i = 0; i < ui->options_dropdown_count; i++) {
        ui->options_dropdown_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ui->options_dropdown_buttons[i], (sfVector2f){options_dropdown_width - 4, options_item_height - 3});
        sfRectangleShape_setPosition(ui->options_dropdown_buttons[i], (sfVector2f){options_dropdown_x + 2, options_dropdown_y + 1 + i * options_item_height});
        sfRectangleShape_setFillColor(ui->options_dropdown_buttons[i], (sfColor){50, 50, 55, 255});

        ui->options_dropdown_labels[i] = sfText_create(font);
        sfText_setString(ui->options_dropdown_labels[i], options_names[i]);
        sfText_setCharacterSize(ui->options_dropdown_labels[i], 16);
        sfText_setFillColor(ui->options_dropdown_labels[i], sfWhite);
        sfText_setPosition(ui->options_dropdown_labels[i], (sfVector2f){options_dropdown_x + 35, options_dropdown_y + 7 + i * options_item_height});

        // Create checkbox
        ui->options_checkboxes[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(ui->options_checkboxes[i], (sfVector2f){checkbox_size, checkbox_size});
        sfRectangleShape_setPosition(ui->options_checkboxes[i], (sfVector2f){options_dropdown_x + 10, options_dropdown_y + 9 + i * options_item_height});
        sfRectangleShape_setFillColor(ui->options_checkboxes[i], (sfColor){30, 30, 35, 255});
        sfRectangleShape_setOutlineColor(ui->options_checkboxes[i], (sfColor){150, 150, 155, 255});
        sfRectangleShape_setOutlineThickness(ui->options_checkboxes[i], 2);
    }

    // Initialize file explorer
    ui->file_explorer = init_file_explorer(font, config);
    if (!ui->file_explorer) {
        printf("Warning: Failed to initialize file explorer\n");
    }

    return ui;
}

void cleanup_ui(ui_elements_t *ui) {
    if (!ui)
        return;

    if (ui->bg)
        sfRectangleShape_destroy(ui->bg);

    for (int i = 0; i < ui->button_count; i++) {
        if (ui->buttons[i])
            sfRectangleShape_destroy(ui->buttons[i]);
        if (ui->labels[i])
            sfText_destroy(ui->labels[i]);
        if (ui->tool_icons[i])
            sfCircleShape_destroy(ui->tool_icons[i]);
    }

    free(ui->buttons);
    free(ui->labels);
    free(ui->tool_icons);

    if (ui->color_preview)
        sfRectangleShape_destroy(ui->color_preview);
    if (ui->r_slider)
        sfRectangleShape_destroy(ui->r_slider);
    if (ui->g_slider)
        sfRectangleShape_destroy(ui->g_slider);
    if (ui->b_slider)
        sfRectangleShape_destroy(ui->b_slider);
    if (ui->r_bar)
        sfRectangleShape_destroy(ui->r_bar);
    if (ui->g_bar)
        sfRectangleShape_destroy(ui->g_bar);
    if (ui->b_bar)
        sfRectangleShape_destroy(ui->b_bar);
    if (ui->size_slider)
        sfRectangleShape_destroy(ui->size_slider);
    if (ui->size_bar)
        sfRectangleShape_destroy(ui->size_bar);

    // Top bar cleanup
    if (ui->topbar)
        sfRectangleShape_destroy(ui->topbar);
    if (ui->file_button)
        sfRectangleShape_destroy(ui->file_button);
    if (ui->file_label)
        sfText_destroy(ui->file_label);

    // Dropdown cleanup
    if (ui->dropdown_bg)
        sfRectangleShape_destroy(ui->dropdown_bg);

    for (int i = 0; i < ui->dropdown_count; i++) {
        if (ui->dropdown_buttons[i])
            sfRectangleShape_destroy(ui->dropdown_buttons[i]);
        if (ui->dropdown_labels[i])
            sfText_destroy(ui->dropdown_labels[i]);
    }

    free(ui->dropdown_buttons);
    free(ui->dropdown_labels);

    // Options dropdown cleanup
    if (ui->options_button)
        sfRectangleShape_destroy(ui->options_button);
    if (ui->options_label)
        sfText_destroy(ui->options_label);
    if (ui->options_dropdown_bg)
        sfRectangleShape_destroy(ui->options_dropdown_bg);

    for (int i = 0; i < ui->options_dropdown_count; i++) {
        if (ui->options_dropdown_buttons[i])
            sfRectangleShape_destroy(ui->options_dropdown_buttons[i]);
        if (ui->options_dropdown_labels[i])
            sfText_destroy(ui->options_dropdown_labels[i]);
        if (ui->options_checkboxes[i])
            sfRectangleShape_destroy(ui->options_checkboxes[i]);
    }

    free(ui->options_dropdown_buttons);
    free(ui->options_dropdown_labels);
    free(ui->options_checkboxes);

    // File explorer cleanup
    if (ui->file_explorer)
        cleanup_file_explorer(ui->file_explorer);

    free(ui);
}

void render_ui(sfRenderWindow *window, app_t *app) {
    ui_elements_t *ui = app->ui;
    paint_state_t *paint = app->paint;

    // Draw background
    sfRenderWindow_drawRectangleShape(window, ui->bg, NULL);

    // Draw tool buttons
    for (int i = 0; i < ui->button_count; i++) {
        // Highlight selected tool
        if (i == (int)paint->current_tool) {
            sfRectangleShape_setFillColor(ui->buttons[i], (sfColor){100, 150, 200, 255});
        } else {
            sfRectangleShape_setFillColor(ui->buttons[i], (sfColor){70, 70, 75, 255});
        }

        sfRenderWindow_drawRectangleShape(window, ui->buttons[i], NULL);
        sfRenderWindow_drawCircleShape(window, ui->tool_icons[i], NULL);
        sfRenderWindow_drawText(window, ui->labels[i], NULL);
    }

    // Draw color preview
    sfRectangleShape_setFillColor(ui->color_preview, paint->current_color);
    sfRenderWindow_drawRectangleShape(window, ui->color_preview, NULL);

    // Draw color sliders
    sfRenderWindow_drawRectangleShape(window, ui->r_bar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->g_bar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->b_bar, NULL);

    // Update slider positions based on color
    sfRectangleShape_setPosition(ui->r_slider, (sfVector2f){app->toolbar_x + 25 + (paint->current_color.r / 255.0f) * 190, 635});
    sfRectangleShape_setPosition(ui->g_slider, (sfVector2f){app->toolbar_x + 25 + (paint->current_color.g / 255.0f) * 190, 685});
    sfRectangleShape_setPosition(ui->b_slider, (sfVector2f){app->toolbar_x + 25 + (paint->current_color.b / 255.0f) * 190, 735});

    sfRenderWindow_drawRectangleShape(window, ui->r_slider, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->g_slider, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->b_slider, NULL);

    // Draw size slider
    sfRenderWindow_drawRectangleShape(window, ui->size_bar, NULL);

    // Update size slider position
    float size_percent = (paint->brush_size - 1) / 49.0f; // Size range 1-50
    sfRectangleShape_setPosition(ui->size_slider, (sfVector2f){app->toolbar_x + 25 + size_percent * 190, 800});
    sfRenderWindow_drawRectangleShape(window, ui->size_slider, NULL);

    // Draw labels
    sfText *text = sfText_create(ui->font);
    sfText_setCharacterSize(text, 16);
    sfText_setFillColor(text, sfWhite);

    sfText_setString(text, "Color:");
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 15, 535});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_setString(text, "R");
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 230, 640});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_setString(text, "G");
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 230, 690});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_setString(text, "B");
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 230, 740});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_setString(text, "Size:");
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 15, 770});
    sfRenderWindow_drawText(window, text, NULL);

    char size_str[32];
    sprintf(size_str, "%d px", paint->brush_size);
    sfText_setString(text, size_str);
    sfText_setPosition(text, (sfVector2f){app->toolbar_x + 230, 810});
    sfRenderWindow_drawText(window, text, NULL);

    sfText_destroy(text);

    // Draw top bar
    sfRenderWindow_drawRectangleShape(window, ui->topbar, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->file_button, NULL);
    sfRenderWindow_drawText(window, ui->file_label, NULL);
    sfRenderWindow_drawRectangleShape(window, ui->options_button, NULL);
    sfRenderWindow_drawText(window, ui->options_label, NULL);

    // Draw file dropdown menu if open
    if (app->dropdown_open) {
        sfRenderWindow_drawRectangleShape(window, ui->dropdown_bg, NULL);

        for (int i = 0; i < ui->dropdown_count; i++) {
            // Highlight on hover
            sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(window);
            sfVector2f btn_pos = sfRectangleShape_getPosition(ui->dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(ui->dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                sfRectangleShape_setFillColor(ui->dropdown_buttons[i], (sfColor){70, 100, 130, 255});
            } else {
                sfRectangleShape_setFillColor(ui->dropdown_buttons[i], (sfColor){50, 50, 55, 255});
            }

            sfRenderWindow_drawRectangleShape(window, ui->dropdown_buttons[i], NULL);
            sfRenderWindow_drawText(window, ui->dropdown_labels[i], NULL);
        }
    }

    // Draw options dropdown menu if open
    if (app->options_dropdown_open) {
        sfRenderWindow_drawRectangleShape(window, ui->options_dropdown_bg, NULL);

        for (int i = 0; i < ui->options_dropdown_count; i++) {
            // Highlight on hover
            sfVector2i mouse_pos = sfMouse_getPositionRenderWindow(window);
            sfVector2f btn_pos = sfRectangleShape_getPosition(ui->options_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(ui->options_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                sfRectangleShape_setFillColor(ui->options_dropdown_buttons[i], (sfColor){70, 100, 130, 255});
            } else {
                sfRectangleShape_setFillColor(ui->options_dropdown_buttons[i], (sfColor){50, 50, 55, 255});
            }

            sfRenderWindow_drawRectangleShape(window, ui->options_dropdown_buttons[i], NULL);

            // Draw checkbox (not for symmetry option - index 2)
            if (i != 2) {
                sfRectangleShape_setFillColor(ui->options_checkboxes[i], (sfColor){30, 30, 35, 255});

                // Determine if checkbox should be checked
                bool is_checked = false;
                if (i == 0)
                    is_checked = paint->show_grid;
                else if (i == 1)
                    is_checked = paint->snap_to_grid;
                else if (i == 3)
                    is_checked = paint->show_debug;

                // Draw checkmark if enabled
                if (is_checked) {
                    sfRectangleShape_setFillColor(ui->options_checkboxes[i], (sfColor){50, 200, 50, 255});
                }

                sfRenderWindow_drawRectangleShape(window, ui->options_checkboxes[i], NULL);
            }

            // Update symmetry label dynamically
            if (i == 2) {
                const char *sym_modes[] = {"Symmetry: None", "Symmetry: Horizontal", "Symmetry: Vertical", "Symmetry: Both", "Symmetry: Radial 4", "Symmetry: Radial 8"};
                sfText_setString(ui->options_dropdown_labels[i], sym_modes[paint->symmetry_mode]);
            }

            sfRenderWindow_drawText(window, ui->options_dropdown_labels[i], NULL);
        }
    }

    // Draw file explorer overlay (always last so it's on top)
    if (ui->file_explorer) {
        render_file_explorer(window, ui->file_explorer);
    }
}

void handle_ui_click(app_t *app, sfVector2i mouse_pos) {
    // Check if file explorer is open - handle its clicks first (it's on top)
    if (app->ui->file_explorer && app->ui->file_explorer->mode != FILE_EXPLORER_CLOSED) {
        handle_file_explorer_click(app, mouse_pos);
        return;
    }

    // Check if clicking on File button
    sfVector2f file_pos = sfRectangleShape_getPosition(app->ui->file_button);
    sfVector2f file_size = sfRectangleShape_getSize(app->ui->file_button);

    if (is_point_in_rect(mouse_pos, file_pos, file_size)) {
        app->dropdown_open = !app->dropdown_open;
        app->options_dropdown_open = 0; // Close options menu
        printf("File menu %s\n", app->dropdown_open ? "opened" : "closed");
        return;
    }

    // Check if clicking on Options button
    sfVector2f options_pos = sfRectangleShape_getPosition(app->ui->options_button);
    sfVector2f options_size = sfRectangleShape_getSize(app->ui->options_button);

    if (is_point_in_rect(mouse_pos, options_pos, options_size)) {
        app->options_dropdown_open = !app->options_dropdown_open;
        app->dropdown_open = 0; // Close file menu
        printf("Options menu %s\n", app->options_dropdown_open ? "opened" : "closed");
        return;
    }

    // Check if clicking on file dropdown menu items
    if (app->dropdown_open) {
        for (int i = 0; i < app->ui->dropdown_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(app->ui->dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                const char *actions[] = {"New", "Open", "Save"};
                printf("%s clicked\n", actions[i]);

                if (i == 0) { // New
                    save_undo_state(app->paint);
                    clear_canvas(app->paint);
                    printf("New canvas created\n");
                } else if (i == 1) { // Open
                    if (app->ui->file_explorer)
                        open_file_explorer(app->ui->file_explorer, FILE_EXPLORER_OPEN);
                } else if (i == 2) { // Save
                    if (app->ui->file_explorer)
                        open_file_explorer(app->ui->file_explorer, FILE_EXPLORER_SAVE);
                }

                app->dropdown_open = 0;
                return;
            }
        }
    }

    // Check if clicking on options dropdown menu items
    if (app->options_dropdown_open) {
        for (int i = 0; i < app->ui->options_dropdown_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->options_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(app->ui->options_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                if (i == 0) { // Toggle Show Grid
                    app->paint->show_grid = !app->paint->show_grid;
                    printf("Grid %s\n", app->paint->show_grid ? "ON" : "OFF");
                } else if (i == 1) { // Toggle Snap to Grid
                    app->paint->snap_to_grid = !app->paint->snap_to_grid;
                    printf("Snap to grid %s\n", app->paint->snap_to_grid ? "ON" : "OFF");
                } else if (i == 2) { // Cycle Symmetry Mode
                    app->paint->symmetry_mode = (app->paint->symmetry_mode + 1) % 6;
                    const char *modes[] = {"None", "Horizontal", "Vertical", "Both", "Radial 4", "Radial 8"};
                    printf("Symmetry mode: %s\n", modes[app->paint->symmetry_mode]);
                } else if (i == 3) { // Toggle Show Debug
                    app->paint->show_debug = !app->paint->show_debug;
                    printf("Debug info %s\n", app->paint->show_debug ? "ON" : "OFF");
                } else if (i == 4) { // Flip Horizontal
                    save_undo_state(app->paint);
                    flip_canvas_horizontal(app->paint);
                } else if (i == 5) { // Flip Vertical
                    save_undo_state(app->paint);
                    flip_canvas_vertical(app->paint);
                }

                return; // Keep menu open for toggles
            }
        }
    }

    // Close dropdowns if clicking elsewhere
    if (app->dropdown_open || app->options_dropdown_open) {
        app->dropdown_open = 0;
        app->options_dropdown_open = 0;
        return;
    }

    // Check tool buttons
    for (int i = 0; i < app->ui->button_count; i++) {
        sfVector2f btn_pos = sfRectangleShape_getPosition(app->ui->buttons[i]);
        sfVector2f btn_size = sfRectangleShape_getSize(app->ui->buttons[i]);

        if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
            app->paint->current_tool = (tool_type_t)i;
            const char *tool_names[] = {"Pen", "Eraser", "Brush", "Spray", "Line", "Rectangle", "Circle", "Fill"};
            printf("🔧 Tool changed to: %s\n", tool_names[i]);
            return;
        }
    }

    // Check color sliders
    sfVector2f r_pos = sfRectangleShape_getPosition(app->ui->r_bar);
    sfVector2f slider_size = {200, 20};

    if (is_point_in_rect(mouse_pos, r_pos, slider_size)) {
        app->dragging_slider = 0;
        update_color_slider(app, mouse_pos);
        return;
    }

    sfVector2f g_pos = sfRectangleShape_getPosition(app->ui->g_bar);
    if (is_point_in_rect(mouse_pos, g_pos, slider_size)) {
        app->dragging_slider = 1;
        update_color_slider(app, mouse_pos);
        return;
    }

    sfVector2f b_pos = sfRectangleShape_getPosition(app->ui->b_bar);
    if (is_point_in_rect(mouse_pos, b_pos, slider_size)) {
        app->dragging_slider = 2;
        update_color_slider(app, mouse_pos);
        return;
    }

    // Check size slider
    sfVector2f size_pos = sfRectangleShape_getPosition(app->ui->size_bar);
    if (is_point_in_rect(mouse_pos, size_pos, slider_size)) {
        app->dragging_slider = 3;
        update_color_slider(app, mouse_pos);
        return;
    }
}

void update_color_slider(app_t *app, sfVector2i mouse_pos) {
    if (app->dragging_slider == 0) {
        // Red slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->r_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.r = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 1) {
        // Green slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->g_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.g = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 2) {
        // Blue slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->b_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->current_color.b = (uint8_t)(percent * 255);
    } else if (app->dragging_slider == 3) {
        // Size slider
        sfVector2f pos = sfRectangleShape_getPosition(app->ui->size_bar);
        float percent = (mouse_pos.x - pos.x) / 200.0f;
        percent = fmax(0, fmin(1, percent));
        app->paint->brush_size = 1 + (int)(percent * 49); // Range 1-50
    }
}
