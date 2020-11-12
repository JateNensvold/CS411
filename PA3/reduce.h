#include <stdio.h>
#include <mpi.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int n, local_sum;
int rank, p;

int *GenerateArray(int size);
int GetRandomNum(int upper, int lower);
void PrintArray(int *arr, int size);
int SumArray(int *arr, int size);

// Reduce functions
void MyAllReduce();
void NaiveAllReduce();
void MPILibraryAllReduce();