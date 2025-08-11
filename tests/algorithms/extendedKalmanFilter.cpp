// extended_kalman_filter_test.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Eigen/Dense>
#include <stdexcept>

#include "algorithms/extendedKalmanFilter.h" // adjust include path if needed

// Concrete EKF for tests: linear prediction x' = A x + b, measurement z = H x.
// Jacobians are constant A and H.
class TestExtendedKalmanFilter : public ExtendedKalmanFilter {
public:
    TestExtendedKalmanFilter(const Eigen::MatrixXd &A,
                             const Eigen::VectorXd &b,
                             const Eigen::MatrixXd &H,
                             const Eigen::MatrixXd &Q,
                             const Eigen::MatrixXd &R,
                             const Eigen::VectorXd &x0,
                             const Eigen::MatrixXd &P0)
        : ExtendedKalmanFilter(Q, R, x0, P0), A_(A), b_(b), H_(H)
    {
        assert(A_.rows() == A_.cols());
        assert(H_.cols() == x0.size());
    }

protected:
    Eigen::VectorXd predictionFunction(const Eigen::VectorXd &state) override {
        return A_ * state + b_;
    }

    Eigen::MatrixXd predictionJacobian(const Eigen::VectorXd & /*state*/) override {
        return A_;
    }

    Eigen::VectorXd measurementFunction(const Eigen::VectorXd &state) override {
        return H_ * state;
    }

    Eigen::MatrixXd measurementJacobian(const Eigen::VectorXd & /*state*/) override {
        return H_;
    }

private:
    Eigen::MatrixXd A_;
    Eigen::VectorXd b_;
    Eigen::MatrixXd H_;
};

// helpers
void expectMatrixNear(const Eigen::MatrixXd &expected, const Eigen::MatrixXd &actual, double tol = 1e-9) {
    ASSERT_EQ(expected.rows(), actual.rows());
    ASSERT_EQ(expected.cols(), actual.cols());
    for (int r = 0; r < expected.rows(); ++r) {
        for (int c = 0; c < expected.cols(); ++c) {
            EXPECT_NEAR(expected(r,c), actual(r,c), tol) << "at (" << r << "," << c << ")";
        }
    }
}

void expectVectorNear(const Eigen::VectorXd &expected, const Eigen::VectorXd &actual, double tol = 1e-9) {
    ASSERT_EQ(expected.size(), actual.size());
    for (int i = 0; i < expected.size(); ++i) {
        EXPECT_NEAR(expected(i), actual(i), tol) << "at (" << i << ")";
    }
}

// ------------------------- Tests -------------------------

TEST(ExtendedKalmanFilterConstructor, ThrowsOnQSizeMismatch) {
    const int n = 2;
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(n);
    Eigen::MatrixXd P0 = Eigen::MatrixXd::Identity(n, n);
    Eigen::MatrixXd R = Eigen::MatrixXd::Identity(1, 1);

    Eigen::MatrixXd Q_bad = Eigen::MatrixXd::Identity(3, 3);

    EXPECT_THROW({
        TestExtendedKalmanFilter A_dummy(Eigen::MatrixXd::Identity(n,n),
                                         Eigen::VectorXd::Zero(n),
                                         Eigen::MatrixXd::Zero(1,n),
                                         Q_bad,
                                         R,
                                         x0,
                                         P0);
    }, std::invalid_argument);
}

TEST(ExtendedKalmanFilterConstructor, ThrowsOnPSizeMismatch) {
    const int n = 2;
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(n);
    Eigen::MatrixXd P_bad = Eigen::MatrixXd::Identity(3, 3); // wrong
    Eigen::MatrixXd Q = Eigen::MatrixXd::Identity(n,n);
    Eigen::MatrixXd R = Eigen::MatrixXd::Identity(1, 1);

    EXPECT_THROW({
        TestExtendedKalmanFilter A_dummy(Eigen::MatrixXd::Identity(n,n),
                                         Eigen::VectorXd::Zero(n),
                                         Eigen::MatrixXd::Zero(1,n),
                                         Q,
                                         R,
                                         x0,
                                         P_bad);
    }, std::invalid_argument);
}

TEST(ExtendedKalmanFilterConstructor, ThrowsOnRNotSquare) {
    const int n = 2;
    Eigen::VectorXd x0 = Eigen::VectorXd::Zero(n);
    Eigen::MatrixXd P0 = Eigen::MatrixXd::Identity(n,n);
    Eigen::MatrixXd Q = Eigen::MatrixXd::Identity(n,n);

    Eigen::MatrixXd R_bad(1,2);
    R_bad << 1, 0;

    EXPECT_THROW({
        TestExtendedKalmanFilter A_dummy(Eigen::MatrixXd::Identity(n,n),
                                         Eigen::VectorXd::Zero(n),
                                         Eigen::MatrixXd::Zero(1,n),
                                         Q,
                                         R_bad,
                                         x0,
                                         P0);
    }, std::invalid_argument);
}

