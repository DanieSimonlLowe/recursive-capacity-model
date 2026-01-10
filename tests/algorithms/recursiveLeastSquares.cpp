#include <gtest/gtest.h>
#include <Eigen/Dense>
#include "Algorithms/RecursiveLeastSquares.h"

class RecursiveLeastSquaresTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup
        dimension = 3;
        forgettingFactor = 0.9;
        initialCovariance = 1.0;
    }

    size_t dimension;
    double forgettingFactor;
    double initialCovariance;
};

// Constructor Tests
TEST_F(RecursiveLeastSquaresTest, ConstructorValidParameters) {
    EXPECT_NO_THROW({
        RecursiveLeastSquares rls(dimension, forgettingFactor, initialCovariance);
    });
}

TEST_F(RecursiveLeastSquaresTest, ConstructorZeroDimension) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(0, forgettingFactor, initialCovariance);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorNegativeInitialCovariance) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(dimension, forgettingFactor, -1.0);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorZeroInitialCovariance) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(dimension, forgettingFactor, 0.0);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorInvalidForgettingFactorZero) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(dimension, 0.0, initialCovariance);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorInvalidForgettingFactorNegative) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(dimension, -0.1, initialCovariance);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorInvalidForgettingFactorGreaterThanOne) {
    EXPECT_THROW({
        RecursiveLeastSquares rls(dimension, 1.1, initialCovariance);
    }, std::invalid_argument);
}

TEST_F(RecursiveLeastSquaresTest, ConstructorForgettingFactorExactlyOne) {
    EXPECT_NO_THROW({
        RecursiveLeastSquares rls(dimension, 1.0, initialCovariance);
    });
}

// Initialization Tests
TEST_F(RecursiveLeastSquaresTest, InitialStateIsZero) {
    RecursiveLeastSquares rls(dimension, forgettingFactor, initialCovariance);
    Eigen::VectorXd state = rls.getState(); // Note: method name has typo "gate" instead of "get"
    
    EXPECT_EQ(state.size(), dimension);
    for (int i = 0; i < dimension; ++i) {
        EXPECT_DOUBLE_EQ(state(i), 0.0);
    }
}

TEST_F(RecursiveLeastSquaresTest, InitialCovarianceIsIdentityScaled) {
    RecursiveLeastSquares rls(dimension, forgettingFactor, initialCovariance);
    Eigen::MatrixXd covariance = rls.getCovariance();
    
    EXPECT_EQ(covariance.rows(), dimension);
    EXPECT_EQ(covariance.cols(), dimension);
    
    Eigen::MatrixXd expected = Eigen::MatrixXd::Identity(dimension, dimension) * initialCovariance;
    
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            EXPECT_DOUBLE_EQ(covariance(i, j), expected(i, j));
        }
    }
}

// update Tests
TEST_F(RecursiveLeastSquaresTest, SingleupdateBasic) {
    RecursiveLeastSquares rls(2, 1.0, 1.0);
    
    Eigen::VectorXd x(2);
    x << 1.0, 0.0;
    double y = 2.0;
    
    EXPECT_NO_THROW({
        rls.update(x, y);
    });
    
    // After update, state should be non-zero
    Eigen::VectorXd state = rls.getState();
    EXPECT_NE(state.norm(), 0.0);
}

TEST_F(RecursiveLeastSquaresTest, MultipleupdatesConvergence) {
    RecursiveLeastSquares rls(2, 0.99, 100.0);
    
    // Generate data from known linear relationship: y = 2*x1 + 3*x2
    std::vector<Eigen::VectorXd> inputs;
    std::vector<double> outputs;
    
    for (int i = 0; i < 100; ++i) {
        Eigen::VectorXd x(2);
        x << i * 0.1, (i + 10) * 0.05;
        double y = 2.0 * x(0) + 3.0 * x(1);
        
        inputs.push_back(x);
        outputs.push_back(y);
        
        rls.update(x, y);
    }
    
    Eigen::VectorXd finalState = rls.getState();
    
    // Should converge close to [2.0, 3.0]
    EXPECT_NEAR(finalState(0), 2.0, 0.1);
    EXPECT_NEAR(finalState(1), 3.0, 0.1);
}

