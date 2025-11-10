/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Renderer-D3D11/D3D11Renderer.h"
#include <cmath>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

namespace ZED
{
	struct VSConstants
	{
		float mvp[16];
	};

	static const char* kVS = R"(
cbuffer VSConstants : register(b0)
{
	float4x4 u_MVP;
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
	o.pos = mul(u_MVP, float4(input.pos, 1.0f));
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

		if (!CreateBackbufferTargets(width, height))
			return;
	}

	void D3D11Renderer::BeginFrame(float r, float g, float b, float a)
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
	}

	void D3D11Renderer::DrawTestCube(float timeSeconds)
	{
		if (!m_context) return;

		// Build MVP: perspective * rotation * translation
		const float aspect = (m_height > 0) ? (float)m_width / (float)m_height : 1.0f;
	    float4x4 P = MakePerspective(60.0f * 3.14159265f / 180.0f, aspect, 0.1f, 100.0f);
	    float4x4 R = MakeRotationY(timeSeconds * 1.0f);
	    float4x4 T = MakeTranslation(0.0f, 0.0f, 3.0f);
	    float4x4 M = Mul(T, R);          // model = translate then rotate (inno if we should switch to rotate then translate ngl)
	    float4x4 MVP = Mul(P, M);        // projection * model

		// Upload constants
		D3D11_MAPPED_SUBRESOURCE mapped{};
		if (SUCCEEDED(m_context->Map(m_cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		{
			VSConstants* c = reinterpret_cast<VSConstants*>(mapped.pData);
			for (int i = 0; i < 16; ++i) c->mvp[i] = MVP.m[i];
			m_context->Unmap(m_cbuffer.Get(), 0);
		}

		// Bind pipeline
		UINT stride = sizeof(float) * 6;
		UINT offset = 0;
		m_context->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &stride, &offset);
		m_context->IASetIndexBuffer(m_ib.Get(), DXGI_FORMAT_R32_UINT, 0);
		m_context->IASetInputLayout(m_layout.Get());
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_context->VSSetShader(m_vs.Get(), nullptr, 0);
		m_context->VSSetConstantBuffers(0, 1, m_cbuffer.GetAddressOf());
		m_context->PSSetShader(m_ps.Get(), nullptr, 0);

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
		m_cbuffer.Reset();
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
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // simplest, broad support

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

		// Basic depth state
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

		// Basic rasterizer
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

		D3D11_BUFFER_DESC cbd{};
		cbd.ByteWidth = sizeof(VSConstants);
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_device->CreateBuffer(&cbd, nullptr, m_cbuffer.GetAddressOf())))
			return false;

		return true;
	}

	bool D3D11Renderer::CreateCubeGeometry()
	{
		// 8 vertices, colored
		const float v[] =
		{
			// x, y, z,				r, g, b
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
			// front (-Z)
			0,1,2,  0,2,3,
			// back (+Z)
			4,6,5,  4,7,6,
			// left (-X)
			4,5,1,  4,1,0,
			// right (+X)
			3,2,6,  3,6,7,
			// top (+Y)
			1,5,6,  1,6,2,
			// bottom (-Y)
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

	// Minimal column-major matrices (HLSL default)
	D3D11Renderer::float4x4 D3D11Renderer::Mul(const float4x4& a, const float4x4& b)
	{
		float4x4 r{};
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				float sum = 0.0f;
				for (int k = 0; k < 4; ++k)
					sum += a.m[k * 4 + row] * b.m[col * 4 + k];
				r.m[col * 4 + row] = sum;
			}
		}
		return r;
	}

	D3D11Renderer::float4x4 D3D11Renderer::MakePerspective(float fovy, float aspect, float znear, float zfar)
	{
		float f = 1.0f / std::tan(fovy * 0.5f);
		float4x4 m{};
		m.m[0] = f / aspect;
		m.m[5] = f;
		m.m[10] = zfar / (zfar - znear);
		m.m[11] = 1.0f;
		m.m[14] = (-znear * zfar) / (zfar - znear);
		return m;
	}

	D3D11Renderer::float4x4 D3D11Renderer::MakeRotationY(float a)
	{
		float c = std::cos(a), s = std::sin(a);
		float4x4 m{};
		m.m[0] = c;	m.m[2] = -s;
		m.m[5] = 1;
		m.m[8] = s;	m.m[10] = c;
		m.m[15] = 1;
		return m;
	}

	D3D11Renderer::float4x4 D3D11Renderer::MakeTranslation(float x, float y, float z)
	{
		float4x4 m{};
		m.m[0] = 1; m.m[5] = 1; m.m[10] = 1; m.m[15] = 1;
		m.m[12] = x; m.m[13] = y; m.m[14] = z;
		return m;
	}
}