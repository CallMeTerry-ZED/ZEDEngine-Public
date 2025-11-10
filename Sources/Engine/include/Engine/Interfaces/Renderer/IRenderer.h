/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef IRENDERER_H
#define IRENDERER_H

#pragma once

#include <cstdint>

namespace ZED
{
    class ZEDENGINE_API IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        // nativeHandle is an OS window handle (HWND on Windows)
        virtual bool Init(void* nativeHandle, int width, int height) = 0;

        // Called on window resize
        virtual void Resize(int width, int height) = 0;

        // Begin a frame with a clear color
        virtual void BeginFrame(float r, float g, float b, float a) = 0;

        // Simple demo draw: spinning cube
        virtual void DrawTestCube(float timeSeconds) = 0;

        // Present the swap chain
        virtual void EndFrame() = 0;

        // Release graphics resources
        virtual void Shutdown() = 0;
    };
}

#endif
