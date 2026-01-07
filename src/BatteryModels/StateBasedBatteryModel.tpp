

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    StateBasedBatteryModel(double capacity, Eigen::VectorXd& params, bool useMeasuredCapacity): 
    useMeasuredCapacity(useMeasuredCapacity),
    deltaTimeMult(std::pow(10, params[SocEstimator::getParamsCount() + 
                                    ECMStateEstimator::getParamsCount() + 
                                    VoltageInterpolator::getParamsCount() + 
                                    CurrentInterpolator::getParamsCount() +
                                    SocOcvCurve::getParamsCount() +
                                    CapacityEstimator::getParamsCount() +
                                    OcvEstimator::getParamsCount()
                                ])),
    socCountThreshold(int(std::pow(10,params[SocEstimator::getParamsCount() + 
                                       ECMStateEstimator::getParamsCount() + 
                                       VoltageInterpolator::getParamsCount() + 
                                       CurrentInterpolator::getParamsCount() +
                                       SocOcvCurve::getParamsCount() +
                                       CapacityEstimator::getParamsCount() +
                                       OcvEstimator::getParamsCount()
                                       + 1]))),
    maxTimeDiffMult(std::pow(10,params[SocEstimator::getParamsCount() + 
                                       ECMStateEstimator::getParamsCount() + 
                                       VoltageInterpolator::getParamsCount() + 
                                       CurrentInterpolator::getParamsCount() +
                                       SocOcvCurve::getParamsCount() +
                                       CapacityEstimator::getParamsCount() +
                                       OcvEstimator::getParamsCount()
                                       + 2])),
    initalCapacity(capacity),
    deltaTime(0),
    socEstimatorSetupEcm(false),
    socEstimatorSetupOcv(false),
    countSinceOcv(1000),
    totalVoltageErrorSq(0),
    voltagePredictionCount(0),
    totalCapacityErrorSq(0),
    capacityPredictionCount(0),
    resistancePredictionCount(0),
    missedCapacityPredictionCount(0),
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
    capacityEstimator(capacity,params.segment(SocEstimator::getParamsCount() + 
                                     ECMStateEstimator::getParamsCount() + 
                                     VoltageInterpolator::getParamsCount() + 
                                     CurrentInterpolator::getParamsCount() +
                                     SocOcvCurve::getParamsCount(), 
                                     CapacityEstimator::getParamsCount())),
    ocvEstimator(params.segment(SocEstimator::getParamsCount() + 
                                     ECMStateEstimator::getParamsCount() + 
                                     VoltageInterpolator::getParamsCount() + 
                                     CurrentInterpolator::getParamsCount() +
                                     SocOcvCurve::getParamsCount() +
                                     CapacityEstimator::getParamsCount(), 
                                     OcvEstimator::getParamsCount())),
    socEstimator(ECMStateEstimator::getDimension(),&socOcvCurve,params.segment(0, SocEstimator::getParamsCount()))
{
    // Constructor body is now empty - all initialization done in initializer list
    socEstimator.setCapacity(capacity * 3600.0);
}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
size_t StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    getParamsCount() {
    return SocEstimator::getParamsCount() + 
        ECMStateEstimator::getParamsCount() + 
        VoltageInterpolator::getParamsCount() + 
        CurrentInterpolator::getParamsCount() +
        SocOcvCurve::getParamsCount() +
        CapacityEstimator::getParamsCount() + 
        OcvEstimator::getParamsCount() + 3;
    // Extra parameters:
    // deltaTimeMult
    // socCountThreshold
    // maxTimeDiffMult
}

template<typename ECMStateEstimator, typename VoltageInterpolator, typename CurrentInterpolator, typename SocOcvCurve, typename SocEstimator, typename CapacityEstimator, typename OcvEstimator>
Eigen::VectorXd StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
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

    if (OcvEstimator::getParamsCount() > 0) {
        v.segment(offset, OcvEstimator::getParamsCount()) = OcvEstimator::getLowerBounds();
        offset += OcvEstimator::getParamsCount();
    }
    
    v[offset] = -1;
    v[offset + 1] = 0; // 10^x
    v[offset + 2] = 0; // 10^x
    
    return v;
}

