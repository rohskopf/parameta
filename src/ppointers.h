

#pragma once

#include "mpi.h"

/*
// LAMMPS include files
#include "lammps.h"
#include "input.h"
#include "atom.h"
#include "library.h"
#include "memory.h"
*/

#include "pma.h"

namespace PMA_NS
{
    class PPointers
    {
    public:
        PPointers(PMA *ptr) :
            pma(ptr),
            pmemory(ptr->pmemory),
			perror(ptr->perror),
            pinput(ptr->pinput),
            objfun(ptr->objfun),
            ptimer(ptr->ptimer)
            //lmp(ptr->lmp)
            {}

        virtual ~PPointers() {}

    protected:
        PMA *pma;
        PMemory *&pmemory;
		PError *&perror;
        PInput *&pinput;
        PTimer *&ptimer;
        Objfun *&objfun;
        //LAMMPS_NS::LAMMPS *&lmp;
    };
}

