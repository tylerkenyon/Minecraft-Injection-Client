#pragma once
#include "../core/Module.h"
#include <vector>

// Axis-Aligned Bounding Box structure for rendering
struct AxisAlignedBB {
    double minX, minY, minZ;
    double maxX, maxY, maxZ;
    
    AxisAlignedBB() : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) {}
    AxisAlignedBB(double x1, double y1, double z1, double x2, double y2, double z2)
        : minX(x1), minY(y1), minZ(z1), maxX(x2), maxY(y2), maxZ(z2) {}
};

class ESP : public Module {
public:
    ESP();
    
    void onEnable() override;
    void onDisable() override;
    void onRender(const RenderEvent& event) override;

private:
    void renderESP();
    void drawFilledBox(const AxisAlignedBB& boundingBox);
    void drawOutlinedBox(const AxisAlignedBB& boundingBox);
    void setupGL();
    void restoreGL();
    
    // ESP render mode
    enum RenderMode {
        MODE_BOX_OUTLINE,
        MODE_BOX_FILLED,
        MODE_BOX_BOTH
    };
    
    RenderMode renderMode;
    
    // Color settings (RGBA, normalized 0-1)
    struct Color {
        float r, g, b, a;
        Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(float red, float green, float blue, float alpha = 1.0f) 
            : r(red), g(green), b(blue), a(alpha) {}
    };
    
    Color boxColor;
    Color fillColor;
    
    // Cache for entity bounding boxes
    std::vector<AxisAlignedBB> entityBoxes;
};
