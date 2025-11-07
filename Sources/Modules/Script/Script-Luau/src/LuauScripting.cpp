/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Script-Luau/LuauScripting.h"
#include <fstream>
#include <iostream>

// Well this is cool ting
// using namespace go brrrrr
using namespace ZED;

// ---------- helpers ----------
static std::vector<char> readFile(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    return std::vector<char>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

// ---------- IScripting ----------
bool LuauScripting::Init()
{
    // Nothing global required yet, We can just install shared libs/bindings here later
    return true;
}

void LuauScripting::Shutdown()
{
    for (auto& kv : instances)
    {
        auto& inst = kv.second;

        if (inst.hasDestroy)
        {
            lua_getref(inst.L, inst.tableRef);
            lua_getfield(inst.L, -1, kOnDestroy);
            lua_pushvalue(inst.L, -2); // self
            if (lua_pcall(inst.L, 1, 0, 0) != 0)
            {
                std::cerr << "[Luau] OnDestroy error during Shutdown: " << lua_tostring(inst.L, -1) << "\n";
                lua_pop(inst.L, 1); // pop error
            }
            lua_pop(inst.L, 1); // pop [self]
        }

        if (inst.tableRef != LUA_NOREF) {
            lua_unref(inst.L, inst.tableRef);
            inst.tableRef = LUA_NOREF;
        }

        if (inst.L) { lua_close(inst.L); inst.L = nullptr; }
    }
    instances.clear();
    scripts.clear();
}

ScriptId LuauScripting::LoadBytecodeFile(const std::string& path)
{
    ScriptId sid{ nextScriptId++ };
    ScriptDef def;
    def.path = path;
    std::error_code ec; // don’t throw on missing during dev
    def.lastWrite = std::filesystem::last_write_time(path, ec);

    // Don’t instantiate yet do it when an entity asks for Start()
    scripts.emplace(sid.value, std::move(def));
    return sid;
}

void LuauScripting::Start(ScriptId id, Entity e)
{
    auto it = scripts.find(id.value);
    if (it == scripts.end()) return;

    Instance inst;
    if (!loadIntoNewState(it->second, inst))
    {
        std::cerr << "[Luau] Failed to start script " << id.value << " for entity " << e << "\n";
        return;
    }

    // attach entity id: self.entity = <uint32>
    lua_getref(inst.L, inst.tableRef);                  // [self]
    lua_pushinteger(inst.L, static_cast<lua_Integer>(e));
    lua_setfield(inst.L, -2, "entity");
    lua_pop(inst.L, 1);

    // call OnStart(self) if present
    if (inst.hasStart)
    {
        lua_getref(inst.L, inst.tableRef);              // [self]
        lua_getfield(inst.L, -1, kOnStart);             // [self, fn]
        lua_pushvalue(inst.L, -2);                      // [self, fn, self]
        if (lua_pcall(inst.L, 1, 0, 0) != 0)
        {
            std::cerr << "[Luau] OnStart error: " << lua_tostring(inst.L, -1) << "\n";
            lua_pop(inst.L, 1); // pop error
        }
        lua_pop(inst.L, 1); // pop [self]
    }

    instances.emplace(Key(id, e), std::move(inst));
}

void LuauScripting::Stop(ScriptId id, Entity e)
{
    auto k = Key(id, e);
    auto it = instances.find(k);
    if (it == instances.end()) return;

    Instance& inst = it->second;

    if (inst.hasDestroy)
    {
        lua_getref(inst.L, inst.tableRef);
        lua_getfield(inst.L, -1, kOnDestroy);
        lua_pushvalue(inst.L, -2); // self
        if (lua_pcall(inst.L, 1, 0, 0) != 0)
        {
            std::cerr << "[Luau] OnDestroy error: " << lua_tostring(inst.L, -1) << "\n";
            lua_pop(inst.L, 1); // pop error
        }
        lua_pop(inst.L, 1); // pop [self]
    }

    // Unref before closing
    if (inst.tableRef != LUA_NOREF) {
        lua_unref(inst.L, inst.tableRef);
        inst.tableRef = LUA_NOREF;
    }

    if (inst.L) { lua_close(inst.L); inst.L = nullptr; }
    instances.erase(it);
}

void LuauScripting::Update(ScriptId id, Entity e, double dt)
{
    // Simple hot reload if bytecode mtime changed, rebuild just this instance
    if (hotReload)
    {
        auto defIt = scripts.find(id.value);
        if (defIt != scripts.end())
        {
            std::error_code ec;
            auto nowWrite = std::filesystem::last_write_time(defIt->second.path, ec);
            if (!ec && nowWrite != defIt->second.lastWrite)
            {
                // rebuild instance
                Stop(id, e);
                defIt->second.lastWrite = nowWrite;
                Start(id, e);
            }
        }
    }

    auto k = Key(id, e);
    auto it = instances.find(k);
    if (it == instances.end()) return;

    Instance& inst = it->second;
    if (!inst.hasUpdate) return;

    lua_getref(inst.L, inst.tableRef);
    lua_getfield(inst.L, -1, kOnUpdate);
    lua_pushvalue(inst.L, -2);      // self
    lua_pushnumber(inst.L, dt);
    if (lua_pcall(inst.L, 2, 0, 0) != 0)
    {
        std::cerr << "[Luau] OnUpdate error: " << lua_tostring(inst.L, -1) << "\n";
        lua_pop(inst.L, 1); // pop error
    }
    lua_pop(inst.L, 1); // pop [self]
}

void LuauScripting::PushEvent(int type, int a, int b, int c, int d)
{
    // Broadcast to all instances that have OnEvent
    for (auto& kv : instances)
    {
        Instance& inst = kv.second;
        if (!inst.hasEvent) continue;

        lua_getref(inst.L, inst.tableRef);
        lua_getfield(inst.L, -1, kOnEvent);
        lua_pushvalue(inst.L, -2);     // self
        lua_pushinteger(inst.L, type);
        lua_pushinteger(inst.L, a);
        lua_pushinteger(inst.L, b);
        lua_pushinteger(inst.L, c);
        lua_pushinteger(inst.L, d);
        if (lua_pcall(inst.L, 6, 0, 0) != 0)
        {
            std::cerr << "[Luau] OnEvent error: " << lua_tostring(inst.L, -1) << "\n";
            lua_pop(inst.L, 1); // pop error
        }
        lua_pop(inst.L, 1); // pop [self]
    }
}

// ---------- internals ----------
bool LuauScripting::loadIntoNewState(const ScriptDef& def, Instance& out)
{
    auto bc = readFile(def.path);
    if (bc.empty())
    {
        std::cerr << "[Luau] Bytecode file empty: " << def.path << "\n";
        return false;
    }

    // fresh state per instance
    out.L = luaL_newstate();
    luaL_openlibs(out.L); // TODO: replace with curated libs for sandboxing

    // Luau: load bytecode directly from a buffer (no reader thunk)
    if (luau_load(out.L, kChunkName, bc.data(), bc.size(), /*env*/0) != 0)
    {
        std::cerr << "[Luau] luau_load: " << lua_tostring(out.L, -1) << "\n";
        lua_pop(out.L, 1); // pop error
        return false;
    }

    // Run the chunk; convention: it returns a table (script "self" prototype)
    if (lua_pcall(out.L, 0, 1, 0) != 0)
    {
        std::cerr << "[Luau] chunk error: " << lua_tostring(out.L, -1) << "\n";
        lua_pop(out.L, 1); // pop error
        return false;
    }

    // Ensure we have a table on top; fabricate one if not
    if (!lua_istable(out.L, -1))
    {
        lua_pop(out.L, 1);     // pop non-table result
        lua_newtable(out.L);   // push fresh table
    }

    // Take a reference to the table at the top of the stack
    out.tableRef = lua_ref(out.L, -1);
    // lua_ref does NOT pop the value; clean up the stack yourself
    lua_pop(out.L, 1);

    detectHooks(out);
    return true;
}

void LuauScripting::detectHooks(Instance& inst)
{
    inst.hasStart = inst.hasUpdate = inst.hasDestroy = inst.hasEvent = false;

    lua_getref(inst.L, inst.tableRef); // [self]

    auto has = [&](const char* key)->bool
    {
        lua_getfield(inst.L, -1, key);
        bool ok = lua_isfunction(inst.L, -1);
        lua_pop(inst.L, 1);
        return ok;
    };

    inst.hasStart   = has(kOnStart);
    inst.hasUpdate  = has(kOnUpdate);
    inst.hasDestroy = has(kOnDestroy);
    inst.hasEvent   = has(kOnEvent);

    lua_pop(inst.L, 1); // pop [self]
}