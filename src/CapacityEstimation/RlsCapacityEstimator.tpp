template<typename  RLS>
RlsCapacityEstimator<RLS>::RlsCapacityEstimator(const double capacity,const Eigen::VectorXd& params) : model(2) {
    model.setParams(params);
    maxRelativeVariance = std::pow(10,params(RLS::getParamsCount()));
    // [1, 1/area] = diffSoc
    Eigen::VectorXd state = model.getState();
    state(1) = 1.0 / capacity;
    model.setState(state);
}

template<typename  RLS>
void RlsCapacityEstimator<RLS>::update(double current, double deltaTime, double diffSoc) {
    Eigen::VectorXd x(2);
    x(0) = 1;
    x(1) = -current * deltaTime * (1.0 / 3600.0);
    model.update(x,diffSoc);
}

template<typename  RLS>
double const RlsCapacityEstimator<RLS>::getCapacity() {
    const double slope = std::abs(model.getState()(1));
    if (slope < 1e-5) return 1e100;
    
    return 1.0 / slope;
}

template<typename  RLS>
double const RlsCapacityEstimator<RLS>::getCapacityVariance() {
    const double slope = std::abs(model.getState()(1));
    if (slope < 1e-5) return 1e100;
    const double var = std::abs(model.getCovariance()(1,1));
    // order 3 delta method from https://stats.stackexchange.com/questions/90287/delta-method-with-higher-order-terms-to-improve-variance-estimation-accuracy
    // https://math.stackexchange.com/questions/92648/calculation-of-the-n-th-central-moment-of-the-normal-distribution-mathcaln
    
    /*
    Let Dn be the nth component of the delta method linked to the nth  Central moment
    assume the distrbution is a normal distrubtion

    let g(x) = 1/x;

    https://stats.stackexchange.com/questions/90287/delta-method-with-higher-order-terms-to-improve-variance-estimation-accuracy
    D1 = g'(u)^2 * Var
    D1 = (-u^-2)^2 * Var
    D1 = u^-4 * Var

    https://stats.stackexchange.com/questions/90287/delta-method-with-higher-order-terms-to-improve-variance-estimation-accuracy
    D2 = g'(u)g''(u)E((X-u)^3) 
    E((X-u)^3) = 0 By https://math.stackexchange.com/questions/92648/calculation-of-the-n-th-central-moment-of-the-normal-distribution-mathcaln
    D2 = 0;

    https://stats.stackexchange.com/questions/90287/delta-method-with-higher-order-terms-to-improve-variance-estimation-accuracy
    D3 = (g''(u)^2/4 + 2g'(u)g'''(u)/3!)E((X-u)^4)
    E((X-u)^3) = 3*Var^2 By https://math.stackexchange.com/questions/92648/calculation-of-the-n-th-central-moment-of-the-normal-distribution-mathcaln
    D3 = (g''(u)^2/4 + 2g'(u)g'''(u)/3!)*3*Var^2
    D3 = (3/4*g''(u)^2 + 3*2 * g'(u)g'''(u)/3!)Var^2
    D3 = (3/4*g''(u)^2 + 3*2 * g'(u)g'''(u)/(3*2*1))*Var^2
    D3 = (3/4*g''(u)^2 + 3*2/(3*2*1) * g'(u)g'''(u))*Var^2
    D3 = (3/4*g''(u)^2 +  g'(u)g'''(u))*Var^2
    D3 = (3/4*(2*u^-3)^2 + (-u^-2)(-6*u^-4))*Var^2
    D3 = (3/4*4*u^-6 + 6*u^-2*u^-4)*Var^2
    D3 = (3*u^-6 + 6*u^-6)*Var^2
    D3 = 9*u^-6*Var^2

    As I am doing the delta method to the third order then 
    Var(g(X)) ≈ D1 + D2 + D3
    Var(g(X)) ≈ u^-4 * Var + 0 + 9*u^-6*Var^2
    Var(g(X)) ≈ u^-4 * Var + 9*u^-6*Var^2
    */
    
    const double dinv = 1.0 / (slope*slope);

    return var * dinv*dinv + 9*var*var * dinv*dinv*dinv;
}

template<typename  RLS>
bool const RlsCapacityEstimator<RLS>::canCalculateStateCapacity() {
    if (std::abs(model.getState()(1)) < 1e-5) return false;

    const double var = getCapacityVariance();
    const double cap = getCapacity();
    
    return var < cap * cap * maxRelativeVariance;
}

template<typename  RLS>
size_t RlsCapacityEstimator<RLS>::getParamsCount() {
    return RLS::getParamsCount() + 1;
}

template<typename  RLS>
const Eigen::VectorXd  RlsCapacityEstimator<RLS>::getLowerBounds() {
    Eigen::VectorXd v(getParamsCount());
    v.segment(0, RLS::getParamsCount()) = RLS::getLowerBounds();
    v(RLS::getParamsCount()) = -3;
    return v;
}

template<typename  RLS>
const Eigen::VectorXd  RlsCapacityEstimator<RLS>::getUpperBounds() {
    Eigen::VectorXd v(getParamsCount());
    v.segment(0, RLS::getParamsCount()) = RLS::getUpperBounds();
    v(RLS::getParamsCount()) = 3;
    return v;
}
