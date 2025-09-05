#include "interpolation/fastCubicSplineInterpolator.h"

FastCubicSplineInterpolator::FastCubicSplineInterpolator(double ts): InterpolatorBase() {
    position = 0;
    isSetup = false;
    i = 0;

    for (int i = 0; i < CUBIC_SPLINE_WINDOW_SIZE; ++i) {
        M[i] = 0.0;
    }
}

FastCubicSplineInterpolator* FastCubicSplineInterpolator::clone() {
    return new FastCubicSplineInterpolator(timeStepSize);
}

void FastCubicSplineInterpolator::update(double measurement, double time) {
    if (position == CUBIC_SPLINE_WINDOW_SIZE) {
        position = 0;
        isSetup = false;
    }
    times[position] = time;
    measurements[position] = measurement;
    position++;
}

bool FastCubicSplineInterpolator::canPredict() {
    return position == CUBIC_SPLINE_WINDOW_SIZE;
}

void FastCubicSplineInterpolator::reset() {
    position = 0;
}

inline void FastCubicSplineInterpolator::setup() {
    if (isSetup || position != CUBIC_SPLINE_WINDOW_SIZE) {
        return;
    }
    
    const int n = CUBIC_SPLINE_WINDOW_SIZE;
    
    // First and last are allways zero.
    // Initialize M array (second derivatives) to zero
    // for (int i = 0; i < n; ++i) {
    //     M[i] = 0.0;
    // }
    
    const int m = n - 2;  // Number of interior points
    
    if (m <= 0) {
        isSetup = true;
        return;
    }
    
    // Compute intervals h[i] = t[i+1] - t[i]
    //double h[n-1];
    for (int i = 0; i < n-1; ++i) {
        h[i] = times[i+1] - times[i];
    }
    
    // Compute slopes
    // double slopes[n-1];
    for (int i = 0; i < n-1; ++i) {
        slopes[i] = (measurements[i+1] - measurements[i]) / h[i];
    }
    
    // RHS vector d = 6*(slope[i+1] - slope[i])
    // double d[m];
    for (int i = 0; i < m; ++i) {
        d[i] = 6.0 * (slopes[i+1] - slopes[i]);
    }
    
    // Tridiagonal system components
    //double diag[m];
    //double off_diag[m-1];
    
    // Diagonal elements: 2*(h[i] + h[i+1])
    for (int i = 0; i < m; ++i) {
        diag[i] = 2.0 * (h[i] + h[i+1]);
    }
    
    // Off-diagonal elements: h[i+1]
    for (int i = 0; i < m-1; ++i) {
        off_diag[i] = h[i+1];
    }
    
    // Solve tridiagonal system
    //double x[m];
    // solveTridiagonal(off_diag, diag, off_diag, d, M+1, m);
    solveTridiagonal();

    // for (int i = 0; i < m; ++i) {
    //     M[i+1] = x[i];
    // }
    
    // isSetup = true;
}

inline void FastCubicSplineInterpolator::solveTridiagonal() {
    // const double* lower,  // sub-diagonal
    // const double* diag,   // diagonal  
    // const double* upper,  // super-diagonal
    // const double* rhs,    // right hand side
    // double* x,            // solution
    // int n) {              // size
    
    const int n = CUBIC_SPLINE_WINDOW_SIZE - 2; 
    double* x = M+1;
    //double c_prime[n-1];
    //double d_prime[n];
    
    // Forward sweep
    // if (n > 1) {
    //     c_prime[0] = off_diag[0] / diag[0];
    // }
    c_prime[0] = off_diag[0] / diag[0];
    d_prime[0] = d[0] / diag[0];
    
    for (int i = 1; i < n-1; ++i) {
        double denominator = diag[i] - off_diag[i-1] * c_prime[i-1];
        c_prime[i] = off_diag[i] / denominator;
        d_prime[i] = (d[i] - off_diag[i-1] * d_prime[i-1]) / denominator;
    }
    
    // if (n > 1) {
    //     d_prime[n-1] = (d[n-1] - off_diag[n-2] * d_prime[n-2]) / 
    //                    (diag[n-1] - off_diag[n-2] * c_prime[n-2]);
    // }
    d_prime[n-1] = (d[n-1] - off_diag[n-2] * d_prime[n-2]) / 
                   (diag[n-1] - off_diag[n-2] * c_prime[n-2]);
    
    // Back substitution
    x[n-1] = d_prime[n-1];
    for (int i = n-2; i >= 0; --i) {
        x[i] = d_prime[i] - c_prime[i] * x[i+1];
    }
}


double FastCubicSplineInterpolator::predict(double time) {
    if (!isSetup) {
        setup();
        isSetup = true;
        i = 0;
    }
    
    // Find the interval containing the time
    // int i = 0; // this allways increases.
    for (i = 0; i < CUBIC_SPLINE_WINDOW_SIZE - 2; ++i) {
        if (time <= times[i+1]) {
            break;
        }
    }
    
    // Ensure i is within bounds
    // if (i >= CUBIC_SPLINE_WINDOW_SIZE - 1) {
    //     i = CUBIC_SPLINE_WINDOW_SIZE - 2;
    // }
    
    // Compute the cubic spline value using the standard formula
    double h = times[i+1] - times[i];
    double a = (times[i+1] - time) / h;
    double b = (time - times[i]) / h;
    
    return a * measurements[i] + b * measurements[i+1] +
           ((a*a*a - a) * M[i] + (b*b*b - b) * M[i+1]) * (h*h) / 6.0;
}