#include "Algorithms/splines/HermiteSpline.h"

// Constructor with auto-computed derivatives (using finite differences)
HermiteSpline::HermiteSpline(Eigen::VectorXd xs, Eigen::VectorXd ys) : xs(xs), ys(ys) {
    computeDerivatives();
    computeCoefficients();
}

// Constructor with user-provided derivatives
HermiteSpline::HermiteSpline(Eigen::VectorXd xs, Eigen::VectorXd ys, Eigen::VectorXd derivatives) 
    : xs(xs), ys(ys), ms(derivatives) {
    computeCoefficients();
}

void HermiteSpline::computeDerivatives() {
    int n = xs.size();
    ms.resize(n);
    
    if (n < 2) {
        ms.setZero();
        return;
    }
    
    // First derivative: forward difference
    ms(0) = (ys(1) - ys(0)) / (xs(1) - xs(0));
    
    // Interior derivatives: central difference
    for (int i = 1; i < n - 1; i++) {
        double h1 = xs(i) - xs(i-1);
        double h2 = xs(i+1) - xs(i);
        double slope1 = (ys(i) - ys(i-1)) / h1;
        double slope2 = (ys(i+1) - ys(i)) / h2;
        
        // Weighted average (Catmull-Rom style)
        ms(i) = (h2 * slope1 + h1 * slope2) / (h1 + h2);
    }
    
    // Last derivative: backward difference
    ms(n-1) = (ys(n-1) - ys(n-2)) / (xs(n-1) - xs(n-2));
}

void HermiteSpline::computeCoefficients() {
    int n = xs.size();
    int segments = n - 1;
    
    c0s.resize(segments);
    c1s.resize(segments);
    c2s.resize(segments);
    c3s.resize(segments);
    
    for (int i = 0; i < segments; i++) {
        double h = xs(i+1) - xs(i);
        double y0 = ys(i);
        double y1 = ys(i+1);
        double m0 = ms(i);
        double m1 = ms(i+1);
        
        // Hermite basis expansion in standard polynomial form
        // p(t) = h00*y0 + h10*h*m0 + h01*y1 + h11*h*m1
        // where t = (x - x0)/h, and h_ij are Hermite basis functions
        // Converting to p(t) = c0 + c1*t + c2*t^2 + c3*t^3
        
        c0s(i) = y0;
        c1s(i) = h * m0;
        c2s(i) = 3.0 * (y1 - y0) - h * (2.0 * m0 + m1);
        c3s(i) = 2.0 * (y0 - y1) + h * (m0 + m1);
    }
}

size_t HermiteSpline::getIndex(double x) const {
    int n = xs.size();
    
    if (x <= xs(0)) {
        return 0;
    } else if (x >= xs(n - 1)) {
        return n - 2;
    }
    
    // Binary search
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

double HermiteSpline::predict(double x) const {
    int i = getIndex(x);
    
    // Normalized parameter t in [0, 1]
    double h = xs(i+1) - xs(i);
    double t = (x - xs(i)) / h;
    
    // Evaluate cubic polynomial: c0 + c1*t + c2*t^2 + c3*t^3
    double t2 = t * t;
    double t3 = t2 * t;
    
    return c0s(i) + c1s(i) * t + c2s(i) * t2 + c3s(i) * t3;
}

double HermiteSpline::derivative(double x) const {
    int i = getIndex(x);
    
    // Derivative with respect to x: dp/dx = (dp/dt) * (dt/dx)
    // where dt/dx = 1/h
    double h = xs(i+1) - xs(i);
    double t = (x - xs(i)) / h;
    
    // dp/dt = c1 + 2*c2*t + 3*c3*t^2
    double dpdt = c1s(i) + 2.0 * c2s(i) * t + 3.0 * c3s(i) * t * t;
    
    // Convert to dp/dx
    return dpdt / h;
}