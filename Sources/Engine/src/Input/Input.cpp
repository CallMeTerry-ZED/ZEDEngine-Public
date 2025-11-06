/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/Input/Input.h"
#include <iostream>

namespace ZED
{
    IInput* Input::s_InputImpl = nullptr;

    // Sets the active input implementation
    void Input::SetInputImplementation(IInput* impl)
    {
        s_InputImpl = impl;
    }

    // Retrieves the current input implementation
    IInput* Input::GetInput()
    {
        if (!s_InputImpl)
        {
            std::cerr << "[ZED::Input] Warning: No input implementation linked!\n";
        }
        return s_InputImpl;
    }

    bool Input::IsKeyDown(Key k) const
    {
        return s_InputImpl ? s_InputImpl->IsKeyDown(k) : false;
    }

}