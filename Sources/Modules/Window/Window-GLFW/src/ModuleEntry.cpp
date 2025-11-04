/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-GLFW/GLFWWindow.h"
#include "Engine/IWindow.h"

extern "C" ZEDENGINE_API ZED::IWindow* CreateWindow()
{
    return new ZED::GLFWWindow();
}