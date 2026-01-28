#include "EventBus.h"

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(EventType type, EventHandler handler) {
    subscribers[type].push_back(handler);
}

void EventBus::post(const Event& event) {
    auto it = subscribers.find(event.type);
    if (it != subscribers.end()) {
        for (auto& handler : it->second) {
            handler(event);
        }
    }
}

void EventBus::clear() {
    subscribers.clear();
}
