#include "../include/forkedServer.h"

int forkedServer(int *clientSocket)
{
    char serMsg[255] = "Welcome to mathserver API\n"
                       "Options:\n"
                       "    matinv -n [size of matrix] -P [parallel, 1-] -I [rand || fast]\n"
                       "-------------------------\n";
    char recvData[10][100];
    send(*clientSocket, serMsg, sizeof(serMsg), 0);
    recv(*clientSocket, recvData, sizeof(recvData), 0);
    for (int i = 0;i < 10;i++)
    {
        printf("%s\n", recvData[i]);
    }
    /*char *fileName = initCalculation(recvDa);
    char *resBuf[1024];

    FILE *resFile = fopen(fileName, "r");
    if (resFile == NULL)
    {
        return 1;
    }
    fseek(resFile, 0L, SEEK_END);
    int size = ftell(resFile);
    fseek(resFile, 0L, SEEK_SET);

    fread(resBuf, sizeof(char), size, resFile);
    fclose(resFile);
    send(*clientSocket, resBuf, sizeof(resBuf), 0);

    close(*clientSocket);
    */

    return 1;
}