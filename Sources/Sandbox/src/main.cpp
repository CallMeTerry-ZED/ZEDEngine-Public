/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Engine/IWindow.h"
#include "Engine/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Config/Config.h"
#include "Engine/Module/ModuleLoader.h"
#include "Engine/Events/EventSystem.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/ScriptComponent.h"
#include "Engine/ECS/Systems/ScriptSystems.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Interfaces/Scripting/IScripting.h"

#include <Windows.h>
#include <iostream>

typedef ZED::IWindow* (*CreateWindowFunc)();
typedef void (*RegisterTimeFunc)();
typedef void (*RegisterInputFunc)();
typedef ZED::IScripting*   (*CreateScriptingFunc)();

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

    ZED::IScripting* scripting = nullptr;
    if (auto createScripting = (CreateScriptingFunc)
        ZED::Module::ModuleLoader::GetFunction("Scripting", "CreateScripting"))
    {
        scripting = createScripting();
    }

    // Create a window via the Window module
    auto createWindow = (CreateWindowFunc)
        ZED::Module::ModuleLoader::GetFunction("Window", "ZED_CreateWindow");
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

    // After RegisterInput(), get the input instance
    auto* input = ZED::Input::GetInput();

    // Needed so we can use other modules than sdl for windowing
    // But still be able to use sdl for our input
    input->AttachToNativeWindow(window->GetNativeHandle());

    // Set up input event callback
    input->SetEventCallback([](const ZED::InputEvent& e)
    {
        // Logging of input events *just to ensure it works
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
            default:
                break;
        }
    });

    // --- ECS + Scripting smoke test ---
    // hook lifecycle signals once
    ZED::ScriptLifecycleSystem::connect(ZED::ECS::ECS::Registry());

    // Load a compiled luau bytecode file, then attach to an entity
    if (scripting)
    {
        auto sid = scripting->LoadBytecodeFile("Assets/Scripts/test.luau.bc");
        auto& reg = ZED::ECS::ECS::Registry();
        auto e = reg.create();
        reg.emplace<ZED::ScriptComponent>(e, ZED::ScriptComponent{ sid.value, true });
    }

    // Main loop
    const double dt = 1.0 / 60.0;
    while (window->IsRunning())
    {
        // Poll window events
        window->PollEvents();

        // Poll input events
        ZED::Input::GetInput()->PollEvents();

        // Tick scripts (per-entity)
        ZED::ScriptUpdateSystem::tick(ZED::ECS::ECS::Registry(), dt);

        // Move deferred events into the immediate queue, then dispatch all
        ZED::EventSystem::Get().DispatchDeferred();
        ZED::EventSystem::Get().Dispatch();

        // Key test: continuous key state check
        if (ZED::Input::GetInput()->IsKeyDown(ZED::Key::W))
        {
            std::cout << "[Continuous] W is held down\n";
        }
        else if (ZED::Input::GetInput()->IsKeyDown(ZED::Key::S))
        {
            std::cout << "[Continuous] S is held down\n";
        }
        else if (ZED::Input::GetInput()->IsKeyDown(ZED::Key::A))
        {
            std::cout << "[Continuous] A is held down\n";
        }
        else if (ZED::Input::GetInput()->IsKeyDown(ZED::Key::D))
        {
            std::cout << "[Continuous] D is held down\n";
        }

        ZED::Time::Sleep(16);
    }

    window->Shutdown();
    delete window;

    // Cleanup loaded modules
    ZED::Module::ModuleLoader::Cleanup();

    return 0;
}