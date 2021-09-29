#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <random>
#include "mpi.h"

#include "pinput.h"
#include "pmemory.h"
#include "perror.h"
#include "objfun.h"

#include "de.h"

using namespace std;

using namespace PMA_NS;

De::De(PMA *pma) : PPointers(pma) 
{

	rank = MPI::COMM_WORLD.Get_rank ( ); // Get_rank gets the rank of the calling process in the communicator
	char debug[64];
	sprintf (debug, "D_DE_PROC%d", rank);
    //std::cout << debug << std::endl;
	fh_debug = fopen(debug, "w");

    if (rank==0){
	    fh_history = fopen("HISTORY", "w");
    }

}

De::~De() 
{

    pmemory->deallocate(intervals);
    pmemory->deallocate(x);
    pmemory->deallocate(x_sub);
    pmemory->deallocate(u);
    pmemory->deallocate(u_bool);
    pmemory->deallocate(v);
    pmemory->deallocate(f);
    pmemory->deallocate(f_sub);

    fclose(fh_debug);

    if (rank==0){
	    fclose(fh_history);
    }

    if (print_all_solutions==1){
        if (rank==0) fclose(fh_history_all);
    }

};

/*
Initialization function sets input variables
*/
void De::initialize(int nargs,char **args)
{

    if (rank==0){
        printf(" Initializing DE.\n");
    }

    if (nargs != 10){
        //printf("Error: Number of given arguments %d should be %d. Realize that the executable counts as an argument.\n",nargs,9);
        char error_message[128];
        sprintf (error_message, "Number of given arguments %d should be %d.", nargs,11);
        perror->exit("de.cpp", error_message);
    }
    // Extract input arguments and hyperparameters
    seed = atoi(args[2]); // random number seed
    //nepp = atoi(args[3]); // number elements per proc
    nd = atoi(args[3]); // dimension number
    ng = atoi(args[4]); // number of generations
    np = atoi(args[5]); // population number >= 4
    cr = atof(args[6]); // crossover rate [0,1]
    drate = atof(args[7]); // differential rate [0,2]
    print_every = atoi(args[8]); // interval to print screen output of generations
    print_all_solutions = atoi(args[9]); // bool (0 or 1) to create a HISTORY_ALL file
    if (print_all_solutions==1){
        if (rank==0) fh_history_all = fopen("HISTORY_ALL", "w");
    }

    nipp = np/pma->procs; // number individuals per proc
    if (nipp < 4){
        char error_message[128];
        sprintf (error_message, "Number of individuals per proc (nipp) less than 4.");
        perror->exit("de.cpp", error_message);
    }

    if (floor(nipp) != nipp){
        char error_message[128];
        sprintf (error_message, "Population size %d not divisible by number of procs %d.", np, pma->procs);
        perror->exit("de.cpp", error_message);
    }

    if (rank==0){
        printf(" nd: %d\n", nd);
        printf(" ng: %d\n", ng);
        printf(" np: %d\n", np);
        printf(" cr: %f\n", cr);
        printf(" nipp: %d\n", nipp);
    }
    
    // Create a new seed based on number of seconds since the new year
    time_t now;
    struct tm newyear;
    double seconds;
    time(&now);  // get current time; same as: now = time(NULL)
    newyear = *localtime(&now);
    newyear.tm_hour = 0; newyear.tm_min = 0; newyear.tm_sec = 0;
    newyear.tm_mon = 0;  newyear.tm_mday = 1;
    seconds = difftime(now,mktime(&newyear));
    // Add this number of seconds to the seed
    seed = seed + seconds;
    // Add the rank to get unique seed for this proc
    seed_proc = seed + rank;

    // Read the INTERVALS file for parameter intervals and decimals
    pmemory->allocate(intervals,nd,2);
    ifstream fh_intervals("INTERVALS");
    string line;
    double i1,i2,dec;
    for (int i=0; i<nd; i++){
        getline(fh_intervals,line);
        stringstream ss(line);
        ss >> intervals[i][0] >> intervals[i][1];
        if (line==""){
            char error_message[128];
            sprintf (error_message, "Number of lines in interval file is not %d.", nd);
            perror->exit("de.cpp", error_message);
        }
    }
    fh_intervals.close();
  
    // Initialize random population, a (np x nd) array
    pmemory->allocate(x,np*nd);
    std::default_random_engine generator(seed);
    int individual_indx = 0;
    for (int i=0; i<np; i++){
        for (int j=0; j<nd; j++){
            //printf("intervals[j]: %d %f %f\n",j, intervals[j][0],intervals[j][1]);
            std::uniform_real_distribution<double> distribution(intervals[j][0],intervals[j][1]);

            //x[i][j] = distribution(generator);

            x[j+individual_indx] = distribution(generator);
        }
        individual_indx = individual_indx + nd;
    }

    pmemory->allocate(x_sub, nipp*nd);
    // Scatter the random numbers from the root process to all processes in the MPI world
    MPI_Scatter(x, nipp*nd, MPI::DOUBLE, x_sub, nipp*nd, MPI::DOUBLE, 0, MPI_COMM_WORLD);

    /*
    fprintf(fh_debug, "Full population:\n");
    for (int i=0; i<np*nd; i++){
        fprintf(fh_debug, "  %f\n", x[i]);
    }
    fprintf(fh_debug, "Sub population:\n");
    for (int i=0; i<nipp*nd; i++){
        fprintf(fh_debug, "  %f\n", x_sub[i]);
    }
    */

    /* Allocate other arrays to be used later. */
    pmemory->allocate(u,nd);
    pmemory->allocate(u_bool,nd);
    pmemory->allocate(v,nd);
    pmemory->allocate(f,np);
    pmemory->allocate(f_sub,nipp);

} //initialize

