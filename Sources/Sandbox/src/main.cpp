/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

// Single Engine include to catch em all haha....yes these comments are getting boring to me...
#include "ZEDEngine.h"

// Windows/Mac/Linux includes
#include <iostream>

typedef ZED::IWindow* (*CreateWindowFunc)();
typedef ZED::IScripting* (*CreateScriptingFunc)();
typedef ZED::IRenderer* (*CreateRendererFunc)();
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

    auto createRenderer = (CreateRendererFunc)
        ZED::Module::ModuleLoader::GetFunction("Renderer", "CreateRenderer");
    if (!createRenderer)
    {
        std::cerr << "[ZED::Main] CreateRenderer not found\n";
        return -1;
    }

    ZED::IRenderer* renderer = createRenderer();
    // Width and Height must be the same as the window
    if (!renderer->Init(window->GetNativeHandle(), 800, 600))
    {
        std::cerr << "[ZED::Main] Failed to init renderer\n";
    }

    // After RegisterInput(), get the input instance
    auto* input = ZED::Input::GetInput();

    // Needed so we can use other modules than sdl for windowing
    // But still be able to use sdl for our input
    input->AttachToNativeWindow(window->GetNativeHandle());

    // Setup ECS registry
    auto& reg = ZED::ECS::ECS::Registry();

    // hook lifecycle signals once
    ZED::ScriptLifecycleSystem::connect(ZED::ECS::ECS::Registry());

    // Setup example scripts
    ZED::ScriptId spinningScriptId{0};
    ZED::ScriptId pulsingScriptId{0};
    ZED::ScriptId transformScriptId{0};
    ZED::ScriptId timeScriptId{0};

    if (scripting)
    {
        scripting->Init();

        // Load example scripts
        spinningScriptId = scripting->LoadBytecodeFile("Assets/Scripts/spinning_cube.luau.bc");
        pulsingScriptId = scripting->LoadBytecodeFile("Assets/Scripts/pulsing_cube.luau.bc");
        transformScriptId = scripting->LoadBytecodeFile("Assets/Scripts/transform_example.luau.bc");
        timeScriptId = scripting->LoadBytecodeFile("Assets/Scripts/time_example.luau.bc");

        std::cout << "[Main] Loaded example scripts\n";
    }

    // Create camera entity with editor mode enabled
    {
        auto camEnt = reg.create();
        ZED::TransformComponent camTr{};
        camTr.position = ZED::Vec3(0.0f, 0.0f, -6.0f);
        reg.emplace<ZED::TransformComponent>(camEnt, camTr);

        ZED::CameraComponent camComp{};
        camComp.primary = true;
        camComp.editorMode = true;  // Enable editor mode camera control
        camComp.aspect = static_cast<float>(800) / static_cast<float>(600);
        reg.emplace<ZED::CameraComponent>(camEnt, camComp);

        // Attach camera_example script to camera entity
        if (scripting && timeScriptId.value != 0)
        {
            // Using time_example for camera to show time info
            reg.emplace<ZED::ScriptComponent>(camEnt, ZED::ScriptComponent{ timeScriptId.value, true });
        }
    }

    // Transform Test: create entities with different scripts attached
    {
        // Entity 1: Spinning cube
        auto e1 = reg.create();
        reg.emplace<ZED::TransformComponent>(e1, ZED::TransformComponent{
            .position = ZED::Vec3(-4.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });
        if (scripting && spinningScriptId.value != 0)
        {
            reg.emplace<ZED::ScriptComponent>(e1, ZED::ScriptComponent{ spinningScriptId.value, true });
        }

        // Entity 2: Pulsing cube
        auto e2 = reg.create();
        reg.emplace<ZED::TransformComponent>(e2, ZED::TransformComponent{
            .position = ZED::Vec3( 0.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });
        if (scripting && pulsingScriptId.value != 0)
        {
            reg.emplace<ZED::ScriptComponent>(e2, ZED::ScriptComponent{ pulsingScriptId.value, true });
        }

        // Entity 3: Transform example (rotates on all axes)
        auto e3 = reg.create();
        reg.emplace<ZED::TransformComponent>(e3, ZED::TransformComponent{
            .position = ZED::Vec3( 4.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });
        if (scripting && transformScriptId.value != 0)
        {
            reg.emplace<ZED::ScriptComponent>(e3, ZED::ScriptComponent{ transformScriptId.value, true });
        }

        // Entity 4: Another spinning cube (different speed could be set in script)
        auto e4 = reg.create();
        reg.emplace<ZED::TransformComponent>(e4, ZED::TransformComponent{
            .position = ZED::Vec3( 8.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });
        if (scripting && spinningScriptId.value != 0)
        {
            reg.emplace<ZED::ScriptComponent>(e4, ZED::ScriptComponent{ spinningScriptId.value, true });
        }
    }

    // Initialize camera system
    ZED::CameraSystem::Init();
    ZED::CameraSystem::SetAspect(static_cast<float>(800) / static_cast<float>(600));

    // Initialize camera controller
    ZED::CameraController::SetEnabled(true);
    ZED::CameraController::SetMoveSpeed(5.0f);
    ZED::CameraController::SetMouseSensitivity(0.002f);

    // Main loop
    while (window->IsRunning())
    {
        ZED::Time::Update();
        double time = ZED::Time::GetElapsedTime();
        double deltaTime = ZED::Time::GetDeltaTime();

        time += deltaTime;

        // Poll window events
        window->PollEvents();

        // Poll input events
        ZED::Input::GetInput()->PollEvents();

        // Handle mouse capture for editor mode camera
        bool leftMouseDown = ZED::Input::GetInput()->IsKeyDown(ZED::Key::MouseLeft);
        if (leftMouseDown && !window->IsMouseCaptured())
        {
            window->SetMouseCapture(true);
            window->SetMouseVisible(false);
        }
        else if (!leftMouseDown && window->IsMouseCaptured())
        {
            window->SetMouseCapture(false);
            window->SetMouseVisible(true);
        }

        // Move deferred events into the immediate queue, then dispatch all
        ZED::EventSystem::Get().DispatchDeferred();
        ZED::EventSystem::Get().Dispatch();

        // Update camera controller (must be after events are dispatched)
        ZED::CameraController::Update(ZED::ECS::ECS::Registry(), deltaTime);

        // Camera update
        ZED::CameraSystem::Update(ZED::ECS::ECS::Registry());
        const ZED::Mat4& view = ZED::CameraSystem::GetView();
        const ZED::Mat4& proj = ZED::CameraSystem::GetProj();

        // Render all transforms as cubes
        renderer->BeginFrame(0.06f, 0.06f, 0.08f, 1.0f, view, proj);

        auto tview = reg.view<ZED::TransformComponent>();
        for (auto e : tview)
        {
            // Skip camera entity when rendering
            if (reg.any_of<ZED::CameraComponent>(e))
            {
                continue;
            }

            const auto& tr = tview.get<ZED::TransformComponent>(e);
            renderer->DrawCube(tr.ToMatrix());
        }

        renderer->EndFrame();

        // Tick scripts (per-entity) - scripts handle all transform updates
        ZED::ScriptUpdateSystem::tick(ZED::ECS::ECS::Registry(), deltaTime);

        ZED::Time::Sleep(1);
    }

    renderer->Shutdown();
    window->Shutdown();
    if (scripting)
    {
        scripting->Shutdown();
    }
    delete renderer;
    delete window;
    delete scripting;

    // Cleanup loaded modules
    ZED::Module::ModuleLoader::Cleanup();

    return 0;
}