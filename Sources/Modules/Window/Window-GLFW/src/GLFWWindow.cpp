/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-GLFW/GLFWWindow.h"
#include "Engine/Events/EventSystem.h"
#include <iostream>
#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#else
    #define GLFW_EXPOSE_NATIVE_X11    // or WAYLAND if you’re using that
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

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
            Event ev{};
            ev.type = EventType::WindowClose;
            EventSystem::Get().Post(ev);
        });

        // Window resize callback
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow*, int w, int h)
        {
            Event ev{};
            ev.type = EventType::WindowResized;
            ev.a = w;
            ev.b = h;
            EventSystem::Get().Post(ev);
        });

        // Window focus callback (0 = lost, 1 = gained)
        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow*, int focused)
        {
            Event ev{};
            ev.type = focused ? EventType::WindowFocusGained
                              : EventType::WindowFocusLost;
            EventSystem::Get().Post(ev);
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
            Event ev{};
            ev.type = EventType::WindowClose;
            EventSystem::Get().Post(ev);
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

    void* GLFWWindow::GetNativeHandle() const
    {
        #ifdef _WIN32
            return (void*)glfwGetWin32Window(m_Window);
        #elif defined(__APPLE__)
            return (void*)glfwGetCocoaWindow(m_Window);
        #else
            return (void*)glfwGetX11Window(m_Window);
        #endif
    }
}