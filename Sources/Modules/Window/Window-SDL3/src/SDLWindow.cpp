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
}