/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H

#pragma once

#include <cstdint>

namespace ZED
{
    // Minimal placeholder so entities can “opt-in” to scripting later
    // "script" will be the ScriptId.value returned by the scripting module
    struct ScriptComponent
    {
        uint64_t script = 0;
        bool     enabled = true;
    };
}

#endif
