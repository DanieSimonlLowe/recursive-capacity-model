#include "Loading/ProcessNasaCycles.h"

enum ErrorMetric { VoltageError, CapacityError, ResistanceError };

class BatteryModelBase : public CycleHandler {
  public:
    virtual ~BatteryModelBase() = default;
    // BatteryModelBase(double capacity, Eigen::VectorXd& params, bool useMeasuredCapacity=false);

    // static size_t getParamsCount();

    // static Eigen::VectorXd getLowerBounds();

    // static Eigen::VectorXd getUpperBounds();

    virtual void onCharge(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                          const Eigen::VectorXd &temperature, const Eigen::VectorXd &time) = 0;

    virtual void onDischarge(const Eigen::VectorXd &voltage, const Eigen::VectorXd &current,
                             const Eigen::VectorXd &temperature, const Eigen::VectorXd &time,
                             const double &capacity) = 0;

    virtual void onImpedance(double Rct, double Re) = 0;

    virtual double getObjectiveValue(ErrorMetric metric) const = 0;

    virtual void display(ErrorMetric metric) const = 0;
};
