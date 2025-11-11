/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

#pragma once

#include "entt/entt.hpp"
#include "Engine/Math/Math.h"
#include "Engine/ECS/Components/TransformComponent.h"
#include "Engine/ECS/Components/CameraComponent.h"

namespace ZED
{
    struct ZEDENGINE_API TransformSystem
    {
        // Apply incremental rotation (radians) to an entity
        static void Rotate(entt::registry& r, entt::entity e, const Vec3& delta)
        {
            if (!r.all_of<TransformComponent>(e)) return;
            auto& tr = r.get<TransformComponent>(e);
            tr.rotation += delta;
        }

        // Apply incremental translation to an entity
        static void Translate(entt::registry& r, entt::entity e, const Vec3& delta)
        {
            if (!r.all_of<TransformComponent>(e)) return;
            auto& tr = r.get<TransformComponent>(e);
            tr.position += delta;
        }

        // Setters (overwrite)
        static void SetPosition(entt::registry& r, entt::entity e, const Vec3& p)
        {
            if (!r.all_of<TransformComponent>(e)) return;
            r.get<TransformComponent>(e).position = p;
        }

        static void SetRotation(entt::registry& r, entt::entity e, const Vec3& rads)
        {
            if (!r.all_of<TransformComponent>(e)) return;
            r.get<TransformComponent>(e).rotation = rads;
        }

        static void SetScale(entt::registry& r, entt::entity e, const Vec3& s)
        {
            if (!r.all_of<TransformComponent>(e)) return;
            r.get<TransformComponent>(e).scale = s;
        }

        // Rotate all transforms by angularVelocity * dt, skipping primary cameras
        static void SpinAll(entt::registry& r, double dt, const Vec3& angularVelocity)
        {
            auto view = r.view<TransformComponent>();
            for (auto ent : view)
            {
                // Skip if this entity has a primary camera
                if (r.any_of<CameraComponent>(ent))
                {
                    const auto& cam = r.get<CameraComponent>(ent);
                    if (cam.primary)
                        continue;
                }

                auto& tr = view.get<TransformComponent>(ent);
                tr.rotation += angularVelocity * static_cast<float>(dt);
            }
        }
    };
}

#endif
