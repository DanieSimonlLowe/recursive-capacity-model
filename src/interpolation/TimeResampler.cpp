#include "loading/timeResampler.h"

TimeResampler::TimeResampler() {
    // TODO
    this->timeStepSize = timeStepSize;
}

void TimeResampler::onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) {
    
        NotKnotSpline vecSpline(time, voltage);
        NotKnotSpline curSpline(time, current);
        NotKnotSpline temSpline(time, temperature);

}

void TimeResampler::onDischarge(const Eigen::VectorXd &voltage,
                                const Eigen::VectorXd &current,
                                const Eigen::VectorXd &temperature,
                                const Eigen::VectorXd &time,
                                const double &capacity) {
    NotKnotSpline vecSpline(time, voltage);
    NotKnotSpline curSpline(time, current);
    NotKnotSpline temSpline(time, temperature);


}

void TimeResampler::onImpedance(double Rct, double Re) {
    // pass this dose nothing.
}