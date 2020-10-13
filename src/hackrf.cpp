#include <rf_stuff/sdr/hackrf.hpp>
#include <rf_stuff/const.hpp>

#include <iostream>
#include <stdexcept>
#include <limits>
#include <libhackrf/hackrf.h>

using namespace rf_stuff::sdr;

auto HackRF::Open(const uint16_t &idx) -> void
{
    auto err = hackrf_init();
    if (HACKRF_SUCCESS != err)
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }

    if (HACKRF_SUCCESS != (err = hackrf_open(&device)))
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }
}

auto HackRF::Start(const fn_samples &fn) -> void
{
    rx_handle = fn;
    int err = 0;
    auto fnpass = [](hackrf_transfer *tx) {
        auto handle = (HackRF *)tx->rx_ctx;

        std::vector<std::complex<TComplex>> out_buf;
        out_buf.reserve(tx->valid_length);

        //Samples from HackRF
        typedef int8_t TSample;
        auto buf = (TSample*)tx->buffer;
        auto buf_len = tx->buffer_length / sizeof(TSample);

        //HackRF: TSample, scale up to TComplex
        for (auto x = 0; x < buf_len; x += 2)
        {
            out_buf.push_back({buf[x] / (float)std::numeric_limits<TSample>::max(), buf[x + 1] / (float)std::numeric_limits<TSample>::max()});
        }
        auto new_buf = Samples::Create(std::move(out_buf), handle->GetCenterFrequency(), handle->GetSampleRate());
        handle->rx_handle(new_buf);

        return 0;
    };
    if (HACKRF_SUCCESS != (err = hackrf_start_rx(device, fnpass, this)))
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }
}

auto HackRF::Close() -> void
{
    if (HACKRF_SUCCESS == hackrf_is_streaming(device))
    {
        hackrf_stop_rx(device);
        hackrf_stop_tx(device);
    }
    hackrf_close(device);
}

auto HackRF::IsStreaming() const -> bool
{
    int err = 0;
    if (HACKRF_TRUE != (err = hackrf_is_streaming(device)))
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }
    return true;
}

auto HackRF::SetCenterFrequency(const uint64_t &f) -> void
{
    auto err = hackrf_set_freq(device, f);
    if (err != HACKRF_SUCCESS)
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }
    center_frequency = f;
}

auto HackRF::SetSampleRate(const uint64_t &r) -> void
{
    auto err = hackrf_set_sample_rate(device, r);
    if (err != HACKRF_SUCCESS)
    {
        throw std::runtime_error(hackrf_error_name((hackrf_error)err));
    }
    sample_rate = r;
}

auto HackRF::SetGain(const uint16_t &lna, const uint16_t &vga) -> void
{
    if (lna % 8 != 0)
    {
        throw std::runtime_error("LNA gain must be in increments of 8dB");
    }
    if (lna > 40)
    {
        throw std::runtime_error("LNA gain must be less than 40dB");
    }
    if (vga % 2 != 0)
    {
        throw std::runtime_error("VGA gain must be in increments of 2dB");
    }
    if (vga > 62)
    {
        throw std::runtime_error("VGA gain must be less than 62dB");
    }
    hackrf_set_lna_gain(device, lna);
    hackrf_set_vga_gain(device, vga);
}