#!/bin/sh

#To run this script, recommended use: "sbatch -N <numberofnodes> -n <number_of_processes> <path>/sub.sh"

# setting the max wallclock time after which the job will be killed; 
# note: the format is hr:min:sec (default set here to 10 mins)
#SBATCH --time=00:20:00

# Note that my helloworld resides on this path. You need to change the path where your executable resides
mpirun myReduce 256000 MPILibraryAllReduce
#mpirun myReduce 256000 myAllReduce
#mpirun myReduce 2000 NaiveAllReduce


# you can add more lines of commands here, if you wish.