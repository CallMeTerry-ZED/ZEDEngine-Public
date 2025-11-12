/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_mouse.h>
#include "Engine/IWindow.h"

namespace ZED
{
    class ZEDENGINE_API SDLWindow : public IWindow
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
        SDL_Window* m_Window = nullptr;
        bool m_running = true;
        bool m_mouseCaptured = false;
        bool m_mouseVisible = true;
    };
}

#endif
