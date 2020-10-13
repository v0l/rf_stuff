#pragma once

#include <rf_stuff/const.hpp>
#include <rf_stuff/samples.hpp>

#include <stdint.h>
#include <complex>
#include <functional>
#include <vector>

namespace rf_stuff::sdr
{
    typedef std::function<void(std::shared_ptr<Samples>)> fn_samples;

    class SDR
    {
        virtual auto Open(const uint16_t&) -> void = 0;
        virtual auto Start(const fn_samples &fn) -> void = 0;
        virtual auto Close() -> void = 0;

        virtual auto SetCenterFrequency(const uint64_t&) -> void = 0;
        virtual auto GetCenterFrequency() const -> const uint64_t& = 0;

        virtual auto SetSampleRate(const uint64_t&) -> void = 0;
        virtual auto GetSampleRate() const -> const uint64_t& = 0;

        virtual auto SetGain(const uint16_t& lna, const uint16_t &vga) -> void = 0;

        virtual auto IsStreaming() const -> bool = 0;
    };
} // namespace fft::sdr
