

#pragma once

#include <vector>
#include <string>
#include "mpi.h"

#include <iostream>
#include <new>
#include <cstdlib>
#include "ppointers.h"

using namespace std;

namespace PMA_NS
{
  class PInput: protected PPointers
  {
  public:
    PInput(class PMA *);
    ~PInput();
    void readInput();
    void readData();

    /* General hyperparameters */
    int nd; // number of dimensions
    int ni; // number of iterations
    int np; // number of population
    
    /* Differential evolution hyperparameters */
    double cr; // crossover rate
    double drate; // differential rate
    

  };
}

