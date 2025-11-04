/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/IWindow.h"
#include "Engine/Time.h"
#include "Engine/Config/Config.h"

#include <Windows.h>
#include <iostream>

typedef ZED::IWindow* (*CreateWindowFunc)();
typedef void (*RegisterTimeFunc)();

int main(int argc, char* argv[])
{
    // Load INI config
    ZED::Config::Load("Configs/zedengine.ini");

    const auto& ini = ZED::Config::Get();
    std::string windowModuleName = ini.GetValue("Modules", "Window", "libWindow-SDL3.dll");
    std::string timeModuleName = ini.GetValue("Modules", "Time", "libWindow-SDL3.dll");

    std::cout << "Configured window module: " << windowModuleName << "\n";
    std::cout << "Configured time module: " << timeModuleName << "\n";

    // Load time module and register it
    HMODULE timeModule = LoadLibraryA(timeModuleName.c_str());
    if (!timeModule)
    {
        std::cerr << "Failed to load time module: " << timeModuleName << "\n";
        return -1;
    }

    RegisterTimeFunc registerTime = (RegisterTimeFunc)GetProcAddress(timeModule, "RegisterTime");
    if (registerTime)
    {
        registerTime();
    }
    else
    {
        std::cerr << "RegisterTime not found in: " << timeModuleName << "\n";
    }

    // Load window module and create window
    HMODULE windowModule = LoadLibraryA(windowModuleName.c_str());
    if (!windowModule)
    {
        std::cerr << "Failed to load window module: " << windowModuleName << "\n";
        return -1;
    }

    auto createWindow = (CreateWindowFunc)GetProcAddress(windowModule, "CreateWindow");
    if (!createWindow)
    {
        std::cerr << "CreateWindow not found in: " << windowModuleName << "\n";
        return -1;
    }

    ZED::IWindow* window = createWindow();
    if (!window->Init("ZED-APP: Sandbox", 800, 600))
    {
        std::cerr << "Failed to init window\n";
        return -1;
    }

    while (window->IsRunning())
    {
        window->PollEvents();
        ZED::Time::Sleep(16);
    }

    // Cleanup
    window->Shutdown();
    delete window;
    FreeLibrary(timeModule);
    FreeLibrary(windowModule);

    return 0;
}