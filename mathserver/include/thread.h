#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include <pthread.h>

struct threadParam 
{
    int clientSocket;
    int calculationType;
    int **data;
};

void *threadInit(void *threadparam);

int** threadHandle(int calculationType, int *data[]);

int** matrixInteversionAlgorithm(int *data[]);

int** kMeansAlgorithm(int *data[]);

void threadKamikaze();