

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    BatteryModel(double capacity, Eigen::VectorXd& params, bool useMeasuredCapacity): 
    capacity(capacity),
    useMeasuredCapacity(useMeasuredCapacity),
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
    socOcvCurve(ReadOcvSocCSV("/mnt/c/Users/Danie/Desktop/project/data/soc_volt_nasa.csv"),
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

template<typename ECMStateEstimator, typename VoltageInterpolator, typename CurrentInterpolator, typename SocOcvCurve, typename SocEstimator, typename CapacityEstimator>
Eigen::VectorXd BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getLowerBounds() {
    Eigen::VectorXd v(getParamsCount());
    
    size_t offset = 0;
    
    if (SocEstimator::getParamsCount() > 0) {
        v.segment(offset, SocEstimator::getParamsCount()) = SocEstimator::getLowerBounds();
        offset += SocEstimator::getParamsCount();
    }
    
    if (ECMStateEstimator::getParamsCount() > 0) {
        v.segment(offset, ECMStateEstimator::getParamsCount()) = ECMStateEstimator::getLowerBounds();
        offset += ECMStateEstimator::getParamsCount();
    }
    
    if (VoltageInterpolator::getParamsCount() > 0) {
        v.segment(offset, VoltageInterpolator::getParamsCount()) = VoltageInterpolator::getLowerBounds();
        offset += VoltageInterpolator::getParamsCount();
    }
    
    if (CurrentInterpolator::getParamsCount() > 0) {
        v.segment(offset, CurrentInterpolator::getParamsCount()) = CurrentInterpolator::getLowerBounds();
        offset += CurrentInterpolator::getParamsCount();
    }
    
    if (SocOcvCurve::getParamsCount() > 0) {
        v.segment(offset, SocOcvCurve::getParamsCount()) = SocOcvCurve::getLowerBounds();
        offset += SocOcvCurve::getParamsCount();
    }
    
    if (CapacityEstimator::getParamsCount() > 0) {
        v.segment(offset, CapacityEstimator::getParamsCount()) = CapacityEstimator::getLowerBounds();
        offset += CapacityEstimator::getParamsCount();
    }
    
    v[offset] = -2;
    v[offset + 1] = -0.01;
    
    return v;
}

template<typename ECMStateEstimator, typename VoltageInterpolator, typename CurrentInterpolator, typename SocOcvCurve, typename SocEstimator, typename CapacityEstimator>
Eigen::VectorXd BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getUpperBounds() {
    Eigen::VectorXd v(getParamsCount());
    
    size_t offset = 0;
    
    if (SocEstimator::getParamsCount() > 0) {
        v.segment(offset, SocEstimator::getParamsCount()) = SocEstimator::getUpperBounds();
        offset += SocEstimator::getParamsCount();
    }
    
    if (ECMStateEstimator::getParamsCount() > 0) {
        v.segment(offset, ECMStateEstimator::getParamsCount()) = ECMStateEstimator::getUpperBounds();
        offset += ECMStateEstimator::getParamsCount();
    }
    
    if (VoltageInterpolator::getParamsCount() > 0) {
        v.segment(offset, VoltageInterpolator::getParamsCount()) = VoltageInterpolator::getUpperBounds();
        offset += VoltageInterpolator::getParamsCount();
    }
    
    if (CurrentInterpolator::getParamsCount() > 0) {
        v.segment(offset, CurrentInterpolator::getParamsCount()) = CurrentInterpolator::getUpperBounds();
        offset += CurrentInterpolator::getParamsCount();
    }
    
    if (SocOcvCurve::getParamsCount() > 0) {
        v.segment(offset, SocOcvCurve::getParamsCount()) = SocOcvCurve::getUpperBounds();
        offset += SocOcvCurve::getParamsCount();
    }
    
    if (CapacityEstimator::getParamsCount() > 0) {
        v.segment(offset, CapacityEstimator::getParamsCount()) = CapacityEstimator::getUpperBounds();
        offset += CapacityEstimator::getParamsCount();
    }
    
    v[offset] = 3;
    v[offset + 1] = 1;
    
    return v;
}

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
        // Precise value does not matter.

        deltaTime = med * deltaTimeMult;
        ecmStateEstimator.setDeltaTime(deltaTime);
    }

    
    const double maxVolt = voltages.maxCoeff();
    const double minVolt = voltages.minCoeff();
    const double maxVoltSq = std::max(maxVolt * maxVolt, minVolt * minVolt);

    SocEstimator socEstimator(ECMStateEstimator::getDimension(),maxVoltSq,&socOcvCurve,socEstimatorParams);
    socEstimator.setCapacity(capacity);

    int n = voltages.size();  // assumes all vectors have the same length
    double startTime = times[0];
    
    bool socEstimatorValuesSet = false;
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
                for (int j = 0; j < steps; ++j) {
                    double t = startTime + j * deltaTime;
                    // work with t
                    voltageSamples[j] = voltageInterpolator.predict(t);
                    currentSamples[j] = currentInterpolator.predict(t);
                }
                startTime = time;

                ecmStateEstimator.update(currentSamples,voltageSamples);
            }

            if (ecmStateEstimator.canCalculateState()) {
                socEstimator.setOhmicResistance(ecmStateEstimator.getOhmicResistance());
                socEstimator.setBranchResistances(ecmStateEstimator.getBranchResistances());
                socEstimator.setBranchCapacities(ecmStateEstimator.getBranchCapacities());
                socEstimatorValuesSet = true;
            }
        }
        if (socEstimatorValuesSet && i > 0) {
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
            if (!useMeasuredCapacity) {
                const double endSoc = socEstimator.getSoc();

                capacityEstimator.update(current, diffTime, endSoc-startSoc);
                if (capacityEstimator.getCapacityVariance() < capacityConfidenceThreshold * capacityEstimator.getCapacity()) {
                    socEstimator.setCapacity(capacityEstimator.getCapacity());
                }
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
    if (!useMeasuredCapacity) {
        const double capError = capacityEstimator.getCapacity() - capacity;
        totalCapacityErrorSq += capError*capError;
        capacityPredictionCount++;
    } else {
        this->capacity = capacity;
    }
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    onImpedance(double Rct, double Re) {
    // this is not used.
}


template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
double BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getObjectiveValue(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::VoltageError:
            if (voltagePredictionCount > 0) {
                return totalVoltageErrorSq / voltagePredictionCount;
            } else {
                return 1e100;
            }
        case ErrorMetric::CapacityError:
        default:
            if (capacityPredictionCount > 0) {
                return totalCapacityErrorSq / capacityPredictionCount;
            } else {
                return 1e100;
            }
        
    }
}