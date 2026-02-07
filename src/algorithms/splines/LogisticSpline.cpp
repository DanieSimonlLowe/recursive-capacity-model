#include "Algorithms/splines/LogisticSpline.h"
#include <algorithm>
#define MIN_VALUE 1e-2

LogisticSpline::LogisticSpline(const Eigen::VectorXd &xs, const Eigen::VectorXd &ys) {
    const int n = (int)xs.size();

    const int numSegments = n - 1;
    const int paramsPerSegment = 4;
    const int paramCount = numSegments * paramsPerSegment;

    // Each segment: 2 Interpolation constraints (start, end)
    // Each segment: 2 drivitage contrants f'(x) = f'(x) and f''(x) = f''(x)
    // Boundary condition: at end f'(x) = 1

    const int eqCount = 2 * numSegments + 2 * (numSegments - 1) + 2;
    Eigen::VectorXd Y = Eigen::VectorXd::Zero(eqCount);
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(eqCount, paramCount);

    for (int i = 0; i < numSegments; i++) {
        int paramIndex = i * 4;
        int eqationIndex = i * 4;

        // sdd, sd,s,e,edd, ed
        if (i > 0) {
            // f''(xs)
            A(eqationIndex - 1, paramIndex + 2) = -1 / std::max(xs[i] * xs[i], MIN_VALUE);
            A(eqationIndex - 1, paramIndex + 3) =
                1 / std::max((1 - xs[i]) * (1 - xs[i]), MIN_VALUE);
        }
        // f'(xs)
        A(eqationIndex, paramIndex + 1) = 1;
        A(eqationIndex, paramIndex + 2) = 1 / std::max(xs[i], MIN_VALUE);
        A(eqationIndex, paramIndex + 3) = 1 / std::max(1 - xs[i], MIN_VALUE);

        // f(xs) = S
        A(eqationIndex + 1, paramIndex) = 1;
        A(eqationIndex + 1, paramIndex + 1) = xs[i];
        A(eqationIndex + 1, paramIndex + 2) = std::log(std::max(xs[i], MIN_VALUE));
        A(eqationIndex + 1, paramIndex + 3) = -std::log(std::max(1 - xs[i], MIN_VALUE));
        Y[eqationIndex + 1] = ys[i];

        // f(xe) = E
        A(eqationIndex + 2, paramIndex) = 1;
        A(eqationIndex + 2, paramIndex + 1) = xs[i + 1];
        A(eqationIndex + 2, paramIndex + 2) = std::log(std::max(xs[i + 1], MIN_VALUE));
        A(eqationIndex + 2, paramIndex + 3) = -std::log(std::max(1 - xs[i + 1], MIN_VALUE));
        Y[eqationIndex + 2] = ys[i + 1];

        if (i != numSegments - 1) {
            // f''(xe)
            A(eqationIndex + 3, paramIndex + 2) = -1 / std::max(xs[i + 1] * xs[i + 1], MIN_VALUE);
            A(eqationIndex + 3, paramIndex + 3) =
                1 / std::max((1 - xs[i + 1]) * (1 - xs[i + 1]), MIN_VALUE);
            // f'(xe)
            A(eqationIndex + 4, paramIndex + 1) = 1;
            A(eqationIndex + 4, paramIndex + 2) = 1 / std::max(xs[i + 1], MIN_VALUE);
            A(eqationIndex + 4, paramIndex + 3) = 1 / std::max(1 - xs[i + 1], MIN_VALUE);
        } else {
            // f'(xe)
            A(eqationIndex + 3, paramIndex + 1) = 1;
            A(eqationIndex + 3, paramIndex + 2) = 1 / std::max(xs[i + 1], MIN_VALUE);
            A(eqationIndex + 3, paramIndex + 3) = 1 / std::max(1 - xs[i + 1], MIN_VALUE);
        }
    }

    this->xs = xs;
    this->parameters = A.colPivHouseholderQr().solve(Y);
}

int LogisticSpline::getIndex(double x) const {
    int n = (int)xs.size();
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

double LogisticSpline::predict(double x) const {
    int index = getIndex(x);

    const int paramPos = (int)index * 4;
    return this->parameters(paramPos) + this->parameters(paramPos + 1) * x +
           this->parameters(paramPos + 2) * std::log(std::max(x, MIN_VALUE)) +
           this->parameters(paramPos + 3) * std::log(std::max(1 - x, MIN_VALUE));
}

double LogisticSpline::derivative(double x) const {
    int index = getIndex(x);

    const int paramPos = (int)index * 4;
    return this->parameters(paramPos + 1) +
           this->parameters(paramPos + 2) * 1 / std::max(x, MIN_VALUE) +
           this->parameters(paramPos + 3) * 1 / std::max(1 - x, MIN_VALUE);
}
