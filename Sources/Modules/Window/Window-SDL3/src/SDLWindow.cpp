/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Window-SDL3/SDLWindow.h"
#include <iostream>

namespace ZED
{

    bool SDLWindow::Init(const char* title, int width, int height)
    {
        if (!SDL_Init(SDL_INIT_VIDEO || SDL_INIT_EVENTS))
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
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                m_running = false;
                Shutdown();
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