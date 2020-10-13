
#include <rf_stuff/sdr/hackrf.hpp>
#include <rf_stuff/ui/sdl2.hpp>
#include <rf_stuff/samples.hpp>
#include <rf_stuff/ui/spectrogram.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#if defined(__unix__)
#include <signal.h>
#endif

using namespace rf_stuff;
using namespace std::chrono_literals;

static bool quit = false;
static std::shared_ptr<rf_stuff::ui::Spectrogram> fft;

void got_signal(int h)
{
    if (h == SIGINT)
    {
        quit = true;
    }
}

void got_samples(std::shared_ptr<Samples> iq)
{
    //std::cout << "Got: " << iq.size() << " samples" << std::endl;
    if (fft)
    {
        fft->Display(iq);
    }
}

int main(int argc, char **argv)
{
    ui::SDL2 ui("Demo", 1280, 720);
    ui.Init();

    ui::Spectrogram spec(0, 0, 1280, 360);
    spec.SetFFTSize(2048);
    fft.reset(&spec);

    ui.AddElement(fft);

    sdr::HackRF rf;
    rf.Open(0);
    rf.Start(got_samples);

    signal(SIGINT, got_signal);

    rf.SetSampleRate(20e6);
    rf.SetCenterFrequency(101.1e6);
    rf.SetGain(32, 10);

    ui.AddInputHandler([&rf](const SDL_Event &ev) {
        if (ev.type == SDL_KEYDOWN)
        {
            if (ev.key.keysym.sym == SDLK_LEFT)
            {
                rf.SetCenterFrequency(rf.GetCenterFrequency() - 100e3);
            }
            else if (ev.key.keysym.sym == SDLK_RIGHT)
            {
                rf.SetCenterFrequency(rf.GetCenterFrequency() + 100e3);
            }
        }
    });
    while (!ui.WantsExit() && !quit)
    {
        ui.Update();
        ui.Render();
    }

    quit = true;
    rf.Close();
    ui.Close();
}
