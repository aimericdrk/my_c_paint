# Configuration System Documentation

## Overview
The C-Paint application now supports external JSON configuration, allowing you to customize the UI appearance without recompiling the code.

## Configuration File
The configuration is stored in `config/ui_config.json`

## What Can Be Configured

### Window Settings
- `width`, `height`: Window dimensions
- `title`: Application window title
- `framerate`: Target FPS

### Canvas Settings
- `width`, `height`: Canvas dimensions
- `x`, `y`: Canvas position
- `background_color`: [R, G, B, A]

### Toolbar
- `x`, `y`, `width`: Toolbar position and size
- `background_color`: Toolbar background [R, G, B, A]
- **Tools section**: Button positions, sizes, spacing, colors
- **Color preview**: Position and dimensions
- **Sliders**: RGB and size slider positions, colors

### Top Bar
- `height`: Top bar height
- `color`: Background color
- **File button**: Position, size, text, colors
- **Options button**: Position, size, text, colors

### Dropdowns
- **File menu**: Position, width, item height, items list
- **Options menu**: Position, width, items with types (checkbox/cycle/action)

### Colors
- `background`: Main background color
- `text_primary`: Text color
- `grid`: Grid line color
- `cursor_fill`: Cursor fill color
- `cursor_outline`: Cursor outline color

### Grid
- `default_size`: Default grid cell size in pixels

### Cursor
- `radius`: Cursor circle radius
- `outline_thickness`: Outline width

### Fonts
- `primary_path`: Path to primary font file
- `fallback_path`: Path to fallback font

## How to Edit Configuration

1. Open `config/ui_config.json` in any text editor
2. Modify the values you want to change
3. Save the file
4. Restart the application

**Note**: The application will fall back to default hardcoded values if the JSON file cannot be loaded or contains errors.

## Example Changes

### Change Toolbar Position
```json
"toolbar": {
    "x": 1200,  // Move toolbar 50px to the right
    ...
}
```

### Change Color Theme
```json
"colors": {
    "background": [30, 30, 35, 255],  // Darker background
    "text_primary": [255, 200, 100, 255],  // Orange text
    ...
}
```

### Adjust Button Sizes
```json
"toolbar": {
    "tools": {
        "button_width": 240,  // Wider buttons
        "button_height": 60,  // Taller buttons
        ...
    }
}
```

## Implementation Details

The configuration system uses:
- **cJSON library**: Lightweight JSON parser
- **config.h/config.c**: Configuration loading and access functions
- **Helper functions**: `config_get_int()`, `config_get_float()`, `config_get_string()`, `config_get_color()`

## Current Status

✅ **Implemented**:
- JSON configuration file structure
- Configuration loading at startup
- Fallback to defaults if config missing

🔄 **In Progress**:
- Full UI initialization from JSON config (toolbar background currently loads from JSON)

📋 **Next Steps**:
- Replace all remaining hardcoded values in `init_ui()` with config-loaded values
- Add runtime config reload (without restart)
- Add config validation and error reporting
- Create config editor UI within the application
