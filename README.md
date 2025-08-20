

Setup instructions:
cd /mnt/c/Users/Danie/Desktop/project/build

cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make all
./tests/EVLifeCycle_test

./EVLifeCycle_main

0.269693,1,9.65345e-05,0.999982

TODO Try:
Before that use EKF with
f(x) = a + bx + csin(wx) + dcos(wx)

Notes:

do cubic spline, (both types)
https://math.libretexts.org/Bookshelves/Applied_Mathematics/Numerical_Methods_(Chasnov)/05%3A_Interpolation/5.03%3A_Cubic_Spline_Interpolation

do RSL

https://www.sciencedirect.com/science/article/abs/pii/S0167865514000786
Recursive GP regression 

As an alternative to explicit harmonic regression, a Gaussian Process (GP) regressor with a composite kernel can model a linear trend plus periodicity. For example, use a periodic kernel (e.g. the exponentiated-sine-squared kernel) combined with a linear (dot-product) kernel
