#include "../include/paint.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Helper function to check if a file is an image
static int is_image_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext)
        return 0;

    return (strcasecmp(ext, ".png") == 0 || strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0 || strcasecmp(ext, ".bmp") == 0 || strcasecmp(ext, ".tga") == 0);
}

// Compare function for sorting entries (directories first, then files alphabetically)
static int compare_entries(const void *a, const void *b) {
    file_entry_t *entry_a = (file_entry_t *)a;
    file_entry_t *entry_b = (file_entry_t *)b;

    // Parent directory always first
    if (entry_a->type == FILE_TYPE_PARENT)
        return -1;
    if (entry_b->type == FILE_TYPE_PARENT)
        return 1;

    // Directories before files
    if (entry_a->type == FILE_TYPE_DIRECTORY && entry_b->type != FILE_TYPE_DIRECTORY)
        return -1;
    if (entry_a->type != FILE_TYPE_DIRECTORY && entry_b->type == FILE_TYPE_DIRECTORY)
        return 1;

    // Alphabetical order
    return strcasecmp(entry_a->name, entry_b->name);
}

void refresh_file_list(file_explorer_t *explorer) {
    if (!explorer)
        return;

    explorer->entry_count = 0;
    explorer->selected_index = -1;

    DIR *dir = opendir(explorer->current_path);
    if (!dir) {
        printf("Failed to open directory: %s\n", explorer->current_path);
        return;
    }

    // Add parent directory entry if not at root
    if (strcmp(explorer->current_path, "/") != 0) {
        strcpy(explorer->entries[explorer->entry_count].name, "..");
        explorer->entries[explorer->entry_count].type = FILE_TYPE_PARENT;
        explorer->entries[explorer->entry_count].is_image = 0;
        explorer->entry_count++;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && explorer->entry_count < MAX_FILE_ENTRIES) {
        // Skip hidden files and current directory
        if (entry->d_name[0] == '.')
            continue;

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", explorer->current_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0)
            continue;

        file_entry_t *file_entry = &explorer->entries[explorer->entry_count];
        strncpy(file_entry->name, entry->d_name, MAX_FILENAME_LENGTH - 1);
        file_entry->name[MAX_FILENAME_LENGTH - 1] = '\0';

        if (S_ISDIR(st.st_mode)) {
            file_entry->type = FILE_TYPE_DIRECTORY;
            file_entry->is_image = 0;
        } else {
            file_entry->type = FILE_TYPE_FILE;
            file_entry->is_image = is_image_file(entry->d_name);
        }

        explorer->entry_count++;
    }

    closedir(dir);

    // Sort entries
    qsort(explorer->entries, explorer->entry_count, sizeof(file_entry_t), compare_entries);

    explorer->scroll_offset = 0;
}

