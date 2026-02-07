#include "Algorithms/NonNegativeLeastSquares.h"

inline int maxIndex(std::vector<bool> &included, Eigen::VectorXd &vector) {
    int maxIdx = -1;
    double maxVal = -std::numeric_limits<double>::infinity();

    for (int i = 0; i < vector.size(); ++i) {
        if (included[i] && vector[i] > maxVal) {
            maxVal = vector[i];
            maxIdx = i;
        }
    }

    return maxIdx;
}

Eigen::MatrixXd filterColumns(const Eigen::MatrixXd &matrix, const std::vector<bool> &excluded,
                              int excludedCount) {
    int totalCols = (int)matrix.cols();
    int keepCount = totalCols - excludedCount;

    Eigen::MatrixXd result(matrix.rows(), keepCount);

    int resultCol = 0;
    for (int i = 0; i < totalCols; ++i) {
        if (!excluded[i]) {
            result.col(resultCol) = matrix.col(i);
            ++resultCol;
        }
    }

    return result;
}

Eigen::VectorXd GetSp(const Eigen::MatrixXd &A, const Eigen::VectorXd &Y,
                      const std::vector<bool> &R, int R_left) {
    Eigen::MatrixXd Ap = filterColumns(A, R, R_left);
    return (Ap.transpose() * Ap).inverse() * Ap.transpose() * Y;
}

void FillS(Eigen::VectorXd &s, Eigen::VectorXd &sp, int n, const std::vector<bool> &R) {
    int j = 0;
    for (int i = 0; i < n; ++i) {
        if (!R[i]) {
            s(i) = sp(j);
            ++j;
        } else {
            s(i) = 0;
        }
    }
}

inline double MinFunc(std::vector<bool> &excluded, Eigen::VectorXd &x, Eigen::VectorXd &s) {
    double minVal = std::numeric_limits<double>::infinity();
    for (int i = 0; i < s.size(); ++i) {
        double func = x[i] / (x[i] - s[i]);
        if (!excluded[i] && s[i] < 0 && func < minVal) {
            minVal = func;
        }
    }
    return minVal;
}

// https://en.wikipedia.org/wiki/Non-negative_least_squares
Eigen::VectorXd NonNegativeLeastSquares(Eigen::MatrixXd &A, Eigen::VectorXd &Y,
                                        std::vector<bool> &canBeNegative, const double tolerance) {
    const int n = (int)A.cols();

    std::vector<bool> R(n);
    int R_left = 0;
    for (int i = 0; i < n; ++i) {
        if (canBeNegative[i]) {
            R[i] = false;
        } else {
            R[i] = true;
            R_left++;
        }
    }
    Eigen::VectorXd x = Eigen::VectorXd::Zero(n);
    Eigen::VectorXd s = Eigen::VectorXd(n);
    Eigen::VectorXd w = A.transpose() * (Y - A * x);

    int j = maxIndex(R, w);
    while (R_left > 0 && w[j] > tolerance) {
        R[j] = false;
        Eigen::VectorXd sp = GetSp(A, Y, R, R_left);
        FillS(s, sp, n, R);
        while (sp.minCoeff() <= 0) {
            double a = MinFunc(R, x, s);
            x += a * (s - x);

            for (int i = 0; i < n; ++i) {
                if (x(i) <= 0) {
                    R[i] = true;
                }
            }
            sp = GetSp(A, Y, R, R_left);
            FillS(s, sp, n, R);
        }
        x = s;
        w = A.transpose() * (Y - A * x);
    }
    return x;
}
