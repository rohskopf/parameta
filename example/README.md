# Simple example - minimizing the 3D Rastrigin function.

![](test.gif)

The [Rastrigin function](https://en.wikipedia.org/wiki/Rastrigin_function) is a typical benchmark for global optimization problems. Here we will minimize the 3D function, which is already coded into `objfun.cpp` in the `src` directory.

To run the code on 4 processors for example, do:

    mpirun -np 4 pma 1 1010 2 100 20 0.5 1 1 1

To plot the optimization progress as a gif, do:

    python plot.py
