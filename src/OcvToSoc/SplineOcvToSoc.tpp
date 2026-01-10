
template <typename T>
SplineOcvToSoc<T>::SplineOcvToSoc(const OcvSocData &data, const Eigen::VectorXd &params) {
    spline = new T(data.ocv, data.soc);
}

template <typename T> double SplineOcvToSoc<T>::getSoc(double ocv) { return spline->predict(ocv); }
