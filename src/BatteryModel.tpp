
// TODO I need to restructure the interplators (or at least one interplator) to allow me to do this.

// TODO go thouh and add some constants.

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    BatteryModel(double capacity, Eigen::VectorXd& params): 
    capacity(capacity),
    deltaTimeMult(std::pow(10, params[SocEstimator::getParamsCount() + 
                                    ECMStateEstimator::getParamsCount() + 
                                    VoltageInterpolator::getParamsCount() + 
                                    CurrentInterpolator::getParamsCount() +
                                    SocOcvCurve::getParamsCount() +
                                    CapacityEstimator::getParamsCount()])),
    capacityConfidenceThreshold(params[SocEstimator::getParamsCount() + 
                                       ECMStateEstimator::getParamsCount() + 
                                       VoltageInterpolator::getParamsCount() + 
                                       CurrentInterpolator::getParamsCount() +
                                       SocOcvCurve::getParamsCount() +
                                       CapacityEstimator::getParamsCount() + 1]),
    deltaTime(0),
    totalVoltageErrorSq(0),
    voltagePredictionCount(0),
    totalCapacityErrorSq(0),
    capacityPredictionCount(0),
    ecmStateEstimator(params.segment(SocEstimator::getParamsCount(), ECMStateEstimator::getParamsCount())),
    voltageInterpolator(params.segment(SocEstimator::getParamsCount() + ECMStateEstimator::getParamsCount(), 
                                       VoltageInterpolator::getParamsCount())),
    currentInterpolator(params.segment(SocEstimator::getParamsCount() + 
                                       ECMStateEstimator::getParamsCount() + 
                                       VoltageInterpolator::getParamsCount(), 
                                       CurrentInterpolator::getParamsCount())),
    socOcvCurve(readOcvSocCSV("/mnt/c/Users/Danie/Desktop/project/data/soc_volt_nasa.csv"),
                params.segment(SocEstimator::getParamsCount() + 
                              ECMStateEstimator::getParamsCount() + 
                              VoltageInterpolator::getParamsCount() + 
                              CurrentInterpolator::getParamsCount(), 
                              SocOcvCurve::getParamsCount())),
    capacityEstimator(params.segment(SocEstimator::getParamsCount() + 
                                     ECMStateEstimator::getParamsCount() + 
                                     VoltageInterpolator::getParamsCount() + 
                                     CurrentInterpolator::getParamsCount() +
                                     SocOcvCurve::getParamsCount(), 
                                     CapacityEstimator::getParamsCount())),
    socEstimatorParams(params.segment(0, SocEstimator::getParamsCount()))
{
    // Constructor body is now empty - all initialization done in initializer list
}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
size_t BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getParamsCount() {
    return SocEstimator::getParamsCount() + 
        ECMStateEstimator::getParamsCount() + 
        VoltageInterpolator::getParamsCount() + 
        CurrentInterpolator::getParamsCount() +
        SocOcvCurve::getParamsCount() +
        CapacityEstimator::getParamsCount() + 2;
    // Extra parameters:
    // deltaTimeMult
    // capacityConfidenceThreshold 
}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
Eigen::VectorXd BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getLowerBounds() {
    Eigen::VectorXd v(getParamsCount());

    v << SocEstimator::getLowerBounds(),
        ECMStateEstimator::getLowerBounds(),
        VoltageInterpolator::getLowerBounds(),
        CurrentInterpolator::getLowerBounds(),
        SocOcvCurve::getLowerBounds(),
        CapacityEstimator::getLowerBounds()
         << -2 << -0.01;
    return v;
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
Eigen::VectorXd BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getUpperBounds() {
    Eigen::VectorXd v(getParamsCount());

    v << SocEstimator::getUpperBounds(),
        ECMStateEstimator::getUpperBounds(),
        VoltageInterpolator::getUpperBounds(),
        CurrentInterpolator::getUpperBounds(),
        SocOcvCurve::getUpperBounds(),
        CapacityEstimator::getUpperBounds()
         << 3 << 1;
    return v;
};

// getObjectiveValue
template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    processData(const Eigen::VectorXd &voltages,
                        const Eigen::VectorXd &currents,
                        const Eigen::VectorXd &times) {
    if (deltaTime == 0) {
        std::vector<double> deltas;
        deltas.reserve(times.size() - 1);

        for (int i = 1; i < times.size(); ++i) {
            deltas.push_back(times[i] - times[i - 1]);
        }

        std::nth_element(deltas.begin(), deltas.begin() + deltas.size() / 2, deltas.end());
        double med = deltas[deltas.size() / 2];
        // Precise value dose not matter.

        deltaTime = med * deltaTimeMult;
        ecmStateEstimator.setDeltaTime(deltaTime);
    }

    
    const double maxValt = voltages.maxCoeff();
    const double minValt = voltages.maxCoeff();
    double maxValtSq = maxValt * maxValt;
    if (abs(minValt) > abs(maxValt)) {
        maxValtSq = minValt * minValt;
    }
    SocEstimator socEstimator(ECMStateEstimator::getDimension(),maxValtSq,&socOcvCurve,socEstimatorParams);
    socEstimator.setCapacity(capacity);

    int n = voltages.size();  // assumes all vectors have the same length
    double startTime = times[0];
    
    bool socEstimatorValesSet = false;
    int socEstimatorMeasureCount = 0;
    for (int i = 0; i  < n; i++) {
        double voltage = voltages[i];
        double current = currents[i];
        double time = times[i];

        currentInterpolator.update(current,time);
        voltageInterpolator.update(voltage,time);
        if (i % 5 == 4) {
            if (currentInterpolator.canPredict() && voltageInterpolator.canPredict()) {
                int steps = static_cast<int>((time - startTime) / deltaTime) + 1;
                Eigen::VectorXd voltageSamples(steps);
                Eigen::VectorXd currentSamples(steps);
                for (int i = 0; i < steps; ++i) {
                    double t = startTime + i * deltaTime;
                    // work with t
                    voltageSamples[i] = voltageInterpolator.predict(t);
                    currentSamples[i] = currentInterpolator.predict(t);
                }
                startTime = time;

                ecmStateEstimator.update(currentSamples,voltageSamples);
            }

            if (ecmStateEstimator.canCalculateState()) {
                socEstimator.setOhmicResistance(ecmStateEstimator.getOhmicResistance());
                socEstimator.setBranchResistances(ecmStateEstimator.getBranchCapacities());
                socEstimator.setBranchCapacities(ecmStateEstimator.getBranchCapacities());
                socEstimatorValesSet = true;
            }
        }
        if (socEstimatorValesSet) {
            const double startSoc = socEstimator.getSoc();
            const double diffTime = time - times[i-1];
            if (socEstimatorMeasureCount > 10) {
                const double predVoltage = socEstimator.predictVoltage(current,diffTime);
                const double error = voltage - predVoltage;
                totalVoltageErrorSq += error * error;
                voltagePredictionCount++;
            } else {
                socEstimator.predictVoltage(current,diffTime);
            }
            socEstimator.measure(current,voltage);
            socEstimatorMeasureCount++;
            const double endSoc = socEstimator.getSoc();

            capacityEstimator.update(current, diffTime, endSoc-startSoc);
            if (capacityEstimator.getCapacityVariance() < capacityConfidenceThreshold * capacityEstimator.getCapacity()) {
                socEstimator.setCapacity(capacityEstimator.getCapacity());
            }
        }
        
    }
}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) {
    processData(voltage,current,time);
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    onDischarge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time,
                            const double &capacity) {
    processData(voltage,current,time);

    const double capError = capacityEstimator.getCapacity() - capacity;
    totalCapacityErrorSq += capError*capError;
    capacityPredictionCount++;
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    onImpedance(double Rct, double Re) {
    // this is not used.
}
