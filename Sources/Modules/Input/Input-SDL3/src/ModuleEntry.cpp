/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Input-SDL3/SDLInput.h"
#include "Engine/Input/Input.h"
#include "Engine/Interfaces/Input/IInput.h"

extern "C" ZEDENGINE_API void RegisterInput()
{
    static ZED::SDLInput input;
    ZED::SetInputImplementation(&input);
}