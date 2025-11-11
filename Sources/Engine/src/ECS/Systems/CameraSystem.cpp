/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/ECS/Systems/CameraSystem.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Events/Event.h"

namespace ZED
{
	bool CameraSystem::s_inited = false;
	float CameraSystem::s_aspect = 16.0f / 9.0f;
	Mat4 CameraSystem::s_view = Mat4(1.0f);
	Mat4 CameraSystem::s_proj = Mat4(1.0f);

	void CameraSystem::Init()
	{
		if (s_inited) return;
		s_inited = true;

		// Subscribe to window resize; update aspect
		EventSystem::Get().Subscribe(EventType::WindowResized, [](const Event& e)
		{
			OnResize(e.a, e.b);
		});
	}

	void CameraSystem::SetAspect(float aspect)
	{
		if (aspect > 0.0f)
			s_aspect = aspect;
	}

	void CameraSystem::Update(entt::registry& r)
	{
		// Find active camera (primary first, else any)
		entt::entity active = entt::null;
		CameraComponent* cam = nullptr;
		TransformComponent* tr  = nullptr;

		// Try primary
		{
			auto view = r.view<CameraComponent, TransformComponent>();
			for (auto e : view)
			{
				auto& c = view.get<CameraComponent>(e);
				if (c.primary)
				{
					active = e;
					cam = &view.get<CameraComponent>(e);
					tr  = &view.get<TransformComponent>(e);
					break;
				}
			}
		}

		// If none marked primary, pick the first
		if (active == entt::null)
		{
			auto view = r.view<CameraComponent, TransformComponent>();
			for (auto e : view)
			{
				active = e;
				cam = &view.get<CameraComponent>(e);
				tr  = &view.get<TransformComponent>(e);
				break;
			}
		}

		// If still none, use identity
		if (!cam || !tr)
		{
			s_view = Mat4(1.0f);
			s_proj = Mat4(1.0f);
			return;
		}

		// Update component aspect if different
		if (cam->aspect != s_aspect)
			cam->aspect = s_aspect;

		// Build View from transform (LH): inverse of T*Rz*Ry*Rx*S
		Mat4 model = tr->ToMatrix();
		s_view = glm::inverse(model);

		// Build Proj
		if (cam->projection == CameraProjection::Perspective)
		{
			s_proj = PerspectiveLH_ZO(cam->fovRadians, cam->aspect, cam->znear, cam->zfar);
		}
		// Ortho could go here later
	}

	const Mat4& CameraSystem::GetView() { return s_view; }
	const Mat4& CameraSystem::GetProj() { return s_proj; }

	void CameraSystem::OnResize(int width, int height)
	{
		if (width > 0 && height > 0)
		{
			s_aspect = static_cast<float>(width) / static_cast<float>(height);
		}
	}
}