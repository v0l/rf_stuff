#include <rf_stuff/ui/sdl2.hpp>
#include <rf_stuff/const.hpp>

#include <SDL2/SDL_keycode.h>

#include <stdexcept>
#include <chrono>

using namespace rf_stuff::ui;

auto SDL2::Init() -> void
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        Close();
        throw std::runtime_error(SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr)
    {
        Close();
        throw std::runtime_error(SDL_GetError());
    }

    if(TTF_Init() != 0)
    {
        throw std::runtime_error("TTF Init failed");
    }
}

auto SDL2::Close() -> void
{
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }
    if (renderer != nullptr)
    {
        SDL_DestroyRenderer(renderer);
    }
    SDL_Quit();
}

auto SDL2::Update() -> void
{
    auto u_start = std::chrono::system_clock::now();

    SDL_Event ev;
    if (SDL_PollEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
        {
            wants_exit = true;
            return;
        }

        if (ev.type == SDL_KEYDOWN)
        {
            //F1 = toggle debug info
            if (ev.key.keysym.sym == SDLK_F1)
            {
                this->show_debug = !this->show_debug;
            }
        }

        //handle mouse input
        if (ev.type == SDL_MOUSEMOTION)
        {
            mx = ev.motion.x;
            my = ev.motion.y;
        }

        for(const auto &fn : input_handles)
        {
            fn(ev);
        }
    }

    for (const auto &e : elements)
    {
        e->Update(this, ev);
    }

    auto u_time = std::chrono::system_clock::now() - u_start;
    t_update = std::chrono::duration_cast<std::chrono::microseconds>(u_time).count() / 1000.0f;
}

auto SDL2::Render() -> void
{
    auto dt = SDL_GetTicks();
    auto r_start = std::chrono::system_clock::now();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    for (const auto &e : elements)
    {
        e->Render(renderer);
    }

    if (show_debug)
    {
        if (debug_font == nullptr)
        {
            debug_font = TTF_OpenFont(UIFont, 16);
        }

        SDL_Rect r_debug = {10, 10, 200, 200};
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xf0);
        SDL_RenderFillRect(renderer, &r_debug);

        if (last_debug_update + 200 < dt)
        {
            auto old_texture = debug_texture;

            SDL_Color msg_color = {0, 0, 0, 0xff};
            char msg[512];

            sprintf(msg, "U/R: %.2f/%.2f", t_update, t_render);
            auto msg_surface = TTF_RenderText_Blended(debug_font, msg, msg_color);
            //auto msg_surface = TTF_RenderText_Solid(debug_font, msg, msg_color);
            debug_texture = SDL_CreateTextureFromSurface(renderer, msg_surface);
            SDL_FreeSurface(msg_surface);

            debug_texture_size = {r_debug.x, r_debug.y, 0, 0};
            SDL_QueryTexture(debug_texture, NULL, NULL, &debug_texture_size.w, &debug_texture_size.h);

            if (old_texture != nullptr)
            {
                SDL_DestroyTexture(old_texture);
            }
            last_debug_update = dt;
        }
        SDL_RenderCopy(renderer, debug_texture, NULL, &debug_texture_size);
    }
    SDL_RenderPresent(renderer);

    auto r_time = std::chrono::system_clock::now() - r_start;
    t_render = std::chrono::duration_cast<std::chrono::microseconds>(r_time).count() / 1000.0f;
}