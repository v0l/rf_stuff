#pragma once

#include <rf_stuff/samples.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <stdint.h>

#include <complex>
#include <fftw3.h>

namespace rf_stuff::common
{
    class FFT
    {
    public:
        static auto New() -> std::shared_ptr<FFT>
        {
            return std::make_shared<FFT>();
        }
        
        static auto New(const uint16_t &sz) -> std::shared_ptr<FFT>
        {
            return std::make_shared<FFT>(sz);
        }

        auto SetFFTSize(const uint16_t&) -> void;
        auto Process(std::shared_ptr<Samples> iq) -> void;
        auto GetData() const -> const fftwf_complex* 
        {
            return fft_out;
        }
        
        auto GetSize() const -> const uint16_t&
        {
            return fft_size;
        }

        auto GetCounter() const -> uint64_t
        {
            //copy value
            return counter;
        }

        auto GetSampleRate() const -> const uint64_t&
        {
            return sample_rate;
        }

        auto GetCenterFrequency() const -> const uint64_t&
        {
            return center_frequency;
        }

    private:
        FFT() : fft_size(0), counter(0), sample_rate(0), center_frequency(0)
        {

        }

        FFT(const uint16_t &size) : fft_size(size), counter(0), sample_rate(0), center_frequency(0)
        {
            SetFFTSize(fft_size);
        }

        std::mutex fft_plan_lock;
        uint64_t counter, sample_rate, center_frequency;

        fftwf_complex *fft_in, *fft_out;
        fftwf_plan fft_plan;
        uint16_t fft_size;
    };
} // namespace rf_stuff::common
