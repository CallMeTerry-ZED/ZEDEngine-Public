/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Time.h"
#include "Engine/ITime.h"

#include <iostream>

namespace ZED
{
    ITime* g_timeImpl = nullptr;

    void SetTimeImplementation(ITime* impl)
    {
        g_timeImpl = impl;
    }

    ITime* GetTimeImplementation()
    {
        return g_timeImpl;
    }

    void Time::Sleep(unsigned int milliseconds)
    {
        if (g_timeImpl)
        {
            g_timeImpl->Sleep(milliseconds);
        }
        else
        {
            std::cerr << "[ZED::Time] Warning: No sleep implementation linked!" << std::endl;
        }
    }
}