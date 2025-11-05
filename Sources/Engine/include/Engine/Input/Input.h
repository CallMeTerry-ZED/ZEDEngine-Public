/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#ifndef INPUT_H
#define INPUT_H

#pragma once

#include "Engine/Interfaces/Input/IInput.h"

namespace ZED
{
    ZEDENGINE_API void SetInputImplementation(IInput* impl);
    ZEDENGINE_API IInput* GetInput();
}

#endif
