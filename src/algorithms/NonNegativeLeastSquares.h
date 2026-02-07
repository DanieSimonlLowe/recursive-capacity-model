#include <Eigen/Dense>
#include <vector>

Eigen::VectorXd NonNegativeLeastSquares(Eigen::MatrixXd &A, Eigen::VectorXd &Y,
                                        std::vector<bool> &canBeNegative, const double tolerance);