void navigate_to_directory(file_explorer_t *explorer, const char *path) {
    if (!explorer || !path)
        return;

    // Normalize the path
    char normalized_path[MAX_PATH_LENGTH];

    if (strcmp(path, "..") == 0) {
        // Go to parent directory
        char *last_slash = strrchr(explorer->current_path, '/');
        if (last_slash && last_slash != explorer->current_path) {
            *last_slash = '\0';
            strncpy(normalized_path, explorer->current_path, MAX_PATH_LENGTH - 1);
        } else {
            strcpy(normalized_path, "/");
        }
    } else if (path[0] == '/') {
        // Absolute path
        strncpy(normalized_path, path, MAX_PATH_LENGTH - 1);
    } else {
        // Relative path
        if (strcmp(explorer->current_path, "/") == 0)
            snprintf(normalized_path, MAX_PATH_LENGTH, "/%s", path);
        else
            snprintf(normalized_path, MAX_PATH_LENGTH, "%s/%s", explorer->current_path, path);
    }

    normalized_path[MAX_PATH_LENGTH - 1] = '\0';

    // Check if directory exists
    struct stat st;
    if (stat(normalized_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        strncpy(explorer->current_path, normalized_path, MAX_PATH_LENGTH - 1);
        explorer->current_path[MAX_PATH_LENGTH - 1] = '\0';
        refresh_file_list(explorer);
    } else {
        printf("Invalid directory: %s\n", normalized_path);
    }
}

file_explorer_t *init_file_explorer(sfFont *font, ui_config_t *config) {
    file_explorer_t *explorer = malloc(sizeof(file_explorer_t));
    if (!explorer)
        return NULL;

    explorer->mode = FILE_EXPLORER_CLOSED;
    explorer->entry_count = 0;
    explorer->scroll_offset = 0;
    explorer->selected_index = -1;
    explorer->input_cursor_pos = 0;
    explorer->dragging_scrollbar = 0;
    strcpy(explorer->filename_input, "untitled.png");

    // Load icon configuration
    cJSON *fe_cfg = config ? config_get_section(config, "file_explorer.icons") : NULL;
    explorer->icon_size = config_get_int(fe_cfg, "icon_size", 20);
    const char *folder_icon_path = config_get_string(fe_cfg, "folder", "assets/icons/folder.png");
    const char *image_icon_path = config_get_string(fe_cfg, "image", "assets/icons/image.png");
    const char *file_icon_path = config_get_string(fe_cfg, "file", "assets/icons/file.png");

    // Load icon textures
    explorer->folder_icon_texture = sfTexture_createFromFile(folder_icon_path, NULL);
    explorer->image_icon_texture = sfTexture_createFromFile(image_icon_path, NULL);
    explorer->file_icon_texture = sfTexture_createFromFile(file_icon_path, NULL);

    // Create icon sprites
    if (explorer->folder_icon_texture) {
        explorer->folder_icon_sprite = sfSprite_create(explorer->folder_icon_texture);
        sfSprite_setScale(explorer->folder_icon_sprite, (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->folder_icon_texture).x,
                                                                     (float)explorer->icon_size / sfTexture_getSize(explorer->folder_icon_texture).y});
    } else {
        explorer->folder_icon_sprite = NULL;
    }

    if (explorer->image_icon_texture) {
        explorer->image_icon_sprite = sfSprite_create(explorer->image_icon_texture);
        sfSprite_setScale(explorer->image_icon_sprite,
                          (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->image_icon_texture).x, (float)explorer->icon_size / sfTexture_getSize(explorer->image_icon_texture).y});
    } else {
        explorer->image_icon_sprite = NULL;
    }

    if (explorer->file_icon_texture) {
        explorer->file_icon_sprite = sfSprite_create(explorer->file_icon_texture);
        sfSprite_setScale(explorer->file_icon_sprite,
                          (sfVector2f){(float)explorer->icon_size / sfTexture_getSize(explorer->file_icon_texture).x, (float)explorer->icon_size / sfTexture_getSize(explorer->file_icon_texture).y});
    } else {
        explorer->file_icon_sprite = NULL;
    }

    // Get home directory
    const char *home = getenv("HOME");
    if (home)
        strncpy(explorer->current_path, home, MAX_PATH_LENGTH - 1);
    else
        strcpy(explorer->current_path, "/");
    explorer->current_path[MAX_PATH_LENGTH - 1] = '\0';

    // Create overlay background (semi-transparent)
    explorer->overlay_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->overlay_bg, (sfVector2f){1400, 920});
    sfRectangleShape_setPosition(explorer->overlay_bg, (sfVector2f){0, 0});
    sfRectangleShape_setFillColor(explorer->overlay_bg, (sfColor){0, 0, 0, 180});

    // Create main explorer window
    explorer->explorer_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->explorer_bg, (sfVector2f){700, 600});
    sfRectangleShape_setPosition(explorer->explorer_bg, (sfVector2f){350, 160});
    sfRectangleShape_setFillColor(explorer->explorer_bg, (sfColor){40, 40, 45, 255});
    sfRectangleShape_setOutlineColor(explorer->explorer_bg, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->explorer_bg, 2);

    // Title text
    explorer->title_text = sfText_create(font);
    sfText_setString(explorer->title_text, "Open File");
    sfText_setFont(explorer->title_text, font);
    sfText_setCharacterSize(explorer->title_text, 24);
    sfText_setFillColor(explorer->title_text, sfWhite);
    sfText_setPosition(explorer->title_text, (sfVector2f){370, 170});

    // Path bar
    explorer->path_bar = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->path_bar, (sfVector2f){680, 35});
    sfRectangleShape_setPosition(explorer->path_bar, (sfVector2f){360, 210});
    sfRectangleShape_setFillColor(explorer->path_bar, (sfColor){30, 30, 35, 255});
    sfRectangleShape_setOutlineColor(explorer->path_bar, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineThickness(explorer->path_bar, 1);

    explorer->path_text = sfText_create(font);
    sfText_setFont(explorer->path_text, font);
    sfText_setCharacterSize(explorer->path_text, 16);
    sfText_setFillColor(explorer->path_text, sfWhite);
    sfText_setPosition(explorer->path_text, (sfVector2f){370, 218});

    // File list area - create buttons for file entries
    explorer->file_buttons = malloc(sizeof(sfRectangleShape *) * 15); // Show 15 entries at a time
    explorer->file_labels = malloc(sizeof(sfText *) * 15);

    for (int i = 0; i < 15; i++) {
        explorer->file_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(explorer->file_buttons[i], (sfVector2f){650, 30});
        sfRectangleShape_setPosition(explorer->file_buttons[i], (sfVector2f){365, 255 + i * 32});
        sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){50, 50, 55, 255});

        explorer->file_labels[i] = sfText_create(font);
        sfText_setFont(explorer->file_labels[i], font);
        sfText_setCharacterSize(explorer->file_labels[i], 16);
        sfText_setFillColor(explorer->file_labels[i], sfWhite);
        sfText_setPosition(explorer->file_labels[i], (sfVector2f){375, 258 + i * 32});
    }

    // Scrollbar
    explorer->scrollbar = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->scrollbar, (sfVector2f){15, 480});
    sfRectangleShape_setPosition(explorer->scrollbar, (sfVector2f){1025, 255});
    sfRectangleShape_setFillColor(explorer->scrollbar, (sfColor){30, 30, 35, 255});

    explorer->scrollbar_thumb = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->scrollbar_thumb, (sfVector2f){13, 60});
    sfRectangleShape_setPosition(explorer->scrollbar_thumb, (sfVector2f){1026, 256});
    sfRectangleShape_setFillColor(explorer->scrollbar_thumb, (sfColor){100, 100, 105, 255});

    // Filename input box (for save mode)
    explorer->filename_input_box = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->filename_input_box, (sfVector2f){550, 35});
    sfRectangleShape_setPosition(explorer->filename_input_box, (sfVector2f){360, 670});
    sfRectangleShape_setFillColor(explorer->filename_input_box, (sfColor){30, 30, 35, 255});
    sfRectangleShape_setOutlineColor(explorer->filename_input_box, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineThickness(explorer->filename_input_box, 1);

    explorer->filename_input_text = sfText_create(font);
    sfText_setFont(explorer->filename_input_text, font);
    sfText_setCharacterSize(explorer->filename_input_text, 16);
    sfText_setFillColor(explorer->filename_input_text, sfWhite);
    sfText_setPosition(explorer->filename_input_text, (sfVector2f){370, 678});

    // Cursor indicator
    explorer->cursor_indicator = sfText_create(font);
    sfText_setString(explorer->cursor_indicator, "|");
    sfText_setFont(explorer->cursor_indicator, font);
    sfText_setCharacterSize(explorer->cursor_indicator, 16);
    sfText_setFillColor(explorer->cursor_indicator, sfWhite);

    // Initialize format options
    explorer->format_count = 5;
    strcpy(explorer->format_options[0], ".png");
    strcpy(explorer->format_options[1], ".jpg");
    strcpy(explorer->format_options[2], ".bmp");
    strcpy(explorer->format_options[3], ".tga");
    strcpy(explorer->format_options[4], "personalised");
    explorer->selected_format_index = 0;
    strcpy(explorer->extension, ".png");
    explorer->format_dropdown_open = 0;
    explorer->extension_editable = 0;
    explorer->base_filename[0] = '\0';

    // Format button
    explorer->format_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->format_button, (sfVector2f){120, 35});
    sfRectangleShape_setPosition(explorer->format_button, (sfVector2f){700, 715});
    sfRectangleShape_setFillColor(explorer->format_button, (sfColor){60, 60, 65, 255});
    sfRectangleShape_setOutlineColor(explorer->format_button, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->format_button, 1);

    explorer->format_button_label = sfText_create(font);
    sfText_setString(explorer->format_button_label, ".png");
    sfText_setFont(explorer->format_button_label, font);
    sfText_setCharacterSize(explorer->format_button_label, 16);
    sfText_setFillColor(explorer->format_button_label, sfWhite);
    sfText_setPosition(explorer->format_button_label, (sfVector2f){735, 720});

    // Format dropdown background
    explorer->format_dropdown_bg = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->format_dropdown_bg, (sfVector2f){120, 175});
    sfRectangleShape_setPosition(explorer->format_dropdown_bg, (sfVector2f){700, 540});
    sfRectangleShape_setFillColor(explorer->format_dropdown_bg, (sfColor){40, 40, 45, 255});
    sfRectangleShape_setOutlineColor(explorer->format_dropdown_bg, (sfColor){80, 80, 85, 255});
    sfRectangleShape_setOutlineThickness(explorer->format_dropdown_bg, 2);

    // Format dropdown items
    explorer->format_dropdown_buttons = malloc(sizeof(sfRectangleShape *) * 5);
    explorer->format_dropdown_labels = malloc(sizeof(sfText *) * 5);

    for (int i = 0; i < 5; i++) {
        explorer->format_dropdown_buttons[i] = sfRectangleShape_create();
        sfRectangleShape_setSize(explorer->format_dropdown_buttons[i], (sfVector2f){116, 33});
        sfRectangleShape_setPosition(explorer->format_dropdown_buttons[i], (sfVector2f){702, 542 + i * 35});
        sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){50, 50, 55, 255});

        explorer->format_dropdown_labels[i] = sfText_create(font);
        sfText_setString(explorer->format_dropdown_labels[i], explorer->format_options[i]);
        sfText_setFont(explorer->format_dropdown_labels[i], font);
        sfText_setCharacterSize(explorer->format_dropdown_labels[i], 14);
        sfText_setFillColor(explorer->format_dropdown_labels[i], sfWhite);
        sfText_setPosition(explorer->format_dropdown_labels[i], (sfVector2f){710, 548 + i * 35});
    }

    // OK button
    explorer->ok_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->ok_button, (sfVector2f){100, 35});
    sfRectangleShape_setPosition(explorer->ok_button, (sfVector2f){830, 715});
    sfRectangleShape_setFillColor(explorer->ok_button, (sfColor){70, 120, 200, 255});
    sfRectangleShape_setOutlineColor(explorer->ok_button, (sfColor){90, 140, 220, 255});
    sfRectangleShape_setOutlineThickness(explorer->ok_button, 1);

    explorer->ok_label = sfText_create(font);
    sfText_setString(explorer->ok_label, "OK");
    sfText_setFont(explorer->ok_label, font);
    sfText_setCharacterSize(explorer->ok_label, 18);
    sfText_setFillColor(explorer->ok_label, sfWhite);
    sfText_setPosition(explorer->ok_label, (sfVector2f){867, 720});

    // Cancel button
    explorer->cancel_button = sfRectangleShape_create();
    sfRectangleShape_setSize(explorer->cancel_button, (sfVector2f){100, 35});
    sfRectangleShape_setPosition(explorer->cancel_button, (sfVector2f){940, 715});
    sfRectangleShape_setFillColor(explorer->cancel_button, (sfColor){100, 100, 105, 255});
    sfRectangleShape_setOutlineColor(explorer->cancel_button, (sfColor){120, 120, 125, 255});
    sfRectangleShape_setOutlineThickness(explorer->cancel_button, 1);

    explorer->cancel_label = sfText_create(font);
    sfText_setString(explorer->cancel_label, "Cancel");
    sfText_setFont(explorer->cancel_label, font);
    sfText_setCharacterSize(explorer->cancel_label, 18);
    sfText_setFillColor(explorer->cancel_label, sfWhite);
    sfText_setPosition(explorer->cancel_label, (sfVector2f){960, 720});

    return explorer;
}

