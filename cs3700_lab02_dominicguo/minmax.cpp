#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>

#define ARRAY_SIZE 1000000

int main (int argc,  char *argv[]) {

   int myid, numprocs;
   int namelen;
   int* numbers = new int[ARRAY_SIZE];
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);
 
   printf("Process %d on %s\n", myid, processor_name);
 
   for (int i=0; i<ARRAY_SIZE; i++)
      numbers[i] = rand() % ARRAY_SIZE;  //could be randomly generated

   int s = (int)floor(ARRAY_SIZE/numprocs);
   int s0 = s + ARRAY_SIZE%numprocs;

   int startIndex = s0 + (myid-1)*s;
   int endIndex = startIndex + s;

   double startwtime;
   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int max_num = numbers[0]
   int min_num = numbers[0]

   if (myid == 0) {
      // master worker - comput the master's numbers

      for (i=0; i<s0; i++) {
         if(numbers[i] < min_num)
             min_num = numbers[i];

         if(numbers[i] > max_num)
             max_num = numbers[i];
      }

      printf("Process %d - startIndex 0 endIndex %d; min %ld, max %ld\n",
             myid, s0-1, min_num, max_num);
   } else {
      //slave's work
      for (i= startIndex; i<endIndex; i++) {
         if(numbers[i] < min_num)
             min_num = numbers[i];

         if(numbers[i] > max_num)
             max_num = numbers[i];
      }

      printf ("Process %d - startIndex %d endIndex %d; min %ld, max %ld\n",
              myid, startIndex, endIndex-1, min_num, max_num);
   }

   int minimum = ARRAY_SIZE;
   MPI_Reduce(&min_num, &minimum, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

   int maximum = 0;
   MPI_Reduce(&max_num, &maximum, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f minimum = %ld maximum = %ld\n",
             runTime, minimum, maximum);
   }

   delete[] numbers;

   MPI_Finalize();
}

