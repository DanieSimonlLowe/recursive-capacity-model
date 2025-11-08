#include "Optimization/Optimizer.h"

class MSEOptimizer : public bayesopt::ContinuousModel {
private:
    HyperparameterOptimizable* baseOptimizable;
    Eigen::VectorXd lower;
    Eigen::VectorXd upper;

public:
    MSEOptimizer(HyperparameterOptimizable* optimizable) 
        : ContinuousModel(optimizable->getParamsCount(), bayesopt::Parameters()), 
          baseOptimizable(optimizable)
    {
        // Get bounds from the optimizable object
        std::vector<double> lowerBoundsVec = optimizable->getLowerBounds();
        std::vector<double> upperBoundsVec = optimizable->getUpperBounds();
        
        const size_t dim = optimizable->getParamsCount();
        
        // Validate bounds dimensions
        if (lowerBoundsVec.size() != dim || upperBoundsVec.size() != dim) {
            throw std::invalid_argument("Bounds dimensions must match optimizable parameter count");
        }
        
        // Convert to Eigen::VectorXd
        lower.resize(dim);
        upper.resize(dim);
        
        for (size_t i = 0; i < dim; ++i) {
            lower[i] = lowerBoundsVec[i];
            upper[i] = upperBoundsVec[i];
        }
        
        // Validate bounds
        for (int i = 0; i < lower.size(); ++i) {
            if (lower[i] >= upper[i]) {
                throw std::invalid_argument("Lower bound must be less than upper bound for all dimensions");
            }
        }

        // Configure BayesOpt parameters
        mParameters.alpha = 5e-3;               // Reduced alpha for better exploration
        mParameters.force_jump = 3;
    }
    
    // Fix 1: Use bayesopt::vectord instead of std::vector<double>
    double evaluateSample(const bayesopt::vectord& params) override {
        // Convert bayesopt::vectord to Eigen::VectorXd
        Eigen::VectorXd eigenParams(params.size());
        for (size_t i = 0; i < params.size(); ++i) {
            eigenParams[i] = lower[i] + params[i] * (upper[i] - lower[i]);
        }
        std::cout << "Params v:\n" << eigenParams << std::endl;
        
        auto* optimizable = baseOptimizable->clone();
        optimizable->setParams(eigenParams);
        
        ProcessNasaCycles("/mnt/c/Users/Danie/Desktop/project/data/B0006.mat", "B0006", *optimizable);
        
        // Get the objective value from the optimizable object
        // (assuming it has a method to return the objective value after processing)
        double objective = optimizable->getObjectiveValue();

        std::cout << "raw objective:" << objective << std::endl;
        if(objective > 1e100 || std::isnan(objective)) {
            objective = 1e100;
        }
        if (objective < 1e-12) {
            objective = 1e-12;
        }

        std::cout << "capped objective:" << objective << std::endl;
        
        delete optimizable;
        
        return objective;
    }
};

// Main function for general hyperparameter Optimization
Eigen::VectorXd bayesianOptimize(HyperparameterOptimizable& optimizable)
{
    MSEOptimizer optimizer(&optimizable);
    
    const size_t dim = optimizable.getParamsCount();
    
    // Fix 2: Use bayesopt::vectord instead of std::vectord
    bayesopt::vectord bestParams(dim);
    optimizer.optimize(bestParams);
    
    // Get bounds for conversion
    std::vector<double> lowerBoundsVec = optimizable.getLowerBounds();
    std::vector<double> upperBoundsVec = optimizable.getUpperBounds();
    
    std::cout << "Best params: ";
    for (size_t i = 0; i < bestParams.size(); ++i) {
        bestParams[i] = lowerBoundsVec[i] + bestParams[i] * (upperBoundsVec[i] - lowerBoundsVec[i]);
        std::cout << bestParams[i] << " ";
    }
    
    // Convert back to Eigen::VectorXd
    Eigen::VectorXd result(bestParams.size());
    for (size_t i = 0; i < bestParams.size(); ++i) {
        result[i] = bestParams[i];
    }

    optimizable.setParams(result);
    optimizable.display();
    
    return result;
}