#pragma once
#include "Loading/ProcessNasaCycles.h"
#include "Interpolation/InterpolatorBase.h"
#include "Loading/ReadOcvSocCSV.h"

enum ErrorMetric {
    VoltageError,
    CapacityError,
    ResistanceError
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
class BatteryModel: public CycleHandler {
    public:
        BatteryModel(double capacity, Eigen::VectorXd& params, bool useMeasuredCapacity=false);

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

        double getObjectiveValue(ErrorMetric metric) const;

        void display(ErrorMetric metric) const;
    
    private:
        const bool useMeasuredCapacity;
        const double deltaTimeMult;
        const int socCountThreshold;
        double deltaTime;
        double maxTimeDiffMult;

        bool socEstimatorSetupEcm;
        bool socEstimatorSetupOcv;
        int countSinceOcv;

        double totalVoltageErrorSq = 0;
        int voltagePredictionCount = 0;
        double meanVoltage = 0;
        double totalVoltageVariance = 0;
        int missedVoltagePredictionCount = 0;
        
        double initalCapacity = 0;
        double totalCapacityErrorSq = 0;
        int capacityPredictionCount = 0;
        double meanCapacity = 0;
        double totalCapacityVariance = 0;
        int missedCapacityPredictionCount = 0;

        double totalResistanceErrorSq = 0;
        int resistancePredictionCount = 0;
        double meanResistance = 0;
        double totalResistanceVariance = 0;
        int missedResistancePredictionCount = 0;

        void processData(const Eigen::VectorXd &voltage,
                        const Eigen::VectorXd &current,
                        const Eigen::VectorXd &time);

        ECMStateEstimator ecmStateEstimator;
        VoltageInterpolator voltageInterpolator;
        CurrentInterpolator currentInterpolator;
        SocOcvCurve socOcvCurve;
        CapacityEstimator capacityEstimator;
        SocEstimator socEstimator;
        OcvEstimator ocvEstimator;
};

#include "BatteryModel.tpp"
