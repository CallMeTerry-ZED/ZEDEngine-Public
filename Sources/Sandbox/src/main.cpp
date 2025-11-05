/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/IWindow.h"
#include "Engine/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Config/Config.h"
#include "Engine/Module/ModuleLoader.h"

#include <Windows.h>
#include <iostream>

typedef ZED::IWindow* (*CreateWindowFunc)();
typedef void (*RegisterTimeFunc)();
typedef void (*RegisterInputFunc)();

int main(int argc, char* argv[])
{
    // Load INI configuration
    ZED::Config::Load("Configs/zedengine.ini");

    // Load all modules listed in the INI under [Modules]
    ZED::Module::ModuleLoader::LoadModulesFromINI();

    // Register SDLTime implementation
    auto registerTime = (RegisterTimeFunc)
        ZED::Module::ModuleLoader::GetFunction("Time", "RegisterTime");
    if (registerTime)
    {
        registerTime();
    }

    // Register SDLInput implementation
    auto registerInput = (RegisterInputFunc)
        ZED::Module::ModuleLoader::GetFunction("Input", "RegisterInput");
    if (registerInput)
    {
        registerInput();
    }

    // Create a window via the Window module
    auto createWindow = (CreateWindowFunc)
        ZED::Module::ModuleLoader::GetFunction("Window", "CreateWindow");
    if (!createWindow)
    {
        std::cerr << "[ZED::Main] CreateWindow not found\n";
        return -1;
    }

    ZED::IWindow* window = createWindow();
    if (!window->Init("ZED-APP: Sandbox", 800, 600))
    {
        std::cerr << "[ZED::Main] Failed to init window\n";
        return -1;
    }

    // Main loop
    while (window->IsRunning())
    {
        window->PollEvents();
        ZED::Time::Sleep(16);
    }

    window->Shutdown();
    delete window;

    // Cleanup loaded modules
    ZED::Module::ModuleLoader::Cleanup();

    return 0;
}