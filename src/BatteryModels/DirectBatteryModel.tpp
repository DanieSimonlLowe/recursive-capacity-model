
template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                   OcvEstimator>::DirectBatteryModel(double capacity, Eigen::VectorXd &params,
                                                     bool useMeasuredCapacity)
    : deltaTimeMult(std::pow(
          10, params[VoltageInterpolator::getParamsCount() + CurrentInterpolator::getParamsCount() +
                     OcvToSoc::getParamsCount() + CapacityEstimator::getParamsCount() +
                     OcvEstimator::getParamsCount()])),

      socCountThreshold(int(std::pow(
          10, params[VoltageInterpolator::getParamsCount() + CurrentInterpolator::getParamsCount() +
                     OcvToSoc::getParamsCount() + CapacityEstimator::getParamsCount() +
                     OcvEstimator::getParamsCount() + 1]))),

      maxTimeDiffMult(std::pow(
          10, params[VoltageInterpolator::getParamsCount() + CurrentInterpolator::getParamsCount() +
                     OcvToSoc::getParamsCount() + CapacityEstimator::getParamsCount() +
                     OcvEstimator::getParamsCount() + 2])),

      initialCapacity(capacity), deltaTime(0), countSinceOcv(1000), totalCapacityErrorSq(0),
      capacityPredictionCount(0), missedCapacityPredictionCount(0),

      voltageInterpolator(params.segment(0, VoltageInterpolator::getParamsCount())),
      currentInterpolator(params.segment(VoltageInterpolator::getParamsCount(),
                                         CurrentInterpolator::getParamsCount())),

      ocvToSoc(ReadOcvSocCSV("/mnt/c/Users/Danie/Desktop/project/data/soc_volt_nasa.csv"),
               params.segment(VoltageInterpolator::getParamsCount() +
                                  CurrentInterpolator::getParamsCount(),
                              OcvToSoc::getParamsCount())),

      capacityEstimator(capacity, params.segment(VoltageInterpolator::getParamsCount() +
                                                     CurrentInterpolator::getParamsCount() +
                                                     OcvToSoc::getParamsCount(),
                                                 CapacityEstimator::getParamsCount())),

      ocvEstimator(params.segment(
          VoltageInterpolator::getParamsCount() + CurrentInterpolator::getParamsCount() +
              OcvToSoc::getParamsCount() + CapacityEstimator::getParamsCount(),
          OcvEstimator::getParamsCount())),
      meanCapacity(0), totalCapacityVariance(0) {}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
size_t DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                          OcvEstimator>::getParamsCount() {
    return VoltageInterpolator::getParamsCount() + CurrentInterpolator::getParamsCount() +
           OcvToSoc::getParamsCount() + CapacityEstimator::getParamsCount() +
           OcvEstimator::getParamsCount() + 3; // deltaTimeMult, socCountThreshold, maxTimeDiffMult
}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
Eigen::VectorXd DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc,
                                   CapacityEstimator, OcvEstimator>::getLowerBounds() {
    Eigen::VectorXd v(getParamsCount());
    size_t offset = 0;

    if (VoltageInterpolator::getParamsCount() > 0) {
        v.segment(offset, VoltageInterpolator::getParamsCount()) =
            VoltageInterpolator::getLowerBounds();
        offset += VoltageInterpolator::getParamsCount();
    }

    if (CurrentInterpolator::getParamsCount() > 0) {
        v.segment(offset, CurrentInterpolator::getParamsCount()) =
            CurrentInterpolator::getLowerBounds();
        offset += CurrentInterpolator::getParamsCount();
    }

    if (OcvToSoc::getParamsCount() > 0) {
        v.segment(offset, OcvToSoc::getParamsCount()) = OcvToSoc::getLowerBounds();
        offset += OcvToSoc::getParamsCount();
    }

    if (CapacityEstimator::getParamsCount() > 0) {
        v.segment(offset, CapacityEstimator::getParamsCount()) =
            CapacityEstimator::getLowerBounds();
        offset += CapacityEstimator::getParamsCount();
    }

    if (OcvEstimator::getParamsCount() > 0) {
        v.segment(offset, OcvEstimator::getParamsCount()) = OcvEstimator::getLowerBounds();
        offset += OcvEstimator::getParamsCount();
    }

    v[offset] = -1;
    v[offset + 1] = 0;
    v[offset + 2] = 0;

    return v;
}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
Eigen::VectorXd DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc,
                                   CapacityEstimator, OcvEstimator>::getUpperBounds() {
    Eigen::VectorXd v(getParamsCount());
    size_t offset = 0;

    if (VoltageInterpolator::getParamsCount() > 0) {
        v.segment(offset, VoltageInterpolator::getParamsCount()) =
            VoltageInterpolator::getUpperBounds();
        offset += VoltageInterpolator::getParamsCount();
    }

    if (CurrentInterpolator::getParamsCount() > 0) {
        v.segment(offset, CurrentInterpolator::getParamsCount()) =
            CurrentInterpolator::getUpperBounds();
        offset += CurrentInterpolator::getParamsCount();
    }

    if (OcvToSoc::getParamsCount() > 0) {
        v.segment(offset, OcvToSoc::getParamsCount()) = OcvToSoc::getUpperBounds();
        offset += OcvToSoc::getParamsCount();
    }

    if (CapacityEstimator::getParamsCount() > 0) {
        v.segment(offset, CapacityEstimator::getParamsCount()) =
            CapacityEstimator::getUpperBounds();
        offset += CapacityEstimator::getParamsCount();
    }

    if (OcvEstimator::getParamsCount() > 0) {
        v.segment(offset, OcvEstimator::getParamsCount()) = OcvEstimator::getUpperBounds();
        offset += OcvEstimator::getParamsCount();
    }

    v[offset] = 1;
    v[offset + 1] = 3;
    v[offset + 2] = 2;

    return v;
}

