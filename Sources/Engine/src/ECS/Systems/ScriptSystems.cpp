/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/ECS/Systems/ScriptSystems.h"
#include "Engine/Interfaces/Scripting/IScripting.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/ECS/Components/ScriptComponent.h"

namespace ZED
{
    void ScriptLifecycleSystem::connect(entt::registry& r)
    {
        // Handlers must have signature: void(registry&, entity)
        r.on_construct<ScriptComponent>().connect<&ScriptLifecycleSystem::onAdd>();
        r.on_destroy  <ScriptComponent>().connect<&ScriptLifecycleSystem::onRemove>();
    }

    void ScriptLifecycleSystem::onAdd(entt::registry& r, entt::entity e)
    {
        IScripting* s = Scripting::Get();
        if (!s) return;

        auto& sc = r.get<ScriptComponent>(e);
        if (!sc.enabled) return;

        s->Start({sc.script}, static_cast<uint32_t>(e));
    }

    void ScriptLifecycleSystem::onRemove(entt::registry& r, entt::entity e)
    {
        IScripting* s = Scripting::Get();
        if (!s) return;

        auto& sc = r.get<ScriptComponent>(e);
        s->Stop({sc.script}, static_cast<uint32_t>(e));
    }

    void ScriptUpdateSystem::tick(entt::registry& r, double dt)
    {
        IScripting* s = Scripting::Get();
        if (!s) return;

        auto view = r.view<ScriptComponent>();
        for (auto ent : view)
        {
            auto& sc = view.get<ScriptComponent>(ent);
            if (sc.enabled)
                s->Update({sc.script}, static_cast<uint32_t>(ent), dt);
        }
    }
}