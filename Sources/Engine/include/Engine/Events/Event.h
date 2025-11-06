/*
* © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef EVENT_H
#define EVENT_H

#pragma once
#include <cstdint>

namespace ZED
{
    // Enumerates all event types that can be dispatched through the engine's event system.
    enum class EventType : uint8_t {
        // --- Window events ---
        WindowClose,
        WindowResized,
        WindowFocusGained,
        WindowFocusLost,

        // --- Keyboard events ---
        KeyDown,
        KeyUp,
        TextInput,

        // --- Mouse events ---
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,

        // --- Gamepad events ---
        GamepadButtonDown,
        GamepadButtonUp,
        GamepadAxisMotion,

        // --- Device lifecycle events ---
        DeviceConnected,
        DeviceDisconnected
    };

    // Generic event payload.
    struct Event {
        EventType type{};
        int a = 0;    // e.g., key code, button id, width
        int b = 0;    // e.g., modifier flags, height
        int c = 0;    // e.g., x coordinate
        int d = 0;    // e.g., y coordinate
    };
}

#endif // EVENT_H