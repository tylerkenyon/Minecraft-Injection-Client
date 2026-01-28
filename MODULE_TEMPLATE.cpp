// Example Module Template
// Copy this to create new modules quickly

// ============================================
// File: src/modules/TemplateModule.h
// ============================================
#pragma once
#include "../core/Module.h"

class TemplateModule : public Module {
public:
    TemplateModule();
    
    // Override lifecycle methods
    void onEnable() override;
    void onDisable() override;
    
    // Override event handlers as needed
    void onTick(const TickEvent& event) override;
    void onRender(const RenderEvent& event) override;
    void onKeyPress(const KeyPressEvent& event) override;

private:
    // Module state variables
    bool exampleState;
    int exampleCounter;
    
    // Helper methods
    void doSomething();
};

// ============================================
// File: src/modules/TemplateModule.cpp
// ============================================
#include "TemplateModule.h"
#include "../jni/JNIUtils.h"
#include "../jni/MinecraftClasses.h"
#include "../utils/Logger.h"

TemplateModule::TemplateModule() 
    : Module("TemplateModule", "Description of what this module does")
    , exampleState(false)
    , exampleCounter(0) {
    
    // Set keybind (Windows Virtual Key Code)
    // VK_G = 0x47, VK_H = 0x48, etc.
    setKeyBind(0x47); // G key
}

void TemplateModule::onEnable() {
    LOG_INFO("TemplateModule enabled - Press G to toggle");
    
    // Initialize module state
    exampleState = false;
    exampleCounter = 0;
    
    // Perform enable actions
    doSomething();
}

void TemplateModule::onDisable() {
    LOG_INFO("TemplateModule disabled");
    
    // Cleanup/reset
    exampleState = false;
}

void TemplateModule::onTick(const TickEvent& event) {
    // Called every game tick (~50ms / 20 times per second)
    
    auto& jni = JNIUtils::getInstance();
    auto& mc = MinecraftClasses::getInstance();
    
    if (!jni.isAttached()) {
        return;
    }
    
    // Example: Get player
    jobject player = mc.getThePlayer();
    if (!player) {
        return;
    }
    
    // Example: Get player position
    auto& entityFields = mc.getEntityFields();
    if (entityFields.posX && entityFields.posY && entityFields.posZ) {
        double x = jni.getDoubleField(player, entityFields.posX);
        double y = jni.getDoubleField(player, entityFields.posY);
        double z = jni.getDoubleField(player, entityFields.posZ);
        
        // Do something with position
        exampleCounter++;
        if (exampleCounter % 100 == 0) {
            LOG_DEBUG("Player at: " + std::to_string(x) + ", " + 
                      std::to_string(y) + ", " + std::to_string(z));
        }
    }
    
    // Example: Modify player
    if (exampleState) {
        // Set motion Y (make player jump)
        if (entityFields.motionY) {
            jni.setDoubleField(player, entityFields.motionY, 0.42);
        }
    }
}

void TemplateModule::onRender(const RenderEvent& event) {
    // Called every render frame (variable, typically 60+ FPS)
    // Use for visual/ESP features
    
    // Note: This is a simplified version
    // In production, you'd hook into OpenGL/DirectX here
}

void TemplateModule::onKeyPress(const KeyPressEvent& event) {
    // Called when any key is pressed
    // Module's keybind is automatically handled by base class
    
    // Handle additional keys specific to this module
    if (event.keyCode == 0x48) { // H key
        exampleState = !exampleState;
        LOG_INFO("Example state toggled: " + std::string(exampleState ? "ON" : "OFF"));
    }
}

void TemplateModule::doSomething() {
    LOG_DEBUG("Helper method called");
}

// ============================================
// Registration in src/dllmain.cpp
// ============================================
/*

#include "modules/TemplateModule.h"

// In ClientMain() function, add:
moduleManager.addModule(std::make_shared<TemplateModule>());

*/

// ============================================
// Update CMakeLists.txt
// ============================================
/*

Add to MinecraftClient library sources:
    src/modules/TemplateModule.cpp
    src/modules/TemplateModule.h

*/

// ============================================
// Common Patterns
// ============================================

// 1. Access Player Capabilities
/*
auto& caps = mc.getPlayerCapabilities();
jobject player = mc.getThePlayer();

// Get capabilities object
jfieldID capField = jni.getFieldID(
    jni.getEnv()->GetObjectClass(player),
    "capabilities",
    "Lnet/minecraft/entity/player/PlayerCapabilities;"
);
jobject capabilities = jni.getObjectField(player, capField);

// Modify capabilities
if (caps.allowFlying) {
    jni.setBooleanField(capabilities, caps.allowFlying, JNI_TRUE);
}
*/

// 2. Get World Entities
/*
jobject world = mc.getTheWorld();
jfieldID listField = mc.getLoadedEntityList();
jobject entityList = jni.getObjectField(world, listField);

JNIEnv* env = jni.getEnv();
jclass listClass = env->FindClass("java/util/List");
jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

jint count = env->CallIntMethod(entityList, sizeMethod);
for (jint i = 0; i < count; i++) {
    jobject entity = env->CallObjectMethod(entityList, getMethod, i);
    // Process entity...
}
*/

// 3. Timer/Counter Pattern
/*
private:
    int tickCounter;
    
onTick:
    tickCounter++;
    if (tickCounter % 20 == 0) {
        // Runs once per second (20 ticks = 1 second)
    }
*/

// 4. Toggle State Pattern
/*
private:
    bool feature1Enabled;
    bool feature2Enabled;
    
onKeyPress:
    if (event.keyCode == VK_F1) {
        feature1Enabled = !feature1Enabled;
    }
*/

// ============================================
// Windows Virtual Key Codes (Common)
// ============================================
/*
VK_F1-VK_F12: 0x70-0x7B (Function keys)
VK_A-VK_Z: 0x41-0x5A (Letter keys)
VK_0-VK_9: 0x30-0x39 (Number keys)
VK_SPACE: 0x20
VK_SHIFT: 0x10
VK_CONTROL: 0x11
VK_INSERT: 0x2D
VK_DELETE: 0x2E
VK_HOME: 0x24
VK_END: 0x23
VK_PRIOR (Page Up): 0x21
VK_NEXT (Page Down): 0x22
*/
