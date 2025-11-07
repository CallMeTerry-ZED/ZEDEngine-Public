/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef SCRIPTING_H
#define SCRIPTING_H

#pragma once

#include "Engine/Interfaces/Scripting/IScripting.h"

namespace ZED
{
    /**
     * Thin accessor for the active scripting implementation (module).
     */
    class ZEDENGINE_API Scripting
    {
    public:
        // Set the active scripting implementation
        static void SetImplementation(IScripting* impl);

        // Get the active scripting implementation
        static IScripting* Get();

    private:
        static IScripting* s_Impl;
    };
}


#endif
