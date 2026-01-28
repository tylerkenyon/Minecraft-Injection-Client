# Minecraft Injection Client

A modular C++ JNI-based injection client for Minecraft 1.8.9 with expandable module system.

## Features

- **JNI Integration**: Attaches to the JVM and interacts with Minecraft using JNI
- **MCP 1.8.9 Mappings**: Uses MCP (Mod Coder Pack) 1.8.9 class names with fallback to obfuscated names
- **Modular Architecture**: Easy-to-extend module system with base classes
- **Event System**: EventBus for inter-module communication
- **DLL Injection**: Uses CreateRemoteThread for process injection
- **Two Injection Methods**:
  - `MinecraftClient.dll` - Manual injection
  - `Injector.exe` - Automatic injection tool

## Included Modules

### Flight Module
- Toggle with `F` key
- Enables flight in survival mode
- Automatically maintains flight capability

### ESP Module
- Toggle with `E` key
- Shows entity positions (foundation for rendering through walls)
- Logs entity coordinates

## Architecture

```
┌─────────────────────────────────────────┐
│           MinecraftClient.dll           │
├─────────────────────────────────────────┤
│  ┌────────────┐      ┌──────────────┐  │
│  │ JNI Utils  │◄────►│  Minecraft   │  │
│  │            │      │   Classes    │  │
│  └────────────┘      └──────────────┘  │
│         ▲                               │
│         │                               │
│  ┌──────┴────────┐   ┌──────────────┐  │
│  │ ModuleManager │◄──┤   EventBus   │  │
│  └───────┬───────┘   └──────────────┘  │
│          │                              │
│  ┌───────┴────────────────────────┐    │
│  │  Modules (Flight, ESP, ...)    │    │
│  └────────────────────────────────┘    │
└─────────────────────────────────────────┘
```

## Building

### Prerequisites

- CMake 3.15 or higher
- Visual Studio 2019 or higher (with C++ desktop development)
- JDK 8 or higher (for JNI headers)
- Windows SDK

### Build Instructions

1. **Set JAVA_HOME environment variable** (if not already set):
   ```batch
   set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_xxx
   ```

2. **Clone the repository**:
   ```batch
   git clone https://github.com/tylerkenyon/Minecraft-Injection-Client.git
   cd Minecraft-Injection-Client
   ```

3. **Build with CMake**:
   ```batch
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

4. **Output files** will be in `build/bin/`:
   - `MinecraftClient.dll` - The injectable client
   - `Injector.exe` - Injection tool

### Manual Build (Visual Studio)

1. Open Visual Studio 2019/2022
2. File → Open → CMake
3. Select `CMakeLists.txt`
4. Build → Build All

## Usage

### Method 1: Automatic Injection (Recommended)

1. Start Minecraft 1.8.9 (or compatible version)
2. Run the injector as **Administrator**:
   ```batch
   Injector.exe javaw.exe MinecraftClient.dll
   ```

### Method 2: Manual Injection

1. Start Minecraft 1.8.9
2. Use any DLL injector to inject `MinecraftClient.dll` into the Java process
3. Popular injectors:
   - Xenos Injector
   - Process Hacker
   - Extreme Injector

### Controls

Once injected:
- **F** - Toggle Flight module
- **E** - Toggle ESP module
- **F1** - List all modules and their status
- **END** - Shutdown the client

### Logs

The client creates a log file at: `minecraft_client.log` in the same directory as the injected DLL.

## Adding New Modules

### 1. Create Header File (`src/modules/YourModule.h`)

```cpp
#pragma once
#include "../core/Module.h"

class YourModule : public Module {
public:
    YourModule();
    
    void onEnable() override;
    void onDisable() override;
    void onTick(const TickEvent& event) override;

private:
    // Your module state
};
```

### 2. Create Implementation (`src/modules/YourModule.cpp`)

```cpp
#include "YourModule.h"
#include "../jni/JNIUtils.h"
#include "../jni/MinecraftClasses.h"
#include "../utils/Logger.h"

YourModule::YourModule() 
    : Module("YourModule", "Description") {
    setKeyBind(0x47); // G key
}

void YourModule::onEnable() {
    LOG_INFO("YourModule enabled");
}

void YourModule::onDisable() {
    LOG_INFO("YourModule disabled");
}

void YourModule::onTick(const TickEvent& event) {
    // Your module logic
}
```

### 3. Register Module (`src/dllmain.cpp`)

```cpp
#include "modules/YourModule.h"

// In ClientMain() function:
moduleManager.addModule(std::make_shared<YourModule>());
```

### 4. Update CMakeLists.txt

Add your module files to the `MinecraftClient` library sources.

## Module System

### Base Module Class

All modules inherit from `Module` class with these methods:

- `onEnable()` - Called when module is enabled
- `onDisable()` - Called when module is disabled
- `onTick(TickEvent)` - Called every game tick (~50ms)
- `onRender(RenderEvent)` - Called every render frame
- `onKeyPress(KeyPressEvent)` - Called on key press

### Event System

The EventBus handles event distribution:

```cpp
EventBus::getInstance().subscribe(EventType::TICK, [](const Event& e) {
    // Handle tick event
});

EventBus::getInstance().post(TickEvent());
```

## JNI Utilities

### Finding Classes

```cpp
auto& jni = JNIUtils::getInstance();
jclass playerClass = jni.findClass("net/minecraft/entity/player/EntityPlayer");
```

### Accessing Fields

```cpp
jfieldID healthField = jni.getFieldID(playerClass, "health", "F");
jfloat health = jni.getFloatField(playerObject, healthField);
```

### Calling Methods

```cpp
jmethodID method = jni.getMethodID(playerClass, "jump", "()V");
jni.callVoidMethod(playerObject, method);
```

## Minecraft Classes (MCP 1.8.9)

Common classes accessible through `MinecraftClasses`:

- `getMinecraft()` - Main Minecraft instance
- `getThePlayer()` - EntityPlayerSP instance
- `getTheWorld()` - World instance
- `getPlayerCapabilities()` - PlayerCapabilities fields
- `getEntityFields()` - Entity position/rotation fields

## Security & Legal

**DISCLAIMER**: This is an educational project demonstrating JNI, DLL injection, and game modification techniques.

- Modify games only with proper authorization
- Respect game Terms of Service
- Use only on private/offline servers or with permission
- Not responsible for bans or violations

## Expandability

The project is designed for easy expansion:

1. **Add Modules**: Create new module classes
2. **Add Events**: Extend EventBus with new event types
3. **Add Minecraft Classes**: Extend MinecraftClasses wrapper
4. **Add Rendering**: Hook into OpenGL/DirectX for visual modules
5. **Add Network**: Hook packet handlers for network modules

## Troubleshooting

### Injection Fails

- Run injector as Administrator
- Ensure Minecraft is running
- Check that DLL path is correct
- Verify Java process is `javaw.exe`

### Client Not Working

- Check `minecraft_client.log` for errors
- Ensure JVM is accessible (JNI attached)
- Verify Minecraft version is compatible (1.8.9)
- Check if MCP mappings match your Minecraft version

### Module Not Responding

- Check if module is enabled (press F1)
- Verify keybinds are correct
- Check logs for error messages

## Contributing

Contributions are welcome! Areas for improvement:

- Better rendering system (OpenGL/DirectX hooks)
- More modules (Speed, KillAura, AutoClicker, etc.)
- GUI system
- Config save/load
- Multiple Minecraft version support

## License

This project is provided as-is for educational purposes.

## Credits

- MCP (Mod Coder Pack) for class mappings
- Minecraft modding community