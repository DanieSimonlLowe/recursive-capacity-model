template<typename  RLS>
RlsCapacityEstimator<RLS>::RlsCapacityEstimator(const Eigen::VectorXd& params) : model(2) {
    model.setParams(params);
    // [1, 1/area] = diffSoc
}

template<typename  RLS>
void RlsCapacityEstimator<RLS>::update(double current, double deltaTime, double diffSoc) {
    Eigen::VectorXd x(2);
    x(0) = 1;
    x(1) = 1/current * deltaTime;
    model.update(x,1/diffSoc);
}

template<typename  RLS>
double RlsCapacityEstimator<RLS>::getCapacity() {
    double slope = model.getState()(1);
    return slope / 3600.0;
}

template<typename  RLS>
double RlsCapacityEstimator<RLS>::getCapacityVariance() {
    return model.getCovariance()(1,1);;
}

template<typename  RLS>
size_t RlsCapacityEstimator<RLS>::getParamsCount() {
    return RLS::getParamsCount();
}

template<typename  RLS>
const Eigen::VectorXd  RlsCapacityEstimator<RLS>::getLowerBounds() {
    return RLS::getLowerBounds();
}

template<typename  RLS>
const Eigen::VectorXd  RlsCapacityEstimator<RLS>::getUpperBounds() {
    return RLS::getUpperBounds();
}