#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

bool **GenerateInitialGoL(int rows, int columns);
bool determineState(bool *before, bool *row, bool *after, int x, int y, int columns);
unsigned long simulate(bool **matrix, int generations, int columns, int rows, int rank, int p);
void display(int totalRows, int totalColumns, int numberRows,
             int generation, bool **localMatrix);

int main(int argc, char *argv[])
{
    int rank, p;
    struct timeval t1, t2;

    srand(time(NULL));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int generations, size;
    int *seed;
    int *randomSeed;
    size = atoi(argv[1]);
    generations = atoi(argv[2]);
    // number of rows each process will manage
    int n = size / p;
    // Location to store seed on each process
    seed = malloc(sizeof(int));
    if (rank == 0)
    {
        printf("size: %d p: %d n: %d\n", size, p, n);
        printf("Master rank: %d\n", rank);
        randomSeed = malloc(p * sizeof(int));
        for (int i = 0; i < p; i++)
        {
            randomSeed[i] = rand();
        }

        MPI_Scatter(randomSeed, 1, MPI_INT, seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // printf("Scattered\n");
        // for (int i = 0; i < generations; i++)
        // {
        //     MPI_Barrier(MPI_COMM_WORLD);
        // }
    }
    bool **matrix;
    MPI_Scatter(randomSeed, 1, MPI_INT, seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
    srand(seed[0]);
    matrix = GenerateInitialGoL(n, size);
    unsigned long sendTime = 0, simTime = 0;
    for (int i = 0; i < generations; i++)
    {
        // if (i==99)
        // {
        //     if (rank != 0)
        //     {
        //         for (int j = 0; j < n; j++)
        //         {
        //             // printf("rank:%d send\n", rank);
        //             MPI_Send(matrix[j], size, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
        //         }
        //     }
        //     else
        //     {
        //         display(size, size, n, generations, matrix);
        //     }
        // }
        struct timeval t1, t2;
        gettimeofday(&t1, NULL);
        sendTime += simulate(matrix, generations, size, n, rank, p);
        gettimeofday(&t2, NULL);
        simTime += (1000000 * (t2.tv_sec - t1.tv_sec)) + (t2.tv_usec - t1.tv_usec);
        // printf("generation:%d\n", i);
        // if (rank != 0)
        // {
        //     for (int j = 0; j < n; j++)
        //     {
        //         // printf("rank:%d send\n", rank);
        //     }
        // }
        // else
        // {
        //     display(size, size, n, generations, matrix);
        // }
    }
    if (rank == 0)
    {
        for (int i = 1; i < p; i++)
        {
            unsigned long times[2] = {0, 0};
            MPI_Status status;
            MPI_Recv(times, 2, MPI_UNSIGNED_LONG, MPI_ANY_TAG, 0, MPI_COMM_WORLD, &status);
            sendTime += times[0];
            simTime += times[1];
            printf("sum:%d - size:%d p:%d, send-time:%lu, comp-time:%lu from:%d\n",
                   i, size, p, sendTime, simTime, status.MPI_SOURCE);
        }
    }
    else
    {
        unsigned long times[2] = {sendTime, simTime};
        MPI_Send(times, 2, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}

bool **GenerateInitialGoL(int rows, int columns)
{
    bool **matrix = malloc(rows * sizeof(bool *));

    for (int i = 0; i < rows; i++)
    {
        matrix[i] = malloc(columns * sizeof(bool));
        for (int j = 0; j < columns; j++)
        {
            matrix[i][j] = ((rand() % 2)) == 0 ? true : false;
        }
    }

    return matrix;
}

unsigned long simulate(bool **matrix, int generations, int columns, int rows, int rank, int p)
{
    bool *before, *after;
    MPI_Status status1, status2;
    bool *previous, *next;
    before = malloc(columns * sizeof(bool));
    after = malloc(columns * sizeof(bool));
    struct timeval t1, t2;
    int dest = (rank + 1) % p;
    gettimeofday(&t1, NULL);
    MPI_Barrier(MPI_COMM_WORLD);
    // Send last row and receive previous row
    MPI_Sendrecv(matrix[rows - 1], columns, MPI_C_BOOL, dest,
                 100, before, columns, MPI_C_BOOL, MPI_ANY_SOURCE,
                 MPI_ANY_TAG, MPI_COMM_WORLD, &status1);
    // Send firt row and recieve next row
    MPI_Sendrecv(matrix[0], columns, MPI_C_BOOL, dest,
                 101, after, columns, MPI_C_BOOL, MPI_ANY_SOURCE,
                 MPI_ANY_TAG, MPI_COMM_WORLD, &status2);
    gettimeofday(&t2, NULL);

    bool **newGeneration = malloc(rows * sizeof(bool *));
    for (int i = 0; i < rows; i++)
    {
        // printf("secondmalloc:%d\n", rank);
        newGeneration[i] = malloc(columns * sizeof(bool));
        for (int j = 0; j < columns; j++)
        {
            if (i - 1 < 0)
            {
                // printf("uses sent:%d\n", rank);
                previous = before;
            }
            else
            {
                previous = matrix[i - 1];
            }
            if (i + 1 > (rows - 1))
            {
                // printf("uses sent:%d\n", rank);
                next = after;
            }
            else
            {
                next = matrix[i + 1];
            }
            newGeneration[i][j] = determineState(previous, matrix[i], next, j, i, columns);
            // printf("y:%d x:%d rank:%d\n", i, j, rank);
        }
    }
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
        matrix[i] = newGeneration[i];
    }
    unsigned long time_in_micros = (1000000 * (t2.tv_sec - t1.tv_sec)) + (t2.tv_usec - t1.tv_usec);
    return time_in_micros;
}

bool determineState(bool *before, bool *row, bool *after, int x, int y, int columns)
{
    int neighbors = 0;
    int beforeX = x - 1;
    if (beforeX < 0)
    {
        beforeX = columns - 1;
    }
    // printf("y:%d x:%d x-1:%d x+1:%d\n", y, x, beforeX, (x + 1) % columns);
    neighbors += (int)before[beforeX];
    neighbors += (int)row[beforeX];
    neighbors += (int)after[beforeX];

    neighbors += (int)before[(x + 1) % columns];
    neighbors += (int)row[(x + 1) % columns];
    neighbors += (int)after[(x + 1) % columns];

    neighbors += (int)before[x];
    neighbors += (int)after[x];

    // printf("neighbors %d\n", neighbors);

    if (3 <= neighbors && neighbors <= 5)
    {
        // printf("true\n");
        return true;
    }
    else
    {
        // printf("false\n");
        return false;
    }
}

void display(int totalRows, int totalColumns, int numberRows,
             int generation, bool **localMatrix)
{

    bool **masterMatrix = malloc(totalRows * sizeof(bool *));
    MPI_Status status;
    printf("Generation %d\n", generation);
    for (int i = 0; i < totalRows; i++)
    {
        int source = i / numberRows;
        if (i < numberRows)
        {
            masterMatrix[i] = localMatrix[i];
        }
        else
        {
            masterMatrix[i] = malloc(totalColumns * sizeof(bool *));
            MPI_Recv(masterMatrix[i], totalColumns, MPI_C_BOOL,
                     source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
        // printf("source %d\n", source);
        for (int j = 0; j < totalColumns; j++)
        {
            printf("%d ", masterMatrix[i][j] ? 1 : 0);
        }
        printf("\n");
    }
}