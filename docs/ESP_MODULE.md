# Enhanced ESP Module

## Overview

The ESP (Extra Sensory Perception) module has been significantly enhanced with advanced 3D rendering capabilities inspired by the nobody-client. It now features full OpenGL-based rendering with configurable options.

## Features

### 3D Bounding Box Rendering

The ESP module now renders 3D bounding boxes around entities using OpenGL, making them visible through walls.

**Key Features:**
- **Filled boxes** with transparency
- **Outlined boxes** with customizable colors
- **Combined mode** showing both filled and outlined boxes
- **Proper depth handling** to render through walls
- **OpenGL state management** to avoid interfering with game rendering

### Rendering Modes

The ESP supports three rendering modes:

1. **MODE_BOX_OUTLINE**: Only draws the box outline
2. **MODE_BOX_FILLED**: Only draws filled transparent boxes
3. **MODE_BOX_BOTH**: Draws both filled and outlined boxes (default)

### Color Configuration

Colors are stored as normalized RGBA values (0.0-1.0):

```cpp
struct Color {
    float r, g, b, a;  // Red, Green, Blue, Alpha
};
```

**Default Colors:**
- **Box outline**: White (1.0, 1.0, 1.0, 1.0)
- **Fill color**: Red with 15% opacity (1.0, 0.0, 0.0, 0.15)

## Technical Implementation

### OpenGL Integration

The ESP module uses OpenGL for rendering:

```cpp
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")
```

**CMake Configuration:**
```cmake
target_link_libraries(MinecraftClient opengl32)
```

### Rendering Pipeline

1. **Entity Collection Phase** (`onRender` event):
   - Get world and player objects from JNI
   - Iterate through `loadedEntityList`
   - Extract entity positions using MCP field mappings
   - Create `AxisAlignedBB` structures for each entity
   - Store in `entityBoxes` vector

2. **OpenGL Setup Phase** (`setupGL`):
   - Save current OpenGL state (`glPushAttrib`, `glPushMatrix`)
   - Enable blending for transparency
   - Disable depth test (render through walls)
   - Disable textures (solid colors only)
   - Enable line smoothing for better quality

3. **Drawing Phase**:
   - Draw filled boxes using `GL_QUADS` (6 faces)
   - Draw outlined boxes using `GL_LINE_LOOP` and `GL_LINES`
   - Apply colors from configuration

4. **Restoration Phase** (`restoreGL`):
   - Restore previous OpenGL state
   - Prevents interference with game rendering

### Bounding Box Structure

```cpp
struct AxisAlignedBB {
    double minX, minY, minZ;  // Minimum corner
    double maxX, maxY, maxZ;  // Maximum corner
};
```

**Default Entity Dimensions:**
- Width: 0.6 blocks (0.3 each side)
- Height: 1.8 blocks (standard player height)

### Drawing Functions

#### `drawFilledBox()`

Draws a filled transparent box using OpenGL quads:

```cpp
glBegin(GL_QUADS);
// Draw 6 faces (bottom, top, front, back, left, right)
glVertex3d(bb.minX, bb.minY, bb.minZ);
// ... 24 vertices total (4 per face × 6 faces)
glEnd();
```

#### `drawOutlinedBox()`

Draws box edges using line loops and lines:

```cpp
// Draw bottom rectangle
glBegin(GL_LINE_LOOP);
glVertex3d(bb.minX, bb.minY, bb.minZ);
// ... 4 vertices
glEnd();

// Draw top rectangle
glBegin(GL_LINE_LOOP);
// ... 4 vertices
glEnd();

// Draw 4 vertical connecting lines
glBegin(GL_LINES);
// ... 8 vertices (4 lines)
glEnd();
```

## Usage

### Basic Usage

```cpp
// Toggle ESP with E key
auto& moduleManager = ModuleManager::getInstance();
auto esp = moduleManager.getModule("ESP");
esp->toggle();
```

### Customization

To change colors or rendering mode, modify the ESP constructor:

```cpp
ESP::ESP() 
    : Module("ESP", "Shows entities through walls with 3D boxes"),
      renderMode(MODE_BOX_BOTH),
      boxColor(1.0f, 1.0f, 1.0f, 1.0f),      // White outline
      fillColor(0.0f, 1.0f, 0.0f, 0.15f) {   // Green fill
    setKeyBind(0x45); // E key
}
```

**Color Examples:**
- Red: `(1.0f, 0.0f, 0.0f, 1.0f)`
- Green: `(0.0f, 1.0f, 0.0f, 1.0f)`
- Blue: `(0.0f, 0.0f, 1.0f, 1.0f)`
- Yellow: `(1.0f, 1.0f, 0.0f, 1.0f)`
- Purple: `(1.0f, 0.0f, 1.0f, 1.0f)`
- Transparent: Set alpha (4th value) to 0.0-1.0

## Performance Considerations

### Optimizations

1. **Entity Limit**: Processes max 100 entities per frame
2. **Vertex Batching**: All boxes drawn in single OpenGL context
3. **State Caching**: Minimizes OpenGL state changes
4. **Local Reference Cleanup**: Proper JNI cleanup prevents memory leaks

### Performance Impact

- **CPU**: Minimal - simple box calculations
- **GPU**: Low - basic geometry rendering
- **Memory**: ~10KB for 100 entity bounding boxes

## Comparison with nobody-client

The implementation is inspired by nobody-client's ESP but adapted for this codebase:

**Similarities:**
- 3D bounding box rendering
- Filled and outlined modes
- OpenGL-based rendering
- Proper state management

**Differences:**
- Simplified (no 2D mode, no health bars, no distance text yet)
- Adapted to this project's JNI architecture
- No ImGui dependency
- Uses MCP mappings instead of direct SDK

## Future Enhancements

Potential features to add:

1. **Health bars**: Display entity health on the side
2. **Name tags**: Show entity names above boxes
3. **Distance indicators**: Display distance to entities
4. **Team colors**: Different colors for team members vs enemies
5. **Entity filtering**: Only show specific entity types
6. **2D ESP mode**: Screen-space bounding boxes
7. **Tracers**: Lines from player to entities
8. **Configuration UI**: Runtime color/mode changes

## Credits

ESP rendering techniques adapted from:
- **nobody-client** by @baier233 - OpenGL box rendering implementation
- Original bounding box drawing functions

## References

- [OpenGL Documentation](https://www.opengl.org/documentation/)
- [MCP Mappings](http://export.mcpbot.bspk.rs/)
- [nobody-client Repository](https://github.com/baier233/nobody-client)