template<typename ECMStateEstimator, typename VoltageInterpolator, typename CurrentInterpolator, typename SocOcvCurve, typename SocEstimator, typename CapacityEstimator, typename OcvEstimator>
Eigen::VectorXd StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
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
    
    if (OcvEstimator::getParamsCount() > 0) {
        v.segment(offset, OcvEstimator::getParamsCount()) = OcvEstimator::getUpperBounds();
        offset += OcvEstimator::getParamsCount();
    }

    v[offset] = 1;
    v[offset + 1] = 3; // 10^x
    v[offset + 2] = 2; // 10^x

    return v;
}

// getObjectiveValue
template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
void StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
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
        ocvEstimator.setDeltaTime(deltaTime);

    }


    int n = voltages.size();  // assumes all vectors have the same length
    double startTime = times[0];
    
    int socEstCount = 0;
    int last_i = 0;
    for (int i = 0; i  < n; i++) {
        double voltage = voltages[i];
        double current = currents[i];
        double time = times[i];
        bool nextIsLargGap = false;
        

        currentInterpolator.update(current,time);
        voltageInterpolator.update(voltage,time);
        if (i < n-1 && times[i+1] - time > deltaTime * maxTimeDiffMult) {
            nextIsLargGap = true;
        }
        if (currentInterpolator.canPredict() && voltageInterpolator.canPredict() && 
            (i == n-1 || nextIsLargGap || i - last_i > 50)) {
            int steps = static_cast<int>((time - startTime) / deltaTime);
            last_i = i;
            if (steps > 3) {
                Eigen::VectorXd voltageSamples(steps);
                Eigen::VectorXd currentSamples(steps);
                for (int j = 0; j < steps; ++j) {
                    double t = startTime + j * deltaTime;
                    // work with t
                    voltageSamples[j] = voltageInterpolator.predict(t);
                    currentSamples[j] = currentInterpolator.predict(t);
                }
                ecmStateEstimator.update(currentSamples,voltageSamples);
                ocvEstimator.update(currentSamples,voltageSamples);
            }
            startTime = time;
        }
        if (nextIsLargGap) {
            startTime = times[i+1];
        }

        // this
        if (ecmStateEstimator.canCalculateState()) {
            socEstimator.setOhmicResistance(ecmStateEstimator.getOhmicResistance());
            socEstimator.setBranchResistances(ecmStateEstimator.getBranchResistances());
            socEstimator.setBranchCapacities(ecmStateEstimator.getBranchCapacities());
            socEstimatorSetupEcm = true;
        }

        if ((countSinceOcv > socCountThreshold || socEstimatorSetupOcv) && ocvEstimator.canCalculateState()) {
            countSinceOcv = 0;
            socEstimatorSetupOcv = true;
            socEstimator.setOcv(ocvEstimator.getOcv());
        }


        if (socEstimatorSetupEcm && socEstimatorSetupOcv && i > 0) {
            const double startSoc = socEstimator.getSoc();
            const double diffTime = times[i] - times[i-1];
            if (socEstCount > 10) { // ignore first 10
                const double predVoltage = socEstimator.predictVoltage(current,diffTime);
                const double error = voltage - predVoltage;
                voltagePredictionCount++;
                const double voltageChange = voltages[i] - voltages[i-1]; // use this instead so I can check aginst just using last voltage.
		const double deltaVoltage = voltageChange - meanVoltage;
                meanVoltage += deltaVoltage / voltagePredictionCount;
                totalVoltageVariance += deltaVoltage * (voltageChange - meanVoltage);
                totalVoltageErrorSq += error * error;
            } else {
                socEstimator.predictVoltage(current,diffTime);
            }
            socEstimator.measure(current,voltage);
            socEstCount++;
            const double endSoc = socEstimator.getSoc();

            capacityEstimator.update(current, diffTime, endSoc-startSoc);
            if (!useMeasuredCapacity && capacityEstimator.canCalculateStateCapacity()) {
                socEstimator.setCapacity(capacityEstimator.getCapacity());
            }
        } else {
            missedVoltagePredictionCount++;
        }
    }

}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
void StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    onCharge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time) {
    processData(voltage,current,time);
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
void StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    onDischarge(const Eigen::VectorXd &voltage,
                            const Eigen::VectorXd &current,
                            const Eigen::VectorXd &temperature,
                            const Eigen::VectorXd &time,
                            const double &capacity) {
    processData(voltage,current,time);
    
    //if (capacityEstimator.canCalculateStateCapacity()) {
    const double predCapacity = capacityEstimator.getCapacity() / 3600.0;
    if (missedCapacityPredictionCount > 5 && std::abs(predCapacity - initalCapacity) > 1e-1) {
        const double capError = predCapacity - capacity;
        const double deltaCapacity = predCapacity - meanCapacity;
        totalCapacityErrorSq += capError*capError;
        capacityPredictionCount++;
        meanCapacity += deltaCapacity / capacityPredictionCount;
        totalCapacityVariance += deltaCapacity * (predCapacity - meanCapacity);
        // std::cout << "Capacity " << capacity << ", " << predCapacity << "\n";
    } else {
        missedCapacityPredictionCount++;
    }
    
    if (useMeasuredCapacity) {
        socEstimator.setCapacity(capacity * 3600.0);
    }
};

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
void StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    onImpedance(double Rct, double Re) {
    if (ecmStateEstimator.canCalculateState()) {
        const double measured = ecmStateEstimator.getOhmicResistance();
        //std::cout << "Resistance " << measured << ", " << Re << "\n";
        const double error = measured - Re;
        const double delta = measured - meanResistance;
        totalResistanceErrorSq += error*error;
        resistancePredictionCount++;
        meanResistance += delta / resistancePredictionCount;
        totalResistanceVariance += delta * (measured - meanResistance);
    } else {
        missedResistancePredictionCount++;
    }
    
}


template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
double StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
    getObjectiveValue(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::ResistanceError:
            if (resistancePredictionCount > 0) {
                return totalResistanceErrorSq / resistancePredictionCount 
                    * (1.0 + 0.5 * double(missedResistancePredictionCount) / double(missedResistancePredictionCount + resistancePredictionCount));
            } else {
                return 1e100;
            }
        case ErrorMetric::VoltageError:
            if (voltagePredictionCount > 0) {
                return totalVoltageErrorSq / voltagePredictionCount
                    * (1.0 + 0.5 * double(missedVoltagePredictionCount) / double(missedVoltagePredictionCount + voltagePredictionCount));
            } else {
                return 1e100;
            }
        case ErrorMetric::CapacityError:
        default:
            if (capacityPredictionCount > 0) {
                return totalCapacityErrorSq / capacityPredictionCount
                    * (1.0 + 0.5 * double(missedCapacityPredictionCount) / double(missedCapacityPredictionCount + totalCapacityVariance));
//		    + 1e-1 * getObjectiveValue(ErrorMetric::VoltageError); // This is added so that it can optimze the capacity better.
            } else {
                return 1e10 + 1e10 * getObjectiveValue(ErrorMetric::VoltageError); // 

            }
        
    }
}

template<typename  ECMStateEstimator, typename  VoltageInterpolator, typename  CurrentInterpolator, typename  SocOcvCurve, typename  SocEstimator, typename  CapacityEstimator, typename OcvEstimator>
void StateBasedBatteryModel<ECMStateEstimator,VoltageInterpolator,CurrentInterpolator,SocOcvCurve,SocEstimator,CapacityEstimator,OcvEstimator>::
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
