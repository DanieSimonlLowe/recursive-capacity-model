
// https://ieeexplore.ieee.org/document/10295043
template<typename RLS>
Rls1EcmOcvEstimator<RLS>::Rls1EcmOcvEstimator(const Eigen::VectorXd& params) : rls(4) {
    rls.setParams(params);
}

template<typename RLS>
void Rls1EcmOcvEstimator<RLS>::update(const Eigen::VectorXd& current, const Eigen::VectorXd& voltage) {
    size_t n = current.size(); // all are same length
    for (size_t i = 1; i < n; ++i) {
        // Do something with (c, v, t)
        Eigen::VectorXd input(4);
        input << -current[i], -current[i-1], voltage[i-1], 1;
        rls.update(input,voltage[i]);
    }
}

template<typename RLS>
void Rls1EcmOcvEstimator<RLS>::setDeltaTime(double deltaTime) {
    // Does nothing as delta time is used no where.
}

template<typename RLS>
bool Rls1EcmOcvEstimator<RLS>::canCalculateState() {
    // Rs w1/w2
    // R  (w0*w2+w1)/(w2*(w2-1))
    // C  -w2*w2/(T*(w0*w2 + w1))
    // OCV -w3/(w2 - 1)
    Eigen::VectorXd weights =  rls.getState();
    const double w0 = weights[0];
    const double w1 = weights[1];
    const double w2 = weights[2];
    const double w3 = weights[3];

    // if a*b > 0 then a/b > 0 and a=/=0 b=/=0

    const double eps = 1e-10; 
    if (std::abs(w2) <= eps) return false; // return false if Rs is undefined
    if (std::abs(w2*(w2-1)) <= eps) return false; // return false if R is undefined
    if (std::abs(w0*w2 + w1) <= eps) return false; // return false if C is undefined
    if (std::abs(w2 - 1) <= eps) return false; // return false if OCV is undefined

    if (w1/w2 <= eps) return false; // return false if Rs is negitve
    if ((w0*w2+w1)/(w2*(w2-1)) <= eps) return false; // return false if R is negitve
    //delta time is allways positive
    if (-w2*w2 / (w0*w2 + w1) <= eps) return false; // return false if C is negitve
    if (-w3 / (w2 - 1) <= eps) return false; // return false if OCV is negitve

    return true;
}

template<typename RLS>
double Rls1EcmOcvEstimator<RLS>::getOcv() {
    Eigen::VectorXd weights =  rls.getState();
    const double w2 = weights[2];
    const double w3 = weights[3];
    return -w3/(w2 - 1);
}


template<typename RLS>
size_t Rls1EcmOcvEstimator<RLS>::getParamsCount() {
    return RLS::getParamsCount();
}

template<typename RLS>
const Eigen::VectorXd Rls1EcmOcvEstimator<RLS>::getLowerBounds() {
    return RLS::getLowerBounds();
}

template<typename RLS>
const Eigen::VectorXd Rls1EcmOcvEstimator<RLS>::getUpperBounds() {
    return RLS::getUpperBounds();
}