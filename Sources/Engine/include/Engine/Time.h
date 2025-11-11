/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef TIME_H
#define TIME_H

#pragma once

namespace ZED
{
    class ZEDENGINE_API Time
    {
    public:
        // Sleep for the specified number of milliseconds
        static void Sleep(unsigned int milliseconds);

        // Update time calculations (call once per frame)
        static void Update();

        // Get delta time in seconds since last frame
        static double GetDeltaTime();

        // Get total elapsed time in seconds since first Update() call
        static double GetElapsedTime();
    };
}

#endif
