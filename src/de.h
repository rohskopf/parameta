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
  class De: protected PPointers
  {
  public:
    De(class PMA *);
    ~De();

    /* Constructor variables */
    FILE * fh_debug;
    FILE * fh_history;
    FILE * fh_history_all;
    int rank;
    
    void initialize(int,char **);
    void run();
    
    /* Input arguments */
    int seed; // random number seed
    int nd; // dimension number
    int ng; // number of generations
    int np; // population number >= 4
    double cr; // crossover rate [0,1]
    double drate; // differential rate [0,2]
    int print_every; // print screen output every this many generations
    int print_all_solutions; // print all solutions in a HISTORY_ALL file

    double **intervals; // initial intervals for parameters

    /* Intermediate arguments */
    int nipp; // number individuals per proc
    int seed_proc; // unique seed for this proc

    /* DE variables */
    double *x; // np*nd array of solutions, the population
    double *x_sub; // nepp*nd array of solutions, the population
    double *u; // nd size array, serves as a holder for the new vector u
    bool *u_bool; // nd size array, showing which componenets of u are assigned
    double *v; // nd size array associated with single individual starting at x[a*nd]
    double *f; // objective function value of total population
    double *f_sub; // objective function value of sub population on a proc
  };
}

