/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#pragma once

#include "entt/entt.hpp"
#include "Engine/Math/Math.h"
#include "Engine/ECS/Components/TransformComponent.h"
#include "Engine/ECS/Components/CameraComponent.h"
#include "Engine/Input/Input.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/Events/Event.h"

namespace ZED
{
    // Controls camera movement/rotation via input (WASD + mouse)
    // Only operates on cameras with editorMode == true
    struct ZEDENGINE_API CameraController
    {
        // Enable/disable the controller
        static void SetEnabled(bool enabled);
        static bool IsEnabled();

        // Update camera based on input (call once per frame)
        static void Update(entt::registry& r, double dt);

        // Configuration
        static void SetMoveSpeed(float speed);        // units per second
        static void SetMouseSensitivity(float sens);   // radians per pixel
        static void SetSpeedMultiplier(float mult);   // shift key multiplier

    private:
        static void OnMouseMove(const Event& e);

        static bool s_enabled;
        static float s_moveSpeed;
        static float s_mouseSensitivity;
        static float s_speedMultiplier;

        // Mouse delta accumulation (per frame)
        static float s_mouseDeltaX;
        static float s_mouseDeltaY;
        static int s_mouseSubId;
    };
}

#endif
