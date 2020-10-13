#include <rf_stuff/ui/spectrogram.hpp>
#include <rf_stuff/const.hpp>

#include <iostream>
#include <math.h>

using namespace rf_stuff::ui;

auto Spectrogram::Setup(SDL_Renderer *r) -> void
{
    SDL_RendererInfo info;
    SDL_GetRendererInfo(r, &info);

    fft_texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->w, this->h);
    background = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->w, this->h);
    font = TTF_OpenFont(UIFont, 16);
    if (font == nullptr || fft_texture == nullptr || background == nullptr)
    {
        throw std::runtime_error("Spectrogram init failed");
    }
    else
    {
        SDL_SetTextureBlendMode(fft_texture, SDL_BLENDMODE_ADD);
        SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);
    }

    //setup background texture
    SetupBackground(r);
}

auto Spectrogram::SetupBackground(SDL_Renderer *r) -> void
{
    auto old_target = SDL_GetRenderTarget(r);

    SDL_SetRenderTarget(r, background);
    SDL_RenderClear(r);
    SDL_SetRenderDrawColor(r, 0xcc, 0xcc, 0xcc, 0x60);
    auto db_range = db_scale_max - db_scale_min;
    const auto incr = 12;
    for (auto db = incr; db < db_range; db += incr)
    {
        auto y_line = this->h * (db / (float)db_range);
        SDL_RenderDrawLine(r, 1, y_line, this->x + this->w - 2, y_line);
    }

    SDL_SetRenderTarget(r, old_target);
}

auto Spectrogram::SetFFTSize(const uint16_t &sz) -> void
{
    fft_size = sz;
    if (fft_plan != nullptr)
    {
        fftwf_free(fft_in);
        fftwf_free(fft_out);
        fftwf_destroy_plan(fft_plan);
        fft_plan = nullptr;
        fft_in = fft_out = nullptr;
    }
    fft_in = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    fft_out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * fft_size);
    fft_plan = fftwf_plan_dft_1d(fft_size, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
    fft_points.reserve(fft_size);
}

auto Spectrogram::Update(const SDL2 *sdl, const SDL_Event &ev) -> void
{
    //is mouse in our bounds
    if (sdl->MouseX() >= this->x && sdl->MouseX() <= this->x + this->w && sdl->MouseY() >= this->y && sdl->MouseY() <= this->y + this->h)
    {
        selector_pos = (sdl->MouseX() - this->x) / (float)this->w;
    }
    else
    {
        selector_pos = -1;
    }

    if (fft)
    {
        auto fc = fft->GetCounter();
        if(fc != fft_counter) 
        {
            fft_counter = fc;
            auto fft_out = fft->GetData();
            
            //update texture
            fft_points.clear();
            auto w_scale = this->w / (float)fft_size;
            auto db_range = (db_scale_max - db_scale_min);
            auto dc = fft_out[0];
            for (auto x = 1; x < fft_size; x++)
            {
                auto fft_val = fft_out[x];
                auto db = 20.0 * log10(sqrt((fft_val[0] * fft_val[0]) + (fft_val[1] * fft_val[1])));
                if (std::isnan(db) || db < db_scale_min)
                {
                    db = db_scale_min;
                }
                else if (std::isinf(db) || db > db_scale_max)
                {
                    db = db_scale_max;
                }
                fft_points.push_back({this->x + (x * w_scale), this->y + (this->h * (1 - ((db + std::abs(db_scale_min)) / db_range)))});
            }

            sample_rate = iq->sample_rate;
            center_frequency = iq->center_frequency;
            update_texture = true;
        }
    }
}

auto Spectrogram::Render(SDL_Renderer *r) -> void
{
    SDL_Rect outline = {x, y, w, h};
    if (update_texture)
    {
        //store current render target
        auto rt = SDL_GetRenderTarget(r);

        //draw to texture
        SDL_SetRenderTarget(r, fft_texture);

        //clear texture
        SDL_RenderClear(r);

        //set draw color
        SDL_SetRenderDrawColor(r, 0xff, 0xff, 0xff, 0xff);

        //draw outline
        SDL_RenderDrawRect(r, &outline);

        //draw fft data
        SDL_RenderDrawLines(r, fft_points.data(), fft_points.size());

        //set target back to previous
        SDL_SetRenderTarget(r, rt);

        //texture update is done
        update_texture = false;
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    //copy background
    SDL_RenderCopy(r, background, NULL, &outline);

    //copy fft texture to screen
    SDL_RenderCopy(r, fft_texture, NULL, &outline);

    //draw selector
    if (selector_pos != -1.0f)
    {
        auto x_sel = this->x + (this->w * selector_pos);
        SDL_SetRenderDrawColor(r, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(r, x_sel, this->y, x_sel, this->y + this->h);

        const int padding = 4;
        char cf_txt[64];
        auto cf_offset = center_frequency + (sample_rate * selector_pos);
        sprintf(cf_txt, "%.3f %s", cf_offset > 1e6 ? cf_offset / 1e6f : cf_offset / 1e3f, cf_offset > 1e6 ? "MHz" : "KHz");
        auto cf_surface = TTF_RenderText_Blended(font, cf_txt, {0xff, 0xff, 0xff, 0xff});

        auto old_cf_texture = center_freq_texture;
        center_freq_texture = SDL_CreateTextureFromSurface(r, cf_surface);
        if (old_cf_texture != nullptr)
        {
            SDL_DestroyTexture(old_cf_texture);
        }

        center_freq_rect = {x_sel + padding, padding, 0, 0};
        SDL_QueryTexture(center_freq_texture, NULL, NULL, &center_freq_rect.w, &center_freq_rect.h);
        SDL_FreeSurface(cf_surface);

        SDL_RenderCopy(r, center_freq_texture, NULL, &center_freq_rect);
    }
}

auto Spectrogram::SetFFTSource(std::shared_ptr<common::FFT> fft) -> void
{
    this->fft = fft;
}