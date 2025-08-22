#include "loading/processNasaCycles.h"

class TimeResampler {
    public:
        TimeResampler(double timeStepSize);

        void onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) override;

        void onDischarge(const Eigen::VectorXd &voltage,
                                const Eigen::VectorXd &current,
                                const Eigen::VectorXd &temperature,
                                const Eigen::VectorXd &time,
                                const double &capacity) override;

        void onImpedance(double Rct, double Re) override;

    private:
        double timeStepSize;
}