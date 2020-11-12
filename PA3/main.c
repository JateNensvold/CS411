#include "reduce.h"

int main(int argc, char *argv[])
{
    // get cmd line args
    if (argc != 3)
    {
        printf("Error: argc = %d. Invalid args. Please enter arguments for n\n", argc);
        exit(1);
    }

    n = atoi(argv[1]);

    // set MPI variables
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    double tbeg = MPI_Wtime();

    int local_size = n / p;

    // generate array of size n/p
    int *arr = GenerateArray(local_size);

    // compute local serial sum of array
    local_sum = SumArray(arr, local_size);

    if(strcmp(argv[2], "myAllReduce") == 0)
    {
        MyAllReduce();
    }
    else if(strcmp(argv[2], "MPILibraryAllReduce") == 0)
    {
        MPILibraryAllReduce();
    }
    else if(strcmp(argv[2], "NaiveAllReduce") == 0)
    {
        NaiveAllReduce();
    }
    else
    {
        printf("Error: unexpected type\n");
        exit(1);
    }

    // calculate total runtime across processes
    MPI_Barrier(MPI_COMM_WORLD);
    double pTime = MPI_Wtime() - tbeg;
    double runTime;

    MPI_Reduce(&pTime, &runTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        // convert to microseconds
        runTime = runTime * 1000000;
        printf("size,%d\n", n);
        printf("processes,%d\n", p);
        printf("runtime,%f\n", runTime);
    }

    MPI_Finalize();
}