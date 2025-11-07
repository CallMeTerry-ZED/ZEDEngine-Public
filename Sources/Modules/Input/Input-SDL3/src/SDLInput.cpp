/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Input-SDL3/SDLInput.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Events/Event.h"
#include <iostream>
#include <algorithm>

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

        // Build the scancode -> Key lookup map
        BuildScancodeMap();

        // Initialise previous keyboard state vector
        int numScancodes = SDL_SCANCODE_COUNT;
        mPrevKeyStates.resize(static_cast<size_t>(numScancodes));
        const bool *currentState = SDL_GetKeyboardState(nullptr);
        std::copy(currentState, currentState + numScancodes, mPrevKeyStates.begin());

        // Initialise previous mouse state
        mPrevMouseButtons = SDL_GetMouseState(&mPrevMouseX, &mPrevMouseY);

        // Open all connected gamepads
        int numPads = 0;
        SDL_JoystickID* ids = SDL_GetGamepads(&numPads);
        for (int i = 0; i < numPads; ++i)
        {
            if (SDL_Gamepad* pad = SDL_OpenGamepad(ids[i]))
            {
                GamepadState gp;
                gp.pad = pad;
                mGamepads.push_back(gp);
            }
        }
        SDL_free(ids);

        //std::cout << "[SDLInput] Initialized (VIDEO+GAMEPAD+EVENTS)\n";
        return true;
    }

    void SDLInput::AttachToNativeWindow(void* native_handle)
    {
        if (mSDLWindow || !native_handle) return;

        SDL_PropertiesID props = SDL_CreateProperties();
        if (!props)
        {
            std::cerr << "[SDLInput] SDL_CreateProperties failed: " << SDL_GetError() << "\n";
            return;
        }

        #if defined(_WIN32)
            SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, native_handle);
        #elif defined(__APPLE__)
            // Wrap an existing NSWindow*
            SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_COCOA_WINDOW_POINTER, native_handle);
        #elif defined(__linux__)
            // If you’re using X11:
            // native_handle is an ::Window (integer). Cast to uintptr_t for the NUMBER property.
            SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER, (Sint64)(uintptr_t)native_handle);
            // If you’re on Wayland and have wl_surface*, use:
            // SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER, native_handle);
        #endif

        // Make sure the window is focusable so k/m input is routed here.
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, true);

        // Don’t let SDL implicitly create or show anything else:
        // (Not strictly required, but explicit is nice.)
        // SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, false);

        mSDLWindow = SDL_CreateWindowWithProperties(props);
        SDL_DestroyProperties(props);

        if (!mSDLWindow) {
            std::cerr << "[SDLInput] SDL_CreateWindowWithProperties failed: " << SDL_GetError() << "\n";
        }
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

    void SDLInput::PollEvents()
    {
        // Pump SDL to update internal keyboard/mouse/gamepad state.  We do not
        // call SDL_PollEvent here; the window module handles that.
        SDL_PumpEvents();

        // --- Keyboard scanning ---
        const bool *state = SDL_GetKeyboardState(nullptr);
        int numScancodes = SDL_SCANCODE_COUNT;
        for (const auto& pair : mScancodeToKey)
        {
            SDL_Scancode sc = pair.first;
            Key key = pair.second;
            bool pressed = (state[sc] != 0);
            bool wasPressed = (mPrevKeyStates[sc] != 0);

            if (pressed != wasPressed)
            {
                mKeyState[key] = pressed;

                InputEvent ie{};
                ie.type = pressed ? InputEventType::KeyDown : InputEventType::KeyUp;
                ie.key  = key;
                if (eventCallback) eventCallback(ie);

                // Post to event system as deferred to avoid blocking other systems
                Event ev{};
                ev.type = pressed ? EventType::KeyDown : EventType::KeyUp;
                ev.a    = static_cast<int>(key);
                EventSystem::Get().PostDeferred(ev);
            }
        }
        std::copy(state, state + numScancodes, mPrevKeyStates.begin());

        // --- Mouse scanning ---
        float mouseX = 0;
        float mouseY = 0;
        Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX != mPrevMouseX || mouseY != mPrevMouseY)
        {
            InputEvent ie{};
            ie.type   = InputEventType::MouseMove;
            ie.mouseX = static_cast<int>(mouseX);
            ie.mouseY = static_cast<int>(mouseY);
            if (eventCallback) eventCallback(ie);

            Event ev{};
            ev.type = EventType::MouseMove;
            ev.c    = static_cast<int>(mouseX);
            ev.d    = static_cast<int>(mouseY);
            EventSystem::Get().PostDeferred(ev);

            mPrevMouseX = mouseX;
            mPrevMouseY = mouseY;
        }

        // Buttons
        static const struct {
            Uint32 mask; Key key; EventType downType; EventType upType;
        } buttonMap[] = {
            { SDL_BUTTON_MASK(SDL_BUTTON_LEFT),   Key::MouseLeft,   EventType::MouseButtonDown, EventType::MouseButtonUp },
            { SDL_BUTTON_MASK(SDL_BUTTON_RIGHT),  Key::MouseRight,  EventType::MouseButtonDown, EventType::MouseButtonUp },
            { SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE), Key::MouseMiddle, EventType::MouseButtonDown, EventType::MouseButtonUp },
            { SDL_BUTTON_MASK(SDL_BUTTON_X1),     Key::MouseButton4,EventType::MouseButtonDown, EventType::MouseButtonUp },
            { SDL_BUTTON_MASK(SDL_BUTTON_X2),     Key::MouseButton5,EventType::MouseButtonDown, EventType::MouseButtonUp }
        };
        for (const auto& bm : buttonMap)
        {
            bool pressed    = (buttons & bm.mask) != 0;
            bool wasPressed = (mPrevMouseButtons & bm.mask) != 0;
            if (pressed != wasPressed)
            {
                mKeyState[bm.key] = pressed;
                InputEvent ie{};
                ie.type = pressed ? InputEventType::MouseButtonDown : InputEventType::MouseButtonUp;
                ie.key  = bm.key;
                if (eventCallback) eventCallback(ie);

                Event ev{};
                ev.type = pressed ? bm.downType : bm.upType;
                ev.a    = static_cast<int>(bm.key);
                EventSystem::Get().PostDeferred(ev);
            }
        }
        mPrevMouseButtons = buttons;

        // --- Gamepad scanning ---
        PollGamepads();
    }

    // Map SDL keycodes (used by text input) to our Key enum.  Used by the callback (not scanning).
    Key SDLInput::TranslateKey(SDL_Keycode keycode)
    {
        switch (keycode)
        {
            case SDLK_A: return Key::A;
            case SDLK_B: return Key::B;
            case SDLK_C: return Key::C;
            case SDLK_D: return Key::D;
            case SDLK_E: return Key::E;
            case SDLK_F: return Key::F;
            case SDLK_G: return Key::G;
            case SDLK_H: return Key::H;
            case SDLK_I: return Key::I;
            case SDLK_J: return Key::J;
            case SDLK_K: return Key::K;
            case SDLK_L: return Key::L;
            case SDLK_M: return Key::M;
            case SDLK_N: return Key::N;
            case SDLK_O: return Key::O;
            case SDLK_P: return Key::P;
            case SDLK_Q: return Key::Q;
            case SDLK_R: return Key::R;
            case SDLK_S: return Key::S;
            case SDLK_T: return Key::T;
            case SDLK_U: return Key::U;
            case SDLK_V: return Key::V;
            case SDLK_W: return Key::W;
            case SDLK_X: return Key::X;
            case SDLK_Y: return Key::Y;
            case SDLK_Z: return Key::Z;
            case SDLK_0: return Key::Num0;
            case SDLK_1: return Key::Num1;
            case SDLK_2: return Key::Num2;
            case SDLK_3: return Key::Num3;
            case SDLK_4: return Key::Num4;
            case SDLK_5: return Key::Num5;
            case SDLK_6: return Key::Num6;
            case SDLK_7: return Key::Num7;
            case SDLK_8: return Key::Num8;
            case SDLK_9: return Key::Num9;
            case SDLK_ESCAPE: return Key::Escape;
            case SDLK_RETURN: return Key::Enter;
            case SDLK_SPACE: return Key::Space;
            case SDLK_TAB: return Key::Tab;
            case SDLK_BACKSPACE: return Key::Backspace;
            case SDLK_LSHIFT: return Key::LeftShift;
            case SDLK_RSHIFT: return Key::RightShift;
            case SDLK_LCTRL: return Key::LeftControl;
            case SDLK_RCTRL: return Key::RightControl;
            case SDLK_LALT: return Key::LeftAlt;
            case SDLK_RALT: return Key::RightAlt;
            case SDLK_LGUI: return Key::LeftSuper;
            case SDLK_RGUI: return Key::RightSuper;
            case SDLK_UP: return Key::Up;
            case SDLK_DOWN: return Key::Down;
            case SDLK_LEFT: return Key::Left;
            case SDLK_RIGHT: return Key::Right;
            case SDLK_PAGEUP: return Key::PageUp;
            case SDLK_PAGEDOWN: return Key::PageDown;
            case SDLK_HOME: return Key::Home;
            case SDLK_END: return Key::End;
            case SDLK_INSERT: return Key::Insert;
            case SDLK_DELETE: return Key::Delete;
            case SDLK_MINUS: return Key::Minus;
            case SDLK_EQUALS: return Key::Equal;
            case SDLK_LEFTBRACKET: return Key::LeftBracket;
            case SDLK_RIGHTBRACKET: return Key::RightBracket;
            case SDLK_BACKSLASH: return Key::Backslash;
            case SDLK_SEMICOLON: return Key::Semicolon;
            case SDLK_APOSTROPHE: return Key::Apostrophe;
            case SDLK_COMMA: return Key::Comma;
            case SDLK_PERIOD: return Key::Period;
            case SDLK_SLASH: return Key::Slash;
            case SDLK_GRAVE: return Key::Grave;
            default: return Key::Unknown;
        }
    }

        void SDLInput::BuildScancodeMap()
    {
        // Map commonly used scancodes to engine keys.  Expand as needed.
        mScancodeToKey.clear();
        mScancodeToKey[SDL_SCANCODE_A] = Key::A;
        mScancodeToKey[SDL_SCANCODE_B] = Key::B;
        mScancodeToKey[SDL_SCANCODE_C] = Key::C;
        mScancodeToKey[SDL_SCANCODE_D] = Key::D;
        mScancodeToKey[SDL_SCANCODE_E] = Key::E;
        mScancodeToKey[SDL_SCANCODE_F] = Key::F;
        mScancodeToKey[SDL_SCANCODE_G] = Key::G;
        mScancodeToKey[SDL_SCANCODE_H] = Key::H;
        mScancodeToKey[SDL_SCANCODE_I] = Key::I;
        mScancodeToKey[SDL_SCANCODE_J] = Key::J;
        mScancodeToKey[SDL_SCANCODE_K] = Key::K;
        mScancodeToKey[SDL_SCANCODE_L] = Key::L;
        mScancodeToKey[SDL_SCANCODE_M] = Key::M;
        mScancodeToKey[SDL_SCANCODE_N] = Key::N;
        mScancodeToKey[SDL_SCANCODE_O] = Key::O;
        mScancodeToKey[SDL_SCANCODE_P] = Key::P;
        mScancodeToKey[SDL_SCANCODE_Q] = Key::Q;
        mScancodeToKey[SDL_SCANCODE_R] = Key::R;
        mScancodeToKey[SDL_SCANCODE_S] = Key::S;
        mScancodeToKey[SDL_SCANCODE_T] = Key::T;
        mScancodeToKey[SDL_SCANCODE_U] = Key::U;
        mScancodeToKey[SDL_SCANCODE_V] = Key::V;
        mScancodeToKey[SDL_SCANCODE_W] = Key::W;
        mScancodeToKey[SDL_SCANCODE_X] = Key::X;
        mScancodeToKey[SDL_SCANCODE_Y] = Key::Y;
        mScancodeToKey[SDL_SCANCODE_Z] = Key::Z;

        mScancodeToKey[SDL_SCANCODE_0] = Key::Num0;
        mScancodeToKey[SDL_SCANCODE_1] = Key::Num1;
        mScancodeToKey[SDL_SCANCODE_2] = Key::Num2;
        mScancodeToKey[SDL_SCANCODE_3] = Key::Num3;
        mScancodeToKey[SDL_SCANCODE_4] = Key::Num4;
        mScancodeToKey[SDL_SCANCODE_5] = Key::Num5;
        mScancodeToKey[SDL_SCANCODE_6] = Key::Num6;
        mScancodeToKey[SDL_SCANCODE_7] = Key::Num7;
        mScancodeToKey[SDL_SCANCODE_8] = Key::Num8;
        mScancodeToKey[SDL_SCANCODE_9] = Key::Num9;

        mScancodeToKey[SDL_SCANCODE_ESCAPE] = Key::Escape;
        mScancodeToKey[SDL_SCANCODE_RETURN] = Key::Enter;
        mScancodeToKey[SDL_SCANCODE_SPACE] = Key::Space;
        mScancodeToKey[SDL_SCANCODE_TAB] = Key::Tab;
        mScancodeToKey[SDL_SCANCODE_BACKSPACE] = Key::Backspace;

        mScancodeToKey[SDL_SCANCODE_LSHIFT] = Key::LeftShift;
        mScancodeToKey[SDL_SCANCODE_RSHIFT] = Key::RightShift;
        mScancodeToKey[SDL_SCANCODE_LCTRL] = Key::LeftControl;
        mScancodeToKey[SDL_SCANCODE_RCTRL] = Key::RightControl;
        mScancodeToKey[SDL_SCANCODE_LALT] = Key::LeftAlt;
        mScancodeToKey[SDL_SCANCODE_RALT] = Key::RightAlt;
        mScancodeToKey[SDL_SCANCODE_LGUI] = Key::LeftSuper;
        mScancodeToKey[SDL_SCANCODE_RGUI] = Key::RightSuper;

        mScancodeToKey[SDL_SCANCODE_UP] = Key::Up;
        mScancodeToKey[SDL_SCANCODE_DOWN] = Key::Down;
        mScancodeToKey[SDL_SCANCODE_LEFT] = Key::Left;
        mScancodeToKey[SDL_SCANCODE_RIGHT] = Key::Right;

        mScancodeToKey[SDL_SCANCODE_PAGEUP] = Key::PageUp;
        mScancodeToKey[SDL_SCANCODE_PAGEDOWN] = Key::PageDown;
        mScancodeToKey[SDL_SCANCODE_HOME] = Key::Home;
        mScancodeToKey[SDL_SCANCODE_END] = Key::End;
        mScancodeToKey[SDL_SCANCODE_INSERT] = Key::Insert;
        mScancodeToKey[SDL_SCANCODE_DELETE] = Key::Delete;

        mScancodeToKey[SDL_SCANCODE_MINUS] = Key::Minus;
        mScancodeToKey[SDL_SCANCODE_EQUALS] = Key::Equal;
        mScancodeToKey[SDL_SCANCODE_LEFTBRACKET] = Key::LeftBracket;
        mScancodeToKey[SDL_SCANCODE_RIGHTBRACKET] = Key::RightBracket;
        mScancodeToKey[SDL_SCANCODE_BACKSLASH] = Key::Backslash;
        mScancodeToKey[SDL_SCANCODE_SEMICOLON] = Key::Semicolon;
        mScancodeToKey[SDL_SCANCODE_APOSTROPHE] = Key::Apostrophe;
        mScancodeToKey[SDL_SCANCODE_COMMA] = Key::Comma;
        mScancodeToKey[SDL_SCANCODE_PERIOD] = Key::Period;
        mScancodeToKey[SDL_SCANCODE_SLASH] = Key::Slash;
        mScancodeToKey[SDL_SCANCODE_GRAVE] = Key::Grave;
    }

        void SDLInput::PollGamepads()
    {
        // Iterate over all opened gamepads and generate events when buttons
        // or axes change.  Use SDL_GetGamepadButton and SDL_GetGamepadAxis
        // to poll current state and compare with previous state.
        for (auto& gp : mGamepads)
        {
            if (!gp.pad) continue;
            // Button scanning
            static const struct {
                SDL_GamepadButton sdlButton;
                Key engineKey;
            } buttonMap[] = {
                { SDL_GAMEPAD_BUTTON_SOUTH,             Key::GamepadA },
                { SDL_GAMEPAD_BUTTON_EAST,             Key::GamepadB },
                { SDL_GAMEPAD_BUTTON_WEST,             Key::GamepadX },
                { SDL_GAMEPAD_BUTTON_NORTH,             Key::GamepadY },
                { SDL_GAMEPAD_BUTTON_BACK,          Key::GamepadBack },
                { SDL_GAMEPAD_BUTTON_GUIDE,         Key::GamepadGuide },
                { SDL_GAMEPAD_BUTTON_START,         Key::GamepadStart },
                { SDL_GAMEPAD_BUTTON_LEFT_STICK,     Key::GamepadLeftStickClick },
                { SDL_GAMEPAD_BUTTON_RIGHT_STICK,    Key::GamepadRightStickClick },
                { SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  Key::GamepadLeftShoulder },
                { SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, Key::GamepadRightShoulder },
                { SDL_GAMEPAD_BUTTON_DPAD_UP,       Key::GamepadDPadUp },
                { SDL_GAMEPAD_BUTTON_DPAD_DOWN,     Key::GamepadDPadDown },
                { SDL_GAMEPAD_BUTTON_DPAD_LEFT,     Key::GamepadDPadLeft },
                { SDL_GAMEPAD_BUTTON_DPAD_RIGHT,    Key::GamepadDPadRight },
                { SDL_GAMEPAD_BUTTON_MISC1,         Key::GamepadMisc1 },
                { SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,       Key::GamepadPaddle1 },
                { SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,       Key::GamepadPaddle2 },
                { SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,       Key::GamepadPaddle3 },
                { SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,       Key::GamepadPaddle4 },
                { SDL_GAMEPAD_BUTTON_TOUCHPAD,      Key::GamepadTouchpad }
            };
            for (const auto& bm : buttonMap)
            {
                bool pressed    = SDL_GetGamepadButton(gp.pad, bm.sdlButton) != 0;
                bool wasPressed = gp.prevButtons[bm.sdlButton];
                if (pressed != wasPressed)
                {
                    gp.prevButtons[bm.sdlButton] = pressed;
                    mKeyState[bm.engineKey] = pressed;

                    InputEvent ie{};
                    ie.type = pressed ? InputEventType::GamepadButtonDown : InputEventType::GamepadButtonUp;
                    ie.key  = bm.engineKey;
                    if (eventCallback) eventCallback(ie);

                    Event ev{};
                    ev.type = pressed ? EventType::GamepadButtonDown : EventType::GamepadButtonUp;
                    ev.a    = static_cast<int>(bm.engineKey);
                    EventSystem::Get().PostDeferred(ev);
                }
            }
            // Axis scanning
            static const struct {
                SDL_GamepadAxis axis;
                Key engineKey;
            } axisMap[] = {
                { SDL_GAMEPAD_AXIS_LEFTX,         Key::GamepadAxisLeftX },
                { SDL_GAMEPAD_AXIS_LEFTY,         Key::GamepadAxisLeftY },
                { SDL_GAMEPAD_AXIS_RIGHTX,        Key::GamepadAxisRightX },
                { SDL_GAMEPAD_AXIS_RIGHTY,        Key::GamepadAxisRightY },
                { SDL_GAMEPAD_AXIS_LEFT_TRIGGER,  Key::GamepadAxisLeftTrigger },
                { SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, Key::GamepadAxisRightTrigger }
            };
            for (const auto& am : axisMap)
            {
                Sint16 value = SDL_GetGamepadAxis(gp.pad, am.axis);
                Sint16 prev  = gp.prevAxes[am.axis];
                if (value != prev)
                {
                    gp.prevAxes[am.axis] = value;

                    InputEvent ie{};
                    ie.type   = InputEventType::GamepadAxisMotion;
                    ie.key    = am.engineKey;
                    ie.mouseX = value; // store axis value for callback
                    if (eventCallback) eventCallback(ie);

                    Event ev{};
                    ev.type = EventType::GamepadAxisMotion;
                    ev.a    = static_cast<int>(am.engineKey);
                    ev.b    = value;
                    EventSystem::Get().PostDeferred(ev);
                }
            }
        }
    }
}