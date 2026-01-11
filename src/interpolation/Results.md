# Interpolators
evaluate TwoPointLinearInterpolator
Current         MSE 0.000112375  1-R^2 0.000130682        Var: 0.859912
Voltage         MSE 0.00287721   1-R^2 0.0597229  Var: 0.048176
Temperature     MSE 4.68396e-05  1-R^2 6.36706e-06        Var: 7.35655
 

# poly RSL
Best params: 2.84325 985.926 14.9734 5.87084 
evaluate P16InterpolatorBase
Current         MSE 25.1534      1-R^2 29.6572    Var: 0.848138
Voltage         MSE 76.4179      1-R^2 1595.27    Var: 0.0479027
Temperature     MSE 296.321      1-R^2 40.1822    Var: 7.37444

# Poly Sin EFK
Best params: 1.23695 0.000138322 0.0225726 14.9957 76.2866 evaluate P16InterpolatorBase
Current         MSE 127.846      R^2 152.351    Var: 0.839154
Voltage         MSE 220.378      R^2 4635.26    Var: 0.047544
Temperature     MSE 142.03       R^2 19.3138    Var: 7.35381
Reason: Failed to get it to sableize.


# evaluate HermiteSplineInterpolator
Current         MSE 0.0445552    1-R^2 0.0195562  Var: 2.27831
Voltage         MSE 1.49318      1-R^2 18.5461    Var: 0.0805121
Temperature     MSE 0.47722      1-R^2 0.150185   Var: 3.17754


10 window size (bigger dose not affect it)
# evaluate CubicSplineInterpolator
Current         MSE 8.61628e-05  1-R^2 0.000100889        Var: 0.85404
Voltage         MSE 0.000535172  1-R^2 0.0111428  Var: 0.0480287
Temperature     MSE 4.8806e-05   1-R^2 6.62716e-06        Var: 7.36453
Time Taken 6445.47 ms

evaluate FastCubicSplineInterpolator
count 10:
Current         MSE 3.24132e-07  1-R^2 1.06964e-07        Var: 3.0303
Voltage         MSE 1.76308e-05  1-R^2 0.000845074        Var: 0.020863
Temperature     MSE 0.000148825  R^2 6.48866e-05        Var: 2.29362
Time taken: 5074.79 ms

count 50:
Current         MSE 1.06394e-07  1-R^2 3.51216e-08        Var: 3.02931
Voltage         MSE 1.5204e-05   1-R^2 0.00268823         Var: 0.00565578
Temperature     MSE 0.000248523  1-R^2 7.54508e-05        Var: 3.29383
Time taken: 3607.81 ms
Time taken: 4630.65 ms
Time taken: 3685.5 ms

count 100:
Current         MSE 1.12756e-07  1-R^2 3.72261e-08        Var: 3.02895
Voltage         MSE 1.59691e-05  1-R^2 0.000729733        Var: 0.0218834
Temperature     MSE 0.000244356  1-R^2 3.41562e-05        Var: 7.15407
Time taken: 5171.45 ms

count 200:
evaluate P16InterpolatorBase
Current         MSE 1.20642e-07  1-R^2 4.02508e-08        Var: 2.99725
Voltage         MSE 1.88026e-05  1-R^2 0.000316596        Var: 0.0593898
Temperature     MSE 0.00048865   1-R^2 2.73998e-05        Var: 17.8341
Time taken: 4032.93 ms


20 window size
# evaluate NotKnotSplineInterpolator
Current         MSE 6.39878e-05  1-R^2 7.54473e-05        Var: 0.848113
Voltage         MSE 0.000441548  1-R^2 0.0091     Var: 0.0485217
Temperature     MSE 4.46601e-05  1-R^2 6.02622e-06        Var: 7.41095

# AkimaSplineInterpolator
Window Size 50
Current         MSE 3.90979e-05  1-R^2 4.75699e-05      Var: 0.821905
Voltage         MSE 0.000303107  1-R^2 0.00675616       Var: 0.0448637
Temperature     MSE 4.17838e-05  1-R^2 5.77133e-06      Var: 7.23989
Reason for no improvment: Not monotone so no magor improvment and diffrance is small so might not be real.