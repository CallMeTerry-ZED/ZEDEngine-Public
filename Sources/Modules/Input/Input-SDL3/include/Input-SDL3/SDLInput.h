/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLINPUT_H
#define SDLINPUT_H

#pragma once
#include "SDL3/SDL.h"
#include "Engine/Interfaces/Input/IInput.h"
#include <unordered_map>

namespace ZED
{
    class ZEDENGINE_API SDLInput : public IInput
    {
    public:
        SDLInput();
        ~SDLInput() override;

        bool Init() override;
        void PollEvents() override;
        void SetEventCallback(const std::function<void(const InputEvent&)>& callback) override;
        //void Update() override;
        bool IsKeyDown(Key key) const override;

    private:
        std::function<void(const InputEvent&)> eventCallback;
        Key TranslateKey(SDL_Keycode keycode);
        std::unordered_map<Key, bool> mKeyState;
    };
}

#endif
