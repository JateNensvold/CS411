#include "reduce.h"

/*******************************************************
 * GenerateArray
 * 
 * Description: Allocates an array of input size. Array 
 * is populated with random integers.
 * 
 * Returns: int array
 ********************************************************/
int *GenerateArray(int size)
{
    // allocate array
    int *arr = (int *)malloc(sizeof(int) * size);

    // populate with random numbers
    srand(time(NULL) + rank);
    for (int i = 0; i < size; i++)
    {
        arr[i] = GetRandomNum(100, 1);
    }

    return arr;
}

/*******************************************************
 * GetRandomNum
 * 
 * Description: Generates a random number between the
 * upper and lower bounds.
 * 
 * Returns: int
 ********************************************************/
int GetRandomNum(int upper, int lower)
{
    return (rand() % (upper - lower + 1)) + lower;
}

/*******************************************************
 * PrintArray
 * 
 * Description: Prints an array
 * 
 * Returns: nothing
 ********************************************************/
void PrintArray(int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", arr[i]);
    }
}

/*******************************************************
 * SumArray
 * 
 * Description: Computes the local sum of the array
 * 
 * Returns: int
 ********************************************************/
int SumArray(int *arr, int size)
{
    int sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
    }

    return sum;
}

/*******************************************************
 * MyAllReduce
 * 
 * Description: Simulates the MPI_AllReduce function
 * 
 * Returns: nothing
 ********************************************************/
void MyAllReduce()
{
    struct timeval t1, t2;
    int tSendrecv = 0;

    // send/recv local sums
    int k = 1;
    double result = round(log(p - 1));

    while (k < p)
    {
        int partner_sum = 0;
    
        // determine partner rank
        int partner = rank ^ k;
        k = 2 * k;

        // send local_sum to partner rank
        gettimeofday(&t1, NULL);
        MPI_Sendrecv(&local_sum, 1, MPI_INT, partner, 0, &partner_sum, 1, MPI_INT, partner, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        gettimeofday(&t2, NULL);

        // update localsum
        local_sum += partner_sum;
        // update communication time
        tSendrecv += (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
    }

    printf("comm,%d\n", tSendrecv);
    //printf("Rank %d computes local sum %d\n", rank, local_sum);
}

/*******************************************************
 * NaiveAllReduce
 * 
 * Description: A naive implemenation of all reduce. Computes local sum before
 * sending the sum to the next process and recieve the sum of the previous process.
 * 
 * Returns: nothing
 ********************************************************/
void NaiveAllReduce()
{
    int previous_sum = 0;
    int dest = (rank + 1) % p;
    int source = rank - 1;
    int global_sum = 0;

    if (rank == 0)
    {
        // send local sum to next rank
        MPI_Send(&local_sum, 1 , MPI_INT, dest, 1, MPI_COMM_WORLD);
    }
    else if (rank < (p-1)){
        // recieve Partial sum from previous rank
        MPI_Recv(&previous_sum, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_sum += previous_sum;
        // send partial sum + local sum to next rank
        MPI_Send(&local_sum, 1 , MPI_INT, dest, 1, MPI_COMM_WORLD);
    }
    else
    {
        // Recieve partial sum from previous rank
        MPI_Recv(&previous_sum, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_sum += previous_sum;
        // Send global sum to previous rank
        MPI_Send(&local_sum, 1 , MPI_INT, source, 1, MPI_COMM_WORLD);
        global_sum = local_sum;

    }

    if (rank == 0)
    {
        // Recieve global sum from next rank
        MPI_Recv(&global_sum, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else if (rank < (p -1))
    {
        // Recieve global sum from next rank
        MPI_Recv(&global_sum, 1, MPI_INT, dest, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // send global sum to previous rank
        MPI_Send(&global_sum, 1, MPI_INT, source, 1, MPI_COMM_WORLD);
    }

    printf("Rank %d naiveReduce sum %d\n", rank, global_sum);
}

/*******************************************************
 * MPILibraryAllReduce
 * 
 * Description: Calls the MPI_AllReduce function on local sums
 * 
 * Returns: nothing
 ********************************************************/
void MPILibraryAllReduce()
{
    struct timeval t1, t2;
    int global_sum = 0;

    // gather all local_sums into global_sum and send to all processes
    gettimeofday(&t1, NULL);
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    gettimeofday(&t2, NULL);

    // calculate communication time
    int tSendrecv = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
    //printf("Rank %d MPIReduce sum %d\n", rank, global_sum);
    printf("communication,%d\n", tSendrecv);
}