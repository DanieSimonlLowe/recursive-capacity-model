# Interpolators

PolyRSLInterpolator
Best params: 2.11189 1000 13.538 3.78126 evaluate PolyRSLInterpolator
Voltage         MSE 76.2914      1-R^2 1592.63    Var: 0.0479027


evaluate TwoPointLinearInterpolator
Current         MSE 0.000112375  1-R^2 0.000130682        Var: 0.859912
Voltage         MSE 0.00287721   1-R^2 0.0597229  Var: 0.048176
Temperature     MSE 4.68396e-05  1-R^2 6.36706e-06        Var: 7.35655

evaluate HermiteSplineInterpolator
Current         MSE 0.0445552    1-R^2 0.0195562  Var: 2.27831
Voltage         MSE 1.49318      1-R^2 18.5461    Var: 0.0805121
Temperature     MSE 0.47722      1-R^2 0.150185   Var: 3.17754

10 window size (bigger dose not affect it)
evaluate CubicSplineInterpolator
Current         MSE 8.61628e-05  1-R^2 0.000100889        Var: 0.85404
Voltage         MSE 0.000535172  1-R^2 0.0111428  Var: 0.0480287
Temperature     MSE 4.8806e-05   1-R^2 6.62716e-06        Var: 7.36453

5 window size (bigger dose not affect it)
evaluate P16InterpolatorBase
Current         MSE 9.74507e-05  R^2 0.000113714        Var: 0.85698
Voltage         MSE 0.000640663  R^2 0.0133202  Var: 0.0480971
Temperature     MSE 5.34028e-05  R^2 7.2555e-06         Var: 7.36032


