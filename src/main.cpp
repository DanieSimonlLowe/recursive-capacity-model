#include "interpolation/interpolatorEvaluator.h"
#include "interpolation/twoPointLinearInterpolator.h"
#include "interpolation/polySineEKFInterpolator.h"
#include "interpolation/bayesianOptimizeMSE.h"

Eigen::VectorXd getPolySineEKFLowerBounds() {
    Eigen::VectorXd lower(5);
    
    // Parameter 0: polyDegree (unsigned int, polynomial order)
    // Reasonable range: 1-10 (higher orders can cause numerical instability)
    lower[0] = 1.0;
    
    // Parameter 1: updateNoise (process noise covariance scaling)
    // Very small positive value to avoid numerical issues
    // Too small -> filter doesn't adapt, too large -> unstable
    lower[1] = 1e-8;
    
    // Parameter 2: measurementNoise (observation noise variance)
    // Must be positive, very small values can cause filter divergence
    lower[2] = 1e-6;
    
    // Parameter 3: initialError (initial covariance scaling)
    // Must be positive, represents initial uncertainty
    lower[3] = 1e-6;
    
    // Parameter 4: resetSecondsEquivalent (time jump for reset)
    // Should be positive, represents time advancement
    lower[4] = 0.001;  // 1ms minimum
    
    return lower;
}

Eigen::VectorXd getPolySineEKFUpperBounds() {
    Eigen::VectorXd upper(5);
    
    // Parameter 0: polyDegree
    // Keep reasonable to avoid overfitting and numerical issues
    // For battery data, typically 3-6 is sufficient
    upper[0] = 6.0;  // Will be cast to int, so this means 0-8
    
    // Parameter 1: updateNoise
    // Upper bound depends on signal magnitude and dynamics
    // For normalized signals, 1.0 is quite large
    upper[1] = 1.0;
    
    // Parameter 2: measurementNoise
    // Should reflect actual measurement uncertainty
    // For battery voltage (typically 2-4V), 0.1V would be 2.5-5% error
    upper[2] = 0.1;
    
    // Parameter 3: initialError
    // Large initial uncertainty allows filter to adapt quickly
    // But too large can cause numerical issues
    upper[3] = 15.0;
    
    // Parameter 4: resetSecondsEquivalent
    // Depends on your data sampling rate and dynamics
    // For battery data sampled every few seconds, this could be hours
    upper[4] = 600.0;  //  hour maximum
    
    return upper;
}

int main(int argc, char **argv) { 

    

    PolySineEKFInterpolator interpolator(0.1, 2, 0.1, 0.1, 10, 10);
    bayesianOptimizeMSE(interpolator,MSEType::Voltage,getPolySineEKFLowerBounds(),getPolySineEKFUpperBounds());
}

    //(new InterpolatorEvaluator(new PolySineEKFInterpolator(0.1,2,0.1,0.1,10,10)))->printResults(); 

    //(new InterpolatorEvaluator(new TwoPointLinearInterpolator(0.1)))->printResults(); 