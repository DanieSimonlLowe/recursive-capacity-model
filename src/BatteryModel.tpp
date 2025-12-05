

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    BatteryModel(double capacity, Eigen::VectorXd& params, bool useMeasuredCapacity): 
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
    socEstimatorSetup(false),
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
    socEstimator(ECMStateEstimator::getDimension(),&socOcvCurve,params.segment(0, SocEstimator::getParamsCount()))
{
    // Constructor body is now empty - all initialization done in initializer list
    socEstimator.setCapacity(capacity);
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


    int n = voltages.size();  // assumes all vectors have the same length
    double startTime = times[0];
    
    for (int i = 0; i  < n; i++) {
        double voltage = voltages[i];
        double current = currents[i];
        double time = times[i];

        currentInterpolator.update(current,time);
        voltageInterpolator.update(voltage,time);
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

    }
    
    if (ecmStateEstimator.canCalculateState()) {
        socEstimator.setOhmicResistance(ecmStateEstimator.getOhmicResistance());
        socEstimator.setBranchResistances(ecmStateEstimator.getBranchResistances());
        socEstimator.setBranchCapacities(ecmStateEstimator.getBranchCapacities());
        socEstimatorSetup = true;
    }

    if (socEstimatorSetup) {
    
        for (int i = 1; i  < n; i++) {
            double voltage = voltages[i];
            double current = currents[i];
            const double startSoc = socEstimator.getSoc();
            const double diffTime = times[i] - times[i-1];
            if (i > 10) { // ignore first 10
                const double predVoltage = socEstimator.predictVoltage(current,diffTime);
                const double error = voltage - predVoltage;
                voltagePredictionCount++;
                const double deltaVoltage = voltage - meanVoltage;
                meanVoltage += deltaVoltage / voltagePredictionCount;
                totalVoltageVariance += deltaVoltage * (voltage - meanVoltage);
                totalVoltageErrorSq += error * error;
            } else {
                socEstimator.predictVoltage(current,diffTime);
            }
            socEstimator.measure(current,voltage);
            
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
        const double deltaCapacity = capacityEstimator.getCapacity() - meanCapacity;
        totalCapacityErrorSq += capError*capError;
        capacityPredictionCount++;
        meanCapacity += deltaCapacity / capacityPredictionCount;
        totalCapacityVariance += deltaCapacity * (capacityEstimator.getCapacity() - meanCapacity);
    } else {
        socEstimator.setCapacity(capacity);
    }
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    onImpedance(double Rct, double Re) {
    if (ecmStateEstimator.canCalculateState()) {
        const double measured = ecmStateEstimator.getOhmicResistance();
        const double error = measured - Re;
        const double delta = measured - meanResistance;
        totalResistanceErrorSq = error*error;
        resistancePredictionCount++;
        meanResistance += delta / resistancePredictionCount;
        totalResistanceVariance += delta * (measured - meanResistance);
    }
    
}


template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
double BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    getObjectiveValue(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::ResistanceError:
            if (resistancePredictionCount > 0) {
                return totalResistanceErrorSq / resistancePredictionCount;
            } else {
                return 1e100;
            }
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

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator>
void BatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator>::
    display(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::ResistanceError:
            std::cout << "Resistance \tMSE " << getObjectiveValue(metric) << "\n"
                << "\t 1-R^2 " << (totalResistanceErrorSq / resistancePredictionCount) / (totalResistanceVariance / (resistancePredictionCount-1)) << "\n"
                << "\t total count " << resistancePredictionCount;
            break;
        case ErrorMetric::VoltageError:
            std::cout << "Voltage \tMSE " << getObjectiveValue(metric) << "\n"
                << "\t 1-R^2 " << (totalVoltageErrorSq / voltagePredictionCount) / (totalVoltageVariance / (voltagePredictionCount-1)) << "\n"
                << "\t total count " << voltagePredictionCount;
            break;
        case ErrorMetric::CapacityError:
        default:
            std::cout << "Capacity \tMSE " << getObjectiveValue(metric) << "\n"
                << "\t 1-R^2 " << (totalCapacityErrorSq / capacityPredictionCount) / (totalCapacityVariance / (capacityPredictionCount-1)) << "\n"
                << "\t total count " << capacityPredictionCount;
            break;
    }
}