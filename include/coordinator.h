#pragma once

typedef struct splitterChild
{
    pid_t id;
    int numOfSorters;
    int startReading;
    int endReading;
    //fifo path with the coordinator
    char fifo_path[64];
    int fifo_fd;
}SplitterChild;



SplitterChild* makeSplitters(int k, int numOfVoters, const char* filename, const char* sorting1, const char* sorting2, pid_t rootPID);


typedef struct timeSorterData
{
    char buffer[64];
    char RtimeTaken[32]; //for real time
    char CtimeTaken[32]; //for CPU time
}TimeSorterData;