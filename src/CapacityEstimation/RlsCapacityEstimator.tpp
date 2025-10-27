template<typename  RLS>
RlsCapacityEstimator<RLS>::RlsCapacityEstimator(const Eigen::VectorXd& params) {
    model(2); // dim = 2
    model->setParams(params);
    // [1, 1/area] = diffSoc
}

template<typename  RLS>
void RlsCapacityEstimator<RLS>::update(double current, double deltaTime, double diffSoc) {
    Eigen::VectorXd x(dimension);
    x(0) = 1;
    x(1) = 1/current * deltaTime;
    model->update(x,1/diffSoc);
}

void RlsCapacityEstimator<RLS>::getCapacity() {
    double slope = model->getState()(1);
    return slope / 3600.0;
}

void RlsCapacityEstimator<RLS>::getCapacityVariance() {
    return model->getCovariance()(1)(1);;
}

size_t RlsCapacityEstimator<RLS>::getParamsCount() {
    return RLS::getParamsCount();
}

size_t RlsCapacityEstimator<RLS>::getLowerBounds() {
    return RLS::getLowerBounds();
}

size_t RlsCapacityEstimator<RLS>::getUpperBounds() {
    return RLS::getUpperBounds();
}