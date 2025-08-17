#pragma once
#include "interpolation/interpolatorBase.h"
#include "algorithms/extendedKalmanFilter.h"
#include <cmath>

class PolySineEKFInterpolator : public InterpolatorBase {
    public:
        PolySineEKFInterpolator(double ts, unsigned int polyDegree, double updateNoise, double measurementNoise, 
                                double initialError, double resetSeccoundsEquivalent);

        PolySineEKFInterpolator* clone() override;

        double predict(double time) override;
        void update(double measurement, double time) override;
        bool canPredict() override;
        void reset() override;
        void setParams(Eigen::VectorXd& params) override;
        size_t getParamsCount() override;


    private:

        class HelperEKF: public ExtendedKalmanFilter {
            public:
                using ExtendedKalmanFilter::ExtendedKalmanFilter; 
                Eigen::VectorXd predictionFunction(const Eigen::VectorXd &state) override;
                Eigen::MatrixXd predictionJacobian(const Eigen::VectorXd &state) override;
                Eigen::VectorXd measurementFunction(const Eigen::VectorXd &state) override;
                Eigen::MatrixXd measurementJacobian(const Eigen::VectorXd &state) override;
                void predict() override;
                

                double time;
                double lastPredictTime;
                bool hasPredicted = false;
                unsigned int polyDegree;
        };

        HelperEKF* ekf;
        unsigned int polyDegree;
        unsigned int updateCount;
        unsigned int stateCount;
        double meanValue;
        double totalVariance;
        double resetSeccoundsEquivalent;

        
};