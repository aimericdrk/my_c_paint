# CSFML 3.0 Compatibility Notes

## API Changes Fixed

This project has been updated to work with **CSFML 3.0**, which introduced several breaking API changes from CSFML 2.x.

### Header Changes
- Changed from `<SFML/...>` to `<CSFML/...>` includes
- Added `<stdbool.h>` for C99 bool support
- Added `<stdint.h>` for uint8_t type

### sfVideoMode Structure
**Old (2.x):**
```c
sfVideoMode mode = {width, height, bitsPerPixel};
```

**New (3.0):**
```c
sfVideoMode mode = {{width, height}, bitsPerPixel};
// mode.size is now an sfVector2u
```

### sfRenderWindow_create
**Old (2.x):**
```c
sfRenderWindow_create(mode, title, style, settings);
```

**New (3.0):**
```c
sfRenderWindow_create(mode, title, style, state, settings);
// Added sfWindowState parameter (sfWindowed or sfFullscreen)
```

### sfRenderTexture_create
**Old (2.x):**
```c
sfRenderTexture_create(width, height, depthBuffer);
```

**New (3.0):**
```c
sfRenderTexture_create((sfVector2u){width, height}, settings);
// Now takes sfVector2u for size
```

### sfSprite_create
**Old (2.x):**
```c
sfSprite *sprite = sfSprite_create();
sfSprite_setTexture(sprite, texture, resetRect);
```

**New (3.0):**
```c
sfSprite *sprite = sfSprite_create(texture);
// Or pass NULL if setting texture later
sfSprite *sprite = sfSprite_create(NULL);
sfSprite_setTexture(sprite, texture, resetRect);
```

### sfText_create
**Old (2.x):**
```c
sfText *text = sfText_create();
sfText_setFont(text, font);
```

**New (3.0):**
```c
sfText *text = sfText_create(font);
// Font is required at creation
```

### Boolean Types
**Old (2.x):**
```c
sfTrue, sfFalse (sfBool type)
```

**New (3.0):**
```c
true, false (C99 bool type)
```

### Integer Types
**Old (2.x):**
```c
sfUint8, sfUint32, etc.
```

**New (3.0):**
```c
uint8_t, uint32_t, etc. (stdint.h types)
```

### sfImage_getPixel
**Old (2.x):**
```c
sfColor color = sfImage_getPixel(image, x, y);
```

**New (3.0):**
```c
sfColor color = sfImage_getPixel(image, (sfVector2u){x, y});
```

### sfVertex Structure
**Old (2.x):**
```c
sfVertex vertex = {{x, y}, color};
```

**New (3.0):**
```c
sfVertex vertex = {{x, y}, color, {0, 0}};
// Must include texCoords field
```

### sfFloatRect Structure
**Old (2.x):**
```c
bounds.left, bounds.top, bounds.width, bounds.height
```

**New (3.0):**
```c
bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y
// Changed to vector-based structure
```

## Migration Summary

All these changes have been applied to the C-Paint codebase to ensure full CSFML 3.0 compatibility. The application now:

- ✅ Compiles without warnings with `-Wall -Wextra -Werror`
- ✅ Uses modern CSFML 3.0 API throughout
- ✅ Compatible with macOS Apple Silicon (M1/M2/M3)
- ✅ Properly linked against CSFML 3.0 libraries

## Building

```bash
# Install CSFML 3.0 via Homebrew
brew install csfml

# Build the project
make

# Run
./my_paint
```

## Tested On

- macOS with Apple Silicon (M1/M2/M3)
- CSFML 3.0.0 via Homebrew
- GCC with -Wall -Wextra -Werror

---

**Note:** If you encounter CSFML-related errors, ensure you have CSFML 3.0+ installed, not the older 2.x version.
