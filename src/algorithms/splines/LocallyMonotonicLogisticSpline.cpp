#include "Algorithms/splines/LocallyMonotonicLogisticSpline.h"
#include "Algorithms/NonNegativeLeastSquares.h"
#include <algorithm>
#define MIN_VALUE 1e-4

LocallyMonotonicLogisticSpline::LocallyMonotonicLogisticSpline(const Eigen::VectorXd &xs,
                                                               const Eigen::VectorXd &ys,
                                                               const double tolerance,
                                                               const double valueWeightMult,
                                                               const double derivativeWeight) {
    const int n = (int)xs.size();

    const int numSegments = n - 1;
    const int paramsPerSegment = 4;
    const int paramCount = numSegments * paramsPerSegment;

    // Each segment: 2 Interpolation constraints (start, end)
    // Each segment: 2 drivitage contrants f'(x) = f'(x) and f''(x) = f''(x)
    // Boundary condition: at end f'(x) = 0

    const int eqCount = 2 * numSegments + 2 * (numSegments - 1) + 2;
    Eigen::VectorXd Y = Eigen::VectorXd::Zero(eqCount);
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(eqCount, paramCount);
    std::vector<bool> unconstrainedParamitors(paramCount, false);
    const double valueWeight = derivativeWeight * valueWeightMult;

    for (int i = 0; i < numSegments; i++) {
        int paramIndex = i * 4;
        int eqationIndex = i * 4;

        unconstrainedParamitors[paramIndex] = true;
        // sdd, sd,s,e,edd, ed
        if (i > 0) {
            // f''(xs)
            A(eqationIndex - 1, paramIndex + 2) = -1 / std::max(xs[i] * xs[i], MIN_VALUE);
            A(eqationIndex - 1, paramIndex + 3) =
                1 / std::max((1 - xs[i]) * (1 - xs[i]), MIN_VALUE);
        }
        // f'(xs)
        A(eqationIndex, paramIndex + 1) = 1 * derivativeWeight;
        A(eqationIndex, paramIndex + 2) = 1 / std::max(xs[i], MIN_VALUE) * derivativeWeight;
        A(eqationIndex, paramIndex + 3) = 1 / std::max(1 - xs[i], MIN_VALUE) * derivativeWeight;

        // f(xs) = S
        A(eqationIndex + 1, paramIndex) = 1 * valueWeight;
        A(eqationIndex + 1, paramIndex + 1) = xs[i] * valueWeight;
        A(eqationIndex + 1, paramIndex + 2) = std::log(std::max(xs[i], MIN_VALUE)) * valueWeight;
        A(eqationIndex + 1, paramIndex + 3) =
            -std::log(std::max(1 - xs[i], MIN_VALUE)) * valueWeight;
        Y[eqationIndex + 1] = ys[i] * valueWeight;

        // f(xe) = E
        A(eqationIndex + 2, paramIndex) = 1 * valueWeight;
        A(eqationIndex + 2, paramIndex + 1) = xs[i + 1] * valueWeight;
        A(eqationIndex + 2, paramIndex + 2) =
            std::log(std::max(xs[i + 1], MIN_VALUE)) * valueWeight;
        A(eqationIndex + 2, paramIndex + 3) =
            -std::log(std::max(1 - xs[i + 1], MIN_VALUE)) * valueWeight;
        Y[eqationIndex + 2] = ys[i + 1] * valueWeight;

        if (i != numSegments - 1) {
            // f''(xe)
            A(eqationIndex + 3, paramIndex + 2) = -1 / std::max(xs[i + 1] * xs[i + 1], MIN_VALUE);
            A(eqationIndex + 3, paramIndex + 3) =
                1 / std::max((1 - xs[i + 1]) * (1 - xs[i + 1]), MIN_VALUE);
            // f'(xe)
            A(eqationIndex + 4, paramIndex + 1) = 1 * derivativeWeight;
            A(eqationIndex + 4, paramIndex + 2) =
                1 / std::max(xs[i + 1], MIN_VALUE) * derivativeWeight;
            A(eqationIndex + 4, paramIndex + 3) =
                1 / std::max(1 - xs[i + 1], MIN_VALUE) * derivativeWeight;
        } else {
            // f'(xe)
            A(eqationIndex + 3, paramIndex + 1) = 1 * derivativeWeight;
            A(eqationIndex + 3, paramIndex + 2) =
                1 / std::max(xs[i + 1], MIN_VALUE) * derivativeWeight;
            A(eqationIndex + 3, paramIndex + 3) =
                1 / std::max(1 - xs[i + 1], MIN_VALUE) * derivativeWeight;
        }
    }

    this->xs = xs;
    this->parameters = NonNegativeLeastSquares(A, Y, unconstrainedParamitors, tolerance);
}

int LocallyMonotonicLogisticSpline::getIndex(double x) const {
    int n = xs.size();
    if (x <= xs(0)) {
        return 0;
    } else if (x >= xs(n - 1)) {
        return n - 2;
    }

    int i = 0;
    int j = n - 1;
    while (j - i > 1) {
        int k = (i + j) / 2;
        if (xs(k) > x) {
            j = k;
        } else {
            i = k;
        }
    }

    return i;
}

double LocallyMonotonicLogisticSpline::predict(double x) const {
    int index = getIndex(x);

    const int paramPos = (int)index * 4;
    return this->parameters(paramPos) + this->parameters(paramPos + 1) * x +
           this->parameters(paramPos + 2) * std::log(x) +
           this->parameters(paramPos + 3) * std::log(1 - x);
}

double LocallyMonotonicLogisticSpline::derivative(double x) const {
    int index = getIndex(x);

    const int paramPos = (int)index * 4;
    return this->parameters(paramPos + 1) + this->parameters(paramPos + 2) * 1 / x +
           this->parameters(paramPos + 3) * 1 / (1 - x);
}
