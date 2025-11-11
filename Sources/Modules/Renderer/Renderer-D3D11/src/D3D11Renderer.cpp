/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Renderer-D3D11/D3D11Renderer.h"
#include <cstring>
#include <cmath>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

namespace ZED
{
	// Shaders: frame (view/proj) + object (model); LH, ZO depth
	static const char* kVS = R"(
cbuffer CBFrame  : register(b0)
{
	float4x4 u_View;
	float4x4 u_Proj;
};
cbuffer CBObject : register(b1)
{
	float4x4 u_Model;
};

struct VSIn
{
	float3 pos : POSITION;
	float3 col : COLOR;
};

struct VSOut
{
	float4 pos : SV_Position;
	float3 col : COLOR;
};

VSOut main(VSIn input)
{
	VSOut o;
	float4 wpos = mul(u_Model, float4(input.pos, 1.0f));
	float4 vpos = mul(u_View,  wpos);
	o.pos = mul(u_Proj, vpos);
	o.col = input.col;
	return o;
}
)";

	static const char* kPS = R"(
struct PSIn
{
	float4 pos : SV_Position;
	float3 col : COLOR;
};

float4 main(PSIn input) : SV_Target
{
	return float4(input.col, 1.0f);
}
)";

	D3D11Renderer::D3D11Renderer() = default;
	D3D11Renderer::~D3D11Renderer() = default;

	bool D3D11Renderer::Init(void* nativeHandle, int width, int height)
	{
		if (!nativeHandle || width <= 0 || height <= 0)
			return false;

		HWND hwnd = reinterpret_cast<HWND>(nativeHandle);
		m_width = width;
		m_height = height;

		if (!CreateDeviceAndSwapChain(hwnd, width, height))
			return false;

		if (!CreateBackbufferTargets(width, height))
			return false;

		if (!CreatePipeline())
			return false;

		if (!CreateCubeGeometry())
			return false;

		// Subscribe to resize events
		m_resizeSubId = EventSystem::Get().Subscribe(EventType::WindowResized, [this](const Event& e)
		{
			if (e.a > 0 && e.b > 0)
			{
				this->Resize(e.a, e.b);
			}
		});

		return true;
	}

	void D3D11Renderer::Resize(int width, int height)
	{
		if (!m_swapChain) return;
		if (width <= 0 || height <= 0) return;

		m_width = width;
		m_height = height;

		ReleaseBackbufferTargets();

		HRESULT hr = m_swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] ResizeBuffers failed: 0x" << std::hex << hr << std::dec << "\n";
			return;
		}

		CreateBackbufferTargets(width, height);
	}

	void D3D11Renderer::BeginFrame(float r, float g, float b, float a, const ZED::Mat4& view, const ZED::Mat4& proj)
	{
		if (!m_context || !m_rtv || !m_dsv) return;

		const float clear[4] = { r, g, b, a };
		m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
		m_context->ClearRenderTargetView(m_rtv.Get(), clear);
		m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		D3D11_VIEWPORT vp{};
		vp.Width = (float)m_width;
		vp.Height = (float)m_height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		m_context->RSSetViewports(1, &vp);

		// Upload frame constants
		if (m_cbFrame)
		{
			D3D11_MAPPED_SUBRESOURCE mapped{};
			if (SUCCEEDED(m_context->Map(m_cbFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				auto* dst = reinterpret_cast<CBFrame*>(mapped.pData);
				std::memcpy(dst->view,  ZED::ValuePtr(view), sizeof(float) * 16);
				std::memcpy(dst->proj,  ZED::ValuePtr(proj), sizeof(float) * 16);
				m_context->Unmap(m_cbFrame.Get(), 0);
			}
		}

		// Bind pipeline static bits
		m_context->IASetInputLayout(m_layout.Get());
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->VSSetShader(m_vs.Get(), nullptr, 0);
		m_context->PSSetShader(m_ps.Get(), nullptr, 0);
		m_context->VSSetConstantBuffers(0, 1, m_cbFrame.GetAddressOf()); // b0
	}

	void D3D11Renderer::DrawCube(const ZED::Mat4& model)
	{
		if (!m_context) return;

		// Upload object constants
		if (m_cbObject)
		{
			D3D11_MAPPED_SUBRESOURCE mapped{};
			if (SUCCEEDED(m_context->Map(m_cbObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				auto* dst = reinterpret_cast<CBObject*>(mapped.pData);
				std::memcpy(dst->model, ZED::ValuePtr(model), sizeof(float) * 16);
				m_context->Unmap(m_cbObject.Get(), 0);
			}
		}

		// Bind geometry
		UINT stride = sizeof(float) * 6;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &stride, &offset);
		m_context->IASetIndexBuffer(m_ib.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Bind object constants at b1
		m_context->VSSetConstantBuffers(1, 1, m_cbObject.GetAddressOf());

		// Draw
		m_context->DrawIndexed(m_indexCount, 0, 0);
	}

	void D3D11Renderer::EndFrame()
	{
		if (m_swapChain)
		{
			m_swapChain->Present(1, 0);
		}
	}

	void D3D11Renderer::Shutdown()
	{
		if (m_resizeSubId != 0)
		{
			EventSystem::Get().Unsubscribe(EventType::WindowResized, m_resizeSubId);
			m_resizeSubId = 0;
		}

		ReleaseBackbufferTargets();

		m_vb.Reset();
		m_ib.Reset();
		m_layout.Reset();
		m_vs.Reset();
		m_ps.Reset();
		m_cbFrame.Reset();
		m_cbObject.Reset();
		m_rs.Reset();
		m_dss.Reset();

		m_swapChain.Reset();
		m_context.Reset();
		m_device.Reset();
	}

	bool D3D11Renderer::CreateDeviceAndSwapChain(HWND hwnd, int width, int height)
	{
		DXGI_SWAP_CHAIN_DESC scd{};
		scd.BufferCount = 2;
		scd.BufferDesc.Width = (UINT)width;
		scd.BufferDesc.Height = (UINT)height;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = hwnd;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#if defined(_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		D3D_FEATURE_LEVEL levels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_0
		};
		D3D_FEATURE_LEVEL created{};

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			levels,
			(UINT)(sizeof(levels) / sizeof(levels[0])),
			D3D11_SDK_VERSION,
			&scd,
			m_swapChain.GetAddressOf(),
			m_device.GetAddressOf(),
			&created,
			m_context.GetAddressOf());

		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] D3D11CreateDeviceAndSwapChain failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}
		return true;
	}

	bool D3D11Renderer::CreateBackbufferTargets(int width, int height)
	{
		ComPtr<ID3D11Texture2D> backbuffer;
		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backbuffer.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] GetBuffer failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		hr = m_device->CreateRenderTargetView(backbuffer.Get(), nullptr, m_rtv.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] CreateRenderTargetView failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		D3D11_TEXTURE2D_DESC dsd{};
		dsd.Width = (UINT)width;
		dsd.Height = (UINT)height;
		dsd.MipLevels = 1;
		dsd.ArraySize = 1;
		dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsd.SampleDesc.Count = 1;
		dsd.Usage = D3D11_USAGE_DEFAULT;
		dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = m_device->CreateTexture2D(&dsd, nullptr, m_depth.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] CreateTexture2D(depth) failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		hr = m_device->CreateDepthStencilView(m_depth.Get(), nullptr, m_dsv.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] CreateDepthStencilView failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		// Depth/raster states
		D3D11_DEPTH_STENCIL_DESC dss{};
		dss.DepthEnable = TRUE;
		dss.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dss.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = m_device->CreateDepthStencilState(&dss, m_dss.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] CreateDepthStencilState failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		D3D11_RASTERIZER_DESC rs{};
		rs.FillMode = D3D11_FILL_SOLID;
		rs.CullMode = D3D11_CULL_BACK;
		rs.DepthClipEnable = TRUE;
		hr = m_device->CreateRasterizerState(&rs, m_rs.GetAddressOf());
		if (FAILED(hr))
		{
			std::cerr << "[D3D11Renderer] CreateRasterizerState failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}

		m_context->OMSetDepthStencilState(m_dss.Get(), 0);
		m_context->RSSetState(m_rs.Get());
		return true;
	}

	void D3D11Renderer::ReleaseBackbufferTargets()
	{
		m_dsv.Reset();
		m_depth.Reset();
		m_rtv.Reset();
	}

	bool D3D11Renderer::CreatePipeline()
	{
		ComPtr<ID3DBlob> vsb;
		ComPtr<ID3DBlob> psb;

		if (!CompileShader(kVS, "main", "vs_5_0", vsb))
			return false;
		if (!CompileShader(kPS, "main", "ps_5_0", psb))
			return false;

		if (FAILED(m_device->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, m_vs.GetAddressOf())))
			return false;
		if (FAILED(m_device->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, m_ps.GetAddressOf())))
			return false;

		D3D11_INPUT_ELEMENT_DESC il[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float)*3,					D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (FAILED(m_device->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), m_layout.GetAddressOf())))
			return false;

		// Constant buffers
		{
			D3D11_BUFFER_DESC bd{};
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			bd.ByteWidth = sizeof(CBFrame);
			if (FAILED(m_device->CreateBuffer(&bd, nullptr, m_cbFrame.GetAddressOf())))
				return false;

			bd.ByteWidth = sizeof(CBObject);
			if (FAILED(m_device->CreateBuffer(&bd, nullptr, m_cbObject.GetAddressOf())))
				return false;
		}

		return true;
	}

	bool D3D11Renderer::CreateCubeGeometry()
	{
		const float v[] =
		{
			-1, -1, -1,				1, 0, 0,
			-1,  1, -1,				0, 1, 0,
			 1,  1, -1,				0, 0, 1,
			 1, -1, -1,				1, 1, 0,
			-1, -1,  1,				1, 0, 1,
			-1,  1,  1,				0, 1, 1,
			 1,  1,  1,				1, 1, 1,
			 1, -1,  1,				0, 0, 0,
		};

		const uint32_t i[] =
		{
			0,1,2,  0,2,3,
			4,6,5,  4,7,6,
			4,5,1,  4,1,0,
			3,2,6,  3,6,7,
			1,5,6,  1,6,2,
			4,0,3,  4,3,7
		};
		m_indexCount = (UINT)(sizeof(i) / sizeof(i[0]));

		D3D11_BUFFER_DESC vbd{};
		vbd.ByteWidth = (UINT)sizeof(v);
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vinit{ v, 0, 0 };
		if (FAILED(m_device->CreateBuffer(&vbd, &vinit, m_vb.GetAddressOf())))
			return false;

		D3D11_BUFFER_DESC ibd{};
		ibd.ByteWidth = (UINT)sizeof(i);
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA iinit{ i, 0, 0 };
		if (FAILED(m_device->CreateBuffer(&ibd, &iinit, m_ib.GetAddressOf())))
			return false;

		return true;
	}

	bool D3D11Renderer::CompileShader(const char* source, const char* entry, const char* target, ComPtr<ID3DBlob>& outBlob)
	{
		ComPtr<ID3DBlob> errs;
		HRESULT hr = D3DCompile(
			source, strlen(source),
			nullptr, nullptr, nullptr,
			entry, target,
			0, 0, outBlob.GetAddressOf(), errs.GetAddressOf());

		if (FAILED(hr))
		{
			if (errs) std::cerr << "[D3D11Renderer] Shader compile error: " << (const char*)errs->GetBufferPointer() << "\n";
			else std::cerr << "[D3D11Renderer] Shader compile failed: 0x" << std::hex << hr << std::dec << "\n";
			return false;
		}
		return true;
	}
}