/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Input-SDL3/SDLInput.h"
#include <iostream>

namespace ZED
{
    SDLInput::SDLInput() = default;
    SDLInput::~SDLInput() = default;

    bool SDLInput::Init()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS))
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            return false;
        }
        return true;
    }

    void SDLInput::SetEventCallback(const std::function<void(const InputEvent&)>& callback)
    {
        eventCallback = callback;
    }

    bool SDLInput::IsKeyDown(Key key) const
    {
        auto it = mKeyState.find(key);
        return it != mKeyState.end() && it->second;
    }

    void SDLInput::PollEvents() {
        SDL_Event e{};
        while (SDL_PollEvent(&e)) {
            if (eventCallback) {
                InputEvent event{};
                switch (e.type) {
                    case SDL_EVENT_KEY_DOWN:
                        event.type = InputEventType::KeyDown;
                        event.key  = TranslateKey(e.key.key);
                        // record key state
                        mKeyState[event.key] = true;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_KEY_UP:
                        event.type = InputEventType::KeyUp;
                        event.key  = TranslateKey(e.key.key);
                        mKeyState[event.key] = false;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        event.type = InputEventType::MouseMove;
                        event.mouseX = e.motion.x;
                        event.mouseY = e.motion.y;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        event.type = InputEventType::MouseButtonDown;
                        // Map SDL mouse buttons to our Key enum
                        switch (e.button.button) {
                            case SDL_BUTTON_LEFT:   event.key = Key::MouseLeft;   break;
                            case SDL_BUTTON_RIGHT:  event.key = Key::MouseRight;  break;
                            case SDL_BUTTON_MIDDLE: event.key = Key::MouseMiddle; break;
                            case SDL_BUTTON_X1:     event.key = Key::MouseButton4; break;
                            case SDL_BUTTON_X2:     event.key = Key::MouseButton5; break;
                            default:               event.key = Key::Unknown;       break;
                        }
                        mKeyState[event.key] = true;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        event.type = InputEventType::MouseButtonUp;
                        switch (e.button.button) {
                            case SDL_BUTTON_LEFT:   event.key = Key::MouseLeft;   break;
                            case SDL_BUTTON_RIGHT:  event.key = Key::MouseRight;  break;
                            case SDL_BUTTON_MIDDLE: event.key = Key::MouseMiddle; break;
                            case SDL_BUTTON_X1:     event.key = Key::MouseButton4; break;
                            case SDL_BUTTON_X2:     event.key = Key::MouseButton5; break;
                            default:               event.key = Key::Unknown;       break;
                        }
                        mKeyState[event.key] = false;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_MOUSE_WHEEL:
                        event.type = InputEventType::MouseWheel;
                        // wheel.x and wheel.y hold the amount scrolled horizontally and vertically
                        event.mouseX = e.wheel.x;
                        event.mouseY = e.wheel.y;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                        event.type = InputEventType::GamepadButtonDown;
                        // Map SDL gamepad buttons to our Key enum
                        switch (e.gbutton.button) {
                            case SDL_GAMEPAD_BUTTON_SOUTH:       event.key = Key::GamepadA;            break;
                            case SDL_GAMEPAD_BUTTON_EAST:       event.key = Key::GamepadB;            break;
                            case SDL_GAMEPAD_BUTTON_WEST:       event.key = Key::GamepadX;            break;
                            case SDL_GAMEPAD_BUTTON_NORTH:       event.key = Key::GamepadY;            break;
                            case SDL_GAMEPAD_BUTTON_BACK:          event.key = Key::GamepadBack;          break;
                            case SDL_GAMEPAD_BUTTON_GUIDE:         event.key = Key::GamepadGuide;         break;
                            case SDL_GAMEPAD_BUTTON_START:         event.key = Key::GamepadStart;         break;
                            case SDL_GAMEPAD_BUTTON_LEFT_STICK:    event.key = Key::GamepadLeftStickClick;  break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:   event.key = Key::GamepadRightStickClick; break;
                            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  event.key = Key::GamepadLeftShoulder;   break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: event.key = Key::GamepadRightShoulder;  break;
                            case SDL_GAMEPAD_BUTTON_DPAD_UP:       event.key = Key::GamepadDPadUp;         break;
                            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:     event.key = Key::GamepadDPadDown;       break;
                            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:     event.key = Key::GamepadDPadLeft;       break;
                            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:    event.key = Key::GamepadDPadRight;      break;
                            case SDL_GAMEPAD_BUTTON_MISC1:         event.key = Key::GamepadMisc1;          break;
                            case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:       event.key = Key::GamepadPaddle1;        break;
                            case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:       event.key = Key::GamepadPaddle2;        break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:       event.key = Key::GamepadPaddle3;        break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:       event.key = Key::GamepadPaddle4;        break;
                            case SDL_GAMEPAD_BUTTON_TOUCHPAD:      event.key = Key::GamepadTouchpad;       break;
                            default:                               event.key = Key::Unknown;               break;
                        }
                        mKeyState[event.key] = true;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_GAMEPAD_BUTTON_UP:
                        event.type = InputEventType::GamepadButtonUp;
                        switch (e.gbutton.button) {
                            case SDL_GAMEPAD_BUTTON_SOUTH:       event.key = Key::GamepadA;            break;
                            case SDL_GAMEPAD_BUTTON_EAST:       event.key = Key::GamepadB;            break;
                            case SDL_GAMEPAD_BUTTON_WEST:       event.key = Key::GamepadX;            break;
                            case SDL_GAMEPAD_BUTTON_NORTH:       event.key = Key::GamepadY;            break;
                            case SDL_GAMEPAD_BUTTON_BACK:          event.key = Key::GamepadBack;          break;
                            case SDL_GAMEPAD_BUTTON_GUIDE:         event.key = Key::GamepadGuide;         break;
                            case SDL_GAMEPAD_BUTTON_START:         event.key = Key::GamepadStart;         break;
                            case SDL_GAMEPAD_BUTTON_LEFT_STICK:     event.key = Key::GamepadLeftStickClick;  break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_STICK:    event.key = Key::GamepadRightStickClick; break;
                            case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  event.key = Key::GamepadLeftShoulder;   break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: event.key = Key::GamepadRightShoulder;  break;
                            case SDL_GAMEPAD_BUTTON_DPAD_UP:       event.key = Key::GamepadDPadUp;         break;
                            case SDL_GAMEPAD_BUTTON_DPAD_DOWN:     event.key = Key::GamepadDPadDown;       break;
                            case SDL_GAMEPAD_BUTTON_DPAD_LEFT:     event.key = Key::GamepadDPadLeft;       break;
                            case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:    event.key = Key::GamepadDPadRight;      break;
                            case SDL_GAMEPAD_BUTTON_MISC1:         event.key = Key::GamepadMisc1;          break;
                            case SDL_GAMEPAD_BUTTON_LEFT_PADDLE1:       event.key = Key::GamepadPaddle1;        break;
                            case SDL_GAMEPAD_BUTTON_LEFT_PADDLE2:       event.key = Key::GamepadPaddle2;        break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1:       event.key = Key::GamepadPaddle3;        break;
                            case SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2:       event.key = Key::GamepadPaddle4;        break;
                            case SDL_GAMEPAD_BUTTON_TOUCHPAD:      event.key = Key::GamepadTouchpad;       break;
                            default:                               event.key = Key::Unknown;               break;
                        }
                        mKeyState[event.key] = false;
                        eventCallback(event);
                        break;
                    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                        // Gamepad axis motion; record which axis moved and its value.  We use
                        // InputEventType::GamepadAxisMotion and set key to the axis enum.  The axis
                        // value (normalised -32768 to 32767) is passed via mouseX for convenience.
                        event.type = InputEventType::GamepadAxisMotion;
                        switch (e.gaxis.axis)
                        {
                            case SDL_GAMEPAD_AXIS_LEFTX:   event.key = Key::GamepadAxisLeftX;   break;
                            case SDL_GAMEPAD_AXIS_LEFTY:   event.key = Key::GamepadAxisLeftY;   break;
                            case SDL_GAMEPAD_AXIS_RIGHTX:  event.key = Key::GamepadAxisRightX;  break;
                            case SDL_GAMEPAD_AXIS_RIGHTY:  event.key = Key::GamepadAxisRightY;  break;
                            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:  event.key = Key::GamepadAxisLeftTrigger;  break;
                            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER: event.key = Key::GamepadAxisRightTrigger; break;
                            default:                       event.key = Key::Unknown;             break;
                        }
                        // Store the axis value (normalised) in mouseX for lack of a dedicated field.
                        event.mouseX = e.gaxis.value;
                        eventCallback(event);
                        break;
                    // case SDL_EVENT_ADDED:
                    //     event.type = InputEventType::DeviceConnected;
                    //     eventCallback(event);
                    //     break;
                    // case SDL_EVENT_DEVICE_REMOVED:
                    //     event.type = InputEventType::DeviceDisconnected;
                    //     eventCallback(event);
                    //     break;
                    default:
                        break;
                }
            }
        }
    }

 Key SDLInput::TranslateKey(SDL_Keycode keycode)
    {
        // Map SDL keycodes to our Key enum.  This covers letters, numbers, keypad,
        // function keys, control keys, modifiers, navigation and punctuation.
        // Unknown keys will return Key::Unknown.
        static const std::unordered_map<SDL_Keycode, Key> keyMap = {
            // Letters (lowercase)
            {SDLK_A, Key::A}, {SDLK_B, Key::B}, {SDLK_C, Key::C}, {SDLK_D, Key::D},
            {SDLK_E, Key::E}, {SDLK_F, Key::F}, {SDLK_G, Key::G}, {SDLK_H, Key::H},
            {SDLK_I, Key::I}, {SDLK_J, Key::J}, {SDLK_K, Key::K}, {SDLK_L, Key::L},
            {SDLK_M, Key::M}, {SDLK_N, Key::N}, {SDLK_O, Key::O}, {SDLK_P, Key::P},
            {SDLK_G, Key::Q}, {SDLK_R, Key::R}, {SDLK_S, Key::S}, {SDLK_T, Key::T},
            {SDLK_U, Key::U}, {SDLK_V, Key::V}, {SDLK_W, Key::W}, {SDLK_X, Key::X},
            {SDLK_Y, Key::Y}, {SDLK_Z, Key::Z},
            // Letters (uppercase) – SDL normalises letters to lowercase keycodes, so this
            // mapping covers both cases implicitly.
            // Numbers (top row)
            {SDLK_0, Key::Num0}, {SDLK_1, Key::Num1}, {SDLK_2, Key::Num2}, {SDLK_3, Key::Num3},
            {SDLK_4, Key::Num4}, {SDLK_5, Key::Num5}, {SDLK_6, Key::Num6}, {SDLK_7, Key::Num7},
            {SDLK_8, Key::Num8}, {SDLK_9, Key::Num9},
            // Numeric keypad
            {SDLK_KP_0, Key::Numpad0}, {SDLK_KP_1, Key::Numpad1}, {SDLK_KP_2, Key::Numpad2},
            {SDLK_KP_3, Key::Numpad3}, {SDLK_KP_4, Key::Numpad4}, {SDLK_KP_5, Key::Numpad5},
            {SDLK_KP_6, Key::Numpad6}, {SDLK_KP_7, Key::Numpad7}, {SDLK_KP_8, Key::Numpad8},
            {SDLK_KP_9, Key::Numpad9}, {SDLK_KP_PLUS, Key::NumpadAdd}, {SDLK_KP_MINUS, Key::NumpadSubtract},
            {SDLK_KP_MULTIPLY, Key::NumpadMultiply}, {SDLK_KP_DIVIDE, Key::NumpadDivide},
            {SDLK_KP_ENTER, Key::NumpadEnter}, {SDLK_KP_PERIOD, Key::NumpadDecimal},
            // Function keys
            {SDLK_F1, Key::F1}, {SDLK_F2, Key::F2}, {SDLK_F3, Key::F3}, {SDLK_F4, Key::F4},
            {SDLK_F5, Key::F5}, {SDLK_F6, Key::F6}, {SDLK_F7, Key::F7}, {SDLK_F8, Key::F8},
            {SDLK_F9, Key::F9}, {SDLK_F10, Key::F10}, {SDLK_F11, Key::F11}, {SDLK_F12, Key::F12},
            {SDLK_F13, Key::F13}, {SDLK_F14, Key::F14}, {SDLK_F15, Key::F15}, {SDLK_F16, Key::F16},
            {SDLK_F17, Key::F17}, {SDLK_F18, Key::F18}, {SDLK_F19, Key::F19}, {SDLK_F20, Key::F20},
            {SDLK_F21, Key::F21}, {SDLK_F22, Key::F22}, {SDLK_F23, Key::F23}, {SDLK_F24, Key::F24},
            // Whitespace / control
            {SDLK_SPACE, Key::Space}, {SDLK_TAB, Key::Tab}, {SDLK_RETURN, Key::Enter},
            {SDLK_BACKSPACE, Key::Backspace}, {SDLK_ESCAPE, Key::Escape},
            // Modifiers
            {SDLK_LSHIFT, Key::LeftShift}, {SDLK_RSHIFT, Key::RightShift},
            {SDLK_LCTRL, Key::LeftControl}, {SDLK_RCTRL, Key::RightControl},
            {SDLK_LALT, Key::LeftAlt}, {SDLK_RALT, Key::RightAlt},
            {SDLK_LGUI, Key::LeftSuper}, {SDLK_RGUI, Key::RightSuper},
            // Navigation
            {SDLK_UP, Key::Up}, {SDLK_DOWN, Key::Down}, {SDLK_LEFT, Key::Left}, {SDLK_RIGHT, Key::Right},
            {SDLK_HOME, Key::Home}, {SDLK_END, Key::End}, {SDLK_PAGEUP, Key::PageUp}, {SDLK_PAGEDOWN, Key::PageDown},
            {SDLK_INSERT, Key::Insert}, {SDLK_DELETE, Key::Delete},
            {SDLK_PRINTSCREEN, Key::PrintScreen}, {SDLK_SCROLLLOCK, Key::ScrollLock}, {SDLK_PAUSE, Key::Pause},
            // Punctuation / symbols
            {SDLK_MINUS, Key::Minus}, {SDLK_EQUALS, Key::Equal},
            {SDLK_LEFTBRACKET, Key::LeftBracket}, {SDLK_RIGHTBRACKET, Key::RightBracket},
            {SDLK_BACKSLASH, Key::Backslash}, {SDLK_SEMICOLON, Key::Semicolon},
            {SDLK_APOSTROPHE, Key::Apostrophe}, {SDLK_COMMA, Key::Comma}, {SDLK_PERIOD, Key::Period},
            {SDLK_SLASH, Key::Slash}, {SDLK_GRAVE, Key::Grave},
            // CapsLock / NumLock
            {SDLK_CAPSLOCK, Key::CapsLock}, {SDLK_NUMLOCKCLEAR, Key::NumLock}
        };
        auto it = keyMap.find(keycode);
        return (it != keyMap.end()) ? it->second : Key::Unknown;
    }

    // void SDLInput::Update()
    // {
    //     mKeyState.clear();
    //     SDL_Event e;
    //     while (SDL_PollEvent(&e))
    //     {
    //         if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP)
    //         {
    //             Key key = TranslateKey(e.key.key);
    //             bool pressed = (e.type == SDL_EVENT_KEY_DOWN);
    //             mKeyState[key] = pressed;
    //         }
    //         // handle mouse buttons similarly if needed
    //     }
    // }
}