/*
** CCU PROJECT, 2026
** my c paint
** File description:
**
*/

#include "paint.h"

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