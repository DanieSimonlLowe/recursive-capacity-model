#include "algorithms/LeastSquares.h"

LeastSquares::LeastSquares(const Eigen::MatrixXd& xs, const Eigen::VectorXd& ys, double regularizationConstant) {
    Eigen::MatrixXd xt = xs.transpose();
    size_t n_features = xs.cols();

    // Regularized normal equation (ridge regression / Tikhonov regularization)
    Eigen::MatrixXd A = xt * xs + regularizationConstant * Eigen::MatrixXd::Identity(n_features, n_features);
    Eigen::VectorXd b = xt * ys;

    // Solve A * weights = b without explicit inversion
    weights = A.ldlt().solve(b);
}

double LeastSquares::predict(const Eigen::VectorXd& x) {
    return x.dot(weights);
}

