#!/bin/bash

echo "======================================="
echo "Minecraft Injection Client - Builder"
echo "======================================="
echo ""

# Check for JAVA_HOME
if [ -z "$JAVA_HOME" ]; then
    echo "Error: JAVA_HOME not set"
    echo "Please set JAVA_HOME to your JDK installation"
    echo "Example: export JAVA_HOME=/usr/lib/jvm/java-8-openjdk"
    exit 1
fi

echo "JAVA_HOME: $JAVA_HOME"
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Run CMake
echo "Running CMake..."
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

# Build Release
echo ""
echo "Building Release configuration..."
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo ""
echo "======================================="
echo "Build completed successfully!"
echo "======================================="
echo ""
echo "Output files:"
echo "  - build/bin/MinecraftClient.dll"
echo "  - build/bin/Injector"
echo ""

cd ..
