# 🚀 Quick Start Guide

## Installation & Build

```bash
# 1. Install dependencies (if needed)
make install

# 2. Check dependencies
make check

# 3. Build the project
make

# 4. Run the application
./my_paint
# or
make run
```

## Essential Controls

### Mouse Controls
- **Left Click + Drag** → Draw
- **Right Click** → Pick color from canvas

### Keyboard Shortcuts
- **Ctrl+S** → Save
- **Ctrl+O** → Open
- **Ctrl+Z** → Undo
- **Ctrl+Y** → Redo
- **Ctrl+N** → Clear canvas
- **F1** → Debug info

## Tools Available

1. **Pen** - Solid drawing
2. **Eraser** - Remove content
3. **Brush** - Soft artistic strokes
4. **Spray** - Spray paint effect
5. **Line** - Straight lines
6. **Rectangle** - Box shapes
7. **Circle** - Round shapes
8. **Fill** - Flood fill areas

## UI Layout

```
┌────────────────────────────────────────────────┐
│                                        TOOLBAR │
│                                           ┌────┤
│                                           │    │
│         CANVAS (1100x800)                 │    │
│                                           │ UI │
│                                           │    │
│                                           │    │
│                                           └────┤
└────────────────────────────────────────────────┘
```

## Tips

- Use **F1** to see debug info (FPS, coordinates, etc.)
- **Right-click** on canvas to quickly pick colors
- Adjust **brush size** with the bottom slider
- Use **RGB sliders** for precise color control
- **Undo stack** holds 50 actions
- Shapes show **preview** before you release

## Default Settings

- Tool: Pen
- Color: Black
- Size: 5 pixels
- Canvas: White
- Save file: `output.png`

---

**Ready? Start painting!** 🎨
