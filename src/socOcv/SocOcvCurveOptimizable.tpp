#define DATA_SPLIT 6

template<typename T>
SocOcvCurveOptimizable<T>::SocOcvCurveOptimizable() {
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
T* SocOcvCurveOptimizable<T>::makeModel(const OcvSocData& data, const Eigen::VectorXd& params) {
    return new T(data, params);
}

template<typename T>
double SocOcvCurveOptimizable<T>::getObjectiveValue(const Eigen::VectorXd& params) {
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
void SocOcvCurveOptimizable<T>::display(const Eigen::VectorXd& params) {
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