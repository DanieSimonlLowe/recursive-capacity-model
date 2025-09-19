#pragma once
#include <type_traits>
#include "socOcv/SocOcvCurveBase.h"
#include "loading/readOcvSocCSV.h"

//concept ECMEstimatorType = std::is_base_of_v<SocOcvCurveBase, T>;
template<typename T>
class SocOcvCurveOptimizable {
    public:
        SocOcvCurveOptimizable();

        void display(const Eigen::VectorXd& params = Eigen::VectorXd());
        double getObjectiveValue(const Eigen::VectorXd& params = Eigen::VectorXd());

    private:
        T* makeModel(const OcvSocData& data, const Eigen::VectorXd& params);

        OcvSocData train;       // for training the model
        OcvSocData validation;  // for tuning and monitoring loss/metrics
        OcvSocData test;        // final unbiased evaluation
};

#include "socOcv/SocOcvCurveOptimizable.tpp"