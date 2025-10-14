AkimaSpline outproformed all others, followed closly by FritschCarlsonSpline as diffrance is very small and FritschCarlsonSpline is garnateed to be montone I will use that.

# CubicSplineSoc OcvCurve
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       8.48757e-07
1 - R^2                  8.84775e-06

# NotKnotSplineSoc OcvCurve
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       2.12849e-06
1 - R^2                  2.21882e-05

# LookUpTableSocOcvCurve
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.000101976
1 - R^2                  0.00106304

# FritschCarlsonSpline OcvSoc
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       2.87892e-07
1 - R^2                  3.00109e-06

# AkimaSpline OcvSoc
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       2.70638e-07
1 - R^2                  2.82123e-06

# PolynomialSpline 3
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       7.66329e-07
1 - R^2                  7.9885e-06

# PolynomialSpline 5
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       1.5175e-05
1 - R^2                  0.00015819

# PolynomialSpline 7
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       9.31185e-07
1 - R^2                  9.70702e-06


# PolynomialSpline 9
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       2.05401e-06
1 - R^2                  2.14117e-05

https://tchlux.github.io/papers/tchlux-2019-HPC-1.pdf

# LogExpOcvSocCurve
Optimal parameters:
  param[0] = -2.389
  param[1] = 942.691
Final objective value (MSE): 0.292692
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.300467
1 - R^2                  3.13218

Optimal parameters:
  param[0] = -2.3864
  param[1] = 961.047
Final objective value (MSE): 0.289394
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.297127
1 - R^2                  3.09736

Optimal parameters:
  param[0] = -2.69453
  param[1] = 223.25
Final objective value (MSE): 0.301242
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.309124
1 - R^2                  3.22242


# PolyLsSocOcvCurve
Optimal parameters:
  param[0] = 19.6745
  param[1] = -16.0179
Final objective value (MSE): 2.49824e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       3.5732e-05
1 - R^2                  0.000372483

Optimal parameters:
  param[0] = 19.3511
  param[1] = -16.4636
Final objective value (MSE): 2.49824e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       3.5732e-05
1 - R^2                  0.000372483

Optimal parameters:
  param[0] = 19.6579
  param[1] = -19.7636
Final objective value (MSE): 2.49824e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       3.5732e-05
1 - R^2                  0.000372483

# LogPolyLsSocOcvCurve
Optimal parameters:
  param[0] = 23.7515
  param[1] = -9.69155
Final objective value (MSE): 1.53314e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       1.49916e-05
1 - R^2                  0.000156278

Optimal parameters:
  param[0] = 24.0447
  param[1] = -10.2347
Final objective value (MSE): 1.54057e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       1.58477e-05
1 - R^2                  0.000165203

Optimal parameters:
  param[0] = 20.0681
  param[1] = -10.2481
Final objective value (MSE): 1.50928e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       1.57016e-05
1 - R^2                  0.000163679

# NegPolyLsSocOcvCurve
Optimal parameters:
  param[0] = 44.307
  param[1] = -80
  param[2] = 6.33724
Final objective value (MSE): 6.64785e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       5.83597e-06
1 - R^2                  6.08363e-05

Optimal parameters:
  param[0] = 46.5105
  param[1] = -73.9167
  param[2] = 11.4351
Final objective value (MSE): 6.71293e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       1.2558e-05
1 - R^2                  0.000130909


Optimal parameters:
  param[0] = 36.8282
  param[1] = -36.8178
  param[2] = 3.27915
Final objective value (MSE): 8.20955e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       6.6538e-06
1 - R^2                  6.93616e-05

# ExpLsSocOcvCurve
Optimal parameters:
  param[0] = 49.998
  param[1] = -79.9913
  param[2] = 7.17583
Final objective value (MSE): 0.00098752
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.000989494
1 - R^2                  0.0103148

Optimal parameters:
  param[0] = 0.0587717
  param[1] = -15.1292
  param[2] = 13.3955
Final objective value (MSE): 3.69878e-05
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       5.21902e-05
1 - R^2                  0.00054405

Optimal parameters:
  param[0] = 48.7516
  param[1] = -79.9891
  param[2] = 5.06215
Final objective value (MSE): 0.00155964
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       0.00191178
1 - R^2                  0.0199291

# LogNegPolyLsSocOcvCurve
Optimization completed!
Optimal parameters:
  param[0] = 47.7591
  param[1] = -80.7018
  param[2] = 23.4558
Final objective value (MSE): 6.11249e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       5.88516e-06
1 - R^2                  6.13491e-05

Optimal parameters:
  param[0] = 49.9648
  param[1] = -91.8831
  param[2] = 5.2904
Final objective value (MSE): 6.14881e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       5.72682e-06
1 - R^2                  5.96984e-05

Optimal parameters:
  param[0] = 49.9988
  param[1] = -56.0625
  param[2] = 5.40927
Final objective value (MSE): 6.14881e-06
Final model performance on test set:
OCV variance             0.0959291
OCV mean                 3.71098
Mean squared error       5.72682e-06
1 - R^2                  5.96984e-05

todo test all linear models shown
https://www.researchgate.net/publication/363656090_Open-Circuit_Voltage_Models_for_Battery_Management_Systems_A_Review

todo once I have a best one try a version that uses localy weighted least sqears to see if I can get it even better.