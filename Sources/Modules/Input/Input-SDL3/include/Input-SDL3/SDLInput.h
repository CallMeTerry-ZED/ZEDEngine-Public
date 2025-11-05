/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLINPUT_H
#define SDLINPUT_H

#pragma once
#include "SDL3/SDL.h"
#include "Engine/Interfaces/Input/IInput.h"

namespace ZED
{
    class ZEDENGINE_API SDLInput : public IInput
    {
    public:
        SDLInput();
        void PollEvents() override;
        void SetEventCallback(void (*callback)(const InputEvent&)) override;

    private:
        void (*eventCallback)(const InputEvent&) = nullptr;
        ZED::Key MapSDLKey(SDL_Keycode keycode);
    };
}

#endif
