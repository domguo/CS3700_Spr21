#!/usr/bin/python3

from mpi4py import MPI
import numpy
import math

world = MPI.COMM_WORLD
numprocs = world.size
myid = world.rank
procname = MPI.Get_processor_name()

print('Process %d on %s' %(myid, procname))

TRIALS = 20
ARRAY_SIZE = 1000000

# initialize array with 0..n-1
numbers = numpy.arange(ARRAY_SIZE)

s = ARRAY_SIZE // numprocs
s0 = s + ARRAY_SIZE%numprocs

startIndex = s0 + (myid-1)*s
endIndex = startIndex + s

totalTime = 0
for j in range(0, TRIALS):
    if myid == 0:
        startwtime = MPI.Wtime()

    part_sum = None

    if myid == 0: # master worker
        part_sum = numpy.sum(numbers[0:s0])
        print("Trial %d: Master %d - s0 %d; part_sum %ld"
            %(j, myid, s0, part_sum))

    else: # slave worker
        part_sum = numpy.sum(numbers[startIndex+1:endIndex])
        print("Trial %d: Slave %d - startIndex %d endIndex %d; part_sum %ld"
            %(j, myid, startIndex+1, endIndex, part_sum))
 
    sum = world.reduce(part_sum, op=MPI.SUM, root=0)

    if myid == 0:
        endwtime = MPI.Wtime()
        runTime = endwtime - startwtime
        print('Trial %d : Execution time (sec) = %f, sum = %d' %(j, runTime, sum))
        totalTime += runTime

world.barrier()

if myid == 0:
    print('Average time for %d trials = %s' %(TRIALS, totalTime/TRIALS))