void cleanup_file_explorer(file_explorer_t *explorer) {
    if (!explorer)
        return;

    sfRectangleShape_destroy(explorer->overlay_bg);
    sfRectangleShape_destroy(explorer->explorer_bg);
    sfRectangleShape_destroy(explorer->path_bar);
    sfText_destroy(explorer->path_text);
    sfRectangleShape_destroy(explorer->filename_input_box);
    sfText_destroy(explorer->filename_input_text);
    sfText_destroy(explorer->cursor_indicator);
    sfRectangleShape_destroy(explorer->scrollbar);
    sfRectangleShape_destroy(explorer->scrollbar_thumb);
    sfRectangleShape_destroy(explorer->ok_button);
    sfText_destroy(explorer->ok_label);
    sfRectangleShape_destroy(explorer->cancel_button);
    sfText_destroy(explorer->cancel_label);
    sfText_destroy(explorer->title_text);

    // Format dropdown cleanup
    sfRectangleShape_destroy(explorer->format_button);
    sfText_destroy(explorer->format_button_label);
    sfRectangleShape_destroy(explorer->format_dropdown_bg);

    for (int i = 0; i < 5; i++) {
        sfRectangleShape_destroy(explorer->format_dropdown_buttons[i]);
        sfText_destroy(explorer->format_dropdown_labels[i]);
    }
    free(explorer->format_dropdown_buttons);
    free(explorer->format_dropdown_labels);

    for (int i = 0; i < 15; i++) {
        sfRectangleShape_destroy(explorer->file_buttons[i]);
        sfText_destroy(explorer->file_labels[i]);
    }

    free(explorer->file_buttons);
    free(explorer->file_labels);

    // Cleanup icon resources
    if (explorer->folder_icon_sprite)
        sfSprite_destroy(explorer->folder_icon_sprite);
    if (explorer->folder_icon_texture)
        sfTexture_destroy(explorer->folder_icon_texture);
    if (explorer->image_icon_sprite)
        sfSprite_destroy(explorer->image_icon_sprite);
    if (explorer->image_icon_texture)
        sfTexture_destroy(explorer->image_icon_texture);
    if (explorer->file_icon_sprite)
        sfSprite_destroy(explorer->file_icon_sprite);
    if (explorer->file_icon_texture)
        sfTexture_destroy(explorer->file_icon_texture);

    free(explorer);
}

