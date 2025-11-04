/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-GLFW/GLFWWindow.h"
#include <iostream>

namespace ZED
{
    bool GLFWWindow::Init(const char* title, int width, int height)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            std::cerr << "Failed to create GLFW window" << std::endl;
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        return true;
    }

    void GLFWWindow::PollEvents()
    {
        glfwPollEvents();

        if (glfwWindowShouldClose(m_Window))
        {
            m_running = false;
            Shutdown();
        }
    }

    void GLFWWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool GLFWWindow::IsRunning() const
    {
        return m_running;
    }
}