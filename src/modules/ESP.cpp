#include "ESP.h"
#include "../jni/JNIUtils.h"
#include "../jni/MinecraftClasses.h"
#include "../utils/Logger.h"

ESP::ESP() 
    : Module("ESP", "Shows entities through walls") {
    setKeyBind(0x45); // E key (VK_E)
}

void ESP::onEnable() {
    LOG_INFO("ESP module enabled - Press E to toggle");
}

void ESP::onDisable() {
    LOG_INFO("ESP module disabled");
}

void ESP::onRender(const RenderEvent& event) {
    renderESP();
}

void ESP::renderESP() {
    auto& jni = JNIUtils::getInstance();
    auto& mc = MinecraftClasses::getInstance();
    
    if (!jni.isAttached()) {
        return;
    }
    
    // Get the world
    jobject world = mc.getTheWorld();
    if (!world) {
        return;
    }
    
    // Get the player for position reference
    jobject player = mc.getThePlayer();
    if (!player) {
        return;
    }
    
    JNIEnv* env = jni.getEnv();
    
    // Get loadedEntityList field from world
    jfieldID loadedEntityListField = mc.getLoadedEntityList();
    if (!loadedEntityListField) {
        return;
    }
    
    jobject entityList = jni.getObjectField(world, loadedEntityListField);
    if (!entityList) {
        return;
    }
    
    // Get List class methods
    jclass listClass = env->FindClass("java/util/List");
    if (!listClass) {
        return;
    }
    
    jmethodID sizeMethod = env->GetMethodID(listClass, "size", "()I");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    
    if (!sizeMethod || !getMethod) {
        return;
    }
    
    // Get entity count
    jint entityCount = env->CallIntMethod(entityList, sizeMethod);
    
    // Get entity fields
    auto& entityFields = mc.getEntityFields();
    
    // Log entity positions (in a real implementation, you would render boxes here)
    for (jint i = 0; i < entityCount && i < 100; i++) { // Limit to 100 entities
        jobject entity = env->CallObjectMethod(entityList, getMethod, i);
        
        if (entity && entity != player) {
            // Get entity position
            if (entityFields.posX && entityFields.posY && entityFields.posZ) {
                jdouble x = jni.getDoubleField(entity, entityFields.posX);
                jdouble y = jni.getDoubleField(entity, entityFields.posY);
                jdouble z = jni.getDoubleField(entity, entityFields.posZ);
                
                // In a real implementation, you would:
                // 1. Convert world coords to screen coords
                // 2. Draw bounding boxes using OpenGL/DirectX
                // 3. Add distance indicators, health bars, etc.
                
                // For now, just log periodically (every 100 frames)
                static int frameCount = 0;
                if (++frameCount % 100 == 0) {
                    LOG_DEBUG("ESP: Entity at (" + 
                             std::to_string(x) + ", " + 
                             std::to_string(y) + ", " + 
                             std::to_string(z) + ")");
                }
            }
        }
        
        if (entity) {
            env->DeleteLocalRef(entity);
        }
    }
    
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(entityList);
}
