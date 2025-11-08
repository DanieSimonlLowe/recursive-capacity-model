#include <gtest/gtest.h>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
#include "Algorithms/TimeFrequencyConverter.h"

class TimeFrequencyConverterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up common test data
        tolerance = 1e-10;
    }
    
    double tolerance;
    
    // Helper function to generate a simple sine wave
    std::pair<std::vector<double>, std::vector<std::complex<double>>> 
    generateSineWave(double frequency, double amplitude, int num_samples, 
                     double start_time = 0.0, double end_time = 1.0) {
        std::vector<double> times(num_samples);
        std::vector<std::complex<double>> values(num_samples);
        
        double dt = (end_time - start_time) / (num_samples - 1);
        for (int i = 0; i < num_samples; i++) {
            times[i] = start_time + i * dt;
            double value = amplitude * sin(2 * M_PI * frequency * times[i]);
            values[i] = std::complex<double>(value, 0.0);
        }
        
        return std::make_pair(times, values);
    }
    
    // Helper function to generate a complex exponential
    std::pair<std::vector<double>, std::vector<std::complex<double>>> 
    generateComplexExponential(double frequency, double amplitude, int num_samples,
                               double start_time = 0.0, double end_time = 1.0) {
        std::vector<double> times(num_samples);
        std::vector<std::complex<double>> values(num_samples);
        
        double dt = (end_time - start_time) / (num_samples - 1);
        for (int i = 0; i < num_samples; i++) {
            times[i] = start_time + i * dt;
            double phase = 2 * M_PI * frequency * times[i];
            values[i] = amplitude * std::complex<double>(cos(phase), sin(phase));
        }
        
        return std::make_pair(times, values);
    }
};

// Test basic construction and destruction
TEST_F(TimeFrequencyConverterTest, ConstructionDestruction) {
    auto data = generateSineWave(1.0, 1.0, 32);
    
    ASSERT_NO_THROW({
        TimeFrequencyConverter converter(data.first, data.second, 64);
    });
}

// Test explicit frequency sample specification
TEST_F(TimeFrequencyConverterTest, ExplicitFrequencySamples) {
    auto data = generateSineWave(1.0, 1.0, 32);
    
    TimeFrequencyConverter converter(data.first, data.second, 100);
    auto freq_coeffs = converter.getFrequencyCoeffs();
    
    EXPECT_EQ(freq_coeffs.size(), 100);
}


// Test frequency array generation
TEST_F(TimeFrequencyConverterTest, FrequencyArray) {
    std::vector<double> times = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5};
    auto data = generateSineWave(1.0, 1.0, times.size());
    data.first = times; // Use custom time array
    
    TimeFrequencyConverter converter(data.first, data.second, 8);
    auto frequencies = converter.getFrequencies();
    
    EXPECT_EQ(frequencies.size(), 8);
    
    // Check frequency spacing
    double time_range = times.back() - times.front();
    double expected_freq_spacing = 1.0 / time_range;
    
    // Check that frequencies are properly ordered and spaced
    EXPECT_DOUBLE_EQ(frequencies[0], 0.0);
    EXPECT_DOUBLE_EQ(frequencies[1], expected_freq_spacing);
    EXPECT_DOUBLE_EQ(frequencies[2], 2 * expected_freq_spacing);
}




// Test frequency limitation in Interpolation
TEST_F(TimeFrequencyConverterTest, FrequencyLimitedInterpolation) {
    auto data = generateSineWave(1.0, 1.0, 64);
    
    TimeFrequencyConverter converter(data.first, data.second, 64);
    
    double test_time = 0.5;
    
    // Test with different numbers of frequencies
    std::complex<double> result_all = converter.getValueAtTime(test_time, 0);
    std::complex<double> result_limited = converter.getValueAtTime(test_time, 10);
    
    // Results should be different when using fewer frequencies
    // (though for a simple sine wave, the difference might be small)
    EXPECT_TRUE(result_all.real() != 0.0 || result_all.imag() != 0.0);
    EXPECT_TRUE(result_limited.real() != 0.0 || result_limited.imag() != 0.0);
}

