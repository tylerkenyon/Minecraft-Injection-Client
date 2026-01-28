#include "Module.h"
#include "../utils/Logger.h"

Module::Module(const std::string& name, const std::string& description)
    : name(name)
    , description(description)
    , enabled(false)
    , keyBind(0) {
    registerEventHandlers();
}

Module::~Module() {
    if (enabled) {
        disable();
    }
}

void Module::toggle() {
    if (enabled) {
        disable();
    } else {
        enable();
    }
}

void Module::enable() {
    if (enabled) return;
    
    enabled = true;
    LOG_INFO("Module enabled: " + name);
    
    onEnable();
}

void Module::disable() {
    if (!enabled) return;
    
    enabled = false;
    LOG_INFO("Module disabled: " + name);
    
    onDisable();
    unregisterEventHandlers();
}

void Module::registerEventHandlers() {
    auto& eventBus = EventBus::getInstance();
    
    // Register tick event
    eventBus.subscribe(EventType::TICK, [this](const Event& e) {
        if (enabled) {
            onTick(static_cast<const TickEvent&>(e));
        }
    });
    
    // Register render event
    eventBus.subscribe(EventType::RENDER, [this](const Event& e) {
        if (enabled) {
            onRender(static_cast<const RenderEvent&>(e));
        }
    });
    
    // Register key press event
    eventBus.subscribe(EventType::KEY_PRESS, [this](const Event& e) {
        const auto& keyEvent = static_cast<const KeyPressEvent&>(e);
        if (keyBind != 0 && keyEvent.keyCode == keyBind) {
            toggle();
        }
        if (enabled) {
            onKeyPress(keyEvent);
        }
    });
}

void Module::unregisterEventHandlers() {
    // Events are automatically filtered by the enabled flag
    // In a production system, you'd want proper unsubscription
}
