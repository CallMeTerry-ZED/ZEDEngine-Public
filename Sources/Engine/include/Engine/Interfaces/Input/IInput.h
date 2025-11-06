/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef IINPUT_H
#define IINPUT_H

#pragma once

#include <functional>

namespace ZED
{
    enum class Key
    {
        Unknown = 0,

        // --- Letters ---
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // --- Numbers (Top row) ---
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // --- Numeric Keypad ---
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
        Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        NumpadAdd, NumpadSubtract, NumpadMultiply, NumpadDivide,
        NumpadEnter, NumpadDecimal,

        // --- Function keys ---
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

        // --- Whitespace / control ---
        Space,
        Tab,
        Enter,
        Backspace,
        Escape,

        // --- Modifiers ---
        LeftShift, RightShift,
        LeftControl, RightControl,
        LeftAlt, RightAlt,
        LeftSuper, RightSuper,  // Windows / Cmd

        // --- Navigation ---
        Up, Down, Left, Right,
        Home, End, PageUp, PageDown,
        Insert, Delete,
        PrintScreen, ScrollLock, Pause,

        // --- Punctuation / Symbols ---
        Minus, Equal,
        LeftBracket, RightBracket,
        Backslash,
        Semicolon,
        Apostrophe,
        Comma, Period,
        Slash,
        Grave,   // backtick

        // --- Media keys ---
        MediaPlayPause,
        MediaStop,
        MediaNext,
        MediaPrevious,
        VolumeUp,
        VolumeDown,
        VolumeMute,

        // --- Special ---
        CapsLock,
        NumLock,

        // --- Mouse ---
        MouseLeft,
        MouseRight,
        MouseMiddle,
        MouseButton4,
        MouseButton5,

        // =======================
        // Gamepad Buttons
        // =======================
        GamepadA,
        GamepadB,
        GamepadX,
        GamepadY,

        GamepadDPadUp,
        GamepadDPadDown,
        GamepadDPadLeft,
        GamepadDPadRight,

        GamepadLeftShoulder,     // L1 / LB
        GamepadRightShoulder,    // R1 / RB

        GamepadLeftStickClick,   // L3
        GamepadRightStickClick,  // R3

        GamepadStart,            // Options / Menu
        GamepadBack,             // Select / Share
        GamepadGuide,            // PS / Xbox button

        // =======================
        // Gamepad Triggers
        // (Digital or analog)
        // =======================
        GamepadLeftTrigger,      // L2
        GamepadRightTrigger,     // R2

        // =======================
        // Optional Extended Buttons
        // =======================
        GamepadTouchpad,         // PS4/PS5
        GamepadCapture,          // Switch Capture button
        GamepadMisc1,            // Extra / Vendor button

        // Elite / Scuf Paddles
        GamepadPaddle1,
        GamepadPaddle2,
        GamepadPaddle3,
        GamepadPaddle4,

        // =======================
        // Gamepad Axes
        // =======================
        GamepadAxisLeftX,
        GamepadAxisLeftY,
        GamepadAxisRightX,
        GamepadAxisRightY,

        GamepadAxisLeftTrigger,   // analog version
        GamepadAxisRightTrigger,
    };

    enum class InputEventType
    {
        // Keyboard
        KeyDown,
        KeyUp,
        TextInput,

        // Mouse
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,

        // Gamepad
        GamepadButtonDown,
        GamepadButtonUp,
        GamepadAxisMotion,

        // Generic axis (mouse wheel, joystick, VR, etc.)
        AxisMotion,

        // Device added/removed
        DeviceConnected,
        DeviceDisconnected,
    };

    struct ZEDENGINE_API InputEvent
    {
        InputEventType type;
        Key key = Key::Unknown;
        int mouseX = 0;
        int mouseY = 0;
    };

    class ZEDENGINE_API IInput
    {
    public:
        virtual ~IInput() = default;
        virtual void PollEvents() = 0;
        // Called once per frame to process OS events
        //virtual void Update() = 0;
        // Query whether a key is currently pressed
        virtual bool IsKeyDown(Key key) const = 0;
        virtual void SetEventCallback(const std::function<void(const InputEvent&)>& callback) = 0;
        virtual bool Init() = 0;
    };
}

#endif
