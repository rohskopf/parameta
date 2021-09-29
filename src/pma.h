
/* Declaration of pointers used in the whole program. */

#pragma once
#include <string>
#include <vector>
#include "mpi.h"

/*
// LAMMPS include files
#include "lammps.h"
#include "input.h"
#include "atom.h"
#include "library.h"
#include "memory.h"
*/


namespace PMA_NS
{
    class PMA
    {
    public:


        //LAMMPS_NS::LAMMPS *lmp;
        // pmemory, perror, and pinput are the Parameta memory, error, and input classes.
        // the "p" before these names is to distinguish the parameter packages with other memory/error/input
        // packages that arise in other softwares like LAMMPS, should one wish to use those as well.
        class PMemory *pmemory;
        class PError *perror;
        class PInput *pinput;
        class PTimer *ptimer;
        class Objfun *objfun;
        class De *de;
        PMA(int, char **);
        ~PMA();
        //char **args;
        void create();
        void initialize();
        void finalize();
        void run(int,char **);
        int procs;
        int rank;
        int nepp; // num elements per proc

        int seed;

        int algorithm; // algorithm type

    };
}

