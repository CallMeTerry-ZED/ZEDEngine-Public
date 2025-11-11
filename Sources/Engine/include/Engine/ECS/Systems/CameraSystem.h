/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#pragma once

#include "entt/entt.hpp"
#include "Engine/Math/Math.h"
#include "Engine/ECS/Components/TransformComponent.h"
#include "Engine/ECS/Components/CameraComponent.h"

namespace ZED
{
    // Computes active camera view/proj each frame and reacts to window resize.
    struct ZEDENGINE_API CameraSystem
    {
        // Call once to subscribe to resize events
        static void Init();

        // Set the aspect ratio (e.g., on startup). Resize events will keep it updated.
        static void SetAspect(float aspect);

        // Compute and store view/proj from the active camera (primary or first available)
        static void Update(entt::registry& r);

        // Getters for the renderer
        static const Mat4& GetView();
        static const Mat4& GetProj();

    private:
        static void OnResize(int width, int height);
        static bool s_inited;
        static float s_aspect;
        static Mat4 s_view;
        static Mat4 s_proj;
    };
}

#endif
