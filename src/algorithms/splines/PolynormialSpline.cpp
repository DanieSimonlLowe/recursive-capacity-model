#include "Algorithms/splines/PolynomialSpline.h"

inline double derivativeCoefficient(const int power, const int degree) {
    if (degree > power) return 0.0;
    double out = 1.0;
    for (int p = 0; p < degree; ++p) {
        out *= (power - p);
    }
    return out;
}

inline double calculateFactorial(int n) {
    double out = 1.0;
    for (int i = 2; i <= n; ++i) {
        out *= i;
    }
    return out;
}

PolynomialSpline::PolynomialSpline(const Eigen::VectorXd& xs, const Eigen::VectorXd& ys, unsigned int power) {
    if (power < 3 || power % 2 == 0) {
        throw std::invalid_argument("power must be a positive odd number."); 
    }
    int n = xs.size();
    
    int numSegments = n - 1;
    int paramsPerSegment = power + 1;
    int paramCount = numSegments * paramsPerSegment;
    
    // Each segment: 2 Interpolation constraints (start, end)
    // Each interior knot: (power-1) continuity constraints for derivatives
    // 2 boundary conditions (natural spline: derivative = 0 at ends)
    int continuity = power - 1;
    int eqCount = 2 * numSegments + continuity * (numSegments - 1) + 2;
    
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(eqCount, paramCount);
    Eigen::VectorXd b = Eigen::VectorXd::Zero(eqCount);

    int currentEq = 0;
    
    // For each segment i, set up Interpolation constraints
    for (int i = 0; i < numSegments; ++i) {
        int paramOffset = i * paramsPerSegment;
        double h = xs(i+1) - xs(i);
        
        // Constraint: S_i(0) = y_i (start of segment)
        A(currentEq, paramOffset) = 1.0;
        b(currentEq) = ys(i);
        currentEq++;
        
        // Constraint: S_i(h) = y_{i+1} (end of segment)
        for (int p = 0; p <= power; ++p) {
            A(currentEq, paramOffset + p) = std::pow(h, p);
        }
        b(currentEq) = ys(i+1);
        currentEq++;
    }
    
    // Continuity constraints at interior knots
    for (int i = 0; i < numSegments - 1; ++i) {
        int paramOffset_i = i * paramsPerSegment;
        int paramOffset_ip1 = (i + 1) * paramsPerSegment;
        double h = xs(i+1) - xs(i);
        
        // For derivatives of order 1 to (power-1)
        for (int d = 1; d <= continuity; ++d) {
            // S_i^(d)(h) = S_{i+1}^(d)(0)
            
            // Left side: d-th derivative of S_i at x = h
            for (int p = d; p <= power; ++p) {
                double coeff = derivativeCoefficient(p, d);
                A(currentEq, paramOffset_i + p) = coeff * std::pow(h, p - d);
            }
            
            // Right side: d-th derivative of S_{i+1} at x = 0
            // The d-th derivative at x=0 is d! * a_d
            A(currentEq, paramOffset_ip1 + d) = -calculateFactorial(d);
            
            b(currentEq) = 0.0;
            currentEq++;
        }
    }
    
    // Natural boundary conditions: set appropriate derivative to 0 at endpoints
    // For cubic (power=3): 2nd derivative = 0 at both ends
    int boundaryDerivativeOrder = (power - 1) / 2;
    
    // Left boundary: d-th derivative at start = 0
    // S_0^(d)(0) = d! * a_d = 0
    A(currentEq, boundaryDerivativeOrder) = calculateFactorial(boundaryDerivativeOrder);
    b(currentEq) = 0.0;
    currentEq++;
    
    // Right boundary: d-th derivative at end = 0
    // S_{n-2}^(d)(h_{n-2}) = 0
    int lastSegment = numSegments - 1;
    int paramOffset_last = lastSegment * paramsPerSegment;
    double h_last = xs(n-1) - xs(n-2);
    
    for (int p = boundaryDerivativeOrder; p <= power; ++p) {
        double coeff = derivativeCoefficient(p, boundaryDerivativeOrder);
        A(currentEq, paramOffset_last + p) = coeff * std::pow(h_last, p - boundaryDerivativeOrder);
    }
    b(currentEq) = 0.0;
    currentEq++;

    // Solve the system
    parameters = A.fullPivHouseholderQr().solve(b);
    this->xs = xs;
    this->power = power;
}

size_t PolynomialSpline::getIndex(double x) const {
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

double PolynomialSpline::predict(double x) {
    size_t index = getIndex(x);
    
    const double x0 = this->xs(index);
    const double xdiff = x - x0;

    double value = 0.0;
    const int paramPos = index * (power + 1);
    for (int i = 0; i <= power; i++) {
        value += this->parameters(paramPos + i) * std::pow(xdiff, i);
    }
    return value;
}

double PolynomialSpline::derivative(double x, int order) {
    size_t index = getIndex(x);

    const double x0 = this->xs(index);
    const double xdiff = x - x0;

    const int paramPos = index * (power + 1);
    double value = 0.0;
    for (int i = order; i <= power; i++) { 
        value += derivativeCoefficient(i, order) * this->parameters(paramPos + i) * std::pow(xdiff, i - order);
    }
    return value;
}