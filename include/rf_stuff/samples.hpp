#pragma once

#include <rf_stuff/const.hpp>

#include <vector>
#include <complex>
#include <memory>
#include <iostream>
#include <iomanip>

namespace rf_stuff
{
    class Samples
    {
    public:
        static auto Create(const std::vector<std::complex<TComplex>> &&samples, const uint64_t &center_freq, const uint64_t &sample_rate)
            -> std::shared_ptr<Samples>
        {
            auto r_new = new Samples(std::move(samples), center_freq, sample_rate);
            return std::shared_ptr<Samples>(r_new);
        }

        const std::vector<std::complex<TComplex>> samples;
        const uint64_t center_frequency, sample_rate;

    private:
        Samples(const std::vector<std::complex<TComplex>> &&samples, const uint64_t &center_freq, const uint64_t &sample_rate)
            : samples(samples), center_frequency(center_freq), sample_rate(sample_rate)
        {
        }
    };
} // namespace rf_stuff