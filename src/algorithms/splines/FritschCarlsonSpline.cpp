#include "algorithms/splines/FritschCarlsonSpline.h"
#include <iostream>

// I make the assumption that the input points are strictly monotone.
FritschCarlsonSpline::FritschCarlsonSpline(Eigen::VectorXd xs, Eigen::VectorXd ys) {
    int n = xs.size();
    
    // Calculate differences and slopes between consecutive points
    Eigen::VectorXd dxs = xs.tail(n - 1) - xs.head(n - 1);
    Eigen::VectorXd dys = ys.tail(n - 1) - ys.head(n - 1);
    Eigen::VectorXd ms = dys.cwiseQuotient(dxs);  // slopes = dy/dx
    
    // Calculate c1s (degree-1 coefficients)
    Eigen::VectorXd c1s(n);
    c1s(0) = ms(0);  // First coefficient is first slope
    
    // Calculate interior c1 values
    for (int i = 0; i < dxs.size() - 1; i++) {
        double m = ms(i);
        double mNext = ms(i + 1);
        
        if (m * mNext <= 0) {
            // Slopes have opposite signs or one is zero
            c1s(i + 1) = 0;
        } else {
            // Both slopes have same sign - use harmonic mean formula
            double dx = dxs(i);
            double dxNext = dxs(i + 1);
            double common = dx + dxNext;
            c1s(i + 1) = 3 * common / ((common + dxNext) / m + (common + dx) / mNext);
        }
    }
    c1s(n - 1) = ms(ms.size() - 1);  // Last coefficient is last slope
    
    // Calculate c2s and c3s coefficients
    Eigen::VectorXd c2s(n - 1);
    Eigen::VectorXd c3s(n - 1);
    
    Eigen::VectorXd c1s_head = c1s.head(n - 1);
    Eigen::VectorXd c1s_tail = c1s.tail(n - 1);
    Eigen::VectorXd invDxs = dxs.cwiseInverse();

    Eigen::VectorXd common_terms = c1s_head + c1s_tail - 2 * ms;
    this->c2s = (ms - c1s_head - common_terms).cwiseProduct(invDxs);
    this->c3s = common_terms.cwiseProduct(invDxs).cwiseProduct(invDxs);

    
    // Store the coefficients as member variables
    this->c1s = c1s;
    this->xs = xs;
    this->ys = ys;
}

size_t FritschCarlsonSpline::getIndex(double x) const {
    int n = xs.size();
    if (x <= xs(0)) {
        return 0;
    } else if (x >= xs(n - 1)) {
        return n-2;
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

double FritschCarlsonSpline::predict(double x) const {
    int i = getIndex(x);
    
    // Evaluate the cubic polynomial for segment i
    double dx = x - xs(i);
    return ys(i) + c1s(i) * dx + c2s(i) * dx * dx + c3s(i) * dx * dx * dx;
}

double FritschCarlsonSpline::derivative(double x) const {
    int i = getIndex(x);
    
    // Evaluate the cubic polynomial for segment i
    double dx = x - xs(i);
    return c1s(i) + 2 * c2s(i) * dx + 3 * c3s(i) * dx * dx;
}