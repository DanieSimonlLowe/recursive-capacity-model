#include "algorithms/splines/MonotoneQuinticSpline.h"
// https://tchlux.github.io/papers/tchlux-2019-HPC-1.pdf

constexpr double FLOAT_TOLERANCE = 1e-10;

struct QuinPoly {
    double diff; // x1 - x0
    double f0;
    double f1;
    double d1f0;
    double d1f1;
    double d2f0;
    double d2f1;
};


bool isMonotone(QuinPoly poly, const double d2f0, const double d2f1) {
    const double f0 = poly.f0;
    const double f1 = poly.f1;
    const double d1f0 = poly.d1f0;
    const double d1f1 = poly.d1f1;
    const double diff = poly.diff;

    if (d1f0 < 0 || d1f1 < 0) {
        return false;
    }
    if (std::abs(f0 - f1) <= FLOAT_TOLERANCE && !(
        std::abs(d1f0) <= FLOAT_TOLERANCE && std::abs(d1f1) <= FLOAT_TOLERANCE &&
        std::abs(d2f0) <= FLOAT_TOLERANCE && std::abs(d2f1) <= FLOAT_TOLERANCE)) {
        return false;
    }
    
    if (std::abs(d1f0) <= FLOAT_TOLERANCE || std::abs(d1f1) <= FLOAT_TOLERANCE) {
        const double w = -diff;
        const double v = f0 - f1;
        if (d2f1 > -4*d1f1 / w) return false;
        if (d2f1 < (3*w*d2f0 - 24*d1f0 - 32*d1f1 + 60*v/w) / (5*w)) return false;
        if (d2f0 < 3*d1f0 / w) return false;
        return true;
    }

    const double A = d1f0 * diff / (f1 - f0);
    const double B = d1f1 * diff / (f1 - f0);

    const double BA_fourthRoot = std::sqrt(std::sqrt(B/A));
    const double BA_3_fourthRoot = BA_fourthRoot * BA_fourthRoot * BA_fourthRoot;  

    const double y0 = 4 * d1f0 / d1f1 * BA_3_fourthRoot;
    const double y1 = diff / d1f1 * BA_3_fourthRoot;
    const double a0 = 4 * BA_fourthRoot;
    const double a1 = -diff / d1f1 * BA_fourthRoot;

    const double sqrt_AB = std::sqrt(A*B);
    const double b0 = 30 - 12 * (d1f0 + d1f1) * diff / ((f1 - f0) * sqrt_AB);
    const double b1 = -3 * diff * diff / (2 * (d1f0 + d1f1) * sqrt_AB);

    const double y = y0 + y1 * d2f0;
    const double a = a0 + a1 * d2f1;
    const double b = b0 + b1 * (d2f0 - d2f1);
    if (b <= 6) {
        return a >  -(b+2)*0.5;
    }
    return y > -2 * std::sqrt(b-2);
}

inline double medain3(const double a, const double b, const double c) {
    return std::max(std::min(a, b), std::min(std::max(a, b), c));
}

void binarySearch(double ad2f0, double ad2f1, double bd2f0, double bd2f1, QuinPoly &poly) {
    double u = 0.5;
    double lowestMonotone = 1.0;
    double highestNonMonotone = 0.0;
    // u is in range 0 to 1 and if isMonotone(u) then for all v where u <= v then isMonotone(v)
    while (u < lowestMonotone - 1e-6) {
        double d2f0 = (1-u) * ad2f0 + u * bd2f0;
        double d2f1 = (1-u) * ad2f1 + u * bd2f1;
        bool monotone = isMonotone(poly, d2f0, d2f1);
        if (monotone) {
            lowestMonotone = u;
            u = (u + highestNonMonotone) * 0.5;
        } else {
            highestNonMonotone = u;
            u = (u + lowestMonotone) * 0.5;
        }
    }
    poly.d2f0 = (1-lowestMonotone) * ad2f0 + lowestMonotone * bd2f0;
    poly.d2f1 = (1-lowestMonotone) * ad2f1 + lowestMonotone * bd2f1;
}


void makeMonotone(QuinPoly &poly) {
    if (poly.f0 == poly.f1) {
        poly.d1f0 = 0;
        poly.d2f0 = 0;
        poly.d1f1 = 0;
        poly.d2f1 = 0;
        return;
    }

    const double d1fMid = 14 * (poly.f1 - poly.f0) / poly.diff;
    poly.d1f0 = medain3(0.0,poly.d1f0,d1fMid);
    poly.d1f1 = medain3(0.0,poly.d1f1,d1fMid);

    if (std::abs(poly.d1f0) < FLOAT_TOLERANCE || std::abs(poly.d1f1) < FLOAT_TOLERANCE) {
        const double w = -poly.diff;
        const double v = poly.f0 - poly.f1;
        if (5*poly.d1f0 + 4 * poly.d1f1 > 20 * v / w) {
            double multFactor = std::max(0.0, 20*v / (w * (5*poly.d1f0 + 4 * poly.d1f1)));
            poly.d1f0 *= multFactor;
            poly.d1f1 *= multFactor;
        }
        poly.d2f0 = std::min(poly.d2f0, (4.0 * (2 * poly.d1f0, + poly.d1f1) + 20*v/w) / w);
        poly.d2f0 = std::max(poly.d2f0, 3 * poly.d1f0 / w);

        poly.d2f1 = std::min(poly.d2f1, -4.0 * poly.d1f0 / w);
        poly.d2f1 = std::max(poly.d2f1, (3*w*poly.d2f0 - 24*poly.d1f0 - 32*poly.d1f1 + 60*v/w)/(5*w));

        return;
    }

    double A = poly.d1f0 * poly.diff / (poly.f1 - poly.f0);
    double B = poly.d1f1 * poly.diff / (poly.f1 - poly.f0);
    if (A > 6 || B > 6) {
        const double dMult = 6.0/std::max(A,B);
        poly.d1f0 *= dMult;
        poly.d1f1 *= dMult;

        // TODO I am not sure I need to do the Mult on A and B.
        // A *= dMult;
        // B *= dMult;
    }

    
    const double sqrtA = std::sqrt(A);
    const double sqrtB = std::sqrt(B);

    binarySearch(poly.d2f0,poly.d2f1,
        -sqrtA/4.0*(7*sqrtA+3*sqrtB),
        sqrtB/4.0*(3*sqrtA+7*sqrtB),
        poly
    );
}


