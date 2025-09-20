#include <Eigen/Dense>

class LeastSquares {
public:
    LeastSquares(const Eigen::MatrixXd& xs, const Eigen::VectorXd& ys, double regularizationConstant = 0);
    double predict(const Eigen::VectorXd& x);

private:
    Eigen::VectorXd weights;
};
