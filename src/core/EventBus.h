#pragma once
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <memory>

// Event types
enum class EventType {
    TICK,
    RENDER,
    KEY_PRESS,
    MOUSE_MOVE
};

// Base event structure
struct Event {
    EventType type;
    virtual ~Event() = default;
};

struct TickEvent : public Event {
    TickEvent() { type = EventType::TICK; }
};

struct RenderEvent : public Event {
    RenderEvent() { type = EventType::RENDER; }
};

struct KeyPressEvent : public Event {
    int keyCode;
    KeyPressEvent(int key) : keyCode(key) { type = EventType::KEY_PRESS; }
};

// Event handler type
using EventHandler = std::function<void(const Event&)>;

class EventBus {
public:
    static EventBus& getInstance();
    
    // Subscribe to an event type
    void subscribe(EventType type, EventHandler handler);
    
    // Post an event to all subscribers
    void post(const Event& event);
    
    // Clear all subscribers
    void clear();

private:
    EventBus() = default;
    ~EventBus() = default;
    
    std::map<EventType, std::vector<EventHandler>> subscribers;
};
