/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Scripting/Scripting.h"
#include <iostream>

namespace ZED
{
    IScripting* Scripting::s_Impl = nullptr;

    void Scripting::SetImplementation(IScripting* impl)
    {
        s_Impl = impl;
    }

    IScripting* Scripting::Get()
    {
        if (!s_Impl)
        {
            std::cerr << "[ZED::Scripting] Warning: No scripting implementation linked!\n";
        }
        return s_Impl;
    }
}

