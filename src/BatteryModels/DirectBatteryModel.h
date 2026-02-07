#pragma once

#include "BatteryModels/BatteryModelBase.h"
#include "Interpolation/InterpolatorBase.h"
#include "Loading/ProcessNasaCycles.h"
#include "Loading/ReadOcvSocCsv.h"
#include <Eigen/Dense>

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
class DirectBatteryModel : public BatteryModelBase {
  public:
    DirectBatteryModel(double capacity, Eigen::VectorXd &params, bool useMeasuredCapacity = false);

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
    const double deltaTimeMult;
    const int socCountThreshold;

    double deltaTime;
    double maxTimeDiffMult;

    int countSinceOcv;

    double initialCapacity = 0.0;
    double totalCapacityErrorSq = 0.0;
    int capacityPredictionCount = 0;
    double meanCapacity = 0.0;
    double totalCapacityVariance = 0.0;
    int missedCapacityPredictionCount = 0;

    void processData(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                     const Eigen::VectorXd &time);

    VoltageInterpolator voltageInterpolator;
    CurrentInterpolator currentInterpolator;
    OcvToSoc ocvToSoc;
    CapacityEstimator capacityEstimator;
    OcvEstimator ocvEstimator;
};

#include "BatteryModels/DirectBatteryModel.tpp"
