/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef ISCRIPTING_H
#define ISCRIPTING_H

#pragma once

#include <string>
#include <cstdint>

namespace ZED
{
    using Entity = uint32_t; // POD-friendly across DLL boundary
    struct ScriptId { uint64_t value = 0; };

    class ZEDENGINE_API IScripting
    {
    public:
        virtual bool Init() = 0;
        virtual void Shutdown() = 0;

        virtual ScriptId LoadBytecodeFile(const std::string& path) = 0;

        // Entity-aware lifecycle hooks
        // TODO: Luau/C#/etc will/should implement these
        virtual void Start (ScriptId id, Entity e) = 0;
        virtual void Stop  (ScriptId id, Entity e) = 0;
        virtual void Update(ScriptId id, Entity e, double dt) = 0;

        // Event fan-out to scripts
        virtual void PushEvent(int type, int a=0, int b=0, int c=0, int d=0) = 0;

        virtual void EnableHotReload(bool enabled) = 0;
        virtual ~IScripting() = default;
    };
}

#endif
