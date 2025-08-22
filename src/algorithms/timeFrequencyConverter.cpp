#include "algorithms/timeFrequencyConverter.h"
#include <iostream>

TimeFrequencyConverter::TimeFrequencyConverter(const std::vector<double>& input_times, 
                        const std::vector<std::complex<double>>& input_measurements,
                        int freq_samples) {
    
    times = input_times;
    measurements = input_measurements;
    M = times.size();
    
    // If frequency samples not specified, use next power of 2
    if (freq_samples == 0) {
        N = 1;
        while (N < M) N *= 2;
    } else {
        N = freq_samples;
    }
    
    frequency_coeffs.resize(N, std::complex<double>(0.0, 0.0));
    
    // Initialize NFFT plan
    nfft_init_1d(&plan, N, M);
    
    // Normalize times to [-0.5, 0.5) for NFFT
    double min_time = times.front();
    double max_time = times.back();
    time_range = max_time - min_time;
    time_offset = min_time;
    
    for (int j = 0; j < M; j++) {
        plan.x[j] = (times[j] - min_time) / time_range - 0.5;
    }
    
    // Precompute if necessary
    if (plan.flags & PRE_ONE_PSI) {
        nfft_precompute_one_psi(&plan);
    }
    
    // Perform the time-to-frequency conversion
    timeToFrequency();
}

TimeFrequencyConverter::~TimeFrequencyConverter() {
    nfft_finalize(&plan);
}

// Convert time domain measurements to frequency domain
void TimeFrequencyConverter::timeToFrequency() {
    // Copy measurements to NFFT plan
    for (int j = 0; j < M; j++) {
        plan.f[j][0] = measurements[j].real();
        plan.f[j][1] = measurements[j].imag();
    }
    
    // Perform adjoint NFFT (time to frequency)
    nfft_adjoint(&plan);
    
    // Copy results with proper scaling
    // NFFT adjoint needs to be scaled by the time step for proper normalization
    double dt = time_range / N; // Effective time step for frequency domain
    
    for (int k = 0; k < N; k++) {
        frequency_coeffs[k] = std::complex<double>(plan.f_hat[k][0], plan.f_hat[k][1]) * dt;
    }
}

// Get measurement value at arbitrary time using frequency domain interpolation
std::complex<double> TimeFrequencyConverter::getValueAtTime(double target_time, int num_frequencies) {
    // If num_frequencies is 0 or greater than N, use all frequencies
    int frequencies_to_use = (num_frequencies <= 0 || num_frequencies > N) ? N : num_frequencies;
    
    if (frequencies_to_use == 0) {
        return std::complex<double>(0.0, 0.0);
    }
    
    // Create pairs of (magnitude, index) for sorting if we need to limit frequencies
    if (frequencies_to_use < N) {
        std::vector<std::pair<double, int>> freq_magnitudes;
        for (int k = 0; k < N; k++) {
            double magnitude = std::abs(frequency_coeffs[k]);
            freq_magnitudes.push_back(std::make_pair(magnitude, k));
        }
        
        // Sort by magnitude in descending order
        std::sort(freq_magnitudes.begin(), freq_magnitudes.end(), 
                  [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                      return a.first > b.first;
                  });
        
        // Evaluate Fourier series using only top frequencies
        std::complex<double> result(0.0, 0.0);
        
        for (int i = 0; i < frequencies_to_use; i++) {
            int k = freq_magnitudes[i].second;
            int freq_index = (k <= N/2) ? k : k - N; // Handle negative frequencies
            double frequency = 2.0 * M_PI * freq_index / time_range;
            
            std::complex<double> exponential(cos(frequency * (target_time - time_offset)), 
                                            sin(frequency * (target_time - time_offset)));
            result += frequency_coeffs[k] * exponential;
        }
        
        return result;
    } else {
        // Use all frequencies - more efficient direct calculation
        std::complex<double> result(0.0, 0.0);
        
        for (int k = 0; k < N; k++) {
            int freq_index = (k <= N/2) ? k : k - N; // Handle negative frequencies
            double frequency = 2.0 * M_PI * freq_index / time_range;
            
            std::complex<double> exponential(cos(frequency * (target_time - time_offset)), 
                                            sin(frequency * (target_time - time_offset)));
            result += frequency_coeffs[k] * exponential;
        }
        
        return result;
    }
}

// Get frequency domain coefficients
std::vector<std::complex<double>> TimeFrequencyConverter::getFrequencyCoeffs() {
    return frequency_coeffs;
}

// Get frequency values corresponding to coefficients
std::vector<double> TimeFrequencyConverter::getFrequencies() {
    std::vector<double> freqs(N);
    
    for (int k = 0; k < N; k++) {
        int freq_index = (k <= N/2) ? k : k - N;
        freqs[k] = freq_index / time_range; // Convert to actual frequency units
    }
    return freqs;
}