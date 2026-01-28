#include <Windows.h>
#include <thread>
#include "utils/Logger.h"
#include "jni/JNIUtils.h"
#include "jni/MinecraftClasses.h"
#include "core/ModuleManager.h"
#include "core/EventBus.h"
#include "modules/Flight.h"
#include "modules/ESP.h"

// Client main loop
void ClientMain() {
    LOG_INFO("=== Minecraft Injection Client Starting ===");
    
    // Give Minecraft time to fully load
    Sleep(2000);
    
    // Attach to JVM
    auto& jni = JNIUtils::getInstance();
    if (!jni.attachToJVM()) {
        LOG_ERROR("Failed to attach to JVM - Make sure Minecraft is running");
        return;
    }
    
    LOG_INFO("Successfully attached to JVM");
    
    // Initialize Minecraft classes
    auto& mc = MinecraftClasses::getInstance();
    if (!mc.initialize()) {
        LOG_ERROR("Failed to initialize Minecraft classes");
        return;
    }
    
    LOG_INFO("Minecraft classes initialized");
    
    // Setup modules
    auto& moduleManager = ModuleManager::getInstance();
    
    // Register modules
    moduleManager.addModule(std::make_shared<Flight>());
    moduleManager.addModule(std::make_shared<ESP>());
    
    moduleManager.initialize();
    
    LOG_INFO("=== Client initialized successfully ===");
    LOG_INFO("Available modules:");
    for (const auto& module : moduleManager.getModules()) {
        LOG_INFO("  - " + module->getName() + ": " + module->getDescription());
    }
    
    // Main loop - post events
    auto& eventBus = EventBus::getInstance();
    
    LOG_INFO("Entering main loop...");
    
    int tickCount = 0;
    while (true) {
        Sleep(50); // ~20 TPS
        
        // Post tick event
        TickEvent tickEvent;
        eventBus.post(tickEvent);
        
        // Post render event every tick (simplified)
        RenderEvent renderEvent;
        eventBus.post(renderEvent);
        
        tickCount++;
        
        // Log status every 200 ticks (~10 seconds)
        if (tickCount % 200 == 0) {
            LOG_DEBUG("Client running - Tick: " + std::to_string(tickCount));
        }
        
        // Handle keyboard input (simplified - in production use proper input hooks)
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            LOG_INFO("F1 pressed - listing modules:");
            for (const auto& module : moduleManager.getModules()) {
                LOG_INFO("  " + module->getName() + ": " + 
                        (module->isEnabled() ? "ENABLED" : "DISABLED"));
            }
            Sleep(300); // Debounce
        }
        
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            LOG_INFO("END pressed - shutting down client");
            break;
        }
        
        // Check module keybinds
        for (const auto& module : moduleManager.getModules()) {
            int keyBind = module->getKeyBind();
            if (keyBind != 0 && (GetAsyncKeyState(keyBind) & 0x8000)) {
                KeyPressEvent keyEvent(keyBind);
                eventBus.post(keyEvent);
                Sleep(300); // Debounce
            }
        }
    }
    
    // Cleanup
    LOG_INFO("Cleaning up...");
    moduleManager.disableAll();
    eventBus.clear();
    jni.detachFromJVM();
    
    LOG_INFO("=== Client shut down ===");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            // Create a new thread for the client
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ClientMain, hModule, 0, nullptr);
            break;
            
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
