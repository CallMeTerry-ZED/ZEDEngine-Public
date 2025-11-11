/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#pragma once

#include "Engine/Math/Math.h"

namespace ZED
{
    // Basic TRS transform stored as position, Euler rotation (radians), and scale.
    // Compose() builds a left-handed, column-major matrix suitable for D3D/HLSL for now.
    struct TransformComponent
    {
        Vec3 position{ 0.0f, 0.0f, 0.0f };
        Vec3 rotation{ 0.0f, 0.0f, 0.0f }; // Euler XYZ in radians
        Vec3 scale   { 1.0f, 1.0f, 1.0f };

        // Compose TRS to Mat4: M = T * Rz * Ry * Rx * S
        // Order chosen for a conventional left-handed engine setup according to google.
        static Mat4 Compose(const TransformComponent& t)
        {
            Mat4 T = Translate(t.position);
            Mat4 Rx = glm::rotate(Mat4(1.0f), t.rotation.x, Vec3(1.0f, 0.0f, 0.0f));
            Mat4 Ry = glm::rotate(Mat4(1.0f), t.rotation.y, Vec3(0.0f, 1.0f, 0.0f));
            Mat4 Rz = glm::rotate(Mat4(1.0f), t.rotation.z, Vec3(0.0f, 0.0f, 1.0f));
            Mat4 S  = glm::scale (Mat4(1.0f), t.scale);

            // Column-major: last multiplied is applied first to a column vector.
            // Apply Scale, then Rx, then Ry, then Rz, then Translate.
            return T * Rz * Ry * Rx * S;
        }

        // Convenience instance method
        Mat4 ToMatrix() const
        {
            return Compose(*this);
        }
    };
}

#endif
