#include <rf_stuff/common/fft.hpp>

using namespace rf_stuff::common;

auto FFT::SetFFTSize(const uint16_t &sz) -> void 
{
    std::lock_guard<std::mutex> lk_plan(fft_plan_lock);

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
}

auto FFT::Process(std::shared_ptr<Samples> iq) -> void
{
    std::lock_guard<std::mutex> lk_plan(fft_plan_lock);

    auto iter = iq->samples.begin();
    auto omega = 2.0f * M_PI / (fft_size - 1);
    for (auto x = 1; x < fft_size; x++)
    {
        //apply window here
        float multiplier = 0.54f - 0.46f * cos(omega * (x - 1));
        fft_in[x][0] = multiplier * (*iter).real();
        fft_in[x][1] = multiplier * (*iter).imag();

        std::advance(iter, 1);
    }

    fftwf_execute_dft(fft_plan, fft_in, fft_out);

    sample_rate = iq->sample_rate;
    center_frequency = iq->center_frequency;
    
    counter++;
}