#include "ESP.h"
#include "../jni/JNIUtils.h"
#include "../jni/MinecraftClasses.h"
#include "../utils/Logger.h"
#include <Windows.h>

// Include OpenGL headers
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

ESP::ESP() 
    : Module("ESP", "Shows entities through walls with 3D boxes"),
      renderMode(MODE_BOX_BOTH),
      boxColor(1.0f, 1.0f, 1.0f, 1.0f),      // White box
      fillColor(1.0f, 0.0f, 0.0f, 0.15f) {   // Red fill with transparency
    setKeyBind(0x45); // E key (VK_E)
}

void ESP::onEnable() {
    LOG_INFO("ESP module enabled - Press E to toggle");
    entityBoxes.clear();
}

void ESP::onDisable() {
    LOG_INFO("ESP module disabled");
    entityBoxes.clear();
}

void ESP::onRender(const RenderEvent& event) {
    renderESP();
}

void ESP::setupGL() {
    // Save OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth test so ESP renders through walls
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    // Disable texture so we draw solid colors
    glDisable(GL_TEXTURE_2D);
    
    // Enable line smoothing for better looking boxes
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0f);
}

void ESP::restoreGL() {
    // Restore OpenGL state
    glPopMatrix();
    glPopAttrib();
}

void ESP::drawFilledBox(const AxisAlignedBB& bb) {
    glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    
    // Draw filled box using quads for all 6 faces
    glBegin(GL_QUADS);
    
    // Bottom face
    glVertex3d(bb.minX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.minY, bb.maxZ);
    glVertex3d(bb.minX, bb.minY, bb.maxZ);
    
    // Top face
    glVertex3d(bb.minX, bb.maxY, bb.minZ);
    glVertex3d(bb.minX, bb.maxY, bb.maxZ);
    glVertex3d(bb.maxX, bb.maxY, bb.maxZ);
    glVertex3d(bb.maxX, bb.maxY, bb.minZ);
    
    // Front face (minZ)
    glVertex3d(bb.minX, bb.minY, bb.minZ);
    glVertex3d(bb.minX, bb.maxY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.minZ);
    glVertex3d(bb.maxX, bb.minY, bb.minZ);
    
    // Back face (maxZ)
    glVertex3d(bb.minX, bb.minY, bb.maxZ);
    glVertex3d(bb.maxX, bb.minY, bb.maxZ);
    glVertex3d(bb.maxX, bb.maxY, bb.maxZ);
    glVertex3d(bb.minX, bb.maxY, bb.maxZ);
    
    // Left face (minX)
    glVertex3d(bb.minX, bb.minY, bb.minZ);
    glVertex3d(bb.minX, bb.minY, bb.maxZ);
    glVertex3d(bb.minX, bb.maxY, bb.maxZ);
    glVertex3d(bb.minX, bb.maxY, bb.minZ);
    
    // Right face (maxX)
    glVertex3d(bb.maxX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.maxZ);
    glVertex3d(bb.maxX, bb.minY, bb.maxZ);
    
    glEnd();
}

void ESP::drawOutlinedBox(const AxisAlignedBB& bb) {
    glColor4f(boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    
    // Draw bottom rectangle
    glBegin(GL_LINE_LOOP);
    glVertex3d(bb.minX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.minY, bb.maxZ);
    glVertex3d(bb.minX, bb.minY, bb.maxZ);
    glEnd();
    
    // Draw top rectangle
    glBegin(GL_LINE_LOOP);
    glVertex3d(bb.minX, bb.maxY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.maxZ);
    glVertex3d(bb.minX, bb.maxY, bb.maxZ);
    glEnd();
    
    // Draw vertical lines connecting top and bottom
    glBegin(GL_LINES);
    glVertex3d(bb.minX, bb.minY, bb.minZ);
    glVertex3d(bb.minX, bb.maxY, bb.minZ);
    
    glVertex3d(bb.maxX, bb.minY, bb.minZ);
    glVertex3d(bb.maxX, bb.maxY, bb.minZ);
    
    glVertex3d(bb.maxX, bb.minY, bb.maxZ);
    glVertex3d(bb.maxX, bb.maxY, bb.maxZ);
    
    glVertex3d(bb.minX, bb.minY, bb.maxZ);
    glVertex3d(bb.minX, bb.maxY, bb.maxZ);
    glEnd();
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
        env->DeleteLocalRef(listClass);
        return;
    }
    
    // Get entity count
    jint entityCount = env->CallIntMethod(entityList, sizeMethod);
    
    // Get entity fields
    auto& entityFields = mc.getEntityFields();
    
    // Clear previous frame's boxes
    entityBoxes.clear();
    
    // Collect entity bounding boxes
    for (jint i = 0; i < entityCount && i < 100; i++) { // Limit to 100 entities
        jobject entity = env->CallObjectMethod(entityList, getMethod, i);
        
        if (entity && entity != player) {
            // Get entity position
            if (entityFields.posX && entityFields.posY && entityFields.posZ) {
                jdouble x = jni.getDoubleField(entity, entityFields.posX);
                jdouble y = jni.getDoubleField(entity, entityFields.posY);
                jdouble z = jni.getDoubleField(entity, entityFields.posZ);
                
                // Create bounding box around entity
                // Standard player size is 0.6 width, 1.8 height
                double width = 0.3;  // Half width
                double height = 1.8;
                
                AxisAlignedBB box(
                    x - width, y, z - width,
                    x + width, y + height, z + width
                );
                
                entityBoxes.push_back(box);
            }
        }
        
        if (entity) {
            env->DeleteLocalRef(entity);
        }
    }
    
    env->DeleteLocalRef(listClass);
    env->DeleteLocalRef(entityList);
    
    // Render all collected boxes
    if (!entityBoxes.empty()) {
        setupGL();
        
        for (const auto& box : entityBoxes) {
            // Draw filled box first (behind)
            if (renderMode == MODE_BOX_FILLED || renderMode == MODE_BOX_BOTH) {
                drawFilledBox(box);
            }
            
            // Draw outline on top
            if (renderMode == MODE_BOX_OUTLINE || renderMode == MODE_BOX_BOTH) {
                drawOutlinedBox(box);
            }
        }
        
        restoreGL();
    }
}
