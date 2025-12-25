#pragma once
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include "SocOcv/SocOcvCurveBase.h"
#include "SocEstimation/SocEstimator.h"
#include "Algorithms/ExtendedKalmanFilter.h"

// https://www.mdpi.com/2313-0105/9/12/583
class EkfSocEstimator : public SocEstimator {
    public:
        EkfSocEstimator(const int dimension, SocOcvCurveBase* SocOcvCurve, const Eigen::VectorXd& params);
    

        double predictVoltage(const double current, const double deltaTime) override;

        void measure(const double current, const double voltage) override;
        double getSoc() override;
        void setOcv(const double ocv) override;

        
        static size_t getParamsCount();
        static const Eigen::VectorXd getLowerBounds();
        static const Eigen::VectorXd getUpperBounds();
    
    private:
        const int dimension;
        SocOcvCurveBase* SocOcvCurve;

        class HelperEKF: public ExtendedKalmanFilter {
            public:
                HelperEKF(const Eigen::MatrixXd &processNoise, const Eigen::MatrixXd &measurementNoise, 
                            const Eigen::VectorXd &initialState, const Eigen::MatrixXd &initialCovariance,
                            const int dimension, SocOcvCurveBase* SocOcvCurve, EkfSocEstimator* parent
                        );

                Eigen::VectorXd predictionFunction(const Eigen::VectorXd &state) override;
                Eigen::MatrixXd predictionJacobian(const Eigen::VectorXd &state) override;
                Eigen::VectorXd measurementFunction(const Eigen::VectorXd &state) override;
                Eigen::MatrixXd measurementJacobian(const Eigen::VectorXd &state) override;
                void clampState() override;
                
                void setUpdateState(const double dt, const double c);
                double deltaTime;
                double current;

                const int dimension;
                SocOcvCurveBase* SocOcvCurve;
                Eigen::VectorXd measurementVector;
                EkfSocEstimator* parent;

        };  

        HelperEKF* ekf;
};