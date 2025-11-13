/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/ECS/ECS.h"

namespace ZED
{
    ZEDENGINE_API entt::registry& ECS::Registry()
    {
        static entt::registry r;
        return r;
    }
}