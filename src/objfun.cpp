#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>       /* round, floor, ceil, trunc */
#include <algorithm>

#include "pinput.h"
#include "pmemory.h"
#include "objfun.h"
#include "de.h"

//using namespace LAMMPS_NS;

using namespace PMA_NS;

Objfun::Objfun(PMA *pma) : PPointers(pma) 
{

	rank = MPI::COMM_WORLD.Get_rank ( ); // Get_rank gets the rank of the calling process in the communicator
	char debug[64];
	sprintf (debug, "D_OBJFUN_PROC%d", rank);
    //std::cout << debug << std::endl;
	fh_debug = fopen(debug, "w");

}

Objfun::~Objfun() {};

double Objfun::calculate(double *v)
{

    //double value = v[0]*v[0] + v[1]*v[1]; // paraboloid

    double pi = 3.14159265359;

    double value = 10.0*2.0 + (v[0]*v[0] - 10.0*cos(2.0*pi*v[0])) + (v[1]*v[1] - 10.0*cos(2.0*pi*v[1]));

    return value;
}
