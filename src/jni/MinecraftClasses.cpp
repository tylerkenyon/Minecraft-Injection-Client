#include "MinecraftClasses.h"
#include "JNIUtils.h"
#include "../utils/Logger.h"

MinecraftClasses& MinecraftClasses::getInstance() {
    static MinecraftClasses instance;
    return instance;
}

MinecraftClasses::MinecraftClasses() 
    : minecraftClass(nullptr)
    , entityPlayerClass(nullptr)
    , entityClass(nullptr)
    , worldClass(nullptr)
    , playerCapabilitiesClass(nullptr)
    , theMinecraftField(nullptr)
    , thePlayerField(nullptr)
    , theWorldField(nullptr)
    , capabilitiesField(nullptr)
    , loadedEntityListMethod(nullptr)
    , initialized(false) {
}

MinecraftClasses::~MinecraftClasses() {
}

bool MinecraftClasses::initialize() {
    if (initialized) {
        return true;
    }
    
    LOG_INFO("Initializing Minecraft classes...");
    
    auto& jni = JNIUtils::getInstance();
    
    if (!jni.isAttached()) {
        LOG_ERROR("JNI not attached");
        return false;
    }
    
    // Find all required classes
    if (!findMinecraftClass() || !findEntityPlayerClass() || 
        !findEntityClass() || !findWorldClass() || !findPlayerCapabilitiesClass()) {
        LOG_ERROR("Failed to find required Minecraft classes");
        return false;
    }
    
    initialized = true;
    LOG_INFO("Minecraft classes initialized successfully");
    return true;
}

