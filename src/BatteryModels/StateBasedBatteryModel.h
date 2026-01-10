#pragma once

#include "BatteryModels/BatteryModelBase.h"
#include "Interpolation/InterpolatorBase.h"
#include "Loading/ProcessNasaCycles.h"
#include "Loading/ReadOcvSocCsv.h"
#include <Eigen/Dense>

template <typename EcmStateEstimator, typename VoltageInterpolator, typename CurrentInterpolator,
          typename SocToOcv, typename OcvToSoc, typename SocEstimator, typename CapacityEstimator,
          typename OcvEstimator>
class StateBasedBatteryModel : public BatteryModelBase {
  public:
    StateBasedBatteryModel(double capacity, Eigen::VectorXd &params,
                           bool useMeasuredCapacity = false);

    static size_t getParamsCount();

    static Eigen::VectorXd getLowerBounds();
    static Eigen::VectorXd getUpperBounds();

    void onCharge(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                  const Eigen::VectorXd &temperature, const Eigen::VectorXd &time) override;

    void onDischarge(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                     const Eigen::VectorXd &temperature, const Eigen::VectorXd &time,
                     const double &capacity) override;

    void onImpedance(double rct, double re) override;

    double getObjectiveValue(ErrorMetric metric) const override;
    void display(ErrorMetric metric) const override;

  private:
    const bool useMeasuredCapacity;
    const double deltaTimeMult;
    const int socCountThreshold;

    double deltaTime;
    double maxTimeDiffMult;

    bool socEstimatorSetupEcm;
    bool socEstimatorSetupOcv;
    int countSinceOcv;

    double totalVoltageErrorSq = 0.0;
    int voltagePredictionCount = 0;
    double meanVoltage = 0.0;
    double totalVoltageVariance = 0.0;
    int missedVoltagePredictionCount = 0;

    double initialCapacity = 0.0;
    double totalCapacityErrorSq = 0.0;
    int capacityPredictionCount = 0;
    double meanCapacity = 0.0;
    double totalCapacityVariance = 0.0;
    int missedCapacityPredictionCount = 0;

    double totalResistanceErrorSq = 0.0;
    int resistancePredictionCount = 0;
    double meanResistance = 0.0;
    double totalResistanceVariance = 0.0;
    int missedResistancePredictionCount = 0;

    void processData(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                     const Eigen::VectorXd &time);

    EcmStateEstimator ecmStateEstimator;
    VoltageInterpolator voltageInterpolator;
    CurrentInterpolator currentInterpolator;
    SocToOcv socToOcv;
    OcvToSoc ocvToSoc;
    CapacityEstimator capacityEstimator;
    SocEstimator socEstimator;
    OcvEstimator ocvEstimator;
};

#include "BatteryModels/StateBasedBatteryModel.tpp"
