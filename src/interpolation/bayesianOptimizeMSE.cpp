#include "interpolation/bayesianOptimizeMSE.h"

class MSEOptimizer : public bayesopt::ContinuousModel {
private:
    InterpolatorBase* baseInterpolator;
    MSEType type;
    Eigen::VectorXd lower;
    Eigen::VectorXd upper;

public:
    MSEOptimizer(InterpolatorBase* interpolator, MSEType mseType, 
                 const Eigen::VectorXd& lower, const Eigen::VectorXd& upper) 
        : ContinuousModel(lower.size(), bayesopt::Parameters()), 
          baseInterpolator(interpolator), 
          type(mseType),
          lower(lower),
          upper(upper)
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

        // vectord lowerBound(lower.size());
        // vectord upperBound(lower.size());
        // for (int i = 0; i<lower.size(); i++) {
        //     lowerBound[i] = lower[i];
        //     upperBound[i] = upper[i];
        // }
        //setBoundingBox(lowerBound,upperBound);
                
        
        // // Configure BayesOpt parameters
        // mParameters.n_init_samples = 10;        // Increased for better initial coverage
        // mParameters.n_iterations = 100;         // Increased iterations
        // mParameters.noise = 1e-1;               // Reduced noise (was too high)
        mParameters.alpha = 5e-3;               // Reduced alpha for better exploration
        // mParameters.epsilon = 1e-8;             // Reduced epsilon for convergence
        
        // // FIXED: Use a more robust kernel
        // mParameters.kernel.name = "kMaternARD5"; // Changed from ARD to ISO for stability
        // mParameters.kernel.hp_mean[0] = 0.5 * (upperBounds - lowerBounds).norm();
        // mParameters.kernel.hp_std[0]  = mParameters.kernel.hp_mean[0];
        
        // mParameters.verbose_level = 3;          // Increased verbosity for debugging
        // mParameters.crit_name = "cLCB";          // Expected Improvement
        mParameters.force_jump = 3;
        
        // // FIXED: Add learning parameters
        // mParameters.l_type = L_EMPIRICAL;            // Use MCMC for learning
        // mParameters.sc_type = SC_ML;           // Use MAP for surrogate
        
        
        
    }
    
    double evaluateSample(const vectord& params) override {
        // Convert vectord to Eigen::VectorXd
        Eigen::VectorXd eigenParams(params.size());
        for (size_t i = 0; i < params.size(); ++i) {
            eigenParams[i] = lower[i] + params[i] * (upper[i] - lower[i]);
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
        // double mse = 0.0;
        // for (size_t i = 0; i < params.size(); ++i) {
        //     mse += params[i] + 5;
        // }

        std::cout << "raw MSE:" << mse << std::endl;
        bool isCaped = false;
        if(mse > 1e100 || std::isnan(mse)) {
            mse = 1e100;
            isCaped = true;
        }
        if (mse < 1e-12) {
            mse = 1e-12;
            isCaped = true;
        }
        double tran_mse = std::sqrt(mse);

        if (isCaped) {
            tran_mse += (((double)rand() / RAND_MAX) - 0.5) * 1e-6;
        }

        std::cout << "capped log MSE:" << tran_mse << std::endl;
        
        //delete interp;
        
        return tran_mse;
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
    
    //gridTest(optimizer,lowerBounds, upperBounds);
        
    // Run optimization
    vectord bestParams(dim);
    optimizer.optimize(bestParams);
    
    // Get results from our internal tracking
    
    
    std::cout << "Best params: ";
    for (size_t i = 0; i < bestParams.size(); ++i) {
        bestParams[i] = lowerBounds[i] + bestParams[i] * (upperBounds[i] - lowerBounds[i]);
        std::cout << bestParams[i] << " ";
    }

    

    
    // Convert back to Eigen::VectorXd
    Eigen::VectorXd result(bestParams.size());
    for (size_t i = 0; i < bestParams.size(); ++i) {
        result[i] = bestParams[i];
    }

    interpolator.setParams(result);
    InterpolatorEvaluator evaluator(&interpolator);
    evaluator.printResults();
    
    return result;
}
