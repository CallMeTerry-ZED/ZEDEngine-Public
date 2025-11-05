/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Input/Input.h"
#include <iostream>

namespace ZED
{
    // Holds the current IInput implementation
    static IInput* s_InputImpl = nullptr;

    // Sets the active input implementation
    void SetInputImplementation(IInput* impl)
    {
        s_InputImpl = impl;
    }

    // Retrieves the current input implementation
    IInput* GetInput()
    {
        if (!s_InputImpl)
        {
            std::cerr << "[ZED::Input] Warning: No input implementation linked!\n";
        }
        return s_InputImpl;
    }

}