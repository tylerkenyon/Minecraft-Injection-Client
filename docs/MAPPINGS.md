# MCP Mapping System

## Overview

The Minecraft Injection Client now uses an MCP (Mod Coder Pack) mapping system to translate obfuscated Minecraft class, field, and method names to their readable counterparts. This makes the code more maintainable and easier to understand.

## How It Works

### Mapping Files

The mappings are stored in CSV format in `src/mappings/`:

- **fields.csv** - 6,586 field mappings (e.g., `field_71439_g` → `thePlayer`)
- **methods.csv** - 6,731 method mappings (e.g., `func_71410_x` → `updateCameraAndRender`)
- **params.csv** - 9,061 parameter mappings (currently unused)

### CSV Format

```csv
searge,name,side,desc
field_71439_g,thePlayer,0,
field_71441_e,theWorld,0,
field_71432_P,theMinecraft,0,Set to 'this' in Minecraft constructor
```

- **searge**: The intermediate obfuscation name (SRG name)
- **name**: The human-readable MCP name
- **side**: 0=client, 1=server, 2=both
- **desc**: Optional description

## Key Minecraft Mappings

### Minecraft Class Fields

| Searge Name | MCP Name | Description |
|-------------|----------|-------------|
| `field_71432_P` | `theMinecraft` | Main Minecraft singleton instance |
| `field_71439_g` | `thePlayer` | Current player (EntityPlayerSP) |
| `field_71441_e` | `theWorld` | Current world (WorldClient) |

### Entity Fields

| Searge Name | MCP Name | Type | Description |
|-------------|----------|------|-------------|
| `field_70165_t` | `posX` | double | Entity X position |
| `field_70163_u` | `posY` | double | Entity Y position |
| `field_70161_v` | `posZ` | double | Entity Z position |
| `field_70177_z` | `rotationYaw` | float | Entity yaw rotation |
| `field_70125_A` | `rotationPitch` | float | Entity pitch rotation |
| `field_70159_w` | `motionX` | double | Entity X motion |
| `field_70181_x` | `motionY` | double | Entity Y motion |
| `field_70179_y` | `motionZ` | double | Entity Z motion |
| `field_70122_E` | `onGround` | boolean | Is entity on ground |

### PlayerCapabilities Fields

| Searge Name | MCP Name | Description |
|-------------|----------|-------------|
| `field_75102_a` | `disableDamage` | Disables player damage |
| `field_75101_c` | `allowFlying` | Allow player to fly |
| `field_75100_b` | `isFlying` | Is player currently flying |
| `field_75098_d` | `isCreativeMode` | Is creative mode enabled |

### EntityPlayer Fields

| Searge Name | MCP Name | Type | Description |
|-------------|----------|------|-------------|
| `field_71075_bZ` | `capabilities` | PlayerCapabilities | Player capabilities object |

### World Fields

| Searge Name | MCP Name | Type | Description |
|-------------|----------|------|-------------|
| `field_72996_f` | `loadedEntityList` | List | All entities in loaded chunks |
