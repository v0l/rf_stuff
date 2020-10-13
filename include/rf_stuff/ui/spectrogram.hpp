#pragma once

#include <rf_stuff/ui/sdl2.hpp>
#include <rf_stuff/ui/element.hpp>
#include <rf_stuff/common/fft.hpp>
#include <rf_stuff/const.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <stdint.h>

namespace rf_stuff::ui
{
    class Spectrogram : public UIElement
    {
    public:
        Spectrogram(const uint16_t &xpos, const uint16_t &ypos, const uint16_t &w, const uint16_t &h, const uint8_t &dec = 0)
            : x(xpos), y(ypos), w(w), h(h), fft_decimation(dec), fft_in(nullptr), fft_out(nullptr), fft_plan(nullptr), 
                update_texture(true), center_freq_texture(nullptr), background(nullptr), db_scale_max(50), db_scale_min(-70)
        { }
        auto SetFFTSource(std::shared_ptr<common::FFT>) -> void;
        
        auto Setup(SDL_Renderer *) -> void override;
        auto Update(const SDL2 *sdl, const SDL_Event &ev) -> void override;
        auto Render(SDL_Renderer *) -> void override;

        const uint16_t x, y, w, h;
        const uint8_t fft_decimation;
    private:
        auto SetupBackground(SDL_Renderer *) -> void;

        std::atomic_bool update_texture;
        SDL_Texture *fft_texture;
        std::vector<SDL_Point> fft_points;
        
        //FFT
        std::shared_ptr<common::FFT> fft;
        uint64_t fft_counter;
        
        //UI related vars
        int16_t db_scale_min, db_scale_max;
        float selector_pos; //-1 means no selector
        uint64_t sample_rate, center_frequency;
        SDL_Texture *background;
        
        //Text vars
        TTF_Font *font;
        SDL_Rect center_freq_rect;
        SDL_Texture *center_freq_texture;
    };
} // namespace rf_stuff::ui