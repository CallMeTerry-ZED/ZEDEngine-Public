/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Script-Luau/LuauScripting.h"
#include "Engine/Scripting/Scripting.h"

extern "C"
{
    ZEDENGINE_API void RegisterScripting()
    {
        // no-op for now
    }

    // Factory
    ZEDENGINE_API ZED::IScripting* CreateScripting()
    {
        auto* impl = new ZED::LuauScripting();
        if (impl->Init())
        {
            ZED::Scripting::SetImplementation(impl);
            return impl;
        }
        delete impl;
        return nullptr;
    }
}
