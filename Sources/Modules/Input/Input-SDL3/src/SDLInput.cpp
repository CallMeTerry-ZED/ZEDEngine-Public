/*
 * © 2025 ZED Interactive. All Rights Reserved.
 */

#include "Input-SDL3/SDLInput.h"
#include <iostream>

namespace ZED
{
    SDLInput::SDLInput() {}

    void SDLInput::SetEventCallback(void (*callback)(const InputEvent&))
    {
        eventCallback = callback;
    }

    ZED::Key SDLInput::MapSDLKey(SDL_Keycode keycode)
    {
        switch (keycode)
        {
            case SDLK_A: return Key::A;
            case SDLK_B: return Key::B;
            case SDLK_ESCAPE: return Key::Escape;
            case SDLK_RETURN: return Key::Enter;
            case SDLK_SPACE: return Key::Space;
            case SDLK_LEFT: return Key::Left;
            case SDLK_RIGHT: return Key::Right;
            case SDLK_UP: return Key::Up;
            case SDLK_DOWN: return Key::Down;
            default: return Key::Unknown;
        }
    }

    void SDLInput::PollEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (!eventCallback) continue;

            InputEvent event{};
            switch (e.type)
            {
                case SDL_EVENT_KEY_DOWN:
                    event.type = InputEventType::KeyDown;
                    event.key = MapSDLKey(e.key.key);
                    break;
                case SDL_EVENT_KEY_UP:
                    event.type = InputEventType::KeyUp;
                    event.key = MapSDLKey(e.key.key);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    event.type = InputEventType::MouseMove;
                    event.mouseX = e.motion.x;
                    event.mouseY = e.motion.y;
                    break;
                default:
                    continue;
            }
            eventCallback(event);
        }
    }
}