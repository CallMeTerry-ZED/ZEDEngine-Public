/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef MATH_H
#define MATH_H

#pragma once

// Centralized math header for the engine.
// Uses GLM with DirectX-friendly conventions (left-handed, depth 0..1).
// All modules should include this instead of including GLM directly.

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ZED
{
    // Aliases
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vec4 = glm::vec4;
    using Mat4 = glm::mat4;

    // Re-export common helpers to keep call sites consistent
    inline Mat4 PerspectiveLH_ZO(float fovRadians, float aspect, float znear, float zfar)
    {
        return glm::perspectiveLH_ZO(fovRadians, aspect, znear, zfar);
    }

    inline Mat4 Translate(const Vec3& v)
    {
        return glm::translate(Mat4(1.0f), v);
    }

    inline Mat4 RotateY(float radians)
    {
        return glm::rotate(Mat4(1.0f), radians, Vec3(1e-7f, 1.0f, 1e-7f)); // stable axis
    }

    inline const float* ValuePtr(const Mat4& m)
    {
        return glm::value_ptr(m);
    }
}

#endif
