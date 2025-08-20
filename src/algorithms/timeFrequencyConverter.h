#pragma once
#include <vector>
#include <complex>
#include <cmath>
#include <nfft3.h>
#include <algorithm>

class TimeFrequencyConverter {
    private:
        int N; // Number of frequency samples
        int M; // Number of time samples
        std::vector<double> times;
        std::vector<std::complex<double>> measurements;
        std::vector<std::complex<double>> frequency_coeffs;
        void timeToFrequency();
        nfft_plan plan; // Missing NFFT plan declaration

    public:
        TimeFrequencyConverter(const std::vector<double>& input_times, 
                        const std::vector<std::complex<double>>& input_measurements,
                        int freq_samples);

        ~TimeFrequencyConverter();
        std::complex<double> getValueAtTime(double target_time, int num_frequencies);
        std::vector<std::complex<double>> getFrequencyCoeffs();
        std::vector<double> getFrequencies();

}; 