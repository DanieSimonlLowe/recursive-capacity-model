#define DATA_SPLIT 6

template<typename T>
SocOcvCurveOptimizer<T>::SocOcvCurveOptimizer() {
    OcvSocData data = readOcvSocCSV("/mnt/c/Users/Danie/Desktop/project/data/soc_volt_nasa.csv");
    
    int n = data.soc.size();
    
    // Count elements for each set using modulo
    int train_count = 0, test_count = 0, validation_count = 0;
    
    int part_count = n / DATA_SPLIT;
    int part_mod = n % DATA_SPLIT;

    validation_count = part_count;
    if (part_mod <= (DATA_SPLIT-2)) {
        train_count = part_count * (DATA_SPLIT-2) + part_mod;
        test_count = part_count;
    } else {
        train_count = part_count * (DATA_SPLIT-2) + (DATA_SPLIT-2);
        test_count = part_count + 1;
    }

    train.soc = Eigen::VectorXd(train_count);
    train.ocv = Eigen::VectorXd(train_count);

    test.soc = Eigen::VectorXd(test_count);
    test.ocv = Eigen::VectorXd(test_count);

    validation.soc = Eigen::VectorXd(validation_count);
    validation.ocv = Eigen::VectorXd(validation_count);

    int train_idx = 0, test_idx = 0, validation_idx = 0;
    
    for (int i = 0; i < n; ++i) {
        int mod_result = i % DATA_SPLIT;
        
        if (mod_result < (DATA_SPLIT-2)) {  // Train set (positions 0, 1, 2, 3 mod 6)
            train.soc(train_idx) = data.soc(i);  // Fixed: was lt.train.soc
            train.ocv(train_idx) = data.ocv(i);
            train_idx++;
        } else if (mod_result == (DATA_SPLIT-2)) {  // Test set (position 4 mod 6)
            test.soc(test_idx) = data.soc(i);
            test.ocv(test_idx) = data.ocv(i);
            test_idx++;
        } else {  // Validation set (position 5 mod 6)
            validation.soc(validation_idx) = data.soc(i);
            validation.ocv(validation_idx) = data.ocv(i);
            validation_idx++;
        }
    }
}

template<typename T>
T* SocOcvCurveOptimizer<T>::makeModel(const OcvSocData& data, const Eigen::VectorXd& params) {
    return new T(data, params);
}

template<typename T>
double SocOcvCurveOptimizer<T>::getObjectiveValue(const Eigen::VectorXd& params) {
    T* model = makeModel(train, params);

    int n = validation.soc.size(); 
    double MSE = 0;
    for (int i = 0; i < n; i++) {
        double pred_ocv = model->getOcv(validation.soc(i));  // Fixed: added (i) and ->
        MSE += (pred_ocv - validation.ocv(i)) * (pred_ocv - validation.ocv(i));  // Fixed: added (i)
    }
    
    delete model;  // Clean up dynamically allocated memory
    return MSE / n;
}

template<typename T>
void SocOcvCurveOptimizer<T>::display(const Eigen::VectorXd& params) {
    T* model = makeModel(train, params);

    int n = test.soc.size(); 
    double MSE = 0;
    double mean = test.ocv.mean();
    double variance = 0;  // Fixed: removed incomplete syntax
    
    for (int i = 0; i < n; i++) {
        double pred_ocv = model->getOcv(test.soc(i));  // Fixed: use test.soc, added (i) and ->
        MSE += (pred_ocv - test.ocv(i)) * (pred_ocv - test.ocv(i));  // Fixed: added (i)
        variance += (test.ocv(i) - mean) * (test.ocv(i) - mean);  // Fixed: added (i)
    }
    MSE = MSE / n; 
    variance = variance / n;
    
    delete model;  // Clean up dynamically allocated memory

    double R2_leftover = MSE / variance; // because easier to read when gets very close to 1.

    std::cout << "OCV variance \t\t " << variance << "\n";  // Fixed: was MSE
    std::cout << "OCV mean \t\t " << mean << "\n";
    std::cout << "Mean squared error \t " << MSE << "\n";  // Fixed: spelling
    std::cout << "1 - R^2 \t\t " << R2_leftover << "\n";  // Fixed: was MSE
}


template<typename T>
class BayesOptObjective : public bayesopt::ContinuousModel {
public:
    BayesOptObjective(SocOcvCurveOptimizer<T>* optimizer) : 
        ContinuousModel(T::getParamsCount(), bayesopt::Parameters()), optimizer_(optimizer) {
        
        // Set parameter bounds
        lower_bounds_ = T::getLowerBounds();
        upper_bounds_ = T::getUpperBounds();
        
        mParameters.n_iterations = 250;        // Reduce iterations
        mParameters.n_init_samples = 50;       // More initial exploration
        //mParameters.alpha = 0.1;               // Increase exploration
        mParameters.noise = 1e-6;              // Add noise to prevent overfitting
        
        mParameters.surr_name = "sStudentTProcessNIG";  
        mParameters.force_jump = 3;

        // rest are defult.
        
        // Set random seed for reproducibility
        //mParameters.random_seed = 1;
    }
    
    double evaluateSample(const vectord& x) override {
        // Convert vectord to Eigen::VectorXd
        Eigen::VectorXd params(x.size());
        for(size_t i = 0; i < x.size(); ++i) {
            params(i) = x[i] * (upper_bounds_[i] - lower_bounds_[i]) + lower_bounds_[i];
        }
        
        // Call the optimizer's objective function
        double result = optimizer_->getObjectiveValue(params);

        if (!std::isfinite(result) || result > 1e8) {
            return 1e8; // Return large penalty for invalid results
        }

        return result;
    }
    
private:
    SocOcvCurveOptimizer<T>* optimizer_;
    Eigen::VectorXd lower_bounds_;
    Eigen::VectorXd upper_bounds_;
};

template<typename T>
void SocOcvCurveOptimizer<T>::optimize() {
    std::cout << "Starting Bayesian Optimization..." << std::endl;

    BayesOptObjective<T> objective(this);

    vectord result(T::getParamsCount());
    objective.optimize(result);
    
    // Convert result back to Eigen format
    Eigen::VectorXd lower_bounds = T::getLowerBounds();
    Eigen::VectorXd upper_bounds = T::getUpperBounds();
    Eigen::VectorXd optimal_params(T::getParamsCount());
    for(int i = 0; i < T::getParamsCount(); ++i) {
        optimal_params(i) = result[i] * (upper_bounds[i] - lower_bounds[i]) + lower_bounds[i];
    }
    
    // Display optimization results
    std::cout << "\nOptimization completed!" << std::endl;
    std::cout << "Optimal parameters:" << std::endl;

    
    for(int i = 0; i < T::getParamsCount(); ++i) {
        std::cout << "  param[" << i << "] = " << optimal_params(i) << std::endl;
    }
    
    double final_objective = getObjectiveValue(optimal_params);
    std::cout << "Final objective value (MSE): " << final_objective << std::endl;
    
    // Display final model performance on test set
    std::cout << "\nFinal model performance on test set:" << std::endl;
    display(optimal_params);
}