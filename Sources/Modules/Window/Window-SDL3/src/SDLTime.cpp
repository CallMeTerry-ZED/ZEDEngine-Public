/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-SDL3/SDLTime.h"
#include <iostream>

namespace ZED
{
    SDLTime::SDLTime()
        : m_frequency(0)
        , m_lastFrameTime(0)
        , m_deltaTime(0.0)
        , m_elapsedTime(0.0)
        , m_initialized(false)
    {
        // Get high-resolution timer frequency
        m_frequency = SDL_GetPerformanceFrequency();
        if (m_frequency == 0)
        {
            std::cerr << "[SDLTime] Warning: SDL_GetPerformanceFrequency() returned 0, falling back to SDL_GetTicks()\n";
        }
    }

    void SDLTime::Sleep(unsigned int milliseconds)
    {
        SDL_Delay(milliseconds);
    }

    void SDLTime::Update()
    {
        uint64_t currentTime;

        if (m_frequency > 0)
        {
            // Use high-resolution performance counter
            currentTime = SDL_GetPerformanceCounter();
        }
        else
        {
            // Fallback to ticks (milliseconds)
            currentTime = static_cast<uint64_t>(SDL_GetTicks()) * 1000;
            m_frequency = 1000;
        }

        if (!m_initialized)
        {
            // First frame: initialize but don't calculate delta
            m_lastFrameTime = currentTime;
            m_deltaTime = 0.0;
            m_elapsedTime = 0.0;
            m_initialized = true;
        }
        else
        {
            // Calculate delta time in seconds
            uint64_t deltaTicks = currentTime - m_lastFrameTime;
            m_deltaTime = static_cast<double>(deltaTicks) / static_cast<double>(m_frequency);

            // Update elapsed time
            m_elapsedTime += m_deltaTime;

            // Store current time for next frame
            m_lastFrameTime = currentTime;
        }
    }

    double SDLTime::GetDeltaTime() const
    {
        return m_deltaTime;
    }

    double SDLTime::GetElapsedTime() const
    {
        return m_elapsedTime;
    }

    extern "C" ZEDENGINE_API void RegisterTime()
    {
        static SDLTime sdlTime;
        std::cout << "[ZED::SDLTime] RegisterTime called\n";
        SetTimeImplementation(&sdlTime);
    }
}