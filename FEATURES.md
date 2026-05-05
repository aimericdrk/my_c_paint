# 🌟 C-Paint Features Documentation

## Complete Feature List

### Core Drawing Tools (8 Total)

#### 1. Pen Tool 🖊️
- **Type:** Continuous drawing
- **Use Case:** Precise lines and outlines
- **Characteristics:**
  - Solid, opaque strokes
  - Size-adjustable
  - Smooth line interpolation
  - No transparency
- **Best For:** Technical drawings, outlines, writing

#### 2. Eraser Tool 🧹
- **Type:** Continuous drawing
- **Use Case:** Removing content
- **Characteristics:**
  - Paints white color
  - 2x brush size for effective erasing
  - Smooth erasing
- **Best For:** Corrections, cleanup

#### 3. Brush Tool 🖌️
- **Type:** Continuous drawing
- **Use Case:** Artistic painting
- **Characteristics:**
  - Soft edges with transparency
  - Layered effect (3 circles)
  - Opacity gradient
  - Natural blending
- **Best For:** Artistic work, shading, soft effects

#### 4. Spray Can Tool 💨
- **Type:** Continuous drawing
- **Use Case:** Texture and effects
- **Characteristics:**
  - 20 particles per application
  - Random distribution
  - Semi-transparent particles
  - Radius based on brush size
- **Best For:** Textures, clouds, organic effects

#### 5. Line Tool 📏
- **Type:** Shape tool
- **Use Case:** Straight lines
- **Characteristics:**
  - Live preview
  - Bresenham algorithm
  - Size-adjustable thickness
  - Any angle
- **Best For:** Technical drawings, borders, rulers

#### 6. Rectangle Tool ▭
- **Type:** Shape tool
- **Use Case:** Box shapes
- **Characteristics:**
  - Live preview
  - Outline only
  - Adjustable thickness
  - Perfect angles
- **Best For:** Frames, borders, layouts

#### 7. Circle Tool ⭕
- **Type:** Shape tool
- **Use Case:** Round shapes
- **Characteristics:**
  - Live preview
  - Parametric drawing
  - Adjustable thickness
  - Perfect circles
- **Best For:** Wheels, planets, decorations

#### 8. Fill Bucket Tool 🪣
- **Type:** Area fill
- **Use Case:** Flooding areas with color
- **Characteristics:**
  - Flood fill algorithm
  - Non-recursive (stack-based)
  - Handles complex shapes
  - Color matching
- **Best For:** Coloring regions, backgrounds

---

## Color System 🎨

### RGB Color Picker
- **Three independent sliders:**
  - Red channel (0-255)
  - Green channel (0-255)
  - Blue channel (0-255)
- **Real-time preview box**
- **Visual feedback on sliders**
- **Smooth dragging**

### Color Picking
- **Right-click functionality**
- **Pick from anywhere on canvas**
- **Instant color capture**
- **RGB values displayed**

### Color Space
- **24-bit color depth**
- **16.7 million colors**
- **True color accuracy**
- **No dithering**

---

## Size Control 📏

### Brush Size Slider
- **Range:** 1-50 pixels
- **Real-time display:** Shows exact size
- **Smooth adjustment:** Drag to change
- **Visual feedback:** Slider position
- **Tool-specific scaling:**
  - Eraser: 2x size
  - Brush: Multi-layer scaling
  - Spray: Affects particle radius

---

## File Operations 💾

### Save Feature
- **Format:** PNG (lossless)
- **Location:** `output.png` in project directory
- **Shortcut:** Ctrl+S
- **Quality:** Full resolution
- **Transparency:** Supported
- **Compression:** Automatic

### Load Feature
- **Format:** PNG (any size)
- **Location:** `output.png` by default
- **Shortcut:** Ctrl+O
- **Scaling:** Preserves original size
- **Error handling:** Graceful failure

---

## Undo/Redo System ↩️↪️

### Undo Stack
- **Capacity:** 50 actions
- **Type:** Full canvas states
- **Shortcut:** Ctrl+Z
- **Intelligence:** Clears redo on new action
- **Memory:** Efficient texture management

### Redo Stack
- **Shortcut:** Ctrl+Y
- **Integration:** Seamless with undo
- **Limit:** Up to saved states
- **Reliability:** Perfect restoration

### State Management
- **Automatic saving:** Before each action
- **Smart memory:** Circular buffer
- **No duplicates:** Efficient storage

---

## User Interface Design 🎯

### Toolbar
- **Location:** Right side (250px wide)
- **Background:** Dark theme (50, 50, 55)
- **Organization:** Logical grouping
- **Spacing:** Clear separation

### Tool Buttons
- **Count:** 8 tools
- **Size:** 220x50 pixels each
- **Spacing:** 60 pixels apart
- **Visual feedback:**
  - Selected: Blue highlight (100, 150, 200)
  - Normal: Gray (70, 70, 75)
  - Outline: Lighter gray border
- **Icons:** Simple circles
- **Labels:** Clear text

