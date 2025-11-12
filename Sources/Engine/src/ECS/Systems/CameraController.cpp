#include "Engine/ECS/Systems/CameraController.h"
#include <iostream>
#include <cmath>

namespace ZED
{
	bool CameraController::s_enabled = false;
	float CameraController::s_moveSpeed = 5.0f;
	float CameraController::s_mouseSensitivity = 0.002f;
	float CameraController::s_speedMultiplier = 2.0f;
	float CameraController::s_mouseDeltaX = 0.0f;
	float CameraController::s_mouseDeltaY = 0.0f;
	int CameraController::s_mouseSubId = 0;

	void CameraController::SetEnabled(bool enabled)
	{
		if (s_enabled == enabled) return;

		s_enabled = enabled;

		if (enabled && s_mouseSubId == 0)
		{
			// Subscribe to mouse move events
			s_mouseSubId = EventSystem::Get().Subscribe(EventType::MouseMove, OnMouseMove);
		}
		else if (!enabled && s_mouseSubId != 0)
		{
			// Unsubscribe
			EventSystem::Get().Unsubscribe(EventType::MouseMove, s_mouseSubId);
			s_mouseSubId = 0;
		}
	}

	bool CameraController::IsEnabled()
	{
		return s_enabled;
	}

	void CameraController::SetMoveSpeed(float speed)
	{
		s_moveSpeed = speed > 0.0f ? speed : 5.0f;
	}

	void CameraController::SetMouseSensitivity(float sens)
	{
		s_mouseSensitivity = sens > 0.0f ? sens : 0.002f;
	}

	void CameraController::SetSpeedMultiplier(float mult)
	{
		s_speedMultiplier = mult > 0.0f ? mult : 2.0f;
	}

	void CameraController::OnMouseMove(const Event& e)
	{
		// When relative mouse mode is enabled, e.c and e.d are already deltas
		// Accumulate them for this frame
		s_mouseDeltaX += static_cast<float>(e.c);
		s_mouseDeltaY += static_cast<float>(e.d);
	}

	void CameraController::Update(entt::registry& r, double dt)
	{
		if (!s_enabled) return;

		// Find primary camera with editorMode
		entt::entity camEnt = entt::null;
		TransformComponent* tr = nullptr;
		CameraComponent* cam = nullptr;

		auto view = r.view<CameraComponent, TransformComponent>();
		for (auto e : view)
		{
			auto& c = view.get<CameraComponent>(e);
			if (c.primary && c.editorMode)
			{
				camEnt = e;
				cam = &c;
				tr = &view.get<TransformComponent>(e);
				break;
			}
		}

		if (!tr || !cam)
		{
			// Reset mouse delta if no camera found
			s_mouseDeltaX = 0.0f;
			s_mouseDeltaY = 0.0f;
			return;
		}

		auto* input = Input::GetInput();
		if (!input)
		{
			s_mouseDeltaX = 0.0f;
			s_mouseDeltaY = 0.0f;
			return;
		}

		// Check if left mouse button is held (for editor mode rotation)
		bool leftMouseHeld = input->IsKeyDown(Key::MouseLeft);

		// Apply mouse rotation (only if left mouse held in editor mode)
		if (leftMouseHeld)
		{
			// Apply accumulated mouse delta
			tr->rotation.y += s_mouseDeltaX * s_mouseSensitivity; // Yaw
			tr->rotation.x -= s_mouseDeltaY * s_mouseSensitivity; // Pitch (inverted)

			// Clamp pitch to avoid gimbal lock
			const float maxPitch = 89.0f * 3.14159265f / 180.0f;
			if (tr->rotation.x > maxPitch) tr->rotation.x = maxPitch;
			if (tr->rotation.x < -maxPitch) tr->rotation.x = -maxPitch;
		}

		// Reset mouse delta for next frame (after using it)
		s_mouseDeltaX = 0.0f;
		s_mouseDeltaY = 0.0f;

		// Movement speed (with shift multiplier)
		float speed = s_moveSpeed;
		if (input->IsKeyDown(Key::LeftShift) || input->IsKeyDown(Key::RightShift))
		{
			speed *= s_speedMultiplier;
		}

		float moveDelta = static_cast<float>(dt) * speed;

		// Build forward/right/up vectors from camera rotation
		// Yaw (Y rotation) affects forward/right, Pitch (X rotation) affects forward/up
		float yaw = tr->rotation.y;
		float pitch = tr->rotation.x;

		Vec3 forward(
			std::sin(yaw) * std::cos(pitch),
			-std::sin(pitch),
			std::cos(yaw) * std::cos(pitch)
		);

		Vec3 right(
			std::cos(yaw),
			0.0f,
			-std::sin(yaw)
		);

		Vec3 up(0.0f, 1.0f, 0.0f); // World up

		// WASD movement
		Vec3 moveDir(0.0f, 0.0f, 0.0f);

		if (input->IsKeyDown(Key::W))
			moveDir += forward;
		if (input->IsKeyDown(Key::S))
			moveDir -= forward;
		if (input->IsKeyDown(Key::A))
			moveDir -= right;
		if (input->IsKeyDown(Key::D))
			moveDir += right;

		// Space/Ctrl for up/down (world space)
		if (input->IsKeyDown(Key::Space))
			moveDir += up;
		if (input->IsKeyDown(Key::LeftControl) || input->IsKeyDown(Key::RightControl))
			moveDir -= up;

		// Normalize and apply movement
		if (moveDir.x != 0.0f || moveDir.y != 0.0f || moveDir.z != 0.0f)
		{
			moveDir = glm::normalize(moveDir);
			tr->position += moveDir * moveDelta;
		}
	}
}