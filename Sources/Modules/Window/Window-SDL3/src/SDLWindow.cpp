/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-SDL3/SDLWindow.h"
#include "Engine/Events/EventSystem.h"
#include <iostream>

namespace ZED
{

    bool SDLWindow::Init(const char* title, int width, int height)
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            return false;
        }
        m_Window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
        if (!m_Window)
        {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
            return false;
        }
        return true;
    }

    void* SDLWindow::GetNativeHandle() const
    {
        if (!m_Window)
            return nullptr;

        SDL_PropertiesID props = SDL_GetWindowProperties(m_Window);
        if (!props)
            return nullptr;

        #if defined(_WIN32)
                return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif defined(__APPLE__)
                return SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        #elif defined(__linux__)
                // X11: returns Window (integer)
                return (void*)(uintptr_t)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        #else
                return nullptr;
        #endif
    }

    void SDLWindow::PollEvents()
    {
        SDL_Event e{};
        while (SDL_PollEvent(&e))
        {
            ZED::Event ev{};
            switch (e.type)
            {
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    // Flag the window as closed and post a WindowClose event
                    m_running = false;
                    ev.type = ZED::EventType::WindowClose;
                    EventSystem::Get().Post(ev);
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    // Post resize event with new width/height
                    ev.type = ZED::EventType::WindowResized;
                    ev.a = e.window.data1; // width
                    ev.b = e.window.data2; // height
                    EventSystem::Get().Post(ev);
                    break;

                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    ev.type = ZED::EventType::WindowFocusGained;
                    EventSystem::Get().Post(ev);
                    break;

                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    ev.type = ZED::EventType::WindowFocusLost;
                    EventSystem::Get().Post(ev);
                    break;

                default:
                    break;
            }
        }
    }

    void SDLWindow::Shutdown()
    {
        SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }

    bool SDLWindow::IsRunning() const
    {
        return m_running;
    }

    void SDLWindow::SetMouseCapture(bool capture)
    {
        m_mouseCaptured = capture;
        if (m_Window)
        {
            if (capture)
            {
                SDL_CaptureMouse(true);
                SDL_SetWindowRelativeMouseMode(m_Window, true);
            }
            else
            {
                SDL_SetWindowRelativeMouseMode(m_Window, false);
                SDL_CaptureMouse(false);
            }
        }
    }

    void SDLWindow::SetMouseVisible(bool visible)
    {
        m_mouseVisible = visible;
        if (m_Window)
        {
            if (visible)
            {
                SDL_ShowCursor();
            }
            else
            {
                SDL_HideCursor();
            }
        }
    }

    bool SDLWindow::IsMouseCaptured() const
    {
        return m_mouseCaptured;
    }

    bool SDLWindow::IsMouseVisible() const
    {
        return m_mouseVisible;
    }
}