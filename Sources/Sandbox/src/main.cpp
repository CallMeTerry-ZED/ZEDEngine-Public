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

    // Create a camera entity
    {
        auto& reg = ZED::ECS::ECS::Registry();

        auto camEnt = reg.create();
        ZED::TransformComponent camTr{};
        camTr.position = ZED::Vec3(0.0f, 0.0f, -6.0f); // LH: camera behind origin looking +Z; inverse will look forward
        reg.emplace<ZED::TransformComponent>(camEnt, camTr);

        ZED::CameraComponent camComp{};
        camComp.primary = true;
        camComp.aspect = static_cast<float>(800) / static_cast<float>(600);
        reg.emplace<ZED::CameraComponent>(camEnt, camComp);
    }

    // Transform Test: create a few entities with transforms
    {
        auto& reg = ZED::ECS::ECS::Registry();

        auto e1 = reg.create();
        reg.emplace<ZED::TransformComponent>(e1, ZED::TransformComponent{
            .position = ZED::Vec3(-2.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });

        auto e2 = reg.create();
        reg.emplace<ZED::TransformComponent>(e2, ZED::TransformComponent{
            .position = ZED::Vec3( 0.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });

        auto e3 = reg.create();
        reg.emplace<ZED::TransformComponent>(e3, ZED::TransformComponent{
            .position = ZED::Vec3( 2.0f, 0.0f, 0.0f),
            .rotation = ZED::Vec3(0.0f, 0.0f, 0.0f),
            .scale    = ZED::Vec3(1.0f, 1.0f, 1.0f)
        });
    }

    // Initialize camera system
    ZED::CameraSystem::Init();
    ZED::CameraSystem::SetAspect(static_cast<float>(800) / static_cast<float>(600));

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

        // Drive transforms: spin all around Y
        ZED::TransformSystem::SpinAll(ZED::ECS::ECS::Registry(), deltaTime, ZED::Vec3(0.0f, 1.0f, 0.0f));

        // Camera update
        ZED::CameraSystem::Update(ZED::ECS::ECS::Registry());
        const ZED::Mat4& view = ZED::CameraSystem::GetView();
        const ZED::Mat4& proj = ZED::CameraSystem::GetProj();

        // Render all transforms as cubes
        renderer->BeginFrame(0.06f, 0.06f, 0.08f, 1.0f, view, proj);

        auto& reg = ZED::ECS::ECS::Registry();
        auto tview = reg.view<ZED::TransformComponent>();
        for (auto e : tview)
        {
            const auto& tr = tview.get<ZED::TransformComponent>(e);
            renderer->DrawCube(tr.ToMatrix());
        }

        renderer->EndFrame();

        // Tick scripts (per-entity)
        ZED::ScriptUpdateSystem::tick(ZED::ECS::ECS::Registry(), deltaTime);

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

        ZED::Time::Sleep(1);
    }

    renderer->Shutdown();
    window->Shutdown();
    delete renderer;
    delete window;

    // Cleanup loaded modules
    ZED::Module::ModuleLoader::Cleanup();

    return 0;
}