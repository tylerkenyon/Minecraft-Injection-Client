#pragma once
#include <jni.h>
#include <memory>
#include <string>

// Forward declaration
class JNIUtils;

// Minecraft class wrapper for MCP 1.8.9 mappings
class MinecraftClasses {
public:
    static MinecraftClasses& getInstance();
    
    bool initialize();
    
    // Get the Minecraft instance
    jobject getMinecraft();
    
    // Get the current player (EntityPlayerSP)
    jobject getThePlayer();
    
    // Get the game world
    jobject getTheWorld();
    
    // Player capabilities
    struct PlayerCapabilities {
        jfieldID isFlying;
        jfieldID allowFlying;
        jfieldID disableDamage;
        jfieldID isCreativeMode;
    };
    
    PlayerCapabilities& getPlayerCapabilities() { return playerCapabilities; }
    
    // Entity fields
    struct EntityFields {
        jfieldID posX;
        jfieldID posY;
        jfieldID posZ;
        jfieldID rotationYaw;
        jfieldID rotationPitch;
        jfieldID motionX;
        jfieldID motionY;
        jfieldID motionZ;
        jfieldID onGround;
    };
    
    EntityFields& getEntityFields() { return entityFields; }
    
    // World fields
    jfieldID getLoadedEntityList();
    
private:
    MinecraftClasses();
    ~MinecraftClasses();
    
    bool findMinecraftClass();
    bool findEntityPlayerClass();
    bool findEntityClass();
    bool findWorldClass();
    bool findPlayerCapabilitiesClass();
    
    jclass minecraftClass;
    jclass entityPlayerClass;
    jclass entityClass;
    jclass worldClass;
    jclass playerCapabilitiesClass;
    
    jfieldID theMinecraftField;
    jfieldID thePlayerField;
    jfieldID theWorldField;
    jfieldID capabilitiesField;
    
    jfieldID loadedEntityListField;
    
    PlayerCapabilities playerCapabilities;
    EntityFields entityFields;
    
    bool initialized;
};
