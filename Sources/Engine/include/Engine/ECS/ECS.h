/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef ECS_H
#define ECS_H

#pragma once

#include "entt/entt.hpp"

namespace ZED
{
    struct ECS
    {
        ZEDENGINE_API static entt::registry& Registry();
    };
}

#endif
