#!/bin/sh

# Shell script for launching multiple jobs

# Compile code
mpicc -o myReduce main.c reduce.c -lm

# Run sub.sh
sbatch -N 1 -n 1 sub.sh
sbatch -N 1 -n 2 sub.sh
sbatch -N 1 -n 4 sub.sh
sbatch -N 1 -n 8 sub.sh
sbatch -N 2 -n 16 sub.sh
sbatch -N 4 -n 32 sub.sh
#sbatch -N 6 -n 64 sub.sh