### Color Controls
- **Preview box:** 220x60 pixels
- **Three RGB sliders:** 200x20 pixels each
- **Labels:** R, G, B indicators
- **Handles:** 10x30 pixel sliders
- **Live update:** Immediate feedback

### Size Control
- **Slider bar:** 200x20 pixels
- **Handle:** 10x30 pixels
- **Display:** Shows "X px"
- **Range indicator:** Visual bar

---

## Canvas Features 🖼️

### Canvas Specifications
- **Dimensions:** 1100x800 pixels
- **Position:** (50, 50) from top-left
- **Background:** White
- **Format:** RGBA texture
- **Rendering:** Hardware accelerated

### Drawing Quality
- **Interpolation:** Smooth line drawing
- **Anti-aliasing:** On circle shapes
- **Transparency:** Full alpha support
- **Blending:** Proper color mixing

---

## Debug Tools 🐛

### Debug Panel (F1 to toggle)
- **FPS Counter:** Real-time performance
- **Mouse Position:** Window coordinates
- **Canvas Position:** Drawing coordinates
- **Tool ID:** Current tool index
- **Color Values:** RGB components
- **Brush Size:** Current size
- **Drawing State:** Active/inactive
- **Undo Stack:** Usage statistics
- **Shape Preview:** State indicator

### Visual Debug
- **Semi-transparent background**
- **Yellow text for visibility**
- **Top-left corner placement**
- **Automatic updates**

---

## Advanced Features 🚀

### Shape Preview System
- **Real-time rendering:** See before committing
- **Overlay display:** Above canvas
- **Accurate positioning:** Exact placement
- **Tool-specific:**
  - Line: Shows connecting line
  - Rectangle: Shows bounding box
  - Circle: Shows radius circle

### Smart Drawing
- **Line interpolation:** No gaps in strokes
- **Smooth curves:** Bresenham algorithm
- **Particle system:** Spray tool randomization
- **Flood fill optimization:** Stack-based iteration

### Event System
- **Mouse tracking:** Precise position
- **Keyboard shortcuts:** Multiple combos
- **Window events:** Close handling
- **UI interactions:** Button clicks, slider drags

### Performance Optimizations
- **60 FPS target:** Smooth rendering
- **Render textures:** GPU acceleration
- **Efficient updates:** Only when needed
- **Memory management:** Proper cleanup

---

## Window Management 🪟

### Window Settings
- **Dimensions:** 1400x900 pixels
- **Title:** "C-Paint - Professional Edition"
- **Style:** Resizable, closeable
- **Frame rate:** 60 FPS limit
- **Background:** Dark theme (40, 40, 45)

### Layout
```
┌──────────────────────────────────────────────────────────┐
│  CANVAS AREA (1100x800)          │  TOOLBAR (250x900)   │
│  Starting at (50, 50)             │  Starting at (1150,0)│
│                                   │                       │
│  • Drawing surface                │  • Tool buttons       │
│  • White background               │  • Color picker       │
│  • Interactive                    │  • Size slider        │
│                                   │  • Settings           │
└──────────────────────────────────────────────────────────┘
```

---

## Technical Highlights ⚙️

### Algorithms Implemented
1. **Bresenham Line Drawing** - Efficient rasterization
2. **Flood Fill** - Stack-based, non-recursive
3. **Parametric Circle** - Trigonometric drawing
4. **Color Interpolation** - Smooth blending
5. **Particle Distribution** - Random spray effect

### Memory Management
- **Automatic cleanup:** All resources freed
- **No leaks:** Valgrind clean
- **Smart allocation:** Only when needed
- **Texture pooling:** Undo system

### Code Quality
- **Modular design:** Separated concerns
- **Clean functions:** Single responsibility
- **Documentation:** Clear comments
- **Error handling:** Graceful failures

---

## Keyboard Shortcuts Summary ⌨️

| Shortcut | Action |
|----------|--------|
| Ctrl+S | Save to PNG |
| Ctrl+O | Open from PNG |
| Ctrl+Z | Undo |
| Ctrl+Y | Redo |
| Ctrl+N | Clear canvas |
| F1 | Toggle debug |
| Left Click | Draw |
| Right Click | Pick color |

---

## Statistics 📊

- **Total Features:** 30+
- **Drawing Tools:** 8
- **UI Elements:** 20+
- **Keyboard Shortcuts:** 7
- **Color Channels:** 3 (RGB)
- **Canvas Pixels:** 880,000
- **Undo Levels:** 50
- **Max Brush Size:** 50px
- **FPS Target:** 60
- **File Formats:** PNG

---

## Platform Support 🖥️

### Tested On
- ✅ macOS (Homebrew CSFML)
- ✅ Linux (apt CSFML)

### Requirements
- **CSFML:** Version 2.5+
- **C Compiler:** GCC or Clang
- **Make:** GNU Make
- **OS:** macOS or Linux

---

**This is a complete, professional-grade paint application with all the features you'd expect from a modern drawing tool!** 🎨✨
