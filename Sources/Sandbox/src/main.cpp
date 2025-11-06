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

    ZED::Input::GetInput()->SetEventCallback([](const ZED::InputEvent& e)
    {
        switch (e.type)
        {
            case ZED::InputEventType::KeyDown:
                std::cout << "Key Down: " << static_cast<int>(e.key) << "\n";
                break;
            case ZED::InputEventType::KeyUp:
                std::cout << "Key Up: " << static_cast<int>(e.key) << "\n";
                break;
            case ZED::InputEventType::MouseMove:
                std::cout << "Mouse Move: (" << e.mouseX << ", " << e.mouseY << ")\n";
                break;
        }
    });

    // Main loop
    while (window->IsRunning())
    {
        // Poll input events first.  SDL queues are global, so if the window
        // consumes events before the input system sees them, keyboard and mouse
        // callbacks will be missed.  Calling the input poller first ensures
        // that input events are processed before the window drains the queue.
        ZED::Input::GetInput()->PollEvents();
        window->PollEvents();

        if (ZED::Input::GetInput()->IsKeyDown(ZED::Key::W))
        {
            std::cout << "[Continuous] W is held down\n";
        }

        ZED::Time::Sleep(16);
    }

    window->Shutdown();
    delete window;

    // Cleanup loaded modules
    ZED::Module::ModuleLoader::Cleanup();

    return 0;
}