/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef IWINDOW_H
#define IWINDOW_H

#pragma once

namespace ZED
{
    class ZEDENGINE_API IWindow
    {
    public:
        virtual bool Init(const char* title, int width, int height) = 0;
        virtual void PollEvents() = 0;
        virtual void Shutdown() = 0;
        virtual bool IsRunning() const = 0;
        virtual ~IWindow() = default;
    };
}

#endif
