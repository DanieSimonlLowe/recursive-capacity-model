#include "algorithms/cubicSpline.h"

CubicSpline::CubicSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y) {
    const int n = t.size();
    M = Eigen::VectorXd::Zero(n);
    const int m = n - 2;
    
    if (m <= 0) {
        this->y = y;
        this->t = t;
        return;
    }
    
    // Intervals - more efficient computation
    Eigen::VectorXd h(n-1);
    for (int i = 0; i < n-1; ++i) {
        h(i) = t(i+1) - t(i);
    }
    
    // Slopes - avoid creating temporary vectors
    Eigen::VectorXd slopes(n-1);
    for (int i = 0; i < n-1; ++i) {
        slopes(i) = (y(i+1) - y(i)) / h(i);
    }
    
    // RHS vector d = 6*(slope[i+1] - slope[i])
    Eigen::VectorXd d(m);
    for (int i = 0; i < m; ++i) {
        d(i) = 6.0 * (slopes(i+1) - slopes(i));
    }
    
    // Tridiagonal system: diag = 2*(h[i] + h[i+1]), off_diag = h[i]
    Eigen::VectorXd diag(m);
    Eigen::VectorXd off_diag(m-1);
    
    for (int i = 0; i < m; ++i) {
        diag(i) = 2.0 * (h(i) + h(i+1));
    }
    for (int i = 0; i < m-1; ++i) {
        off_diag(i) = h(i+1);
    }
    
    // Solve tridiagonal system using Thomas algorithm (O(n))
    Eigen::VectorXd x = solveTridiagonal(off_diag, diag, off_diag, d);
    
    // Fill into M
    M.segment(1, m) = x;
    this->y = y;
    this->t = t;
}

// Thomas algorithm for tridiagonal systems
Eigen::VectorXd CubicSpline::solveTridiagonal(
    const Eigen::VectorXd& lower,  // sub-diagonal
    const Eigen::VectorXd& diag,   // diagonal  
    const Eigen::VectorXd& upper,  // super-diagonal
    const Eigen::VectorXd& rhs) {
    
    const int n = diag.size();
    Eigen::VectorXd c_prime(n-1);
    Eigen::VectorXd d_prime(n);
    Eigen::VectorXd x(n);
    
    // Forward sweep
    c_prime(0) = upper(0) / diag(0);
    d_prime(0) = rhs(0) / diag(0);
    
    for (int i = 1; i < n-1; ++i) {
        double denominator = diag(i) - lower(i-1) * c_prime(i-1);
        c_prime(i) = upper(i) / denominator;
        d_prime(i) = (rhs(i) - lower(i-1) * d_prime(i-1)) / denominator;
    }
    
    d_prime(n-1) = (rhs(n-1) - lower(n-2) * d_prime(n-2)) / 
                   (diag(n-1) - lower(n-2) * c_prime(n-2));
    
    // Back substitution
    x(n-1) = d_prime(n-1);
    for (int i = n-2; i >= 0; --i) {
        x(i) = d_prime(i) - c_prime(i) * x(i+1);
    }
    
    return x;
}

double CubicSpline::predict(double time) {
    int n = t.size();
    const double* begin = t.data();
    const double* end = begin + n;
    const double* it = std::upper_bound(begin, end, time); // first element > time
    int i0 = static_cast<int>(it - begin) - 1;
    int i1 = i0 + 1;

    double t0 = t(i0), t1 = t(i1);

    double h = t1 - t0;

    double xi = time;

    double term1 = M(i0) * std::pow(t1 - xi, 3) / (6.0 * h);
    double term2 = M(i1) * std::pow(xi - t0, 3) / (6.0 * h);
    double term3 = (y(i0) - M(i0) * h * h / 6.0) * (t1 - xi) / h;
    double term4 = (y(i1) - M(i1) * h * h / 6.0) * (xi - t0) / h;

    return term1 + term2 + term3 + term4;
}


double CubicSpline::derivative(double time) { 
    int n = t.size();
    const double* begin = t.data();
    const double* end = begin + n;
    const double* it = std::upper_bound(begin, end, time); // first element > time
    int i0 = static_cast<int>(it - begin) - 1;
    int i1 = i0 + 1;

    double t0 = t(i0), t1 = t(i1);

    double h = t1 - t0;

    double xi = time;

    double term1 = M(i0) * -3.0 * std::pow(t1 - xi, 2) / (6.0 * h);
    double term2 = M(i1) * 3.0 * std::pow(xi - t0, 2) / (6.0 * h);
    double term3 = (y(i0) - M(i0) * h * h / 6.0) * -1 / h;
    double term4 = (y(i1) - M(i1) * h * h / 6.0) * 1 / h;

    return term1 + term2 + term3 + term4;
}