TEST_F(RecursiveLeastSquaresTest, updateWithNoisyData) {
    RecursiveLeastSquares rls(1, 0.95, 10.0);
    
    // Simple 1D case: y = 5*x + noise
    for (int i = 0; i < 50; ++i) {
        Eigen::VectorXd x(1);
        x << i * 0.1;
        double y = 5.0 * x(0) + 0.1 * (rand() % 100 - 50) / 100.0; // small noise
        
        rls.update(x, y);
    }
    
    Eigen::VectorXd state = rls.getState();
    EXPECT_NEAR(state(0), 5.0, 0.5); // Should be close to 5.0 despite noise
}

TEST_F(RecursiveLeastSquaresTest, updateWithZeroInput) {
    RecursiveLeastSquares rls(2, 0.9, 1.0);
    
    Eigen::VectorXd x = Eigen::VectorXd::Zero(2);
    double y = 5.0;
    
    Eigen::VectorXd stateBefore = rls.getState();
    
    EXPECT_NO_THROW({
        rls.update(x, y);
    });
    
    Eigen::VectorXd stateAfter = rls.getState();
    
    // With zero input, state should remain unchanged
    for (int i = 0; i < 2; ++i) {
        EXPECT_DOUBLE_EQ(stateBefore(i), stateAfter(i));
    }
}

TEST_F(RecursiveLeastSquaresTest, updateWithInconsistentDimensions) {
    RecursiveLeastSquares rls(3, 0.9, 1.0);
    
    Eigen::VectorXd x(2); // Wrong dimension
    x << 1.0, 2.0;
    double y = 3.0;
    
    // This should ideally throw an exception or handle gracefully
    // The current implementation might crash or produce undefined behavior
    // Consider adding dimension checking in the actual implementation
}

// Edge Cases
TEST_F(RecursiveLeastSquaresTest, VerySmallForgettingFactor) {
    RecursiveLeastSquares rls(2, 0.001, 1.0);
    
    Eigen::VectorXd x(2);
    x << 1.0, 1.0;
    double y = 2.0;
    
    EXPECT_NO_THROW({
        rls.update(x, y);
    });
}

TEST_F(RecursiveLeastSquaresTest, LargeInitialCovariance) {
    RecursiveLeastSquares rls(2, 0.9, 1e6);
    
    Eigen::MatrixXd covariance = rls.getCovariance();
    EXPECT_DOUBLE_EQ(covariance(0, 0), 1e6);
    EXPECT_DOUBLE_EQ(covariance(1, 1), 1e6);
}

// Getter Method Tests
TEST_F(RecursiveLeastSquaresTest, GettersReturnCorrectTypes) {
    RecursiveLeastSquares rls(dimension, forgettingFactor, initialCovariance);
    
    Eigen::VectorXd state = rls.getState();
    Eigen::MatrixXd covariance = rls.getCovariance();
    
    EXPECT_EQ(state.size(), dimension);
    EXPECT_EQ(covariance.rows(), dimension);
    EXPECT_EQ(covariance.cols(), dimension);
}

// Regression Test for Common Linear System
TEST_F(RecursiveLeastSquaresTest, LinearRegressionExample) {
    RecursiveLeastSquares rls(3, 0.98, 10.0);
    
    // Test data for y = 1.0 + 2.0*x1 + 3.0*x2 (with intercept term)
    std::vector<std::array<double, 4>> testData = {
        {1.0, 1.0, 1.0, 6.0},  // [1, 1, 1] -> 1 + 2*1 + 3*1 = 6
        {1.0, 2.0, 1.0, 8.0},  // [1, 2, 1] -> 1 + 2*2 + 3*1 = 8
        {1.0, 1.0, 2.0, 9.0},  // [1, 1, 2] -> 1 + 2*1 + 3*2 = 9
        {1.0, 3.0, 2.0, 13.0}, // [1, 3, 2] -> 1 + 2*3 + 3*2 = 13
        {1.0, 2.0, 3.0, 14.0}  // [1, 2, 3] -> 1 + 2*2 + 3*3 = 14
    };
    
    for (const auto& data : testData) {
        Eigen::VectorXd x(3);
        x << data[0], data[1], data[2];
        double y = data[3];
        rls.update(x, y);
    }
    
    Eigen::VectorXd finalState = rls.getState();
    
    // Should converge close to [1.0, 2.0, 3.0]
    EXPECT_NEAR(finalState(0), 1.0, 0.1);
    EXPECT_NEAR(finalState(1), 2.0, 0.1);
    EXPECT_NEAR(finalState(2), 3.0, 0.1);
}

