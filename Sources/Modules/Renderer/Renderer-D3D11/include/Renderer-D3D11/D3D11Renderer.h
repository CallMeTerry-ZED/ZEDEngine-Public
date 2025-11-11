/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef D3D11RENDERER_H
#define D3D11RENDERER_H

#pragma once

#include "Engine/Interfaces/Renderer/IRenderer.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Events/Event.h"
#include "Engine/Math/Math.h"

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <cstdint>

namespace ZED
{
    class ZEDENGINE_API D3D11Renderer : public IRenderer
    {
    public:
        D3D11Renderer();
        ~D3D11Renderer() override;

        bool Init(void* nativeHandle, int width, int height) override;
        void Resize(int width, int height) override;

        void BeginFrame(float r, float g, float b, float a) override;
        void DrawTestCube(float timeSeconds) override;
        void EndFrame() override;

        void Shutdown() override;

    private:
        bool CreateDeviceAndSwapChain(HWND hwnd, int width, int height);
        bool CreateBackbufferTargets(int width, int height);
        void ReleaseBackbufferTargets();
        bool CreatePipeline();
        bool CreateCubeGeometry();

        // Shader compile helper (instance method)
        bool CompileShader(const char* source, const char* entry, const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& outBlob);

        // Resize subscription
        int m_resizeSubId = 0;

        // Cached size
        int m_width = 0;
        int m_height = 0;

        // D3D11 objects
        Microsoft::WRL::ComPtr<ID3D11Device> m_device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depth;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;

        // Pipeline state
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbuffer;     // MVP
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rs;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dss;

        // Geometry
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_ib;
        UINT m_indexCount = 0;
    };
}

#endif