TEST(ExtendedKalmanFilterPredict, UpdatesStateAndCovariance) {
    // 2D linear system
    Eigen::MatrixXd A(2,2);
    A << 1.0, 0.1,
         0.0, 1.0;
    Eigen::VectorXd b(2);
    b << 0.5, -0.2;

    Eigen::MatrixXd H(1,2);
    H << 1.0, 0.0;

    Eigen::VectorXd x0(2);
    x0 << 1.0, 2.0;

    Eigen::MatrixXd P0 = 0.1 * Eigen::MatrixXd::Identity(2,2);
    Eigen::MatrixXd Q = 0.01 * Eigen::MatrixXd::Identity(2,2);
    Eigen::MatrixXd R = 0.1 * Eigen::MatrixXd::Identity(1,1);

    TestExtendedKalmanFilter ekf(A, b, H, Q, R, x0, P0);

    // Expected after predict:
    Eigen::VectorXd expected_x = A * x0 + b;
    Eigen::MatrixXd expected_P = A * P0 * A.transpose() + Q;

    ekf.predict();

    Eigen::VectorXd x_after = ekf.getState();
    Eigen::MatrixXd P_after = ekf.getCovariance();

    expectVectorNear(expected_x, x_after, 1e-9);
    expectMatrixNear(expected_P, P_after, 1e-9);
}

TEST(ExtendedKalmanFilterMeasure, CorrectStateAndCovarianceJosephForm) {
    // 2D system
    Eigen::MatrixXd A = Eigen::MatrixXd::Identity(2,2); // not used for predict here
    Eigen::VectorXd b(2);
    b << 0.0, 0.0;

    Eigen::MatrixXd H(1,2);
    H << 1.0, 0.0;

    Eigen::VectorXd x0(2);
    x0 << 2.0, -1.0;

    Eigen::MatrixXd P0(2,2);
    P0 << 0.5, 0.1,
          0.1, 0.4;

    Eigen::MatrixXd Q = 0.0 * Eigen::MatrixXd::Identity(2,2);
    Eigen::MatrixXd R(1,1);
    R << 0.2;

    TestExtendedKalmanFilter ekf(A, b, H, Q, R, x0, P0);

    Eigen::VectorXd measurement(1);
    measurement << 4.0;

    // Manual EKF linear update (Joseph form)
    Eigen::MatrixXd S = H * P0 * H.transpose() + R;
    Eigen::MatrixXd K = P0 * H.transpose() * S.inverse();
    Eigen::VectorXd z_pred = H * x0;
    Eigen::VectorXd expected_x = x0 + K * (measurement - z_pred);

    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(P0.rows(), P0.cols());
    Eigen::MatrixXd temp = (I - K * H);
    Eigen::MatrixXd expected_P = temp * P0 * temp.transpose() + K * R * K.transpose();

    ekf.measure(measurement);

    Eigen::VectorXd x_after = ekf.getState();
    Eigen::MatrixXd P_after = ekf.getCovariance();

    expectVectorNear(expected_x, x_after, 1e-9);
    expectMatrixNear(expected_P, P_after, 1e-9);
}

// Optional: ensure repeated predict/measure behaves consistently (smoke test)
TEST(ExtendedKalmanFilterSequence, PredictMeasureSequence) {
    Eigen::MatrixXd A(2,2);
    A << 1.0, 0.0,
         0.0, 1.0;
    Eigen::VectorXd b = Eigen::VectorXd::Zero(2);
    Eigen::MatrixXd H(1,2);
    H << 1.0, 0.0;

    Eigen::VectorXd x0(2);
    x0 << 0.0, 0.0;
    Eigen::MatrixXd P0 = Eigen::MatrixXd::Identity(2,2);
    Eigen::MatrixXd Q = 0.1 * Eigen::MatrixXd::Identity(2,2);
    Eigen::MatrixXd R = 0.5 * Eigen::MatrixXd::Identity(1,1);

    TestExtendedKalmanFilter ekf(A, b, H, Q, R, x0, P0);

    // Step 1: predict
    ekf.predict();
    Eigen::VectorXd x1 = ekf.getState();
    Eigen::MatrixXd P1 = ekf.getCovariance();

    // For this A==I and b==0, state shouldn't change
    expectVectorNear(x0, x1, 1e-9);
    expectMatrixNear(P1, P0 + Q, 1e-9);

    // Step 2: measure with some value
    Eigen::VectorXd meas(1); meas << 1.0;
    ekf.measure(meas);

    Eigen::VectorXd x2 = ekf.getState();
    Eigen::MatrixXd P2 = ekf.getCovariance();

    // Basic sanity checks: covariance should be finite and smaller (or equal) in observed subspace
    ASSERT_TRUE((P2.array().isFinite()).all());
    EXPECT_LE(P2.trace(), P1.trace() + 1e-9); // overall uncertainty should not blow up
}