bool MinecraftClasses::findMinecraftClass() {
    auto& jni = JNIUtils::getInstance();
    
    // Try to find Minecraft class (MCP name in 1.8.9)
    minecraftClass = jni.findClass("net/minecraft/client/Minecraft");
    
    if (!minecraftClass) {
        // Try obfuscated name
        minecraftClass = jni.findClass("ave");
        if (!minecraftClass) {
            LOG_ERROR("Failed to find Minecraft class");
            return false;
        }
    }
    
    LOG_INFO("Found Minecraft class");
    
    // Get theMinecraft static field (singleton instance)
    // Try MCP name first
    theMinecraftField = jni.getStaticFieldID(minecraftClass, "theMinecraft", "Lnet/minecraft/client/Minecraft;");
    
    if (!theMinecraftField) {
        // Try common obfuscated patterns
        theMinecraftField = jni.getStaticFieldID(minecraftClass, "S", "Lave;");
    }
    
    // Get thePlayer field
    thePlayerField = jni.getFieldID(minecraftClass, "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
    if (!thePlayerField) {
        thePlayerField = jni.getFieldID(minecraftClass, "h", "Lbew;");
    }
    
    // Get theWorld field
    theWorldField = jni.getFieldID(minecraftClass, "theWorld", "Lnet/minecraft/client/multiplayer/WorldClient;");
    if (!theWorldField) {
        theWorldField = jni.getFieldID(minecraftClass, "f", "Lbdb;");
    }
    
    return true;
}

bool MinecraftClasses::findEntityPlayerClass() {
    auto& jni = JNIUtils::getInstance();
    
    entityPlayerClass = jni.findClass("net/minecraft/client/entity/EntityPlayerSP");
    
    if (!entityPlayerClass) {
        entityPlayerClass = jni.findClass("bew");
        if (!entityPlayerClass) {
            LOG_ERROR("Failed to find EntityPlayerSP class");
            return false;
        }
    }
    
    LOG_INFO("Found EntityPlayerSP class");
    
    // Get capabilities field
    capabilitiesField = jni.getFieldID(entityPlayerClass, "capabilities", "Lnet/minecraft/entity/player/PlayerCapabilities;");
    if (!capabilitiesField) {
        capabilitiesField = jni.getFieldID(entityPlayerClass, "bW", "Lrz;");
    }
    
    return true;
}

bool MinecraftClasses::findEntityClass() {
    auto& jni = JNIUtils::getInstance();
    
    entityClass = jni.findClass("net/minecraft/entity/Entity");
    
    if (!entityClass) {
        entityClass = jni.findClass("pk");
        if (!entityClass) {
            LOG_ERROR("Failed to find Entity class");
            return false;
        }
    }
    
    LOG_INFO("Found Entity class");
    
    // Get position fields
    entityFields.posX = jni.getFieldID(entityClass, "posX", "D");
    if (!entityFields.posX) entityFields.posX = jni.getFieldID(entityClass, "s", "D");
    
    entityFields.posY = jni.getFieldID(entityClass, "posY", "D");
    if (!entityFields.posY) entityFields.posY = jni.getFieldID(entityClass, "t", "D");
    
    entityFields.posZ = jni.getFieldID(entityClass, "posZ", "D");
    if (!entityFields.posZ) entityFields.posZ = jni.getFieldID(entityClass, "u", "D");
    
    // Get rotation fields
    entityFields.rotationYaw = jni.getFieldID(entityClass, "rotationYaw", "F");
    if (!entityFields.rotationYaw) entityFields.rotationYaw = jni.getFieldID(entityClass, "y", "F");
    
    entityFields.rotationPitch = jni.getFieldID(entityClass, "rotationPitch", "F");
    if (!entityFields.rotationPitch) entityFields.rotationPitch = jni.getFieldID(entityClass, "z", "F");
    
    // Get motion fields
    entityFields.motionX = jni.getFieldID(entityClass, "motionX", "D");
    if (!entityFields.motionX) entityFields.motionX = jni.getFieldID(entityClass, "v", "D");
    
    entityFields.motionY = jni.getFieldID(entityClass, "motionY", "D");
    if (!entityFields.motionY) entityFields.motionY = jni.getFieldID(entityClass, "w", "D");
    
    entityFields.motionZ = jni.getFieldID(entityClass, "motionZ", "D");
    if (!entityFields.motionZ) entityFields.motionZ = jni.getFieldID(entityClass, "x", "D");
    
    entityFields.onGround = jni.getFieldID(entityClass, "onGround", "Z");
    if (!entityFields.onGround) entityFields.onGround = jni.getFieldID(entityClass, "C", "Z");
    
    return true;
}

bool MinecraftClasses::findWorldClass() {
    auto& jni = JNIUtils::getInstance();
    
    worldClass = jni.findClass("net/minecraft/world/World");
    
    if (!worldClass) {
        worldClass = jni.findClass("adm");
        if (!worldClass) {
            LOG_ERROR("Failed to find World class");
            return false;
        }
    }
    
    LOG_INFO("Found World class");
    
    // Get loadedEntityList field (it's actually a field, not a method)
    // We'll use getFieldID instead
    loadedEntityListMethod = jni.getFieldID(worldClass, "loadedEntityList", "Ljava/util/List;");
    if (!loadedEntityListMethod) {
        loadedEntityListMethod = jni.getFieldID(worldClass, "h", "Ljava/util/List;");
    }
    
    return true;
}

bool MinecraftClasses::findPlayerCapabilitiesClass() {
    auto& jni = JNIUtils::getInstance();
    
    playerCapabilitiesClass = jni.findClass("net/minecraft/entity/player/PlayerCapabilities");
    
    if (!playerCapabilitiesClass) {
        playerCapabilitiesClass = jni.findClass("rz");
        if (!playerCapabilitiesClass) {
            LOG_ERROR("Failed to find PlayerCapabilities class");
            return false;
        }
    }
    
    LOG_INFO("Found PlayerCapabilities class");
    
    // Get capability fields
    playerCapabilities.isFlying = jni.getFieldID(playerCapabilitiesClass, "isFlying", "Z");
    if (!playerCapabilities.isFlying) {
        playerCapabilities.isFlying = jni.getFieldID(playerCapabilitiesClass, "c", "Z");
    }
    
    playerCapabilities.allowFlying = jni.getFieldID(playerCapabilitiesClass, "allowFlying", "Z");
    if (!playerCapabilities.allowFlying) {
        playerCapabilities.allowFlying = jni.getFieldID(playerCapabilitiesClass, "b", "Z");
    }
    
    playerCapabilities.disableDamage = jni.getFieldID(playerCapabilitiesClass, "disableDamage", "Z");
    if (!playerCapabilities.disableDamage) {
        playerCapabilities.disableDamage = jni.getFieldID(playerCapabilitiesClass, "a", "Z");
    }
    
    playerCapabilities.isCreativeMode = jni.getFieldID(playerCapabilitiesClass, "isCreativeMode", "Z");
    if (!playerCapabilities.isCreativeMode) {
        playerCapabilities.isCreativeMode = jni.getFieldID(playerCapabilitiesClass, "d", "Z");
    }
    
    return true;
}

jobject MinecraftClasses::getMinecraft() {
    if (!initialized || !theMinecraftField) {
        return nullptr;
    }
    
    auto& jni = JNIUtils::getInstance();
    return jni.getEnv()->GetStaticObjectField(minecraftClass, theMinecraftField);
}

jobject MinecraftClasses::getThePlayer() {
    jobject minecraft = getMinecraft();
    if (!minecraft || !thePlayerField) {
        return nullptr;
    }
    
    auto& jni = JNIUtils::getInstance();
    return jni.getObjectField(minecraft, thePlayerField);
}

jobject MinecraftClasses::getTheWorld() {
    jobject minecraft = getMinecraft();
    if (!minecraft || !theWorldField) {
        return nullptr;
    }
    
    auto& jni = JNIUtils::getInstance();
    return jni.getObjectField(minecraft, theWorldField);
}

jmethodID MinecraftClasses::getLoadedEntityList() {
    return loadedEntityListMethod;
}