void open_file_explorer(file_explorer_t *explorer, file_explorer_mode_t mode) {
    if (!explorer)
        return;

    explorer->mode = mode;
    explorer->selected_index = -1;
    explorer->scroll_offset = 0;
    explorer->format_dropdown_open = 0;
    explorer->extension_editable = 0;
    explorer->selected_format_index = 0;

    if (mode == FILE_EXPLORER_SAVE) {
        sfText_setString(explorer->title_text, "Save File");
        strcpy(explorer->base_filename, "untitled");
        strcpy(explorer->extension, ".png");
        snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        explorer->input_cursor_pos = strlen(explorer->base_filename);
        sfText_setString(explorer->format_button_label, explorer->extension);
    } else if (mode == FILE_EXPLORER_OPEN) {
        sfText_setString(explorer->title_text, "Open File");
        explorer->filename_input[0] = '\0';
        explorer->base_filename[0] = '\0';
        explorer->input_cursor_pos = 0;
    }

    refresh_file_list(explorer);
}

void close_file_explorer(file_explorer_t *explorer) {
    if (!explorer)
        return;

    explorer->mode = FILE_EXPLORER_CLOSED;
}

void render_file_explorer(sfRenderWindow *window, file_explorer_t *explorer) {
    if (!explorer || explorer->mode == FILE_EXPLORER_CLOSED)
        return;

    // Draw overlay
    sfRenderWindow_drawRectangleShape(window, explorer->overlay_bg, NULL);

    // Draw main window
    sfRenderWindow_drawRectangleShape(window, explorer->explorer_bg, NULL);

    // Draw title
    sfRenderWindow_drawText(window, explorer->title_text, NULL);

    // Draw path bar
    sfRenderWindow_drawRectangleShape(window, explorer->path_bar, NULL);
    sfText_setString(explorer->path_text, explorer->current_path);
    sfRenderWindow_drawText(window, explorer->path_text, NULL);

    // Draw file list
    int visible_count = (explorer->entry_count - explorer->scroll_offset > 15) ? 15 : (explorer->entry_count - explorer->scroll_offset);

    for (int i = 0; i < visible_count; i++) {
        int entry_idx = i + explorer->scroll_offset;
        if (entry_idx >= explorer->entry_count)
            break;

        file_entry_t *entry = &explorer->entries[entry_idx];

        // Highlight selected entry
        if (entry_idx == explorer->selected_index)
            sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){80, 120, 160, 255});
        else
            sfRectangleShape_setFillColor(explorer->file_buttons[i], (sfColor){50, 50, 55, 255});

        sfRenderWindow_drawRectangleShape(window, explorer->file_buttons[i], NULL);

        // Draw icon
        sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->file_buttons[i]);
        sfSprite *icon_to_draw = NULL;

        if (entry->type == FILE_TYPE_DIRECTORY || entry->type == FILE_TYPE_PARENT)
            icon_to_draw = explorer->folder_icon_sprite;
        else if (entry->is_image)
            icon_to_draw = explorer->image_icon_sprite;
        else
            icon_to_draw = explorer->file_icon_sprite;

        if (icon_to_draw) {
            sfSprite_setPosition(icon_to_draw, (sfVector2f){btn_pos.x + 5, btn_pos.y + 5});
            sfRenderWindow_drawSprite(window, icon_to_draw, NULL);
        }

        // Draw filename (offset by icon width)
        sfText_setString(explorer->file_labels[i], entry->name);
        sfText_setPosition(explorer->file_labels[i], (sfVector2f){btn_pos.x + explorer->icon_size + 10, btn_pos.y + 5});
        sfRenderWindow_drawText(window, explorer->file_labels[i], NULL);
    }

    // Draw scrollbar if needed
    if (explorer->entry_count > 15) {
        sfRenderWindow_drawRectangleShape(window, explorer->scrollbar, NULL);

        // Calculate thumb position
        float thumb_ratio = 15.0f / explorer->entry_count;
        float thumb_height = thumb_ratio * 480.0f;
        if (thumb_height < 30)
            thumb_height = 30;

        float scroll_ratio = (float)explorer->scroll_offset / (explorer->entry_count - 15);
        float thumb_y = 256 + scroll_ratio * (480 - thumb_height);

        sfRectangleShape_setSize(explorer->scrollbar_thumb, (sfVector2f){13, thumb_height});
        sfRectangleShape_setPosition(explorer->scrollbar_thumb, (sfVector2f){1026, thumb_y});

        sfRenderWindow_drawRectangleShape(window, explorer->scrollbar_thumb, NULL);
    }

    // Draw filename input box (only in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE) {
        sfRenderWindow_drawRectangleShape(window, explorer->filename_input_box, NULL);
        sfText_setString(explorer->filename_input_text, explorer->filename_input);
        sfRenderWindow_drawText(window, explorer->filename_input_text, NULL);

        // Draw cursor - calculate position using findCharacterPos
        sfText *temp_text = sfText_create(sfText_getFont(explorer->filename_input_text));
        sfText_setString(temp_text, explorer->filename_input);
        sfText_setCharacterSize(temp_text, 16);
        sfText_setPosition(temp_text, (sfVector2f){370, 678});

        sfVector2f cursor_pos = sfText_findCharacterPos(temp_text, explorer->input_cursor_pos);
        sfText_destroy(temp_text);

        sfText_setPosition(explorer->cursor_indicator, (sfVector2f){cursor_pos.x, 678});
        sfRenderWindow_drawText(window, explorer->cursor_indicator, NULL);

        // Draw format button
        sfRenderWindow_drawRectangleShape(window, explorer->format_button, NULL);
        sfRenderWindow_drawText(window, explorer->format_button_label, NULL);

        // Draw format dropdown if open
        if (explorer->format_dropdown_open) {
            sfRenderWindow_drawRectangleShape(window, explorer->format_dropdown_bg, NULL);

            for (int i = 0; i < explorer->format_count; i++) {
                // Highlight selected format
                if (i == explorer->selected_format_index) {
                    sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){70, 120, 200, 255});
                } else {
                    sfRectangleShape_setFillColor(explorer->format_dropdown_buttons[i], (sfColor){50, 50, 55, 255});
                }

                sfRenderWindow_drawRectangleShape(window, explorer->format_dropdown_buttons[i], NULL);
                sfRenderWindow_drawText(window, explorer->format_dropdown_labels[i], NULL);
            }
        }
    }

    // Draw buttons
    sfRenderWindow_drawRectangleShape(window, explorer->ok_button, NULL);
    sfRenderWindow_drawText(window, explorer->ok_label, NULL);
    sfRenderWindow_drawRectangleShape(window, explorer->cancel_button, NULL);
    sfRenderWindow_drawText(window, explorer->cancel_label, NULL);
}

