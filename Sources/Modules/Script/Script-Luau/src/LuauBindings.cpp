/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Script-Luau/LuauBindings.h"
#include "Script-Luau/LuauScripting.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/TransformComponent.h"
#include "Engine/ECS/Components/CameraComponent.h"
#include "Engine/ECS/Systems/TransformSystem.h"
#include "Engine/Input/Input.h"
#include "Engine/Time.h"
#include "Engine/Math/Math.h"
#include <cstdio>
#include <iostream>

namespace ZED
{
    static_assert(sizeof(lua_Integer) >= sizeof(std::underlying_type_t<entt::entity>), "lua_Integer too small for entt::entity underlying type");

    static bool validate_entity(lua_State* L, entt::entity e, entt::registry& reg, const char* ctx = "entity")
    {
        auto uid = static_cast<unsigned long long>(static_cast<std::underlying_type_t<entt::entity>>(e));
        //printf("[Luau-DBG] %s called with entt::entity underlying=%llu (ent value=%llu) registry_addr=%p\n",
               //ctx, uid, (unsigned long long)e, (void*)&reg);

        if (!reg.valid(e))
        {
            printf("[Luau] %s: invalid entity id %llu (reg.valid == false)\n", ctx, uid);
            luaL_error(L, "%s: invalid entity id %llu", ctx, uid);
            return false;
        }
        return true;
    }

    // Forward declarations for Lua C functions
    static int lua_GetTransform(lua_State* L);
    static int lua_SetTransform(lua_State* L);
    static int lua_GetCamera(lua_State* L);
    static int lua_SetCamera(lua_State* L);
    static int lua_IsKeyDown(lua_State* L);
    static int lua_GetDeltaTime(lua_State* L);
    static int lua_GetElapsedTime(lua_State* L);
    static int lua_TransformRotate(lua_State* L);
    static int lua_TransformTranslate(lua_State* L);
    static int lua_TransformScale(lua_State* L);
    static int lua_Vec3New(lua_State* L);
    static int lua_Vec3Index(lua_State* L);
    static int lua_Vec3NewIndex(lua_State* L);
    static int lua_Vec3Add(lua_State* L);
    static int lua_Vec3Sub(lua_State* L);
    static int lua_Vec3Mul(lua_State* L);
    static int lua_Vec3Div(lua_State* L);
    static int lua_Vec3ToString(lua_State* L);

