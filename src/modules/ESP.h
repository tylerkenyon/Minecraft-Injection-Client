#pragma once
#include "../core/Module.h"

class ESP : public Module {
public:
    ESP();
    
    void onEnable() override;
    void onDisable() override;
    void onRender(const RenderEvent& event) override;

private:
    void renderESP();
};