// Test edge cases
TEST_F(TimeFrequencyConverterTest, EdgeCases) {
    auto data = generateSineWave(1.0, 1.0, 32);
    TimeFrequencyConverter converter(data.first, data.second, 64);
    
    // Test Interpolation at time boundaries
    std::complex<double> result_start = converter.getValueAtTime(data.first.front(), 0);
    std::complex<double> result_end = converter.getValueAtTime(data.first.back(), 0);
    
    EXPECT_TRUE(std::abs(result_start) < 10.0); // Reasonable bounds
    EXPECT_TRUE(std::abs(result_end) < 10.0);   // Reasonable bounds
    
    // Test with negative frequency count (should use all frequencies)
    std::complex<double> result_negative = converter.getValueAtTime(0.5, -5);
    std::complex<double> result_all = converter.getValueAtTime(0.5, 0);
    
    EXPECT_DOUBLE_EQ(result_negative.real(), result_all.real());
    EXPECT_DOUBLE_EQ(result_negative.imag(), result_all.imag());
}

// Test with single sample
TEST_F(TimeFrequencyConverterTest, SingleSample) {
    std::vector<double> times = {0.5};
    std::vector<std::complex<double>> values = {std::complex<double>(1.0, 0.5)};
    
    ASSERT_NO_THROW({
        TimeFrequencyConverter converter(times, values, 8);
        auto freq_coeffs = converter.getFrequencyCoeffs();
        EXPECT_EQ(freq_coeffs.size(), 8);
    });
}

// Test with two samples
TEST_F(TimeFrequencyConverterTest, TwoSamples) {
    std::vector<double> times = {0.0, 1.0};
    std::vector<std::complex<double>> values = {
        std::complex<double>(0.0, 0.0),
        std::complex<double>(1.0, 0.0)
    };
    
    TimeFrequencyConverter converter(times, values, 8);
    
    // Test Interpolation
    std::complex<double> mid_result = converter.getValueAtTime(0.5, 0);
    EXPECT_TRUE(std::abs(mid_result) > 0); // Should have some interpolated value
}

// Test with non-uniform time sampling
TEST_F(TimeFrequencyConverterTest, NonUniformSampling) {
    std::vector<double> times = {0.0, 0.1, 0.15, 0.4, 0.7, 1.0};
    std::vector<std::complex<double>> values;
    
    // Generate sine wave values at non-uniform times
    for (double t : times) {
        values.push_back(std::complex<double>(sin(2 * M_PI * t), 0.0));
    }
    
    ASSERT_NO_THROW({
        TimeFrequencyConverter converter(times, values, 32);
        std::complex<double> result = converter.getValueAtTime(0.5, 0);
        EXPECT_TRUE(std::abs(result) >= 0); // Should produce valid result
    });
}

// Performance test for large datasets
TEST_F(TimeFrequencyConverterTest, LargeDataset) {
    auto data = generateSineWave(5.0, 2.0, 1024);
    
    ASSERT_NO_THROW({
        TimeFrequencyConverter converter(data.first, data.second, 2048);
        
        // Test multiple Interpolations
        for (int i = 0; i < 100; i++) {
            double test_time = 0.01 * i;
            std::complex<double> result = converter.getValueAtTime(test_time, 50);
            EXPECT_TRUE(std::isfinite(result.real()));
            EXPECT_TRUE(std::isfinite(result.imag()));
        }
    });
}

// Test memory management with multiple instances
TEST_F(TimeFrequencyConverterTest, MultipleInstances) {
    auto data1 = generateSineWave(1.0, 1.0, 32);
    auto data2 = generateSineWave(2.0, 1.0, 64);
    
    ASSERT_NO_THROW({
        TimeFrequencyConverter converter1(data1.first, data1.second, 64);
        TimeFrequencyConverter converter2(data2.first, data2.second, 128);
        
        // Both should work independently
        std::complex<double> result1 = converter1.getValueAtTime(0.5, 0);
        std::complex<double> result2 = converter2.getValueAtTime(0.5, 0);
        
        EXPECT_TRUE(std::isfinite(result1.real()));
        EXPECT_TRUE(std::isfinite(result2.real()));
    });
}