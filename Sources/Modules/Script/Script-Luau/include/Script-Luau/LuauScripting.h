/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef LUAUSCRIPTING_H
#define LUAUSCRIPTING_H

#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <cstdint>
extern "C"
{
#include "lua.h"
#include "luacode.h"
#include "lualib.h"
}

#ifndef LUA_NOREF
#define LUA_NOREF (-2)
#endif

#include "Engine/Interfaces/Scripting/IScripting.h"

namespace ZED
{
    class LuauScripting final : public IScripting
    {
    public:
        bool Init() override;
        void Shutdown() override;

        ScriptId LoadBytecodeFile(const std::string& path) override;

        void Start (ScriptId id, Entity e) override;
        void Stop  (ScriptId id, Entity e) override;
        void Update(ScriptId id, Entity e, double dt) override;

        void PushEvent(int type, int a=0, int b=0, int c=0, int d=0) override;

        void EnableHotReload(bool enabled) override { hotReload = enabled; }

    private:
        struct ScriptDef
        {
            std::string path;
            std::filesystem::file_time_type lastWrite{};
        };

        struct Instance
        {
            lua_State* L = nullptr;   // isolated state per (script, entity)
            int tableRef  = LUA_NOREF;
            bool hasStart = false, hasUpdate = false, hasDestroy = false, hasEvent = false;
        };

        // key = (scriptId << 32) | entity
        static uint64_t Key(ScriptId id, Entity e)
        {
            return (id.value << 32ull) | static_cast<uint64_t>(e);
        }

        bool loadIntoNewState(const ScriptDef& def, Instance& out);
        void detectHooks(Instance& inst);

        // storage
        uint64_t nextScriptId = 1;
        std::unordered_map<uint64_t, ScriptDef> scripts;            // by ScriptId
        std::unordered_map<uint64_t, Instance>  instances;          // by (ScriptId, Entity)

        bool hotReload = true;

        // constants
        static constexpr const char* kChunkName   = "ZED/LuauChunk";
        static constexpr const char* kOnStart     = "OnStart";
        static constexpr const char* kOnUpdate    = "OnUpdate";
        static constexpr const char* kOnDestroy   = "OnDestroy";
        static constexpr const char* kOnEvent     = "OnEvent";
    };
}

#endif
