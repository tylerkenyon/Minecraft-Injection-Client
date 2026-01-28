#pragma once
#include <string>
#include "EventBus.h"

class Module {
public:
    Module(const std::string& name, const std::string& description);
    virtual ~Module();
    
    // Module lifecycle
    virtual void onEnable() {}
    virtual void onDisable() {}
    
    // Event handlers
    virtual void onTick(const TickEvent& event) {}
    virtual void onRender(const RenderEvent& event) {}
    virtual void onKeyPress(const KeyPressEvent& event) {}
    
    // Toggle the module
    void toggle();
    
    // Enable/disable
    void enable();
    void disable();
    
    // Getters
    bool isEnabled() const { return enabled; }
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    int getKeyBind() const { return keyBind; }
    
    // Setters
    void setKeyBind(int key) { keyBind = key; }

protected:
    std::string name;
    std::string description;
    bool enabled;
    int keyBind;
    
    void registerEventHandlers();
    void unregisterEventHandlers();
};
