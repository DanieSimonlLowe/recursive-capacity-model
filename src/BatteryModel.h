#pragma once
#include "loading/processNasaCycles.h"
#include "interpolation/interpolatorBase.h"
#include "loading/readOcvSocCSV.h"

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
class BatteryModel: public CycleHandler {
    public:
        BatteryModel(double capacity, Eigen::VectorXd& params);

        static size_t getParamsCount();
        
        static Eigen::VectorXd getLowerBounds();
        
        static Eigen::VectorXd getUpperBounds();
                
        void onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time);

        void onDischarge(const Eigen::VectorXd &voltage,
                                const Eigen::VectorXd &current,
                                const Eigen::VectorXd &temperature,
                                const Eigen::VectorXd &time,
                                const double &capacity);

        void onImpedance(double Rct, double Re);
    
    private:
        const double deltaTimeMult;
        const double capacityConfidenceThreshold;
        double deltaTime;
        double capacity;

        double totalVoltageErrorSq;
        int voltagePredictionCount;
        
        double totalCapacityErrorSq;
        int capacityPredictionCount;

        void processData(const Eigen::VectorXd &voltage,
                        const Eigen::VectorXd &current,
                        const Eigen::VectorXd &time);

        ECMStateEstimator ecmStateEstimator;
        VoltageInterpolator voltageInterpolator;
        CurrentInterpolator currentInterpolator;
        SocOcvCurve socOcvCurve;
        CapacityEstimator capacityEstimator;
        Eigen::VectorXd socEstimatorParams;
};

#include "BatteryModel.tpp"