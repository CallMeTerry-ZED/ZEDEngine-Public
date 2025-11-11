/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLTIME_H
#define SDLTIME_H

#pragma once

#include "Engine/ITime.h"
#include <SDL3/SDL_timer.h>
#include <cstdint>

namespace ZED
{
    class ZEDENGINE_API SDLTime : public ITime
    {
    public:
        SDLTime();

        void Sleep(unsigned int milliseconds) override;
        void Update() override;
        double GetDeltaTime() const override;
        double GetElapsedTime() const override;

    private:
        // High-resolution timer state
        uint64_t m_frequency;
        uint64_t m_lastFrameTime;
        double m_deltaTime;
        double m_elapsedTime;
        bool m_initialized;
    };
}

#endif
