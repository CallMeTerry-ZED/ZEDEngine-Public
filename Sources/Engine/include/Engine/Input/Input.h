/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef INPUT_H
#define INPUT_H

#pragma once

#include "Engine/Interfaces/Input/IInput.h"

namespace ZED
{
    class ZEDENGINE_API Input : public IInput
    {
    public:
        // Global accessor and setter *prolly want to set this up better
        static void SetInputImplementation(IInput* impl);
        static IInput* GetInput();

        //void Update() override;
        bool IsKeyDown(Key key) const override;

    private:
        // Holds the current IInput implementation
        static IInput* s_InputImpl;
    };
}

#endif
