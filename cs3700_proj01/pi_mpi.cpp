#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>

#define POINTS 1000000

int main (int argc,  char *argv[]) {

   int myid, numprocs;
   int namelen;
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);

   printf("Process %d on %s\n", myid, processor_name);

   int s = (int)floor(POINTS/numprocs);
   int s0 = s + POINTS%numprocs;

   int startIndex = s0 + (myid-1)*s;
   int endIndex = startIndex + s;

   srand((unsigned)(myid));

   double startwtime;
   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int inside = 0;
   double x;
   double y;

   if (myid == 0)
   {
      // master worker - comput the master's numbers

       for(i = 0; i < s0; i++)
       {
           x = ((double)rand()) / ((double)RAND_MAX);
           y = ((double)rand()) / ((double)RAND_MAX);

           x *= x;
           y *= y;

           if((x + y) < 1)
               inside++;
       }
   }

   else
   {
       //slave's work

       for(i = startIndex; i < endIndex; i++)
       {
           x = ((double)rand()) / ((double)RAND_MAX);
           y = ((double)rand()) / ((double)RAND_MAX);

           x *= x;
           y *= y;

           if((x + y) < 1)
               inside++;
       }
   }

   int sum = 0;

   MPI_Reduce(&inside, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f\n",
             runTime);

      printf("Points: %d\n", POINTS);

      double ratio = (double)sum/(double)POINTS;
      double pi = 4 * ratio;

      printf("Calculated pi: %f\n", pi);
      printf("Delta pi: %f\n", pi-M_PI);
   }

   MPI_Finalize();
}

