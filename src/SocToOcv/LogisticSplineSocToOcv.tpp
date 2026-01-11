
template <typename T>
LogisticSplineSocToOcv<T>::LogisticSplineSocToOcv(const OcvSocData &data, const Eigen::VectorXd &params) {

    spline = new T((data.soc.array() / (1.0 - data.soc.array())).log(), data.ocv);
}

template <typename T> double LogisticSplineSocToOcv<T>::getOcv(double soc) {
    return spline->predict(std::log(soc / (1.0 - soc)));
}

template <typename T> double LogisticSplineSocToOcv<T>::getOcvToSocDerivative(double soc) {
    const double logit_soc = std::log(soc / (1.0 - soc));
    const double dOcv_dLogit = spline->derivative(logit_soc);
    const double dLogit_dSoc = 1.0 / (soc * (1.0 - soc));
    return dOcv_dLogit * dLogit_dSoc;
}
