#include "algorithms/cubicSpline.h"

CubicSpline::CubicSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y) {

    const int n = t.size();
    if (n <= 2) throw std::invalid_argument("Need at least three points.");

    M = Eigen::VectorXd::Zero(n); 

    int m = n - 2;

    // Intervals
    Eigen::VectorXd h = t.tail(n-1) - t.head(n-1);

    // Slopes
    Eigen::VectorXd slopes = (y.tail(n-1) - y.head(n-1)).cwiseQuotient(h);

    // RHS vector d = 6*(slope[i+1] - slope[i])
    Eigen::VectorXd d = 6.0 * (slopes.tail(m) - slopes.head(m));

    // Build tridiagonal matrix A
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(m, m);
    A.diagonal()       = 2.0 * (h.head(m) + h.tail(m));
    A.diagonal(-1)     = h.head(m-1);
    A.diagonal( 1)     = h.tail(m-1);

    // Solve
    Eigen::VectorXd x = A.ldlt().solve(d);

    // Fill into M
    M.segment(1, m) = x;


    this->y = y;
    this->t = t;
}

double CubicSpline::predict(double time) {
    int n = t.size();
    int i0 = n - 2;
    int i1 = n - 1;

    double t0 = t(i0), t1 = t(i1);

    double h = t1 - t0;

    double xi = time;

    double term1 = M(i0) * std::pow(t1 - xi, 3) / (6.0 * h);
    double term2 = M(i1) * std::pow(xi - t0, 3) / (6.0 * h);
    double term3 = (y(i0) - M(i0) * h * h / 6.0) * (t1 - xi) / h;
    double term4 = (y(i1) - M(i1) * h * h / 6.0) * (xi - t0) / h;

    return term1 + term2 + term3 + term4;
}