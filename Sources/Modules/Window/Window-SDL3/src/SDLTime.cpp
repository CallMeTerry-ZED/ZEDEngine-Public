/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-SDL3/SDLTime.h"
#include <iostream>

namespace ZED
{
    void SDLTime::Sleep(unsigned int milliseconds)
    {
        SDL_Delay(milliseconds);
        //std::cout << "[ZED::SDLTime] Sleep called\n";
    }

    extern "C" ZEDENGINE_API void RegisterTime()
    {
        static SDLTime sdlTime;
        std::cout << "[ZED::SDLTime] RegisterTime called\n";
        SetTimeImplementation(&sdlTime);
    }
}