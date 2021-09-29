# Parameta - Parallel Heuristics

Parameta is a massively parallel C++ code for optimization using [metaheuristics](https://en.wikipedia.org/wiki/Metaheuristic). This is useful when the exact functional form of the objective function is either unknown or hard to obtain, therefore rendering gradient-based methods futile.

This is a modular code, so different algorithms and objective functions are meant to be coded by the user.

Currently there is a single optimization algorithm, differential evolution, written in `de.cpp`.

The user simply needs to edit `objfun.cpp` to have the desired objective function. *The objective function depends on whatever is coded in objfun.cpp and objfun.h*

## Running Parameta.

First install by going into the `src` directory and doing:

    make clean
    make

This makes a Parameta executable called `pma`.

To execute the Parameta executable on `nprocs` processors, do:

    mpirun -np nprocs pma algorithm setting1 setting2 ...

where:

- `algorithm` is a number, currently only supporting `1` for differential evolution.
- `settings` are arguments specific to the algorithm being used.

Since the specific form of the executing command depends on the algorithm, we will explain the algorithms below.

## Differential evolution.

This algorithm requires 10 settings, which are described in `de.cpp`.

To run a differential evolution fit, do:

    mpirun -np nprocs pma 1 seed nd ng np cr drate nprint printall

where 

- `1` is the algorithm setting declaring to use the differential evolution algorithm in `de.cpp`.
- `seed` is a random number seed, which can be any integer.
- `nd` is the number of parameters.
- `ng` is the number of generations.
- `np` is the number of individuals in a population.
- `cr` is the crossover rate, which is any float between 0 and 1.
- `drate` is the differential rate, which is any float between 0 and 2.
- `nprint` tells the program to print progress every this many timesteps.
- `printall` is a flag, 0 or 1. A value of 0 tells the program either not to print all parameters per generation, while 1 prints this info.

Important notes:
- This algorithm is parallelized over the number of individuals in the population, i.e. the number of objective function calls per generation.
- The given population size must be divisible by the number of processes, e.g. np = 80 and nprocs = 8 is fine because 80/8=10.
- The number of individuals per process must be >= 4 for the differential evolution algorithm to work, e.g. np = 80 and nprocs = 8 is fine because 80/8=10 is greater than 4.


## Example - minimizing the 3D Rastrigin function with differential evolution.

Please see the `example` directory.
