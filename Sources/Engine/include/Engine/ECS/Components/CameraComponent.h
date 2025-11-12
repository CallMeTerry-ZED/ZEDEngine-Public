/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#pragma once

#include "Engine/Math/Math.h"

namespace ZED
{
    enum class CameraProjection
    {
        Perspective,
        // I'll add ortho later
    };

    struct CameraComponent
    {
        // Lens
        float fovRadians = 60.0f * 3.14159265f / 180.0f;
        float znear      = 0.1f;
        float zfar       = 1000.0f;

        // Projection type and cached aspect
        CameraProjection projection = CameraProjection::Perspective;
        float aspect = 16.0f / 9.0f;

        // Which camera should be used by default
        bool primary = false;

        // Editor mode toggle
        bool editorMode = false;
    };
}

#endif
