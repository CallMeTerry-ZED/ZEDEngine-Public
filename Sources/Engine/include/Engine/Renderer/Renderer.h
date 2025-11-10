/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include "Engine/Interfaces/Renderer/IRenderer.h"

namespace ZED
{
    class ZEDENGINE_API Renderer
    {
    public:
        static void SetImplementation(IRenderer* impl);
        static IRenderer* Get();

    private:
        static IRenderer* s_Impl;
    };
}

#endif
