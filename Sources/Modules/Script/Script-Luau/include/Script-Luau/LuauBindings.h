/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef LUAUBINDINGS_H
#define LUAUBINDINGS_H

#pragma once

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
}

namespace ZED
{
    class LuauBindings
    {
    public:
        // Install all ZED engine bindings into a lua_State
        static void Install(lua_State* L);
    };
}

#endif