void handle_file_explorer_text(file_explorer_t *explorer, uint32_t unicode) {
    if (!explorer || explorer->mode != FILE_EXPLORER_SAVE)
        return;

    int base_len = strlen(explorer->base_filename);

    if (unicode == 8) // Backspace
    {
        if (explorer->input_cursor_pos > 0 && explorer->input_cursor_pos <= base_len) {
            // Remove character before cursor in base_filename
            memmove(&explorer->base_filename[explorer->input_cursor_pos - 1], &explorer->base_filename[explorer->input_cursor_pos], base_len - explorer->input_cursor_pos + 1);
            explorer->input_cursor_pos--;
            // Update full filename
            snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        }
    } else if (unicode == 127) // Delete key
    {
        if (explorer->input_cursor_pos < base_len) {
            // Remove character at cursor position
            memmove(&explorer->base_filename[explorer->input_cursor_pos], &explorer->base_filename[explorer->input_cursor_pos + 1], base_len - explorer->input_cursor_pos);
            // Update full filename
            snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
        }
    } else if (unicode >= 32 && unicode < 127) // Printable characters
    {
        // Check if we're in personalised mode and cursor is in extension area
        if (explorer->extension_editable && explorer->input_cursor_pos >= base_len) {
            int ext_pos = explorer->input_cursor_pos - base_len;
            int ext_len = strlen(explorer->extension);
            if (ext_len < 15) {
                // Insert into extension
                memmove(&explorer->extension[ext_pos + 1], &explorer->extension[ext_pos], ext_len - ext_pos + 1);
                explorer->extension[ext_pos] = (char)unicode;
                explorer->input_cursor_pos++;
                snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
            }
        } else if (explorer->input_cursor_pos <= base_len) {
            // Insert into base filename
            if (base_len < MAX_FILENAME_LENGTH - 16) {
                memmove(&explorer->base_filename[explorer->input_cursor_pos + 1], &explorer->base_filename[explorer->input_cursor_pos], base_len - explorer->input_cursor_pos + 1);
                explorer->base_filename[explorer->input_cursor_pos] = (char)unicode;
                explorer->input_cursor_pos++;
                // Update full filename
                snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
            }
        }
    }
}

