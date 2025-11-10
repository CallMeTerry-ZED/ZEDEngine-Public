/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Renderer-D3D11/D3D11Renderer.h"
#include "Engine/Renderer/Renderer.h"

extern "C"
{
    ZEDENGINE_API ZED::IRenderer* CreateRenderer()
    {
        auto* impl = new ZED::D3D11Renderer();
        ZED::Renderer::SetImplementation(impl);
        return impl;
    }
}