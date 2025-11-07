/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef ECS_H
#define ECS_H

#pragma once

#include "entt/entt.hpp"

namespace ZED
{
    // Central registry inside the engine
    struct ECS
    {
        static entt::registry& Registry()
        {
            static entt::registry r;
            return r;
        }
    };
}

#endif
