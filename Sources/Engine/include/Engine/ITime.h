/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef ITIME_H
#define ITIME_H

#pragma once

namespace ZED
{
    class ZEDENGINE_API ITime
    {
    public:
        virtual ~ITime() = default;

        // Sleep for the specified number of milliseconds
        virtual void Sleep(unsigned int milliseconds) = 0;

        // Update time calculations (call once per frame)
        virtual void Update() = 0;

        // Get delta time in seconds since last frame
        virtual double GetDeltaTime() const = 0;

        // Get total elapsed time in seconds since first Update() call
        virtual double GetElapsedTime() const = 0;
    };

    // Global accessor (like singleton-style access)
    extern ITime* g_timeImpl;
    ZEDENGINE_API void SetTimeImplementation(ITime* impl);
    ITime* GetTimeImplementation();
}

#endif
