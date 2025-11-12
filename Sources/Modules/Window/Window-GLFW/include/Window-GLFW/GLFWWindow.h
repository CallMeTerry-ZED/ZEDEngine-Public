/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#pragma once

#include "Engine/IWindow.h"
#include "GLFW/glfw3.h"

namespace ZED
{
    class ZEDENGINE_API GLFWWindow : public IWindow
    {
    public:
        bool Init(const char* title, int width, int height) override;
        void PollEvents() override;
        void Shutdown() override;
        bool IsRunning() const override;
        void* GetNativeHandle() const override;

        void SetMouseCapture(bool capture) override;
        void SetMouseVisible(bool visible) override;
        bool IsMouseCaptured() const override;
        bool IsMouseVisible() const override;

    private:
        GLFWwindow* m_Window = nullptr;
        bool m_running = true;
        bool m_mouseCaptured = false;
        bool m_mouseVisible = true;
    };
}

#endif
