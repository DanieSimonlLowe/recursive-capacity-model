#pragma once
#include "Loading/ProcessNasaCycles.h"
#include "Interpolation/InterpolatorBase.h"
#include "Loading/ReadOcvSocCSV.h"

enum ErrorMetric {
    VoltageError,
    CapacityError,
    ResistanceError
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
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
        const double capacityConfidenceThreshold;
        double deltaTime;

        bool socEstimatorSetup;

        double totalVoltageErrorSq;
        int voltagePredictionCount;
        double meanVoltage;
        double totalVoltageVariance;
        
        double totalCapacityErrorSq;
        int capacityPredictionCount;
        double meanCapacity;
        double totalCapacityVariance;

        double totalResistanceErrorSq;
        int resistancePredictionCount;
        double meanResistance;
        double totalResistanceVariance;

        void processData(const Eigen::VectorXd &voltage,
                        const Eigen::VectorXd &current,
                        const Eigen::VectorXd &time);

        ECMStateEstimator ecmStateEstimator;
        VoltageInterpolator voltageInterpolator;
        CurrentInterpolator currentInterpolator;
        SocOcvCurve socOcvCurve;
        CapacityEstimator capacityEstimator;
        SocEstimator socEstimator;
};

#include "BatteryModel.tpp"