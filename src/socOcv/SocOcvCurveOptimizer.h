#pragma once
#include <type_traits>
#include <cstddef>
#include <bayesopt/bayesopt.hpp>
#include "SocOcv/SocOcvCurveBase.h"
#include "Loading/ReadOcvSocCSV.h"


template<typename T>
class SocOcvCurveOptimizer {
    public:
        SocOcvCurveOptimizer();

        void display(const Eigen::VectorXd& params = Eigen::VectorXd());
        double getObjectiveValue(const Eigen::VectorXd& params = Eigen::VectorXd());
        void optimize();

    private:
        T* makeModel(const OcvSocData& data, const Eigen::VectorXd& params);
        

        OcvSocData train;       // for training the model
        OcvSocData validation;  // for tuning and monitoring loss/metrics
        OcvSocData test;        // final unbiased evaluation
};

#include "SocOcv/SocOcvCurveOptimizer.tpp"