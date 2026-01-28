# Quick Start Guide

## Prerequisites Checklist

- [ ] Windows 10/11
- [ ] Visual Studio 2019/2022 with C++ development tools
- [ ] CMake 3.15+
- [ ] JDK 8+ (JAVA_HOME environment variable set)
- [ ] Minecraft 1.8.9

## Quick Build

```batch
# Set JAVA_HOME (if not set)
set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_xxx

# Build
build.bat
```

Or manually:
```batch
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Quick Inject

```batch
# Start Minecraft 1.8.9 first, then:
cd build\bin\Release
Injector.exe javaw.exe MinecraftClient.dll
```

## Controls Summary

| Key | Action |
|-----|--------|
| F | Toggle Flight |
| E | Toggle ESP |
| F1 | List modules |
| END | Shutdown client |

## File Structure

```
Minecraft-Injection-Client/
├── src/
│   ├── dllmain.cpp          # DLL entry point & main loop
│   ├── jni/
│   │   ├── JNIUtils.*       # JNI attachment & utilities
│   │   └── MinecraftClasses.* # Minecraft class wrappers
│   ├── core/
│   │   ├── Module.*         # Base module class
│   │   ├── ModuleManager.*  # Module lifecycle manager
│   │   └── EventBus.*       # Event system
│   ├── modules/
│   │   ├── Flight.*         # Flight module
│   │   └── ESP.*            # ESP module
│   ├── utils/
│   │   └── Logger.*         # Logging system
│   └── injector/
│       ├── main.cpp         # Injector entry point
│       └── Injector.*       # DLL injection logic
├── CMakeLists.txt           # Build configuration
├── build.bat                # Windows build script
└── README.md                # Full documentation
```

## Adding Your First Module

1. **Create files**: `src/modules/MyModule.h` and `src/modules/MyModule.cpp`

2. **Copy this template** (MyModule.h):
```cpp
#pragma once
#include "../core/Module.h"

class MyModule : public Module {
public:
    MyModule();
    void onEnable() override;
    void onDisable() override;
    void onTick(const TickEvent& event) override;
};
```

3. **Implement** (MyModule.cpp):
```cpp
#include "MyModule.h"
#include "../utils/Logger.h"

MyModule::MyModule() 
    : Module("MyModule", "My first module") {
    setKeyBind(0x47); // G key
}

void MyModule::onEnable() {
    LOG_INFO("MyModule enabled!");
}

void MyModule::onDisable() {
    LOG_INFO("MyModule disabled!");
}

void MyModule::onTick(const TickEvent& event) {
    // Your logic here
}
```

4. **Register in dllmain.cpp**:
```cpp
#include "modules/MyModule.h"

// In ClientMain():
moduleManager.addModule(std::make_shared<MyModule>());
```

5. **Update CMakeLists.txt**: Add your files to the MinecraftClient library

6. **Rebuild**: `cmake --build build --config Release`

## Common Tasks

### Access Player Position
```cpp
auto& jni = JNIUtils::getInstance();
auto& mc = MinecraftClasses::getInstance();

jobject player = mc.getThePlayer();
if (player) {
    auto& fields = mc.getEntityFields();
    double x = jni.getDoubleField(player, fields.posX);
    double y = jni.getDoubleField(player, fields.posY);
    double z = jni.getDoubleField(player, fields.posZ);
}
```

### Modify Player Movement
```cpp
jni.setDoubleField(player, fields.motionY, 0.5); // Jump
```

### Get All Entities
```cpp
jobject world = mc.getTheWorld();
jfieldID listField = mc.getLoadedEntityList();
jobject entityList = jni.getObjectField(world, listField);
// Iterate through list...
```

## Troubleshooting

**Build fails**: Check JAVA_HOME is set correctly
**Injection fails**: Run as Administrator
**No effect in game**: Check minecraft_client.log for errors
**Wrong Minecraft version**: May need to update obfuscated names

## Next Steps

- Read full README.md for detailed documentation
- Check existing modules (Flight, ESP) for examples
- Explore MinecraftClasses.cpp for available Minecraft APIs
- Add rendering hooks for visual modules
- Implement GUI system for better user experience
