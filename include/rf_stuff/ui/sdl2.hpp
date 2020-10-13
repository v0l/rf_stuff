#pragma once

#include <string>
#include <stdint.h>
#include <vector>
#include <memory>
#include <functional>

#include <rf_stuff/ui/element.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace rf_stuff::ui
{
    typedef std::function<void(const SDL_Event&)> input_handler_fn;

    class SDL2
    {
    public:
        SDL2(const std::string &title, const uint16_t &w, const uint16_t &h)
            : window_title(title), width(w), height(h), window(nullptr), renderer(nullptr),
                show_debug(false), debug_font(nullptr), last_debug_update(0)
        {
        }

        auto Init() -> void;
        auto Update() -> void;
        auto Render() -> void;
        auto Close() -> void;
        
        auto AddElement(const std::shared_ptr<UIElement> &&elm) -> void
        {
            elm->Setup(renderer);
            elements.push_back(elm);
        }

        auto AddInputHandler(const input_handler_fn &fn) -> void
        {
            input_handles.push_back(fn);
        }

        constexpr auto WantsExit() const -> const bool& { return wants_exit; }
        constexpr auto MouseX() const -> const float& { return mx; }
        constexpr auto MouseY() const -> const float& { return my; }

        const std::string window_title;
        const uint16_t width;
        const uint16_t height;
    private:
        bool wants_exit, show_debug;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Thread *thread;

        //debug text vars
        uint32_t last_debug_update;
        TTF_Font *debug_font;
        SDL_Rect debug_texture_size;
        SDL_Texture *debug_texture;

        float mx, my, t_update, t_render;
        std::vector<std::shared_ptr<UIElement>> elements;
        std::vector<input_handler_fn> input_handles;
    };
} // namespace rf_stuff::ui