// getObjectiveValue
template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
void DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                        OcvEstimator>::processData(const Eigen::VectorXd &voltages,
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
        ocvEstimator.setDeltaTime(deltaTime);
    }

    int n = voltages.size(); // assumes all vectors have the same length
    double startTime = times[0];

    int socEstCount = 0;
    int last_i = 0;
    double lastSoc = -1;
    double currentAreaSum = 0;
    for (int i = 0; i < n; i++) {
        double voltage = voltages[i];
        double current = currents[i];
        double time = times[i];
        bool nextIsLargGap = false;
        if (i > 0) {
            currentAreaSum += 0.5 * (current + currents[i - 1]) * (time - times[i - 1]);
        }

        currentInterpolator.update(current, time);
        voltageInterpolator.update(voltage, time);
        if (i < n - 1 && times[i + 1] - time > deltaTime * maxTimeDiffMult) {
            nextIsLargGap = true;
        }
        if (currentInterpolator.canPredict() && voltageInterpolator.canPredict() &&
            (i == n - 1 || nextIsLargGap || i - last_i > 50)) {
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
                countSinceOcv++;
                ocvEstimator.update(currentSamples, voltageSamples);
            }
            startTime = time;
            if ((countSinceOcv > socCountThreshold) && ocvEstimator.canCalculateState()) {

                std::cout << "Ocv, Soc: " << ocvEstimator.getOcv() << ", ";

                const double soc = ocvToSoc.getSoc(ocvEstimator.getOcv());
                std::cout << soc << "\n";
                if (lastSoc != -1) {
                    std::cout << "CurrArea, Diff Soc" << currentAreaSum << ", " << soc - lastSoc
                              << "\n";
                    capacityEstimator.update(currentAreaSum, soc - lastSoc);
                }
                lastSoc = soc;

                countSinceOcv = 0;
                currentAreaSum = 0;
            }
        }
        if (nextIsLargGap) {
            startTime = times[i + 1];
        }
    }
}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
void DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                        OcvEstimator>::onCharge(const Eigen::VectorXd &voltage,
                                                const Eigen::VectorXd &current,
                                                const Eigen::VectorXd &temperature,
                                                const Eigen::VectorXd &time) {
    processData(voltage, current, time);
};

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
void DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                        OcvEstimator>::onDischarge(const Eigen::VectorXd &voltage,
                                                   const Eigen::VectorXd &current,
                                                   const Eigen::VectorXd &temperature,
                                                   const Eigen::VectorXd &time,
                                                   const double &capacity) {
    processData(voltage, current, time);

    // if (capacityEstimator.canCalculateStateCapacity())
    const double predCapacity = capacityEstimator.getCapacity() / 3600.0;
    if (missedCapacityPredictionCount > 5 && std::abs(predCapacity - initialCapacity) > 1e-2) {
        std::cout << "capacity: " << predCapacity << ", " << capacity << "\n";
        const double capError = predCapacity - capacity;
        const double deltaCapacity = predCapacity - meanCapacity;
        totalCapacityErrorSq += capError * capError;
        capacityPredictionCount++;
        meanCapacity += deltaCapacity / capacityPredictionCount;
        totalCapacityVariance += deltaCapacity * (predCapacity - meanCapacity);
        // std::cout << "Capacity " << capacity << ", " << predCapacity << "\n";
    } else {
        missedCapacityPredictionCount++;
    }
};

// This is empty.
template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
void DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                        OcvEstimator>::onImpedance(double Rct, double Re) {}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
double DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                          OcvEstimator>::getObjectiveValue(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::CapacityError:
        default:
            return totalCapacityErrorSq / capacityPredictionCount *
                   (1.0 + 0.5 * double(missedCapacityPredictionCount) /
                              double(missedCapacityPredictionCount + capacityPredictionCount));
            //		    + 1e-1 * getObjectiveValue(ErrorMetric::VoltageError); // This is added
            // so that it can optimze the capacity better.
    }
}

template <typename VoltageInterpolator, typename CurrentInterpolator, typename OcvToSoc,
          typename CapacityEstimator, typename OcvEstimator>
void DirectBatteryModel<VoltageInterpolator, CurrentInterpolator, OcvToSoc, CapacityEstimator,
                        OcvEstimator>::display(ErrorMetric metric) const {
    switch (metric) {
        case ErrorMetric::CapacityError:
        default:
            std::cout << "Capacity \tMSE " << getObjectiveValue(metric) << "\n"
                      << "\t 1-R^2 "
                      << (totalCapacityErrorSq / capacityPredictionCount) /
                             (totalCapacityVariance / (capacityPredictionCount - 1))
                      << "\n"
                      << "\t total count " << capacityPredictionCount;
            break;
    }
}