void handle_file_explorer_keypress(file_explorer_t *explorer, sfKeyCode key) {
    if (!explorer || explorer->mode != FILE_EXPLORER_SAVE)
        return;

    int base_len = strlen(explorer->base_filename);
    int total_len = strlen(explorer->filename_input);
    int max_cursor = explorer->extension_editable ? total_len : base_len;

    switch (key) {
    case sfKeyLeft:
        if (explorer->input_cursor_pos > 0)
            explorer->input_cursor_pos--;
        break;

    case sfKeyRight:
        if (explorer->input_cursor_pos < max_cursor)
            explorer->input_cursor_pos++;
        break;

    case sfKeyHome:
        explorer->input_cursor_pos = 0;
        break;

    case sfKeyEnd:
        explorer->input_cursor_pos = max_cursor;
        break;

    case sfKeyDelete:
        handle_file_explorer_text(explorer, 127);
        break;

    default:
        break;
    }
}

void handle_file_explorer_click(app_t *app, sfVector2i mouse_pos) {
    file_explorer_t *explorer = app->ui->file_explorer;
    if (!explorer || explorer->mode == FILE_EXPLORER_CLOSED)
        return;

    // Check format dropdown clicks (if open and in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE && explorer->format_dropdown_open) {
        for (int i = 0; i < explorer->format_count; i++) {
            sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->format_dropdown_buttons[i]);
            sfVector2f btn_size = sfRectangleShape_getSize(explorer->format_dropdown_buttons[i]);

            if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
                explorer->selected_format_index = i;

                if (i == 4) {
                    // "personalised" selected - enable extension editing
                    explorer->extension_editable = 1;
                } else {
                    // Specific format selected
                    explorer->extension_editable = 0;
                    strcpy(explorer->extension, explorer->format_options[i]);
                    sfText_setString(explorer->format_button_label, explorer->extension);
                    snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);
                    explorer->input_cursor_pos = strlen(explorer->base_filename);
                }

                explorer->format_dropdown_open = 0;
                return;
            }
        }

        // Click outside dropdown - close it
        explorer->format_dropdown_open = 0;
        return;
    }

    // Check format button click (in save mode)
    if (explorer->mode == FILE_EXPLORER_SAVE) {
        sfVector2f format_btn_pos = sfRectangleShape_getPosition(explorer->format_button);
        sfVector2f format_btn_size = sfRectangleShape_getSize(explorer->format_button);

        if (is_point_in_rect(mouse_pos, format_btn_pos, format_btn_size)) {
            explorer->format_dropdown_open = !explorer->format_dropdown_open;
            return;
        }
    }

    // Check Cancel button
    sfVector2f cancel_pos = sfRectangleShape_getPosition(explorer->cancel_button);
    sfVector2f cancel_size = sfRectangleShape_getSize(explorer->cancel_button);
    if (is_point_in_rect(mouse_pos, cancel_pos, cancel_size)) {
        close_file_explorer(explorer);
        return;
    }

    // Check OK button
    sfVector2f ok_pos = sfRectangleShape_getPosition(explorer->ok_button);
    sfVector2f ok_size = sfRectangleShape_getSize(explorer->ok_button);
    if (is_point_in_rect(mouse_pos, ok_pos, ok_size)) {
        if (explorer->mode == FILE_EXPLORER_SAVE) {
            // Save file
            char full_path[MAX_PATH_LENGTH + MAX_FILENAME_LENGTH];
            if (strcmp(explorer->current_path, "/") == 0)
                snprintf(full_path, sizeof(full_path), "/%s", explorer->filename_input);
            else
                snprintf(full_path, sizeof(full_path), "%s/%s", explorer->current_path, explorer->filename_input);

            save_to_file(app->paint, full_path);
            printf("Saved to: %s\n", full_path);
        } else if (explorer->mode == FILE_EXPLORER_OPEN) {
            // Open selected file
            if (explorer->selected_index >= 0 && explorer->selected_index < explorer->entry_count) {
                file_entry_t *entry = &explorer->entries[explorer->selected_index];
                if (entry->type == FILE_TYPE_FILE && entry->is_image) {
                    char full_path[MAX_PATH_LENGTH + MAX_FILENAME_LENGTH];
                    if (strcmp(explorer->current_path, "/") == 0)
                        snprintf(full_path, sizeof(full_path), "/%s", entry->name);
                    else
                        snprintf(full_path, sizeof(full_path), "%s/%s", explorer->current_path, entry->name);

                    load_from_file(app->paint, full_path);
                    printf("Opened: %s\n", full_path);
                }
            }
        }

        close_file_explorer(explorer);
        return;
    }

    // Check file list clicks
    for (int i = 0; i < 15; i++) {
        int entry_idx = i + explorer->scroll_offset;
        if (entry_idx >= explorer->entry_count)
            break;

        sfVector2f btn_pos = sfRectangleShape_getPosition(explorer->file_buttons[i]);
        sfVector2f btn_size = sfRectangleShape_getSize(explorer->file_buttons[i]);

        if (is_point_in_rect(mouse_pos, btn_pos, btn_size)) {
            file_entry_t *entry = &explorer->entries[entry_idx];

            if (entry->type == FILE_TYPE_DIRECTORY || entry->type == FILE_TYPE_PARENT) {
                // Navigate to directory
                navigate_to_directory(explorer, entry->name);
            } else {
                // Select file
                explorer->selected_index = entry_idx;

                // In save mode, populate filename input
                if (explorer->mode == FILE_EXPLORER_SAVE && entry->is_image) {
                    // Separate filename and extension
                    const char *dot = strrchr(entry->name, '.');
                    if (dot && !explorer->extension_editable) {
                        // Copy base filename (without extension)
                        int base_len = dot - entry->name;
                        strncpy(explorer->base_filename, entry->name, base_len);
                        explorer->base_filename[base_len] = '\0';

                        // Copy extension
                        strcpy(explorer->extension, dot);

                        // Update full filename
                        snprintf(explorer->filename_input, MAX_FILENAME_LENGTH, "%s%s", explorer->base_filename, explorer->extension);

                        // Set cursor to end of base filename
                        explorer->input_cursor_pos = strlen(explorer->base_filename);

                        // Update format button label
                        sfText_setString(explorer->format_button_label, explorer->extension);
                    } else {
                        strcpy(explorer->filename_input, entry->name);
                        strcpy(explorer->base_filename, entry->name);
                        explorer->input_cursor_pos = strlen(explorer->filename_input);
                    }
                }
            }
            return;
        }
    }

    // Check scrollbar thumb click (only if there are more items than can be displayed)
    if (explorer->entry_count > 15) {
        sfVector2f thumb_pos = sfRectangleShape_getPosition(explorer->scrollbar_thumb);
        sfVector2f thumb_size = sfRectangleShape_getSize(explorer->scrollbar_thumb);

        if (is_point_in_rect(mouse_pos, thumb_pos, thumb_size)) {
            explorer->dragging_scrollbar = 1;
            return;
        }

        // Check scrollbar track click (jump to position)
        sfVector2f scrollbar_pos = sfRectangleShape_getPosition(explorer->scrollbar);
        sfVector2f scrollbar_size = sfRectangleShape_getSize(explorer->scrollbar);

        if (is_point_in_rect(mouse_pos, scrollbar_pos, scrollbar_size)) {
            // Calculate new scroll position based on click position
            float click_ratio = (mouse_pos.y - scrollbar_pos.y) / scrollbar_size.y;
            int max_scroll = explorer->entry_count - 15;
            explorer->scroll_offset = (int)(click_ratio * max_scroll);

            if (explorer->scroll_offset < 0)
                explorer->scroll_offset = 0;
            if (explorer->scroll_offset > max_scroll)
                explorer->scroll_offset = max_scroll;

            return;
        }
    }
}