// Performance/Stress Test
TEST_F(RecursiveLeastSquaresTest, ManyupdatesStabilityTest) {
    RecursiveLeastSquares rls(5, 0.999, 1.0);
    
    for (int i = 0; i < 1000; ++i) {
        Eigen::VectorXd x = Eigen::VectorXd::Random(5);
        double y = x.sum(); // Simple relationship
        
        EXPECT_NO_THROW({
            rls.update(x, y);
        });
        
        // Check that state and covariance remain finite
        Eigen::VectorXd state = rls.getState();
        Eigen::MatrixXd covariance = rls.getCovariance();
        
        EXPECT_TRUE(state.allFinite());
        EXPECT_TRUE(covariance.allFinite());
    }
}

// Diagnostic Tests to Identify Root Causes of Failures
TEST_F(RecursiveLeastSquaresTest, DiagnosticForgettingFactorAssignment) {
    // Test if forgetting factor is being assigned correctly
    RecursiveLeastSquares rls(2, 0.9, 1.0);
    
    // Create a simple update to see if forgetting factor is used properly
    Eigen::VectorXd x(2);
    x << 1.0, 0.0;
    double y = 1.0;
    
    // Get initial covariance
    Eigen::MatrixXd covBefore = rls.getCovariance();
    
    rls.update(x, y);
    
    // Get covariance after update
    Eigen::MatrixXd covAfter = rls.getCovariance();
    
    // The covariance should change in a predictable way
    // This will help us see if the forgetting factor is working correctly
    std::cout << "Initial covariance diagonal: " << covBefore.diagonal().transpose() << std::endl;
    std::cout << "Final covariance diagonal: " << covAfter.diagonal().transpose() << std::endl;
    
    // If forgetting factor is not assigned, updates might behave unexpectedly
    EXPECT_NE(covBefore(0,0), covAfter(0,0)); // Should change
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticExceptionHandling) {
    // Test if exceptions are thrown correctly (they should throw std::invalid_argument, not pointers)
    try {
        RecursiveLeastSquares rls(0, 0.9, 1.0);
        FAIL() << "Expected std::invalid_argument";
    } catch (std::invalid_argument& e) {
        SUCCEED();
    } catch (...) {
        FAIL() << "Wrong exception type thrown";
    }
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticMatrixOperations) {
    // Test if matrix operations are working correctly
    RecursiveLeastSquares rls(2, 0.9, 1.0);
    
    Eigen::VectorXd x(2);
    x << 1.0, 1.0;
    double y = 2.0;
    
    // Check dimensions before update
    Eigen::MatrixXd cov = rls.getCovariance();
    Eigen::VectorXd state = rls.getState();
    
    std::cout << "Covariance before update:\n" << cov << std::endl;
    std::cout << "State before update: " << state.transpose() << std::endl;
    
    rls.update(x, y);
    
    cov = rls.getCovariance();
    state = rls.getState();
    
    std::cout << "Covariance after update:\n" << cov << std::endl;
    std::cout << "State after update: " << state.transpose() << std::endl;
    
    // Check if matrices remain valid
    EXPECT_TRUE(cov.allFinite());
    EXPECT_TRUE(state.allFinite());
    EXPECT_EQ(cov.rows(), 2);
    EXPECT_EQ(cov.cols(), 2);
    EXPECT_EQ(state.size(), 2);
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticGainCalculation) {
    // Test the gain calculation step by step
    RecursiveLeastSquares rls(2, 0.5, 1.0); // Use 0.5 for easier calculation
    
    Eigen::VectorXd x(2);
    x << 1.0, 0.0;
    double y = 1.0;
    
    // Manual calculation of what should happen:
    // spread = x^T * P * x = [1 0] * I * [1; 0] = 1
    // gain = P * x / (lambda + spread) = I * [1; 0] / (0.5 + 1) = [1; 0] / 1.5 = [0.667; 0]
    
    Eigen::MatrixXd covBefore = rls.getCovariance();
    double expectedSpread = x.transpose() * covBefore * x;
    std::cout << "Expected spread: " << expectedSpread << std::endl;
    
    rls.update(x, y);
    
    Eigen::VectorXd stateAfter = rls.getState();
    std::cout << "State after update: " << stateAfter.transpose() << std::endl;
    
    // With forgetting factor 0.5, x=[1,0], y=1, initial state=[0,0]
    // error = y - x^T * state = 1 - 0 = 1
    // Expected state = [0,0] + [0.667,0] * 1 = [0.667, 0]
    EXPECT_NEAR(stateAfter(0), 0.667, 0.01);
    EXPECT_NEAR(stateAfter(1), 0.0, 0.01);
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticCovarianceupdate) {
    // Test if covariance update formula is working correctly
    RecursiveLeastSquares rls(1, 1.0, 2.0); // Simple 1D case, no forgetting
    
    Eigen::VectorXd x(1);
    x << 1.0;
    double y = 1.0;
    
    Eigen::MatrixXd covBefore = rls.getCovariance();
    std::cout << "Covariance before: " << covBefore(0,0) << std::endl;
    
    rls.update(x, y);
    
    Eigen::MatrixXd covAfter = rls.getCovariance();
    std::cout << "Covariance after: " << covAfter(0,0) << std::endl;
    
    // For 1D case with forgetting factor = 1:
    // spread = x^T * P * x = 1 * 2 * 1 = 2
    // gain = P * x / (lambda + spread) = 2 * 1 / (1 + 2) = 2/3
    // New covariance = (1/lambda) * (P - gain * x^T * P) = 1 * (2 - (2/3) * 1 * 2) = 2 - 4/3 = 2/3
    double expected = 2.0/3.0;
    EXPECT_NEAR(covAfter(0,0), expected, 0.01);
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticSimpleConvergence) {
    // Very simple convergence test with known answer
    RecursiveLeastSquares rls(1, 1.0, 1.0); // No forgetting
    
    // Simple case: y = 3*x, use same input multiple times
    Eigen::VectorXd x(1);
    x << 1.0;
    double y = 3.0;
    
    std::cout << "Step-by-step convergence:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        Eigen::VectorXd state = rls.getState();
        std::cout << "Step " << i << ": state = " << state(0) << std::endl;
        rls.update(x, y);
    }
    
    Eigen::VectorXd finalState = rls.getState();
    std::cout << "Final state: " << finalState(0) << std::endl;
    
    // Should converge towards 3.0
    EXPECT_GT(finalState(0), 2.0); // At least moving in right direction
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticMemoryIssue) {
    // Test if there are any memory or reference issues
    {
        RecursiveLeastSquares rls(2, 0.9, 1.0);
        
        Eigen::VectorXd x(2);
        x << 1.0, 1.0;
        
        for (int i = 0; i < 10; ++i) {
            rls.update(x, i);
            
            // Get references and check they're valid
            Eigen::VectorXd state = rls.getState();
            Eigen::MatrixXd cov = rls.getCovariance();
            
            EXPECT_TRUE(state.allFinite());
            EXPECT_TRUE(cov.allFinite());
        }
    } // rls goes out of scope here
    
    SUCCEED(); // If we get here without crashes, memory handling is OK
}

TEST_F(RecursiveLeastSquaresTest, DiagnosticNumericalStability) {
    // Test numerical stability with very small/large numbers
    RecursiveLeastSquares rls(2, 0.999, 1e-6); // Very small initial covariance
    
    Eigen::VectorXd x(2);
    x << 1e-3, 1e-3; // Very small inputs
    double y = 1e-6; // Very small output
    
    EXPECT_NO_THROW(rls.update(x, y));
    
    Eigen::VectorXd state = rls.getState();
    Eigen::MatrixXd cov = rls.getCovariance();
    
    // Should still be finite
    EXPECT_TRUE(state.allFinite());
    EXPECT_TRUE(cov.allFinite());
    
    // Now test with larger numbers
    x << 1e3, 1e3;
    y = 1e6;
    
    EXPECT_NO_THROW(rls.update(x, y));
    
    state = rls.getState();
    cov = rls.getCovariance();
    
    EXPECT_TRUE(state.allFinite());
    EXPECT_TRUE(cov.allFinite());
}
