

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

TODO do this later cause I want to try it. (Is a 2ECM model.)
https://www.mdpi.com/1996-1073/14/4/1054


https://www.sciencedirect.com/science/article/abs/pii/S0167865514000786
Recursive GP regression 