    void LuauBindings::Install(lua_State* L)
    {
        // Create ZED global table
        lua_newtable(L); // [ZED]

        // --- ECS Bindings ---
        lua_pushstring(L, "GetTransform");
        lua_pushcfunction(L, lua_GetTransform, "GetTransform");
        lua_settable(L, -3); // ZED.GetTransform = function

        lua_pushstring(L, "SetTransform");
        lua_pushcfunction(L, lua_SetTransform, "SetTransform");
        lua_settable(L, -3); // ZED.SetTransform = function

        lua_pushstring(L, "GetCamera");
        lua_pushcfunction(L, lua_GetCamera, "GetCamera");
        lua_settable(L, -3);

        lua_pushstring(L, "SetCamera");
        lua_pushcfunction(L, lua_SetCamera, "SetCamera");
        lua_settable(L, -3);

        // --- Input Bindings ---
        lua_pushstring(L, "IsKeyDown");
        lua_pushcfunction(L, lua_IsKeyDown, "IsKeyDown");
        lua_settable(L, -3);

        // Key constants table
        lua_newtable(L); // [ZED, Key]
        lua_pushstring(L, "W"); lua_pushinteger(L, static_cast<int>(Key::W)); lua_settable(L, -3);
        lua_pushstring(L, "A"); lua_pushinteger(L, static_cast<int>(Key::A)); lua_settable(L, -3);
        lua_pushstring(L, "S"); lua_pushinteger(L, static_cast<int>(Key::S)); lua_settable(L, -3);
        lua_pushstring(L, "D"); lua_pushinteger(L, static_cast<int>(Key::D)); lua_settable(L, -3);
        lua_pushstring(L, "Space"); lua_pushinteger(L, static_cast<int>(Key::Space)); lua_settable(L, -3);
        lua_pushstring(L, "LeftControl"); lua_pushinteger(L, static_cast<int>(Key::LeftControl)); lua_settable(L, -3);
        lua_pushstring(L, "LeftShift"); lua_pushinteger(L, static_cast<int>(Key::LeftShift)); lua_settable(L, -3);
        lua_pushstring(L, "Escape"); lua_pushinteger(L, static_cast<int>(Key::Escape)); lua_settable(L, -3);
        lua_pushstring(L, "MouseLeft"); lua_pushinteger(L, static_cast<int>(Key::MouseLeft)); lua_settable(L, -3);
        lua_pushstring(L, "MouseRight"); lua_pushinteger(L, static_cast<int>(Key::MouseRight)); lua_settable(L, -3);
        lua_pushstring(L, "MouseMiddle"); lua_pushinteger(L, static_cast<int>(Key::MouseMiddle)); lua_settable(L, -3);
        // Add more keys as needed...
        lua_setfield(L, -2, "Key"); // ZED.Key = {...}

        // --- Time Bindings ---
        lua_pushstring(L, "GetDeltaTime");
        lua_pushcfunction(L, lua_GetDeltaTime, "GetDeltaTime");
        lua_settable(L, -3);

        lua_pushstring(L, "GetElapsedTime");
        lua_pushcfunction(L, lua_GetElapsedTime, "GetElapsedTime");
        lua_settable(L, -3);

        // --- Transform System Bindings ---
        lua_newtable(L); // [ZED, Transform]
        lua_pushstring(L, "Rotate");
        lua_pushcfunction(L, lua_TransformRotate, "Transform.Rotate");
        lua_settable(L, -3);
        lua_pushstring(L, "Translate");
        lua_pushcfunction(L, lua_TransformTranslate, "Transform.Translate");
        lua_settable(L, -3);
        lua_pushstring(L, "Scale");
        lua_pushcfunction(L, lua_TransformScale, "Transform.Scale");
        lua_settable(L, -3);
        lua_setfield(L, -2, "Transform"); // ZED.Transform = {...}

        // --- Math: Vec3 ---
        lua_newtable(L); // [ZED, Vec3]
        lua_pushstring(L, "new");
        lua_pushcfunction(L, lua_Vec3New, "Vec3.new");
        lua_settable(L, -3);
        lua_setfield(L, -2, "Vec3"); // ZED.Vec3 = {new = function}

        // Create Vec3 metatable
        luaL_newmetatable(L, "ZED.Vec3"); // [ZED, mt]
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, lua_Vec3Index, "Vec3.__index");
        lua_settable(L, -3);
        lua_pushstring(L, "__newindex");
        lua_pushcfunction(L, lua_Vec3NewIndex, "Vec3.__newindex");
        lua_settable(L, -3);
        lua_pushstring(L, "__add");
        lua_pushcfunction(L, lua_Vec3Add, "Vec3.__add");
        lua_settable(L, -3);
        lua_pushstring(L, "__sub");
        lua_pushcfunction(L, lua_Vec3Sub, "Vec3.__sub");
        lua_settable(L, -3);
        lua_pushstring(L, "__mul");
        lua_pushcfunction(L, lua_Vec3Mul, "Vec3.__mul");
        lua_settable(L, -3);
        lua_pushstring(L, "__div");
        lua_pushcfunction(L, lua_Vec3Div, "Vec3.__div");
        lua_settable(L, -3);
        lua_pushstring(L, "__tostring");
        lua_pushcfunction(L, lua_Vec3ToString, "Vec3.__tostring");
        lua_settable(L, -3);
        lua_pop(L, 1); // pop metatable

