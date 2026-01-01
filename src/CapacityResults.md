BayesianOptimize<
    BatteryModel<Rls2EcmStateEstimator<RecursiveLeastSquares>,
    CubicSplineInterpolator,CubicSplineInterpolator,
    SplineOcvSoc<AkimaSpline>,EkfSocEstimator,
    RlsCapacityEstimator<RecursiveLeastSquares>,
    Rls1EcmOcvEstimator<RecursiveLeastSquares>
    >>(2.1,ErrorMetric::CapacityError,false);


Best params: -3.29475 -1.48909 -6.00788 0.402752 0.798802 886.145 6.10192 1160.55 12.7228 -1.57991 581.192 2.53766 -0.916387 1.36425 0.357746
Final model created with optimized parameters
Capacity        MSE 0.839566
         1-R^2 0.872399