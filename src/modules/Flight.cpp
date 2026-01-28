#include "Flight.h"
#include "../jni/JNIUtils.h"
#include "../jni/MinecraftClasses.h"
#include "../utils/Logger.h"

Flight::Flight() 
    : Module("Flight", "Allows you to fly in survival mode") {
    setKeyBind(0x46); // F key (VK_F)
}

void Flight::onEnable() {
    LOG_INFO("Flight module enabled - Press F to toggle");
    setFlying(true);
}

void Flight::onDisable() {
    LOG_INFO("Flight module disabled");
    setFlying(false);
}

void Flight::onTick(const TickEvent& event) {
    // Continuously maintain flight capability
    setFlying(true);
}

void Flight::setFlying(bool flying) {
    auto& jni = JNIUtils::getInstance();
    auto& mc = MinecraftClasses::getInstance();
    
    if (!jni.isAttached()) {
        LOG_ERROR("JNI not attached");
        return;
    }
    
    // Get the player
    jobject player = mc.getThePlayer();
    if (!player) {
        LOG_DEBUG("Player not available");
        return;
    }
    
    // Get player capabilities field from MinecraftClasses
    jfieldID capabilitiesField = mc.getCapabilitiesField();
    
    if (!capabilitiesField) {
        LOG_ERROR("Could not find capabilities field");
        return;
    }
    
    // Get capabilities object from player
    jobject capabilities = jni.getObjectField(player, capabilitiesField);
    if (!capabilities) {
        LOG_ERROR("Could not get capabilities object");
        return;
    }
    
    // Get capability field IDs
    auto& caps = mc.getPlayerCapabilities();
    
    // Set allowFlying and isFlying
    if (caps.allowFlying) {
        jni.setBooleanField(capabilities, caps.allowFlying, flying ? JNI_TRUE : JNI_FALSE);
    }
    
    if (caps.isFlying) {
        jni.setBooleanField(capabilities, caps.isFlying, flying ? JNI_TRUE : JNI_FALSE);
    }
    
    if (flying) {
        LOG_DEBUG("Flight capability enabled");
    } else {
        LOG_DEBUG("Flight capability disabled");
    }
}
