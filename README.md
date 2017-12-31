
compile with xtensor:
```
g++ -std=c++14 -O4 perfField3D.cpp -o analysis.exe -DXTENSOR
```

without xtensor:

```
g++ -std=c++14 -O4 perfField3D.cpp -o analysis.exe
```


run :

./analysis.exe 20 20 20

```
-----------------------------------------------------
Number of cells              : 20, 20, 20
Number of particles per cell : 100
Experiment repeated          : 5000 times
-----------------------------------------------------


3 new
random read access time  : 1675.53ms
random write access time : 2300.89ms
read access time         : 1493.19ms
write access time        : 1421ms
gradx time               : 8.7184ms
grady time               : 9.718ms
gradz time               : 9.261ms
average time             : 18.4572ms
-----------------------------------------------------

multiple allocations
random read access time  : 1737.98ms
random write access time : 2325.1ms
read access time         : 1560.24ms
write access time        : 1501.17ms
gradx time               : 6.5666ms
grady time               : 8.6734ms
gradz time               : 6.4144ms
average time             : 16.4214ms
-----------------------------------------------------

contiguous vector
random read access time  : 1461.82ms
random write access time : 2298.86ms
read access time         : 1406.62ms
write access time        : 1528.81ms
gradx time               : 12.4512ms
grady time               : 12.9382ms
gradz time               : 12.0328ms
average time             : 38.7036ms
-----------------------------------------------------

xtensor
random read access time  : 1574.19ms
random write access time : 2274.71ms
read access time         : 1557.79ms
write access time        : 1685.85ms
gradx time               : 40.5252ms
grady time               : 43.4696ms
gradz time               : 39.9708ms
average time             : 84.1964ms
-----------------------------------------------------
```
