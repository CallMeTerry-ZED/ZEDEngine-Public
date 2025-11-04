/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-SDL3/SDLWindow.h"
#include "Engine/IWindow.h"

extern "C" ZEDENGINE_API ZED::IWindow* CreateWindow()
{
    return new ZED::SDLWindow();
}