#include "algorithms/splines/notKnotSpline.h"

NotKnotSpline::NotKnotSpline(const Eigen::VectorXd& t, const Eigen::VectorXd& y) {
    const int n = t.size();

    M = Eigen::VectorXd::Zero(n);

    // Intervals
    Eigen::VectorXd h = t.tail(n-1) - t.head(n-1);

    // Slopes
    Eigen::VectorXd slopes = (y.tail(n-1) - y.head(n-1)).cwiseQuotient(h);

    // RHS
    Eigen::VectorXd d = Eigen::VectorXd::Zero(n);
    d.segment(1, n-2) = 6.0 * (slopes.segment(1, n-2) - slopes.segment(0, n-2));
    
    // Build coefficient matrix A (n x n)
    Eigen::SparseMatrix<double> A(n, n);

    A.reserve(Eigen::VectorXi::Constant(n, 3));

    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(3 * n); // Reserve space to avoid reallocations
    // Not-a-knot condition at the first knot:
    //   (h1) * M0 - (h0 + h1) * M1 + (h0) * M2 = 0
    triplets.emplace_back(0, 0, h(1));
    triplets.emplace_back(0, 1, -(h(0) + h(1)));
    triplets.emplace_back(0, 2, h(0));
    d(0)   = 0.0;

    // Not-a-knot condition at the last knot:
    //   (h_{n-2}) * M_{n-3} - (h_{n-2} + h_{n-3}) * M_{n-2} + (h_{n-3}) * M_{n-1} = 0
    triplets.emplace_back(n-1, n-3, h(n-2));
    triplets.emplace_back(n-1, n-2, -(h(n-3) + h(n-2)));
    triplets.emplace_back(n-1, n-1, h(n-3));
    d(n-1)     = 0.0;

    // Solve
    A.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(A);
    if (solver.info() != Eigen::Success) {
        // Fallback to dense solver if sparse fails
        Eigen::MatrixXd A_dense = Eigen::MatrixXd(A);
        M = A_dense.fullPivLu().solve(d);
    } else {
        M = solver.solve(d);
    }

    this->y = y;
    this->t = t;
}

double NotKnotSpline::predict(double time) {
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


double NotKnotSpline::derivative(double time) { 
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