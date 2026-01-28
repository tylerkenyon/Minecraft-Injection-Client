@echo off
echo =======================================
echo Minecraft Injection Client - Builder
echo =======================================
echo.

REM Check if running as admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Warning: Not running as administrator
    echo Some features may not work correctly
    echo.
)

REM Check for JAVA_HOME
if not defined JAVA_HOME (
    echo Error: JAVA_HOME not set
    echo Please set JAVA_HOME to your JDK installation
    echo Example: set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_xxx
    pause
    exit /b 1
)

echo JAVA_HOME: %JAVA_HOME%
echo.

REM Create build directory
if not exist build (
    echo Creating build directory...
    mkdir build
)

cd build

REM Run CMake
echo Running CMake...
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorLevel% neq 0 (
    echo CMake configuration failed
    pause
    exit /b 1
)

REM Build Release
echo.
echo Building Release configuration...
cmake --build . --config Release
if %errorLevel% neq 0 (
    echo Build failed
    pause
    exit /b 1
)

echo.
echo =======================================
echo Build completed successfully!
echo =======================================
echo.
echo Output files:
echo   - build\bin\Release\MinecraftClient.dll
echo   - build\bin\Release\Injector.exe
echo.
echo To inject into Minecraft:
echo   1. Start Minecraft 1.8.9
echo   2. Run: Injector.exe javaw.exe MinecraftClient.dll
echo.

cd ..
pause
