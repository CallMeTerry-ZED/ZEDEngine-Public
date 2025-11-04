/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef ITIME_H
#define ITIME_H

#pragma once

namespace ZED
{
    class ITime
    {
    public:
        virtual ~ITime() = default;
        virtual void Sleep(unsigned int milliseconds) = 0;
    };

    // Global accessor (like singleton-style access)
    extern ITime* g_timeImpl;
    void SetTimeImplementation(ITime* impl);
    ITime* GetTimeImplementation();
}

#endif