MonotoneQuinticSpline::MonotoneQuinticSpline(Eigen::VectorXd xs, Eigen::VectorXd ys, int shrinkFactor) {
    PolynomialSpline base = PolynomialSpline(xs,ys,5);
    this->xs = xs;

    size_t n = xs.size();
    std::vector<QuinPoly> parts(n-1);
    std::vector<bool> changed(n);
    std::vector<double> step_size(n);
    for (int i = 0; i < n-1; i++) {
        QuinPoly poly = {
            xs(i+1) - xs(i),  // x1 - x0
            base.predict(xs(i)), base.predict(xs(i+1)), // f(x0), f(x1)
            base.derivative(xs(i),1), base.derivative(xs(i+1),1), // df(x0), df(x1)
            base.derivative(xs(i),2), base.derivative(xs(i+1),2)}; // d2f(x0), d2f(x1)
        parts[i] = poly;
        changed[i] = false;
        step_size[i] =  poly.d1f0 / shrinkFactor;
    }
    changed[n-1] = false;
    step_size[n-1] = parts[n-2].d1f1 / shrinkFactor;

    std::stack<int> toBeProcessed;
    for (int i = 0; i < n-1; i++) {
        toBeProcessed.push(i);
    }
    while(!toBeProcessed.empty()) {
        int i = toBeProcessed.top();
        toBeProcessed.pop();
        QuinPoly part = parts[i];
        if (isMonotone(part,part.d2f0,part.d2f1)) {
            continue;
        }
        if (abs(part.d1f0) <= FLOAT_TOLERANCE && abs(part.d1f0) <= FLOAT_TOLERANCE) {
            continue;
        }
        if (changed[i]) {
            part.d1f0 -= step_size[i];
        }
        if (changed[i+1]) {
            part.d1f1 -= step_size[i+1]; 
        }

        makeMonotone(part);
        changed[i] = true;
        changed[i+1] = true;
        if (i > 0) {
            parts[i-1].d1f1 = part.d1f0;
            parts[i-1].d2f1 = part.d1f0;
            toBeProcessed.push(i-1);
        }
        if (i < n-1) {
            parts[i+1].d1f0 = part.d1f1;
            parts[i+1].d2f0 = part.d1f1;
            toBeProcessed.push(i+1);
        }
        parts[i] = part;    
    }

    parameters.resize(n-1);
    for (int i = 0; i < n-1; i++) {
        QuinPoly part = parts[i];
        double diff = part.diff;
        Eigen::MatrixXd A = Eigen::MatrixXd::Zero(6, 6);
        Eigen::VectorXd b = Eigen::VectorXd::Zero(6);

        A(0,0) = 1;
        b(0) = part.f0;
        
        A(1,0) = 1; 
        for (int j = 1; j <= 5; ++j) {
            A(1,j) =  std::pow(diff,j);
        }
        b(1) = part.f1;
        
        A(2,1) = 1;
        b(2) = part.d1f0;

        A(3,1) = 1;
        for (int j = 2; j <= 5; ++j) {
            A(3,j) =  j * std::pow(diff,j-1);
        }
        b(3) = part.d1f1;

        A(4,2) = 2;
        b(4) = part.d2f0;

        A(5,2) = 2;
        for (int j = 3; j <= 5; ++j) {
            A(5,j) =  j * (j-1) * std::pow(diff,j-1);
        }
        b(5) = part.d2f0;

        Eigen::VectorXd vec = A.fullPivHouseholderQr().solve(b); // use this as it most stable and don't care about speed.
        for (int j = 0; j < 6; ++j) {
            parameters[i][j] = vec(j);
        }
    }
}


size_t MonotoneQuinticSpline::getIndex(double x) const {
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


double MonotoneQuinticSpline::predict(double x) const {
    size_t index = getIndex(x);
    
    const double x0 = xs(index);
    const double xdiff = x - x0;

    const std::array<double, 6> params = parameters[index];
    return params[0] + params[1] * xdiff + params[2] * std::pow(xdiff,2) + params[3] * std::pow(xdiff,3) 
        + params[4] * std::pow(xdiff,4) + params[5] * std::pow(xdiff,5);
}

double MonotoneQuinticSpline::derivative(double x) const {
    size_t index = getIndex(x);
    
    const double x0 = xs(index);
    const double xdiff = x - x0;

    const std::array<double, 6> params = parameters[index];
    return params[1] + 2 * params[2] * xdiff + 3 * params[3] * std::pow(xdiff,2) 
        + 4 * params[4] * std::pow(xdiff,3) + 5 * params[5] * std::pow(xdiff,4);
}