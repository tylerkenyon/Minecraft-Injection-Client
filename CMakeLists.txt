cmake_minimum_required(VERSION 3.15)
project(MinecraftInjectionClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JDK for JNI headers
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

# Common include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Client DLL (the injectable library)
add_library(MinecraftClient SHARED
    src/dllmain.cpp
    src/jni/JNIUtils.cpp
    src/jni/JNIUtils.h
    src/jni/MinecraftClasses.cpp
    src/jni/MinecraftClasses.h
    src/core/Module.cpp
    src/core/Module.h
    src/core/ModuleManager.cpp
    src/core/ModuleManager.h
    src/core/EventBus.cpp
    src/core/EventBus.h
    src/modules/Flight.cpp
    src/modules/Flight.h
    src/modules/ESP.cpp
    src/modules/ESP.h
    src/utils/Logger.cpp
    src/utils/Logger.h
)

target_link_libraries(MinecraftClient ${JNI_LIBRARIES})

# Injector executable
add_executable(Injector
    src/injector/main.cpp
    src/injector/Injector.cpp
    src/injector/Injector.h
)

# Output directories
set_target_properties(MinecraftClient PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

set_target_properties(Injector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
