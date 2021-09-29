#include <stdlib.h>
#include <iostream>
#include "pma.h"
#include "mpi.h"

using namespace PMA_NS;

int main(int argc, char **argv)
{

  /* Initialize MPI */
  MPI_Init(&argc,&argv);

  /* Begin a PMA instance */
  PMA *pma = new PMA(argc, argv);

  /* Delete the memory */
  delete pma;

  /* Close MPI */
  int MPI_Comm_free(MPI_Comm *comm);
  MPI_Finalize();

  return EXIT_SUCCESS;
}

