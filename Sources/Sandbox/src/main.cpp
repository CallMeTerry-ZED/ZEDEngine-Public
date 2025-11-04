/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/ITime.h"
#include "Engine/IWindow.h"
#include "Engine/Time.h"

#include <Windows.h>

#include <iostream>

typedef ZED::IWindow* (*CreateWindowFunc)();
typedef void (*RegisterTimeFunc)();

int main(int argc, char* argv[])
{
    HMODULE sdlModule = LoadLibraryA("libWindow-SDL3.dll");
    if (!sdlModule)
    {
        std::cerr << "Failed to load Window-SDL3.dll\n";
        return -1;
    }

    RegisterTimeFunc registerTime = (RegisterTimeFunc)GetProcAddress(sdlModule, "RegisterTime");
    if (registerTime)
    {
        registerTime();
    }

    auto createWindow = (CreateWindowFunc)GetProcAddress(sdlModule, "CreateWindow");
    if (!createWindow)
    {
        std::cerr << "CreateWindow not found\n";
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

    window->Shutdown();
    delete window;
    FreeLibrary(sdlModule);
    return 0;
}