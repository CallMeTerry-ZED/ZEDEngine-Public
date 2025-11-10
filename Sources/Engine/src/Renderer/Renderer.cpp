/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Renderer/Renderer.h"
#include <iostream>

namespace ZED
{
    IRenderer* Renderer::s_Impl = nullptr;

    void Renderer::SetImplementation(IRenderer* impl)
    {
        s_Impl = impl;
    }

    IRenderer* Renderer::Get()
    {
        if (!s_Impl)
        {
            std::cerr << "[ZED::Renderer] Warning: No renderer implementation linked!\n";
        }
        return s_Impl;
    }
}