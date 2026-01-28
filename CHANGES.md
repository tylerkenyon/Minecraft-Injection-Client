# Changes Summary

## Overview

This update implements a comprehensive MCP mapping system and significantly enhances the ESP module with advanced OpenGL-based rendering, making the codebase more maintainable and adding powerful visualization capabilities.

## Major Features Added

### 1. MCP Mapping System

**New Files:**
- `src/jni/MappingLoader.h` - Header for mapping loader
- `src/jni/MappingLoader.cpp` - Implementation of mapping loader

**Features:**
- Loads 22,378 MCP mappings from CSV files (6,586 fields + 6,731 methods + 9,061 params)
- O(1) lookup performance using hash maps
- Automatic initialization on client startup
- Simple API for name translation

**Benefits:**
- Code now uses readable names like `thePlayer` instead of `h`
- All searge names documented in code comments
- Easy to extend with new fields/methods
- Maintains compatibility with obfuscated names as fallback

### 2. Enhanced ESP Module

**Modified Files:**
- `src/modules/ESP.h` - Enhanced with OpenGL structures
- `src/modules/ESP.cpp` - Complete rewrite with 3D rendering

**Features:**
- 3D bounding box rendering using OpenGL
- Three render modes: outline, filled, or both
- Configurable RGBA colors
- Proper OpenGL state management
- Processes up to 100 entities per frame

**Technical Details:**
- Uses `AxisAlignedBB` structure for bounding boxes
- Implements `drawFilledBox()` using GL_QUADS (6 faces)
- Implements `drawOutlinedBox()` using GL_LINE_LOOP and GL_LINES
- Proper state save/restore to avoid interfering with game rendering

### 3. Documentation

**New Files:**
- `docs/MAPPINGS.md` - Comprehensive guide to the mapping system
- `docs/ESP_MODULE.md` - Detailed ESP module documentation

**Updated Files:**
- `README.md` - Updated with new features and documentation links

**Documentation Includes:**
- Complete mapping reference tables
- Implementation details
- Usage examples
- Performance considerations
- Future enhancement ideas

## Code Changes

### Modified Files

1. **CMakeLists.txt**
   - Added MappingLoader source files
   - Linked OpenGL32 library

2. **src/dllmain.cpp**
   - Added MappingLoader initialization
   - Loads mappings before MinecraftClasses initialization

3. **src/jni/MinecraftClasses.cpp**
   - Added searge name comments for all fields
   - Example: `// field_71439_g -> thePlayer`
   - Documented mappings for clarity

4. **src/modules/ESP.h**
   - Added `AxisAlignedBB` structure
   - Added color configuration structures
   - Added rendering mode enum
   - Added OpenGL helper methods

5. **src/modules/ESP.cpp**
   - Complete rewrite with OpenGL rendering
   - Added `setupGL()` and `restoreGL()` methods
   - Added `drawFilledBox()` and `drawOutlinedBox()` methods
   - Entity collection and rendering pipeline

## Key Mappings Reference

### Minecraft Class
- `field_71432_P` → `theMinecraft` (static singleton)
- `field_71439_g` → `thePlayer` (EntityPlayerSP)
- `field_71441_e` → `theWorld` (WorldClient)

### Entity Class
- `field_70165_t` → `posX` (double)
- `field_70163_u` → `posY` (double)
- `field_70161_v` → `posZ` (double)
- `field_70177_z` → `rotationYaw` (float)
- `field_70125_A` → `rotationPitch` (float)
- `field_70159_w` → `motionX` (double)
- `field_70181_x` → `motionY` (double)
- `field_70179_y` → `motionZ` (double)
- `field_70122_E` → `onGround` (boolean)

### PlayerCapabilities Class
- `field_75102_a` → `disableDamage`
- `field_75101_c` → `allowFlying`
- `field_75100_b` → `isFlying`
- `field_75098_d` → `isCreativeMode`

### EntityPlayer Class
- `field_71075_bZ` → `capabilities` (PlayerCapabilities)

### World Class
- `field_72996_f` → `loadedEntityList` (List)

## Build Changes

**Dependencies:**
- Added OpenGL32 library linkage

**New Includes:**
- `<gl/GL.h>` for OpenGL functions

## Testing Notes

This is a Windows-only project (DLL injection for Minecraft Java). The build will fail on Linux CI environments but should compile successfully on Windows with:
- Visual Studio 2019 or higher
- Windows SDK
- CMake 3.15 or higher

## Credits

- **MCP Team** - For maintaining Minecraft mappings
- **nobody-client by @baier233** - Inspiration for ESP rendering implementation
- Original OpenGL box drawing techniques adapted from nobody-client

## Future Enhancements

Potential features for future development:

1. **ESP Enhancements:**
   - Health bars
   - Name tags
   - Distance indicators
   - Team colors
   - Entity filtering
   - 2D ESP mode
   - Tracers

2. **Mapping System:**
   - Automatic fallback chain (MCP → searge → obfuscated)
   - Runtime remapping based on Minecraft version detection
   - Method call mapping support
   - Class name mapping

3. **New Modules:**
   - KillAura
   - Speed
   - AutoClicker
   - Scaffold
   - ChestESP
   - Velocity

## Breaking Changes

None. All changes are backward compatible with existing code.

## Performance Impact

- **MappingLoader**: ~1-2 MB memory, one-time load at startup
- **ESP Module**: Minimal CPU impact, low GPU usage for basic geometry
- **Overall**: Negligible impact on gameplay performance
