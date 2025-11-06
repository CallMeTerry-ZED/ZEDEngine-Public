/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLINPUT_H
#define SDLINPUT_H

#pragma once
#include "SDL3/SDL.h"
#include "Engine/Interfaces/Input/IInput.h"
#include <vector>
#include <unordered_map>

namespace ZED
{
    /**
     * SDLInput scans keyboard, mouse and gamepad state without draining
     * SDL's event queue.  It posts input events to the engine's event system
     * and invokes the user callback for immediate reactions.
     */
    class ZEDENGINE_API SDLInput : public IInput
    {
    public:
        SDLInput();
        ~SDLInput() override;

        bool Init() override;
        void PollEvents() override;
        void SetEventCallback(const std::function<void(const InputEvent&)>& callback) override;
        bool IsKeyDown(Key key) const override;

    private:
        std::function<void(const InputEvent&)> eventCallback;
        Key TranslateKey(SDL_Keycode keycode);
        // Map high-level keys to pressed state for IsKeyDown
        std::unordered_map<Key, bool> mKeyState;
        // Previous keyboard state for detecting changes
        std::vector<Uint8> mPrevKeyStates;
        // Map SDL scancodes to engine Key enum for keyboard scanning
        std::unordered_map<SDL_Scancode, Key> mScancodeToKey;

        // Previous mouse position and button state
        float mPrevMouseX = 0;
        float mPrevMouseY = 0;
        Uint32 mPrevMouseButtons = 0;

        // Gamepad state tracking
        struct GamepadState
        {
            SDL_Gamepad* pad = nullptr;
            std::unordered_map<int, bool> prevButtons;
            std::unordered_map<int, Sint16> prevAxes;
        };
        std::vector<GamepadState> mGamepads;

        // Helpers to build mapping and poll gamepads
        void BuildScancodeMap();
        void PollGamepads();
    };
}

#endif
