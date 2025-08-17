#include "interpolation/bayesianOptimizeMSE.h"

class MSEOptimizer : public bayesopt::ContinuousModel {
private:
    InterpolatorBase* baseInterpolator;
    MSEType type;
    mutable vectord bestParams;
    mutable double bestValue;
    mutable bool hasBest;
    Eigen::VectorXd lowerBounds;
    Eigen::VectorXd upperBounds;

public:
    MSEOptimizer(InterpolatorBase* interpolator, MSEType mseType, 
                 const Eigen::VectorXd& lower, const Eigen::VectorXd& upper) 
        : ContinuousModel(lower.size(), bayesopt::Parameters()), 
          baseInterpolator(interpolator), 
          type(mseType),
          bestValue(std::numeric_limits<double>::max()),
          hasBest(false),
          lowerBounds(lower),
          upperBounds(upper)
    {
        if (lower.size() != upper.size()) {
            throw std::invalid_argument("Lower and upper bounds must have the same size");
        }
        
        // Validate bounds
        for (int i = 0; i < lower.size(); ++i) {
            if (lower[i] >= upper[i]) {
                throw std::invalid_argument("Lower bound must be less than upper bound for all dimensions");
            }
        }
        
        bestParams.resize(lower.size());
        
        // Configure BayesOpt parameters
        mParameters.n_init_samples = 10;
        mParameters.n_iterations = 50;
        mParameters.noise = 0.001;
        mParameters.alpha = 0.5;
        mParameters.kernel.name = "kMaternISO5";
        mParameters.verbose_level = 1;
        
        // Convert Eigen bounds to BayesOpt vectord format
        vectord bayesLower(lower.size());
        vectord bayesUpper(upper.size());
        
        for (int i = 0; i < lower.size(); ++i) {
            bayesLower[i] = lower[i];
            bayesUpper[i] = upper[i];
        }
        
        setBoundingBox(bayesLower, bayesUpper);
    }
    
    double evaluateSample(const vectord& params) override {
        // Convert vectord to Eigen::VectorXd
        Eigen::VectorXd eigenParams(params.size());
        for (size_t i = 0; i < params.size(); ++i) {
            eigenParams[i] = params[i];
        }
        std::cout << "Params v:\n" << eigenParams << std::endl;
        
        auto* interp = baseInterpolator->clone();
        interp->setParams(eigenParams);
        
        InterpolatorEvaluator evaluator(interp);

        processNasaCycles("/mnt/c/Users/Danie/Desktop/project/data/B0006.mat", "B0006", evaluator);
        
        double mse = 0.0;
        switch (type) {
            case MSEType::Voltage:     
                mse = evaluator.getVoltageMSE(); 
                break;
            case MSEType::Current:     
                mse = evaluator.getCurrentMSE(); 
                break;
            case MSEType::Temperature: 
                mse = evaluator.getTemperatureMSE(); 
                break;
        }
        std::cout << "MSE:" << mse << std::endl;
        if (mse > 1e25) {
            mse = 1e25;
        }
        
        delete interp;
        
        // Track the best result
        if (mse < bestValue) {
            bestValue = mse;
            bestParams = params;
            hasBest = true;
        }
        
        return mse;
    }
    
    vectord getBestParams() const { return bestParams; }
    double getBestValue() const { return bestValue; }
    bool hasValidResult() const { return hasBest; }
    
    bool checkReachability(const vectord& query) override {
        // Check if query is within bounds
        for (size_t i = 0; i < query.size(); ++i) {
            if (query[i] < lowerBounds[i] || query[i] > upperBounds[i]) {
                return false;
            }
        }
        return true;
    }
    
    // Helper to get initial point within bounds
    vectord getInitialPoint() const {
        vectord initial(lowerBounds.size());
        for (int i = 0; i < lowerBounds.size(); ++i) {
            // Start at the center of each dimension's bounds
            initial[i] = (lowerBounds[i] + upperBounds[i]) / 2.0;
        }
        return initial;
    }
};

// Main function with configurable bounds
Eigen::VectorXd bayesianOptimizeMSE(
    InterpolatorBase& interpolator,
    MSEType mseType,
    const Eigen::VectorXd& lowerBounds,
    const Eigen::VectorXd& upperBounds)
{
    const size_t dim = interpolator.getParamsCount();
    
    // Validate input dimensions
    if (lowerBounds.size() != dim || upperBounds.size() != dim) {
        throw std::invalid_argument("Bounds dimensions must match interpolator parameter count");
    }
    
    MSEOptimizer optimizer(&interpolator, mseType, lowerBounds, upperBounds);
    
    // Initial point (center of search space)
    vectord initialPoint = optimizer.getInitialPoint();
    
    // Run optimization
    optimizer.optimize(initialPoint);
    
    // Get results from our internal tracking
    vectord bestParams;
    double bestMSE;
    
    if (optimizer.hasValidResult()) {
        bestParams = optimizer.getBestParams();
        bestMSE = optimizer.getBestValue();
    } else {
        // Fallback: use the initial point
        bestParams = initialPoint;
        bestMSE = optimizer.evaluateSample(initialPoint);
    }
    
    std::cout << "Best params: ";
    for (size_t i = 0; i < bestParams.size(); ++i) {
        std::cout << bestParams[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Best MSE: " << bestMSE << "\n";
    
    // Convert back to Eigen::VectorXd
    Eigen::VectorXd result(bestParams.size());
    for (size_t i = 0; i < bestParams.size(); ++i) {
        result[i] = bestParams[i];
    }
    
    return result;
}
