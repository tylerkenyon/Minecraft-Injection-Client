# Platform Notes

## Windows-Only Project

This project is **Windows-only** and requires:
- Windows 10/11
- Visual Studio 2019/2022 with MSVC compiler
- Windows SDK

## Why Windows-Only?

The project uses Windows-specific APIs:

1. **DLL Injection**: `CreateRemoteThread`, `VirtualAllocEx`, `WriteProcessMemory`
2. **Process Management**: `OpenProcess`, `CreateToolhelp32Snapshot`
3. **Windows Headers**: `Windows.h`, `TlHelp32.h`
4. **Input Handling**: `GetAsyncKeyState`, Virtual Key Codes

## Linux/macOS Support

This project **cannot** run on Linux or macOS because:
- Minecraft on these platforms doesn't use JVM injection in the same way
- Windows API calls are not available
- DLL injection is a Windows-specific technique

## Building on Windows

Use the provided `build.bat` script or:

```batch
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## Verification on Non-Windows Systems

While you cannot build or run this project on Linux/macOS, you can:
- View and edit the code
- Review the architecture
- Understand the concepts
- Test CMake configuration (will fail at compile stage, which is expected)

## Target Platform

- **OS**: Windows 10/11 (64-bit)
- **Minecraft**: Java Edition 1.8.9
- **Java**: JDK 8+ (for JNI headers at build time, JRE 8 at runtime)
