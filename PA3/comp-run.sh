#!/bin/bash

#To run this script, recommended use: "sbatch -N <numberofnodes> -n <number_of_processes> <path>/sub.sh"

# setting the max wallclock time after which the job will be killed; 
# note: the format is hr:min:sec (default set here to 10 mins)
#SBATCH --time=00:20:00

# SBATCH --time=00:1:00
#MPI helloworld example - this line is a comment
#this is the command to execute your parallel program

if [ "$1" != "" ]; then
    var=$1
    var2=${var%.*}
    var3=${var2##*/}
    echo "mpicc -o $var3 $1"

    if ! mpicc -o $var3 $1; then
        exit 1
    fi

    echo "mpirun $var3"

    # for i in $(seq 1 10);
    # do
    mpirun $var3 ${@:2}
        # mpirun ${@:2}
        # echo $i
    # done
else
    echo "Enter the absolute path to your file"
fi
# echo $1
# mpirun 

# you can add more lines of commands here, if you wish.