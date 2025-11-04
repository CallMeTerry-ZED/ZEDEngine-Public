/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLTIME_H
#define SDLTIME_H

#pragma once

#include "Engine/ITime.h"
#include <SDL3/SDL_timer.h>

namespace ZED
{
    class SDLTime : public ITime
    {
    public:
        void Sleep(unsigned int milliseconds) override;
    };
}

#endif