        // Set ZED as global
        lua_setglobal(L, "ZED"); // ZED = {...}
    }

    // --- ECS Functions ---
    static int lua_GetTransform(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "GetTransform")) return 0;

        if (!reg.all_of<TransformComponent>(ent))
        {
            lua_pushnil(L);
            return 1;
        }

        const auto& tr = reg.get<TransformComponent>(ent);

        lua_newtable(L); // result table

        // position
        lua_pushstring(L, "position");
        lua_newtable(L);
        lua_pushstring(L, "x"); lua_pushnumber(L, tr.position.x); lua_settable(L, -3);
        lua_pushstring(L, "y"); lua_pushnumber(L, tr.position.y); lua_settable(L, -3);
        lua_pushstring(L, "z"); lua_pushnumber(L, tr.position.z); lua_settable(L, -3);
        lua_settable(L, -3);

        // rotation
        lua_pushstring(L, "rotation");
        lua_newtable(L);
        lua_pushstring(L, "x"); lua_pushnumber(L, tr.rotation.x); lua_settable(L, -3);
        lua_pushstring(L, "y"); lua_pushnumber(L, tr.rotation.y); lua_settable(L, -3);
        lua_pushstring(L, "z"); lua_pushnumber(L, tr.rotation.z); lua_settable(L, -3);
        lua_settable(L, -3);

        // scale
        lua_pushstring(L, "scale");
        lua_newtable(L);
        lua_pushstring(L, "x"); lua_pushnumber(L, tr.scale.x); lua_settable(L, -3);
        lua_pushstring(L, "y"); lua_pushnumber(L, tr.scale.y); lua_settable(L, -3);
        lua_pushstring(L, "z"); lua_pushnumber(L, tr.scale.z); lua_settable(L, -3);
        lua_settable(L, -3);

        return 1;
    }

    static int lua_SetTransform(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "SetTransform")) return 0;

        // ensure component exists
        if (!reg.all_of<TransformComponent>(ent))
        {
            reg.emplace<TransformComponent>(ent);
        }

        auto& tr = reg.get<TransformComponent>(ent);

        // arg 2 = position (optional table)
        if (lua_istable(L, 2))
        {
            lua_getfield(L, 2, "x");
            if (lua_isnumber(L, -1)) tr.position.x = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 2, "y");
            if (lua_isnumber(L, -1)) tr.position.y = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 2, "z");
            if (lua_isnumber(L, -1)) tr.position.z = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
        }

        // arg 3 = rotation (optional table)
        if (lua_istable(L, 3))
        {
            lua_getfield(L, 3, "x");
            if (lua_isnumber(L, -1)) tr.rotation.x = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 3, "y");
            if (lua_isnumber(L, -1)) tr.rotation.y = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 3, "z");
            if (lua_isnumber(L, -1)) tr.rotation.z = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
        }

        // arg 4 = scale (optional table)
        if (lua_istable(L, 4))
        {
            lua_getfield(L, 4, "x");
            if (lua_isnumber(L, -1)) tr.scale.x = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 4, "y");
            if (lua_isnumber(L, -1)) tr.scale.y = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, 4, "z");
            if (lua_isnumber(L, -1)) tr.scale.z = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
        }

        return 0;
    }

    static int lua_GetCamera(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "GetCamera")) return 0;

        if (!reg.all_of<CameraComponent>(ent))
        {
            lua_pushnil(L);
            return 1;
        }

        const auto& cam = reg.get<CameraComponent>(ent);

        lua_newtable(L);
        lua_pushstring(L, "fovRadians"); lua_pushnumber(L, cam.fovRadians); lua_settable(L, -3);
        lua_pushstring(L, "znear");     lua_pushnumber(L, cam.znear);     lua_settable(L, -3);
        lua_pushstring(L, "zfar");      lua_pushnumber(L, cam.zfar);      lua_settable(L, -3);
        lua_pushstring(L, "aspect");    lua_pushnumber(L, cam.aspect);    lua_settable(L, -3);
        lua_pushstring(L, "primary");   lua_pushboolean(L, cam.primary);   lua_settable(L, -3);
        lua_pushstring(L, "editorMode");lua_pushboolean(L, cam.editorMode);lua_settable(L, -3);

        return 1;
    }

    static int lua_SetCamera(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "SetCamera")) return 0;

        if (!reg.all_of<CameraComponent>(ent))
        {
            reg.emplace<CameraComponent>(ent);
        }

        auto& cam = reg.get<CameraComponent>(ent);

        if (lua_istable(L, 2))
        {
            lua_getfield(L, 2, "fovRadians");
            if (lua_isnumber(L, -1)) cam.fovRadians = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);

            lua_getfield(L, 2, "znear");
            if (lua_isnumber(L, -1)) cam.znear = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);

            lua_getfield(L, 2, "zfar");
            if (lua_isnumber(L, -1)) cam.zfar = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);

            lua_getfield(L, 2, "aspect");
            if (lua_isnumber(L, -1)) cam.aspect = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);

            lua_getfield(L, 2, "primary");
            if (lua_isboolean(L, -1)) cam.primary = lua_toboolean(L, -1) != 0;
            lua_pop(L, 1);

            lua_getfield(L, 2, "editorMode");
            if (lua_isboolean(L, -1)) cam.editorMode = lua_toboolean(L, -1) != 0;
            lua_pop(L, 1);
        }

        return 0;
    }

    // --- Input Functions ---
    static int lua_IsKeyDown(lua_State* L)
    {
        int key = static_cast<int>(luaL_checkinteger(L, 1));
        auto* input = Input::GetInput();
        if (!input)
        {
            lua_pushboolean(L, 0);
            return 1;
        }
        bool down = input->IsKeyDown(static_cast<Key>(key));
        lua_pushboolean(L, down ? 1 : 0);
        return 1;
    }

    // --- Time Functions ---
    static int lua_GetDeltaTime(lua_State* L)
    {
        lua_pushnumber(L, Time::GetDeltaTime());
        return 1;
    }

    static int lua_GetElapsedTime(lua_State* L)
    {
        lua_pushnumber(L, Time::GetElapsedTime());
        return 1;
    }

    // --- Transform System Functions ---
    static int lua_TransformRotate(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        float x = static_cast<float>(luaL_checknumber(L, 2));
        float y = static_cast<float>(luaL_checknumber(L, 3));
        float z = static_cast<float>(luaL_checknumber(L, 4));

        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "Transform.Rotate")) return 0;
        if (!reg.all_of<TransformComponent>(ent))
        {
            auto eid = static_cast<unsigned long long>(static_cast<std::underlying_type_t<entt::entity>>(ent));
            printf("[Luau] Transform.Rotate: entity %llu has no TransformComponent\n", eid);
            luaL_error(L, "Transform.Rotate: entity %llu has no TransformComponent", eid);
            return -1;
        }

        try
        {
            TransformSystem::Rotate(reg, ent, Vec3(x, y, z));
        } catch (const std::exception& ex)
        {
            luaL_error(L, "Transform.Rotate exception: %s", ex.what());
            return -1;
        } catch (...)
        {
            luaL_error(L, "Transform.Rotate unknown exception");
            return -1;
        }

        return 0;
    }

    static int lua_TransformTranslate(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        float x = static_cast<float>(luaL_checknumber(L, 2));
        float y = static_cast<float>(luaL_checknumber(L, 3));
        float z = static_cast<float>(luaL_checknumber(L, 4));

        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "Transform.Translate")) return 0;
        if (!reg.all_of<TransformComponent>(ent))
        {
            auto eid = static_cast<unsigned long long>(static_cast<std::underlying_type_t<entt::entity>>(ent));
            printf("[Luau] Transform.Translate: entity %llu has no TransformComponent\n", eid);
            luaL_error(L, "Transform.Translate: entity %llu has no TransformComponent", eid);
            return -1;
        }

        try
        {
            TransformSystem::Translate(reg, ent, Vec3(x, y, z));
        } catch (const std::exception& ex)
        {
            luaL_error(L, "Transform.Translate exception: %s", ex.what());
            return -1;
        } catch (...)
        {
            luaL_error(L, "Transform.Translate unknown exception");
            return -1;
        }

        return 0;
    }

    static int lua_TransformScale(lua_State* L)
    {
        lua_Integer entId = luaL_checkinteger(L, 1);
        float x = static_cast<float>(luaL_checknumber(L, 2));
        float y = static_cast<float>(luaL_checknumber(L, 3));
        float z = static_cast<float>(luaL_checknumber(L, 4));

        entt::entity ent = static_cast<entt::entity>(static_cast<std::underlying_type_t<entt::entity>>(entId));
        auto& reg = ECS::ECS::Registry();

        if (!validate_entity(L, ent, reg, "Transform.Scale")) return 0;
        if (!reg.all_of<TransformComponent>(ent)) {
            auto eid = static_cast<unsigned long long>(static_cast<std::underlying_type_t<entt::entity>>(ent));
            printf("[Luau] Transform.Scale: entity %llu has no TransformComponent\n", eid);
            luaL_error(L, "Transform.Scale: entity %llu has no TransformComponent", eid);
            return -1;
        }

        try
        {
            TransformSystem::SetScale(reg, ent, Vec3(x, y, z));
        } catch (const std::exception& ex)
        {
            luaL_error(L, "Transform.Scale exception: %s", ex.what());
            return -1;
        } catch (...) {
            luaL_error(L, "Transform.Scale unknown exception");
            return -1;
        }

        return 0;
    }

    // --- Vec3 Functions ---
    static int lua_Vec3New(lua_State* L)
    {
        float x = static_cast<float>(luaL_optnumber(L, 1, 0.0));
        float y = static_cast<float>(luaL_optnumber(L, 2, 0.0));
        float z = static_cast<float>(luaL_optnumber(L, 3, 0.0));

        // Create userdata
        Vec3* v = static_cast<Vec3*>(lua_newuserdata(L, sizeof(Vec3)));
        *v = Vec3(x, y, z);
        luaL_getmetatable(L, "ZED.Vec3");
        lua_setmetatable(L, -2);
        return 1;
    }

    static Vec3* checkVec3(lua_State* L, int idx)
    {
        void* ud = luaL_checkudata(L, idx, "ZED.Vec3");
        if (!ud) return nullptr;
        return static_cast<Vec3*>(ud);
    }

    static int lua_Vec3Index(lua_State* L)
    {
        Vec3* v = checkVec3(L, 1);
        if (!v) return 0;
        const char* key = luaL_checkstring(L, 2);
        if (strcmp(key, "x") == 0) lua_pushnumber(L, v->x);
        else if (strcmp(key, "y") == 0) lua_pushnumber(L, v->y);
        else if (strcmp(key, "z") == 0) lua_pushnumber(L, v->z);
        else lua_pushnil(L);
        return 1;
    }

    static int lua_Vec3NewIndex(lua_State* L)
    {
        Vec3* v = checkVec3(L, 1);
        if (!v) return 0;
        const char* key = luaL_checkstring(L, 2);
        float val = static_cast<float>(luaL_checknumber(L, 3));
        if (strcmp(key, "x") == 0) v->x = val;
        else if (strcmp(key, "y") == 0) v->y = val;
        else if (strcmp(key, "z") == 0) v->z = val;
        return 0;
    }

    static int lua_Vec3Add(lua_State* L)
    {
        Vec3* a = checkVec3(L, 1);
        Vec3* b = checkVec3(L, 2);
        if (!a || !b) return 0;
        Vec3* result = static_cast<Vec3*>(lua_newuserdata(L, sizeof(Vec3)));
        *result = *a + *b;
        luaL_getmetatable(L, "ZED.Vec3");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int lua_Vec3Sub(lua_State* L)
    {
        Vec3* a = checkVec3(L, 1);
        Vec3* b = checkVec3(L, 2);
        if (!a || !b) return 0;
        Vec3* result = static_cast<Vec3*>(lua_newuserdata(L, sizeof(Vec3)));
        *result = *a - *b;
        luaL_getmetatable(L, "ZED.Vec3");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int lua_Vec3Mul(lua_State* L)
    {
        Vec3* v = checkVec3(L, 1);
        if (!v) return 0;
        if (lua_isnumber(L, 2))
        {
            float s = static_cast<float>(lua_tonumber(L, 2));
            Vec3* result = static_cast<Vec3*>(lua_newuserdata(L, sizeof(Vec3)));
            *result = *v * s;
            luaL_getmetatable(L, "ZED.Vec3");
            lua_setmetatable(L, -2);
            return 1;
        }
        return 0;
    }

    static int lua_Vec3Div(lua_State* L)
    {
        Vec3* v = checkVec3(L, 1);
        if (!v) return 0;
        float s = static_cast<float>(luaL_checknumber(L, 2));
        Vec3* result = static_cast<Vec3*>(lua_newuserdata(L, sizeof(Vec3)));
        *result = *v / s;
        luaL_getmetatable(L, "ZED.Vec3");
        lua_setmetatable(L, -2);
        return 1;
    }

    static int lua_Vec3ToString(lua_State* L)
    {
        Vec3* v = checkVec3(L, 1);
        if (!v) return 0;
        char buf[64];
        snprintf(buf, sizeof(buf), "Vec3(%.2f, %.2f, %.2f)", v->x, v->y, v->z);
        lua_pushstring(L, buf);
        return 1;
    }
}