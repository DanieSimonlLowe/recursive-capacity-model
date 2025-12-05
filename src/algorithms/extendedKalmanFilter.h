#pragma once
#include <Eigen/Dense>
#include <stdexcept>

// https://www.mathworks.com/help/driving/ug/extended-kalman-filters.html
class  ExtendedKalmanFilter {
    // Asumeing noise is addtive
    public:
        ExtendedKalmanFilter(const Eigen::MatrixXd &processNoise, const Eigen::MatrixXd &measurementNoise, 
                            const Eigen::VectorXd &initialState, const Eigen::MatrixXd &initialCovariance);
        virtual ~ExtendedKalmanFilter() = default; // virtual destructor for base class

        virtual void predict();
        virtual void measure(const Eigen::VectorXd &measurement);
        const Eigen::VectorXd& getState();
        const Eigen::MatrixXd& getCovariance();
        void setState(const Eigen::VectorXd &state);

    protected:
        virtual Eigen::VectorXd predictionFunction(const Eigen::VectorXd &state) = 0;
        virtual Eigen::MatrixXd predictionJacobian(const Eigen::VectorXd &state) = 0;

        virtual Eigen::VectorXd measurementFunction(const Eigen::VectorXd &state) = 0;
        virtual Eigen::MatrixXd measurementJacobian(const Eigen::VectorXd &state) = 0;
        virtual void clampState() {}


        const Eigen::MatrixXd Q_; // process noise covariance
        const Eigen::MatrixXd R_; // measurement noise covariance
        Eigen::VectorXd x_; // state vector
        Eigen::MatrixXd P_; // state covariance
};