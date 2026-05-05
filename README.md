# 🎨 C-Paint Professional Edition

A fully-featured paint application written in C using CSFML (Simple and Fast Multimedia Library).

## ✨ Features

### 🖌️ Drawing Tools
- **Pen** - Classic drawing tool for precise lines
- **Eraser** - Remove parts of your drawing
- **Brush** - Soft brush with transparency for artistic effects
- **Spray Can** - Spray paint effect with particle system
- **Line Tool** - Draw straight lines between two points
- **Rectangle Tool** - Create rectangle outlines
- **Circle Tool** - Draw perfect circles
- **Fill Bucket** - Flood fill areas with color

### 🎨 Color System
- **RGB Color Picker** - Interactive sliders for Red, Green, and Blue channels
- **Live Color Preview** - See your selected color in real-time
- **Color Picking** - Right-click on canvas to pick any color
- **Full 24-bit Color** - Support for 16.7 million colors

### ⚙️ Settings & Controls
- **Adjustable Brush Size** - Scale from 1 to 50 pixels
- **Real-time Size Display** - See the exact pixel size
- **Easy UI** - Intuitive toolbar with clear labels
- **Visual Feedback** - Selected tool highlighting

### 💾 File Operations
- **Save to PNG** - Export your artwork (Ctrl+S)
- **Load from PNG** - Import existing images (Ctrl+O)
- **Lossless Format** - Preserve quality with PNG format

### ↩️ Undo/Redo System
- **50 Level Undo Stack** - Undo up to 50 actions (Ctrl+Z)
- **Full Redo Support** - Redo undone actions (Ctrl+Y)
- **Smart State Management** - Efficient memory usage

### 🐛 Debug Tools
- **Real-time FPS Counter** - Monitor performance
- **Mouse Position Display** - Track cursor coordinates
- **Tool State Information** - See current tool settings
- **Undo Stack Monitor** - View undo/redo state
- **Toggle Debug Panel** - Press F1 to show/hide

### 🎯 Shape Preview
- **Live Preview** - See shapes before committing
- **Visual Feedback** - Interactive shape drawing
- **Smooth Rendering** - Anti-aliased preview

## 🛠️ Installation

### macOS
```bash
# Install CSFML using Homebrew
brew install csfml

# Or use the Makefile
make install
```

### Linux (Ubuntu/Debian)
```bash
# Install CSFML
sudo apt-get update
sudo apt-get install libcsfml-dev

# Or use the Makefile
make install
```

## 🚀 Building

```bash
# Check dependencies
make check

# Build the project
make

# Build and run
make run

# Build with debug symbols
make debug

# Clean build files
make clean

# Full clean (including binary)
make fclean

# Rebuild everything
make re

# Show help
make help
```

## 🎮 Controls

### Drawing
- **Left Click** - Draw with selected tool
- **Right Click** - Pick color from canvas
- **Click and Drag** - Continuous drawing (pen, eraser, brush, spray)
- **Click and Release** - Shape tools (line, rectangle, circle)

### Keyboard Shortcuts
- **Ctrl+Z** - Undo last action
- **Ctrl+Y** - Redo last undone action
- **Ctrl+S** - Save canvas to `output.png`
- **Ctrl+O** - Load canvas from `output.png`
- **Ctrl+N** - Clear canvas (new drawing)
- **F1** - Toggle debug information

### UI Interaction
- **Click Tool Buttons** - Select drawing tool
- **Drag Color Sliders** - Adjust RGB values
- **Drag Size Slider** - Change brush size

## 📁 Project Structure

```
C_paint/
├── include/
│   └── paint.h           # Header file with all structures and prototypes
├── src/
│   ├── main.c            # Entry point
│   ├── app.c             # Application loop and event handling
│   ├── paint.c           # Drawing tools implementation
│   ├── ui.c              # User interface and controls
│   └── utils.c           # Utility functions, undo/redo, file I/O
├── Makefile              # Build system with detailed output
└── README.md             # This file
```

## 🎨 Drawing Tools Explained

### Pen Tool
Classic drawing tool that creates solid lines. Perfect for precise work and outlines.

### Eraser Tool
Removes content by painting white. Double the size of the brush for effective erasing.

### Brush Tool
Creates soft, semi-transparent strokes with layered circles for a more natural artistic feel.

### Spray Can
Simulates spray paint with random particle distribution. Great for textures and effects.

### Line Tool
Click to set start point, drag to preview, release to draw a straight line.

### Rectangle Tool
Click to set corner, drag to preview size, release to draw rectangle outline.

### Circle Tool
Click to set center, drag to set radius, release to draw circle outline.

### Fill Bucket
Click on an area to fill it with the current color. Uses flood fill algorithm.

## 🏗️ Technical Details

### Architecture
- **Modular Design** - Separated concerns for maintainability
- **Clean Code** - Well-commented and organized
- **Memory Safe** - Proper allocation and cleanup
- **Efficient Rendering** - Uses render textures for performance

### Algorithms
- **Bresenham Line Drawing** - Smooth, efficient line rendering
- **Flood Fill** - Non-recursive stack-based implementation
- **Circular Interpolation** - Parametric circle drawing
- **State Management** - Efficient undo/redo system

### Performance
- **60 FPS Target** - Smooth user experience
- **Hardware Acceleration** - GPU-accelerated rendering via SFML
- **Optimized Drawing** - Incremental updates only when needed
- **Memory Efficient** - Smart texture management

## 📊 Statistics

- **5 Source Files** - Modular and organized
- **~1500 Lines of Code** - Clean and maintainable
- **8 Drawing Tools** - Comprehensive toolset
- **50 Undo Levels** - Extensive history
- **1100x800 Canvas** - Large workspace
- **16.7M Colors** - Full RGB color space

## 🎓 Learning Resources

This project demonstrates:
- **SFML/CSFML Usage** - Graphics, windows, events
- **Game Loop Architecture** - Update, render, events
- **UI Development** - Interactive elements
- **File I/O** - Save and load functionality
- **State Management** - Undo/redo systems
- **Algorithm Implementation** - Line drawing, flood fill
- **Memory Management** - Proper allocation/deallocation

## 🐛 Debugging

Enable debug mode with F1 key to see:
- Current FPS
- Mouse position
- Canvas coordinates
- Active tool
- Current color (RGB)
- Brush size
- Drawing state
- Undo stack status
- Shape preview state

## 📝 Notes

- Default save location: `output.png` in the project directory
- Canvas size: 1100x800 pixels
- Window size: 1400x900 pixels
- Maximum brush size: 50 pixels
- Undo stack depth: 50 actions

## 🤝 Contributing

This is a student project for EPITECH. Feel free to:
- Report bugs
- Suggest features
- Submit improvements
- Share your creations

## 📜 License

Educational project - EPITECH 2026

## 🎉 Enjoy!

Start creating amazing artwork with C-Paint Professional Edition!

```
     🎨 Happy Painting! 🎨
```

---
**Made with ❤️ using C and CSFML**
