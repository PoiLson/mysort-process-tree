#include "main.h"

int main(int argc, char* argv[])
{
    struct tms tb1, tb2;
    double ticspersec;
    int i, sum = 0;
    //START TIMER
    ticspersec = (double)sysconf(_SC_CLK_TCK);
    double t1 = (double)times(&tb1);

    //main program sorter segment
    const char* filename = argv[1];
    int sorterID = atoi(argv[2]);
    const char* fifo_path = argv[3];
    int startVotersIdx = atoi(argv[4]);
    int endVotersIdx = atoi(argv[5]);
    pid_t rootPID = atoi(argv[6]);


    //OPEN OUR FIFO FOR WRITE
    int fifo_fd = openPipeForWrite(fifo_path);

    //printf("sorter1<%d>: filename=%s,  startReading=%d, stopReading=%d, %s\n",sorterID, filename, startVotersIdx, endVotersIdx, fifo_path);


    
    //READ OUR CHUNK OF VOTERS:
    VoterArray arrayOfVoters = LoadVoters(filename, startVotersIdx, endVotersIdx);
    
    //DO THE SORTING
    bubbleSort(&arrayOfVoters);

    
    //SENT ALL SORTED VOTERS
    for (int idx = 0; idx < endVotersIdx; idx++)
    {
        write(fifo_fd, &arrayOfVoters.voters[idx], sizeof(Voter));
    }

    kill(rootPID, SIGUSR2);


    //STOP TIMER
    double t2 = (double)times(&tb2);
    double cpu_time = (double)((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));

     //THEN SEND THE REAL TIME OF THE PROGRAM
    char Rbuffer[32] = { 0 };
    sprintf(Rbuffer, "%.12lf", (t2 - t1) / ticspersec);
    //printf("sorter2<%d>: R->%s\n", sorterID, Rbuffer);
    write(fifo_fd, Rbuffer, sizeof(Rbuffer));

    //AND FINALLY SEND THE CPU TIME OF THE PROGRAM
    char Cbuffer[32] = { 0 };
    sprintf(Cbuffer, "%.12lf", cpu_time / ticspersec);
    //printf("sorter2<%d>: C->%s\n", sorterID, Cbuffer);
    write(fifo_fd, Cbuffer, sizeof(Cbuffer));


    close(fifo_fd);
    return 0;
}