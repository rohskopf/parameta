#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "mpi.h"

#include "pinput.h"
#include "pmemory.h"

/*
// LAMMPS include files
#include "lammps.h"
#include "input.h"
#include "atom.h"
#include "library.h"
#include "memory.h"
*/

using namespace std;

//using namespace LAMMPS_NS;

using namespace PMA_NS;

PInput::PInput(PMA *pma) : PPointers(pma) {}

PInput::~PInput() 
{


};

void PInput::readInput()
{

}

void PInput::readData()
{

}