/*
Run the differential evolution algorithm.
*/
void De::run()
{

    if (rank==0){
        printf(" Starting DE run.\n");
    }

	if (np % pma->procs !=0) perror->exit("de.cpp", "Population size not divisible by procs.");
  
    std::mt19937 engine(seed_proc); //Standard mersenne_twister_engine seeded with the argument

    // The following distributions are seeded with the previously declared engine (mt19937)
    std::uniform_int_distribution<> dist_nipp (0, nipp-1);
    std::uniform_int_distribution<> dist_np (0, np-1);
    std::uniform_int_distribution<> dist_nd (0, nd-1);

    std::default_random_engine engine2(seed_proc); // for choosing random [0,1]
    std::uniform_real_distribution<double> dist_1(0.0,1.0); // for choosing random [0,1]
    int a_global; // index of a, representing index of an individual in total population
    int b,c,d; // indices representing individuals in x array, starting from 0
    int indx_a,indx_b, indx_c, indx_d; // indices of the individual and its component in the actual array
    int iota; // random starting index for changing vector "u"
    int lambda; // random length to be changed on vector "u"
    double r01; // random number between 0 and 1
    double fu,fv; // objective function values f(u) and f(v)
    double minimum; // minimum of objective function values for whole population
    int indx_min; // index of minimum objective function for whole population

    if (rank==0) printf(" Generation | Z\n");
    for (int g=0; g<ng+1; g++){

        //fprintf(fh_debug,"--- Generation: %d\n", g);
        for (int a=0; a<nipp; a++){

            a_global = rank*nipp + a;
            //fprintf(fh_debug, "a: %d\n", a);
            //fprintf(fh_debug, "a_global: %d\n", a_global);

            for (int j=0; j<nd; j++){
                u_bool[j] = true;
                v[j] = x_sub[a*nd + j];
            }
            //a = m;
            //fprintf(fh_debug, "ind: %d\n", ind);
            /*
            Pick 3 random individuals (b,c,d), not "a", at random from the whole population
            */
            do {
               b = dist_np (engine);
            } while (b == a_global);
            do {
               c = dist_np (engine);
            } while (c == a_global || c==b);
            do {
               d = dist_np (engine);
            } while (d == a_global || d==b || d==c);
            if (b==a_global || c==a_global || d==a_global){
                perror->exit("de.cpp", "Incorrectly picked (b,c,d) indices.");
            }

            /*
            Pick a random index "iota" in [0,nd-1], the index where we'll start mixing on "u".
            */

            iota = dist_nd (engine);
            //fprintf(fh_debug,"  iota: %d\n", iota);

            /*
            Pick a random length "lambda" which determines the length of changed "u".
            */

            lambda = 0;
            while (dist_1(engine2) < cr && lambda < nd-1){
                //fprintf(fh_debug, "  lambda: %d\n", lambda);
                lambda=lambda+1;
            }
            //fprintf(fh_debug,"a b c d: %d %d %d %d\n", a,b,c,d);
            //fprintf(fh_debug,"  lambda: %d\n", lambda);

            for (int l=0; l<lambda; l++){
                int j = (iota+l) % nd;
                //fprintf(fh_debug, " j: %d\n", j);
                indx_b = b*nd + j;
                indx_c = c*nd + j;
                indx_d = d*nd + j;
                u[j] = x[indx_b] + drate*(x[indx_c]-x[indx_d]);
                u_bool[j] = false;
            }

            for (int j=0; j<nd; j++){
                //fprintf(fh_debug, "j: %d\n", j);
                if (u_bool[j]){
                    indx_a = a*nd + j;
                    u[j] = x_sub[indx_a];
                }
                
            } // for (int j=0; j<nd; j++)
     
            /*
            for (int j=0; j<nd; j++){
                fprintf(fh_debug, "    u[%d]: %f\n", j,u[j]);
            }
            */

            /* Compare objective function values for new array "u" and original array. */

            fu = pma->objfun->calculate(u);
            fv = pma->objfun->calculate(v);
            MPI_Barrier(MPI_COMM_WORLD);

            // Replace v with u in x population if f(u) < f(v), for minimization.
            if (fu < fv){
                f_sub[a] = fu;
                for (int j=0; j<nd; j++){
                    x_sub[a*nd + j] = u[j];
                }
                //fprintf(fh_debug, "    REPLACE!!!\n");
            }
            else{
                f_sub[a] = fv;
            }
            
            //printf("%f %f\n", fu,fv);
        } // for (int a=0; a<nipp; a++){

        // Collect individuals back on root proc to find minimums and to update the population.
        MPI_Allgather(x_sub,nipp*nd, MPI::DOUBLE, x, nipp*nd, MPI::DOUBLE, MPI_COMM_WORLD);
        MPI_Allgather(f_sub,nipp, MPI::DOUBLE, f, nipp, MPI::DOUBLE, MPI_COMM_WORLD);

        /*
        fprintf(fh_debug, "Full population:\n");
        for (int i=0; i<np*nd; i++){
            fprintf(fh_debug, "  %f\n", x[i]);
        }    

        fprintf(fh_debug, "Full population objective function values:\n");
        for (int i=0; i<np; i++){
            fprintf(fh_debug, "  %f\n", f[i]);
        }     
        */
        
        minimum = f[0];
        indx_min = 0;
        for (int i=1; i<np; i++){
            if (f[i] < minimum) {
                minimum = f[i];
                indx_min = i;
            }
        }

        //fprintf(fh_debug, "%f %f %f\n", x[indx_min*nd],x[indx_min*nd+1], minimum);
        if (g % print_every == 0){
	        if (rank==0) printf(" %d %f\n", g,minimum);  
        }
        if (rank==0) fprintf(fh_history, "%f %f %f\n", x[indx_min*nd],x[indx_min*nd+1], minimum);
        if (rank==0){
            if (print_all_solutions==1){
                int indx = 0;
                fprintf(fh_history_all, "\n");
                for (int i=0; i<np; i++){
                    fprintf(fh_history_all, "%f %f %f\n", x[indx], x[indx+1], f[i]);
                    indx = indx+nd;
                }
            }
        }


    } // for (int g=0; g<ng; g++){

} //run



