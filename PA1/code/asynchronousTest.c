/*	Cpt S 411, Introduction to Parallel Computing
 *	School of Electrical Engineering and Computer Science
 *	
 *	Example code
 *	Send receive test:
 *   	rank 1 sends to rank 0 (all other ranks sit idle)
 *   	For timing use of C gettimeofday() is recommended.
 * */

#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>

#define SIZE 2097152
int main(int argc, char *argv[])
{
	int rank, p;
	struct timeval t1, t2;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Request request, request2;
    MPI_Status status;

	assert(p >= 2);

	if (rank == 1)
	{
		
		// char x[SIZE];
		char *x;
		x = malloc (SIZE * sizeof(char)); // Array of 10 elements

		int dest = 0;
		gettimeofday(&t1, NULL);
		MPI_Isend(x, SIZE, MPI_CHAR, dest, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

		gettimeofday(&t2, NULL);
		int tSend = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;

		// printf("Rank=%d: message size %d to %d; Send time %d millisec\n", rank, SIZE, dest, tSend);
		printf("%d\n", tSend);
	}
	else if (rank == 0)
	{
        
		char *y;
		y = malloc (SIZE * sizeof(char)); // Array of 10 elements
		MPI_Status status;
		gettimeofday(&t1, NULL);
		MPI_Irecv(y, SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request2);
        MPI_Wait(&request2, &status);

		gettimeofday(&t2, NULL);
		int tRecv = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000;
		// printf("Rank=%d: recieved message size %d from %d; Recv time %d millisec\n", rank, SIZE, status.MPI_SOURCE, tRecv);
		printf("%d\n", tRecv);
	}

	MPI_Finalize();
}