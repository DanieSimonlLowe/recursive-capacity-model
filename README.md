cd /mnt/c/Users/Danie/Desktop/project/build

cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"
make all
./tests/EVLifeCycle_test

./EVLifeCycle_main


TODO Try:
Before that use EKF with
f(x) = a + bx + csin(wx) + dcos(wx)

https://www.sciencedirect.com/science/article/abs/pii/S0167865514000786
Recursive GP regression 

As an alternative to explicit harmonic regression, a Gaussian Process (GP) regressor with a composite kernel can model a linear trend plus periodicity. For example, use a periodic kernel (e.g. the exponentiated-sine-squared kernel) combined with a linear (dot-product) kernel
