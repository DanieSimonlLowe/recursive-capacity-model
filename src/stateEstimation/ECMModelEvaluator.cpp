#include "stateEstimation/ECMModelEvaluator.h"
#define ECM_BATCH_SIZE 50

ECMModelEvaluator::ECMModelEvaluator(RecursiveLeastSquares* rls, InterpolatorBase* voltageInerpolator, InterpolatorBase* currentInterpolator) : 
    rls(rls), voltageInerpolator(voltageInerpolator), currentInterpolator(currentInterpolator)
{

}

void ECMModelEvaluator::setParams(Eigen::VectorXd& params) {
    int index = 0;
    if (rls->getParamsCount() > 0) {
        rls->setParams(params.head(rls->getParamsCount()));
        index += rls->getParamsCount();
    }
    if (voltageInerpolator->getParamsCount() > 0) {
        voltageInerpolator->setParams(params.segment(index,voltageInerpolator->getParamsCount()));
        index += voltageInerpolator->getParamsCount();
    }
    if (currentInterpolator->getParamsCount() > 0) {
        currentInterpolator->setParams(params.segment(index,currentInterpolator->getParamsCount()));
        index += currentInterpolator->getParamsCount();
    }
    deltaTimeMult = std::pow(2.0,params[index]);
    index++;
}

void ECMModelEvaluator::getParamsCount() {
    return rls->getParamsCount() + voltageInerpolator->getParamsCount() + currentInterpolator->getParamsCount() + 1;
}

ECMModelEvaluator* ECMModelEvaluator::clone() {
    return new ECMModelEvaluator(rls->clone(),voltageInerpolator->clone(),currentInterpolator->clone());
}

// TODO lower and upper bounds.
std::vector<double> ECMModelEvaluator::getLowerBounds() {
    Eigen::VectorXd v(this->getParamsCount());

    v << rls->getLowerBounds(), voltageInerpolator->getLowerBounds(), currentInterpolator->getLowerBounds() << -3;
};

std::vector<double> ECMModelEvaluator::getUpperBounds() {
    Eigen::VectorXd v(this->getParamsCount());

    v << rls->getUpperBounds(), voltageInerpolator->getUpperBounds(), currentInterpolator->getUpperBounds() << 3;
};

// display


// getObjectiveValue

void ECMModelEvaluator::processData(const Eigen::VectorXd &voltages,
                        const Eigen::VectorXd &currents,
                        const Eigen::VectorXd &times) {
    if (model == nullptr) {
        std::vector<double> deltas;
        deltas.reserve(times.size() - 1);

        for (int i = 1; i < times.size(); ++i) {
            deltas.push_back(times[i] - times[i - 1]);
        }

        std::nth_element(deltas.begin(), deltas.begin() + deltas.size() / 2, deltas.end());
        double med = deltas[deltas.size() / 2];
        // Precise value dose not matter.

        deltaTime = med * deltaTimeMult;
        model = new T(rls,deltaTime);
    }

    
    int n = voltages.size();  // assumes all vectors have the same length
    double startTime = times[0];
    for (int i = 0; i< n; i++) {
        double voltage = voltages[i];
        double current = currents[i];
        double time = times[i];

        currentInterpolator->update(current,time);
        voltageInerpolator->update(voltage,time);
        if (current->canPredict() && voltageInerpolator->canPredict()) {
            int steps = static_cast<int>((time - startTime) / deltaTime) + 1;
            Eigen::VectorXd voltageSamples(steps);
            Eigen::VectorXd currentSamples(steps);
            for (int i = 0; i <= steps; ++i) {
                double t = startTime + i * deltaTime;
                // work with t
                voltageSamples[i] = voltageInterpolator->predict(t);
                currentSamples[i] = currentInterpolator->predict(t);
            }

            model->update(currentSamples,voltageSamples)
        }

        if (model->canCalculateState()) {
            // TODO OCVEKFEstimator that uses EKF with other stuff
            // This reqires a good soc ocv model.
            // So do the soc ocv model evaluation first. 
        }
    }
}

void ECMModelEvaluator::onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) {
    processData(voltage,current,time);
};


void ECMModelEvaluator::onDischarge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time,
                            const double &capacity) {
    processData(voltage,current,time);
};

void ECMModelEvaluator::onImpedance(double Rct, double Re) {
    // this is not used.
}