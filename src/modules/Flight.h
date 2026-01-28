#pragma once
#include "../core/Module.h"

class Flight : public Module {
public:
    Flight();
    
    void onEnable() override;
    void onDisable() override;
    void onTick(const TickEvent& event) override;

private:
    void setFlying(bool flying);
};
