#pragma once

#include <SDL2/SDL.h>

namespace rf_stuff::ui
{
    class SDL2;
    class UIElement
    {
    public:
        virtual auto Setup(SDL_Renderer *) -> void = 0;
        virtual auto Update(const SDL2 *sdl, const SDL_Event &ev) -> void = 0;
        virtual auto Render(SDL_Renderer *) -> void = 0;
    };
} // namespace rf_stuff::ui