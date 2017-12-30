COMPILE with Xtensor
g++  -std=c++14 field3db.cpp -O3 -o analysis.exe -DXTENSOR

COMPILE without Xtensor
g++  -std=./analysis.exe 20 20 20


Output :
-----------------------------------------------------
Number of cells              : 20, 20, 20
Number of particles per cell : 100
Experiment repeated          : 1000 times
-----------------------------------------------------


multiple allocations
random read access time  : 1809.21ms 0.00180921sec
random write access time : 2368.81ms 0.00236881sec
gradx time               : 10.459ms 1.0459e-05sec
grady time               : 11.922ms 1.1922e-05sec
gradz time               : 8.528ms 8.528e-06sec
average time             : 19.14ms 1.914e-05sec
-----------------------------------------------------

contiguous vector
random read access time  : 1576.58ms 0.00157658sec
random write access time : 2293.83ms 0.00229383sec
gradx time               : 11.485ms 1.1485e-05sec
grady time               : 11.471ms 1.1471e-05sec
gradz time               : 14.907ms 1.4907e-05sec
average time             : 37.468ms 3.7468e-05sec
-----------------------------------------------------

xtensor
random read access time  : 1795.66ms 0.00179566sec
random write access time : 2298.61ms 0.00229861sec
gradx time               : 48.411ms 4.8411e-05sec
grady time               : 43.83ms 4.383e-05sec
gradz time               : 33.907ms 3.3907e-05sec
average time             : 85.575ms 8.5575e-05sec
-----------------------------------------------------
