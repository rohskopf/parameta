

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
  class Objfun: protected PPointers
  {
  public:
    Objfun(class PMA *);
    ~Objfun();

    double calculate(double*);

    /* Constructor variables */
    FILE * fh_debug; // = fopen("DEBUG_DE" ,"w");
    int rank;
    

  };
}

