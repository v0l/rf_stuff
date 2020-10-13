#pragma once

#include <rf_stuff/sdr/sdr.hpp>

#include <libhackrf/hackrf.h>

namespace rf_stuff::sdr
{
    class HackRF : public SDR
    {
    public:
        auto Open(const uint16_t&) -> void override;
        auto Start(const fn_samples &fn) -> void override;
        auto Close() -> void override;

        auto SetCenterFrequency(const uint64_t&) -> void override;
        constexpr auto GetCenterFrequency() const -> const uint64_t& override
        {
            return center_frequency;
        }

        auto SetSampleRate(const uint64_t&) -> void override;
        constexpr auto GetSampleRate() const -> const uint64_t& override
        {
            return sample_rate;
        }

        auto SetGain(const uint16_t& lna, const uint16_t &vga) -> void override;
        auto IsStreaming() const -> bool override;
    private:
        fn_samples rx_handle;
        uint64_t sample_rate, center_frequency;
        hackrf_device *device;
    };
} // namespace fft::sdr