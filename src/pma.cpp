

#include <iostream>
#include <iomanip>
#include <vector>
// pmemory, perror, and pinput are the Parameta memory, error, and input classes.
// the "p" before these names is to distinguish the parameter packages with other memory/error/input
// packages that arise in other softwares like LAMMPS, should one wish to use those as well.
#include "pmemory.h"
#include "perror.h"
#include "pinput.h"
#include "ptimer.h"
#include "objfun.h"
#include "de.h"
#include "mpi.h"

using namespace std;

using namespace PMA_NS;

PMA::PMA(int nargs, char **args)
{

  
  /************************** Set up MPI settings **************************/

  int color,key,global,local;
  MPI_Comm comm;

  // Split the communicators so that multiple instances of LAMMPS can be run
  MPI_Comm_rank(MPI_COMM_WORLD, &global);
  color = global / 1; // Change "1" to 2 in order to use 2 procs per instance, etc..
  key = global; 
  MPI_Comm_split(MPI_COMM_WORLD, color, key, &comm);
  MPI_Comm_rank(comm,&local);

  //  Get the number of processes.
  procs = MPI::COMM_WORLD.Get_size ( ); //Get_size gets number of processes (np) in communicator group
  //  Get the individual process ID.
  rank = MPI::COMM_WORLD.Get_rank ( ); // Get_rank gets the rank of the calling process in the communicator

  //nepp = atoi(arg[2]); // Num elements per proc

  /************************** Initial Screen Output **************************/
  if (rank == 0)
  {
    std::cout << " +-----------------------------------------------------------------+" << std::endl;
    std::cout << " +                           Parameta 0.0                          +" << std::endl;
    std::cout << " +-----------------------------------------------------------------+" << std::endl;
    std::cout << " Running on " << procs << " procs" << std::endl;
  }
  
  ptimer = new PTimer(this);
  if (rank == 0) std::cout << " Job started at " << ptimer->DateAndTime() << std::endl;

  /************************** Create LAMMPS Pointer **************************/
  char *args1[] = {
    (char *) "lmp",
    (char *) "-screen",
    (char*) "none",
  0};

  //lmp = new LAMMPS_NS::LAMMPS(3,args1,comm);

  // Extract desired algorithm
  // printf("narg: %d\n", narg);
  algorithm = atoi(args[1]);

  /************************** Proceed with Parameta **************************/

  // Dynamically allocated pointers
  create();

  // Grab user input to initialize settings
  initialize();

  // Run 
  run(nargs,args);

  // Delete dynamically allocated pointers
  finalize();

  if (rank == 0) std::cout << std::endl << " Job finished at " 
        << ptimer->DateAndTime() << std::endl;
}

void PMA::create()
{
    // Create dynamically allocated pointers
    pmemory = new PMemory(this);
    perror = new PError(this);
    pinput = new PInput(this);
    objfun = new Objfun(this);
    if (algorithm==1){
        de = new De(this);
    }
}

void PMA::initialize()
{
  pinput->readInput();
  pinput->readData();
}

void PMA::run(int nargs, char **args)
{
  if (algorithm==1){
    de->initialize(nargs,args);
    de->run();
  }
}

PMA::~PMA()
{
  //delete lmp;
  delete ptimer;

}

void PMA::finalize()
{

    delete pmemory;
    delete perror;
    delete pinput;
    delete objfun;
    if (algorithm==1){
        delete de;
    }
}

