#include "Algorithms/splines/AkimaSpline.h"
#include <iostream>

// I make the assumption that the input points are strictly monotone.
AkimaSpline::AkimaSpline(Eigen::VectorXd xs, Eigen::VectorXd ys) {
    int n = xs.size();
    
    // Calculate differences and slopes between consecutive points
    Eigen::VectorXd dxs = xs.tail(n - 1) - xs.head(n - 1);
    Eigen::VectorXd dys = ys.tail(n - 1) - ys.head(n - 1);
    Eigen::VectorXd ms = dys.cwiseQuotient(dxs);  // slopes = dy/dx
    
    Eigen::VectorXd dms = ms.tail(n - 1) - ms.head(n - 1);    

    // Calculate c1s (degree-1 coefficients)
    Eigen::VectorXd c1s(n);
    c1s(0) = ms(0);  // First coefficient is first slope
    c1s(1) = (ms(0) + ms(1)) * 0.5;
    c1s(n - 2) = (ms(n-3) + ms(n-2)) * 0.5;
    c1s(n - 1) = ms(ms.size() - 1);  // Last coefficient is last slope
    
    for (int i = 2; i < n-2; i++) {
        const double bottom = dms(i) + dms(i-1);
        if (bottom == 0) {
            c1s(i) = (ms(i-1) + ms(i)) * 0.5;
        } else {
            const double top = dms(i) * ms(i-1) + dms(i-1) * ms(i);
            c1s(i) = top / bottom;
        }
    }
    int size = c1s.size() - 1;
    this->c1s = c1s.head(size);
    this->xs = xs;
    this->ys = ys;

    // (3mi - 2si - si+1) / (xi+1 - xi)
    c2s = (3 * ms.head(size) - 2 * c1s.head(size) - c1s.tail(size)).cwiseQuotient(dxs.head(size));
    
    // (c1si + c1si+1 - 2mi) / (xi+1 - xi)^2
    c3s = (c1s.head(size) + c1s.tail(size) - 2 * ms.head(size)).cwiseQuotient(dxs.cwiseProduct(dxs));
}

size_t AkimaSpline::getIndex(double x) const {
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

double AkimaSpline::predict(double x) const {
    int i = getIndex(x);
    
    // Evaluate the cubic polynomial for segment i
    double dx = x - xs(i);
    return ys(i) + c1s(i) * dx + c2s(i) * dx * dx + c3s(i) * dx * dx * dx;
}

double AkimaSpline::derivative(double x) const {
    int i = getIndex(x);
    
    // Evaluate the cubic polynomial for segment i
    double dx = x - xs(i);
    return c1s(i) + 2 * c2s(i) * dx + 3 * c3s(i) * dx * dx;
}