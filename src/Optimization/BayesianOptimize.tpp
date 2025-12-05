

template<typename BatteryModel>
class MSEOptimizer : public bayesopt::ContinuousModel {
private:
    double capacity;
    Eigen::VectorXd lower;
    Eigen::VectorXd upper;
    ErrorMetric metric;

public:
    MSEOptimizer(double cap, ErrorMetric metric) 
        : ContinuousModel(BatteryModel::getParamsCount(), bayesopt::Parameters()), 
          capacity(cap), metric(metric)

    {
        // Get bounds from the BatteryModel static methods (already Eigen::VectorXd)
        lower = BatteryModel::getLowerBounds();
        upper = BatteryModel::getUpperBounds();
        
        const size_t dim = BatteryModel::getParamsCount();
        
        // Validate bounds dimensions
        if (lower.size() != dim || upper.size() != dim) {
            throw std::invalid_argument("Bounds dimensions must match parameter count");
        }
        
        // Validate bounds
        for (int i = 0; i < lower.size(); ++i) {
            if (lower[i] >= upper[i]) {
                throw std::invalid_argument("Lower bound must be less than upper bound for all dimensions");
            }
        }

        // Configure BayesOpt parameters
        mParameters.force_jump = 3;
        mParameters.n_init_samples = 20;
        mParameters.n_iterations = 130;
        //mParameters.noise = 1e-6;
    }
    
    double evaluateSample(const bayesopt::vectord& params) override {
        // Convert bayesopt::vectord to Eigen::VectorXd and scale to actual bounds
        Eigen::VectorXd eigenParams(params.size());
        for (size_t i = 0; i < params.size(); ++i) {
            eigenParams[i] = lower[i] + params[i] * (upper[i] - lower[i]);
        }
        std::cout << "Params v:\n" << eigenParams << std::endl;
        
        double objective = 1e100;
        try {
            // Create battery model with current parameters
            BatteryModel model(capacity, eigenParams, metric != ErrorMetric::CapacityError);
        
            // Process NASA cycles data
            ProcessNasaCycles("/mnt/c/Users/Danie/Desktop/project/data/B0006.mat", "B0006", model);
        
            // Get the objective value from the model
            objective = model.getObjectiveValue(metric);
        } catch (...) {
            std::cout << "Exception Caught ";
        }
        std::cout << "raw objective:" << objective << std::endl;
        
        // Cap extreme values
        if(objective > 1e100 || std::isnan(objective)) {
            objective = 1e100;
        }
        if (objective < 1e-12) {
            objective = 1e-12;
        }

        objective = std::log(objective);

        std::cout << "capped objective:" << objective << std::endl;
        
        return objective;
    }
};

// Main function for Bayesian hyperparameter optimization
template<typename BatteryModel>
Eigen::VectorXd BayesianOptimize(double capacity, ErrorMetric metric)
{
    MSEOptimizer<BatteryModel> optimizer(capacity, metric);
    
    const size_t dim = BatteryModel::getParamsCount();
    
    // Use bayesopt::vectord for optimization
    bayesopt::vectord bestParams(dim);
    optimizer.optimize(bestParams);
    
    // Get bounds for conversion back to actual parameter space
    Eigen::VectorXd lowerBounds = BatteryModel::getLowerBounds();
    Eigen::VectorXd upperBounds = BatteryModel::getUpperBounds();
    
    std::cout << "Best params: ";
    Eigen::VectorXd result(bestParams.size());
    for (size_t i = 0; i < bestParams.size(); ++i) {
        // Scale from [0,1] back to actual bounds
        result[i] = lowerBounds[i] + bestParams[i] * (upperBounds[i] - lowerBounds[i]);
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
    
    // Create final model with optimized parameters and display results
    BatteryModel finalModel(capacity, result);
    std::cout << "Final model created with optimized parameters" << std::endl;
    ProcessNasaCycles("/mnt/c/Users/Danie/Desktop/project/data/B0007.mat", "B0007", finalModel);
    finalModel.display(metric);
    
    return result;
}