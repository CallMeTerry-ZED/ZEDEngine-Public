/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-GLFW/GLFWWindow.h"
#include "Engine/Events/EventSystem.h"
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

        // Make this window’s context current
        glfwMakeContextCurrent(m_Window);

        // Store this pointer for use in callbacks
        glfwSetWindowUserPointer(m_Window, this);

        // Window close callback
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
            if (!self) return;
            // Mark it no longer running
            self->m_running = false;
            // Post a WindowClose event
            ZED::Event ev{};
            ev.type = ZED::EventType::WindowClose;
            ZED::EventSystem::Get().Post(ev);
        });

        // Window resize callback
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow*, int w, int h)
        {
            ZED::Event ev{};
            ev.type = ZED::EventType::WindowResized;
            ev.a = w;
            ev.b = h;
            ZED::EventSystem::Get().Post(ev);
        });

        // Window focus callback (0 = lost, 1 = gained)
        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow*, int focused)
        {
            ZED::Event ev{};
            ev.type = focused ? ZED::EventType::WindowFocusGained
                              : ZED::EventType::WindowFocusLost;
            ZED::EventSystem::Get().Post(ev);
        });

        return true;
    }

    void GLFWWindow::PollEvents()
    {
        // Process all queued OS events.  Callbacks will publish window events.
        glfwPollEvents();

        // If an external call flagged this window for closing, publish a close event.
        if (glfwWindowShouldClose(m_Window))
        {
            m_running = false;
            ZED::Event ev{};
            ev.type = ZED::EventType::WindowClose;
            ZED::EventSystem::Get().Post(ev);
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