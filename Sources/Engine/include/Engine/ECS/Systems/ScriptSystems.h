/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SCRIPTSYSTEMS_H
#define SCRIPTSYSTEMS_H

#pragma once

#include "entt/entt.hpp"
#include "Engine/ECS/Components/ScriptComponent.h"

// Forward-declare to avoid heavy coupling
namespace ZED { class IScripting; }

namespace ZED
{
    struct ZEDENGINE_API ScriptLifecycleSystem
    {
        // Connect without passing a context pointer; handlers will query Scripting::Get()
        static void connect(entt::registry& r);

        // Must match EnTT signature: void(registry&, entity)
        static void onAdd   (entt::registry& r, entt::entity e);
        static void onRemove(entt::registry& r, entt::entity e);
    };

    struct ZEDENGINE_API ScriptUpdateSystem
    {
        // No context param; will query Scripting::Get()
        static void tick(entt::registry& r, double dt);
    };
}

#endif
