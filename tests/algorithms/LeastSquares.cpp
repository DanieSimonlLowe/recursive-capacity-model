#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Eigen/Dense>
#include "algorithms/LeastSquares.h"

using testing::DoubleNear;
using testing::Pointwise;

class LeastSquaresTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up common test data
        tolerance = 1e-10;
    }

    double tolerance;
};

// Test basic linear regression with perfect fit
TEST_F(LeastSquaresTest, PerfectLinearFit) {
    // y = 2x + 1
    Eigen::MatrixXd xs(3, 2);
    xs << 1, 0,  // x=0, bias term
          1, 1,  // x=1, bias term
          1, 2;  // x=2, bias term
    
    Eigen::VectorXd ys(3);
    ys << 1, 3, 5;  // y = 2*x + 1
    
    LeastSquares ls(xs, ys);
    
    // Test predictions
    Eigen::VectorXd test_point1(2);
    test_point1 << 1, 0;  // x=0
    EXPECT_NEAR(ls.predict(test_point1), 1.0, tolerance);
    
    Eigen::VectorXd test_point2(2);
    test_point2 << 1, 1;  // x=1
    EXPECT_NEAR(ls.predict(test_point2), 3.0, tolerance);
    
    Eigen::VectorXd test_point3(2);
    test_point3 << 1, 3;  // x=3
    EXPECT_NEAR(ls.predict(test_point3), 7.0, tolerance);
}

// Test with noisy data
TEST_F(LeastSquaresTest, NoisyData) {
    // Approximate y = 2x + 1 with noise
    Eigen::MatrixXd xs(5, 2);
    xs << 1, 0,
          1, 1,
          1, 2,
          1, 3,
          1, 4;
    
    Eigen::VectorXd ys(5);
    ys << 1.1, 2.9, 5.1, 6.8, 9.2;  // y ≈ 2*x + 1 with noise
    
    LeastSquares ls(xs, ys);
    
    // Check that predictions are reasonable (within tolerance)
    Eigen::VectorXd test_point(2);
    test_point << 1, 2;
    double prediction = ls.predict(test_point);
    EXPECT_NEAR(prediction, 5.0, 0.5);  // Should be close to 5
}

// Test regularization (Ridge regression)
TEST_F(LeastSquaresTest, RegularizationEffect) {
    // Create overdetermined system with some noise
    Eigen::MatrixXd xs(4, 3);
    xs << 1, 1, 0,
          1, 2, 1,
          1, 3, 4,
          1, 4, 9;
    
    Eigen::VectorXd ys(4);
    ys << 2, 5, 10, 17;
    
    // Test without regularization
    LeastSquares ls_no_reg(xs, ys, 0.0);
    
    // Test with regularization
    LeastSquares ls_reg(xs, ys, 1.0);
    
    // Make predictions
    Eigen::VectorXd test_point(3);
    test_point << 1, 2, 4;
    
    double pred_no_reg = ls_no_reg.predict(test_point);
    double pred_reg = ls_reg.predict(test_point);
    
    // Regularization should generally produce different results
    EXPECT_NE(pred_no_reg, pred_reg);
}

// Test single feature regression
TEST_F(LeastSquaresTest, SingleFeature) {
    // y = 3x
    Eigen::MatrixXd xs(3, 1);
    xs << 1,
          2,
          3;
    
    Eigen::VectorXd ys(3);
    ys << 3, 6, 9;
    
    LeastSquares ls(xs, ys);
    
    Eigen::VectorXd test_point(1);
    test_point << 4;
    EXPECT_NEAR(ls.predict(test_point), 12.0, tolerance);
}

// Test minimum data case (exactly determined system)
TEST_F(LeastSquaresTest, ExactlyDeterminedSystem) {
    // 2 equations, 2 unknowns
    Eigen::MatrixXd xs(2, 2);
    xs << 1, 1,
          1, 2;
    
    Eigen::VectorXd ys(2);
    ys << 3, 5;  // y = 2x + 1
    
    LeastSquares ls(xs, ys);
    
    Eigen::VectorXd test_point1(2);
    test_point1 << 1, 0;
    EXPECT_NEAR(ls.predict(test_point1), 1.0, tolerance);
    
    Eigen::VectorXd test_point2(2);
    test_point2 << 1, 1;
    EXPECT_NEAR(ls.predict(test_point2), 3.0, tolerance);
}

// Test edge case: zero target values
TEST_F(LeastSquaresTest, ZeroTargets) {
    Eigen::MatrixXd xs(3, 2);
    xs << 1, 1,
          1, 2,
          1, 3;
    
    Eigen::VectorXd ys(3);
    ys << 0, 0, 0;
    
    LeastSquares ls(xs, ys);
    
    Eigen::VectorXd test_point(2);
    test_point << 1, 5;
    EXPECT_NEAR(ls.predict(test_point), 0.0, tolerance);
}

// Test numerical stability with large regularization
TEST_F(LeastSquaresTest, LargeRegularization) {
    Eigen::MatrixXd xs(3, 2);
    xs << 1, 1,
          1, 2,
          1, 3;
    
    Eigen::VectorXd ys(3);
    ys << 10, 20, 30;
    
    // Very large regularization should shrink weights towards zero
    LeastSquares ls(xs, ys, 1000.0);
    
    Eigen::VectorXd test_point(2);
    test_point << 1, 1;
    double prediction = ls.predict(test_point);
    
    // With heavy regularization, prediction should be much smaller
    // than without regularization
    EXPECT_LT(std::abs(prediction), 15.0);
}

// Test multivariate regression
TEST_F(LeastSquaresTest, MultivariateRegression) {
    // z = 2x + 3y + 1
    Eigen::MatrixXd xs(4, 3);
    xs << 1, 1, 1,  // bias, x=1, y=1
          1, 2, 1,  // bias, x=2, y=1
          1, 1, 2,  // bias, x=1, y=2
          1, 2, 2;  // bias, x=2, y=2
    
    Eigen::VectorXd ys(4);
    ys << 6, 8, 9, 11;  // z = 2x + 3y + 1
    
    LeastSquares ls(xs, ys);
    
    // Test prediction
    Eigen::VectorXd test_point(3);
    test_point << 1, 3, 2;  // x=3, y=2
    EXPECT_NEAR(ls.predict(test_point), 13.0, tolerance);  // 2*3 + 3*2 + 1 = 13
}

// Performance test for larger datasets
TEST_F(LeastSquaresTest, LargerDataset) {
    const int n_samples = 100;
    const int n_features = 5;
    
    Eigen::MatrixXd xs = Eigen::MatrixXd::Random(n_samples, n_features);
    Eigen::VectorXd true_weights = Eigen::VectorXd::Random(n_features);
    Eigen::VectorXd ys = xs * true_weights;
    
    // Add small amount of noise
    ys += 0.01 * Eigen::VectorXd::Random(n_samples);
    
    LeastSquares ls(xs, ys, 0.01);
    
    // Test that we can recover approximately the true weights
    // by checking predictions on training data
    for (int i = 0; i < std::min(10, n_samples); ++i) {
        Eigen::VectorXd x_test = xs.row(i);
        double prediction = ls.predict(x_test);
        EXPECT_NEAR(prediction, ys(i), 0.1);
    }
}
