#include "../include/main.h"



static volatile sig_atomic_t usr1_signals = 0;
static volatile sig_atomic_t usr2_signals = 0;

void mysigHandler(int sig, siginfo_t *info, void *context)
{
	if (sig == SIGUSR1)
	{
        usr1_signals++;
		//puts("Received USR1 signal");
	}
	else if (sig == SIGUSR2)
	{
        usr2_signals++;
		//puts("Received USR2 signal");
	}
}

void setupSignalHandler()
{
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = mysigHandler;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
}

int getNumberOfVoters(const char* filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        printf("File could not be opened!\n");
        exit(-1);
    }

    off_t size = lseek(fd, 0, SEEK_END);
    int fileSize = size / sizeof(Voter);

    //no need to have the file opened anymore
    close(fd);

    return fileSize;
}

void openPipesForRead(SplitterChild* splitters, int size)
{
    for (int idx = 0; idx < size; idx++)
    {
        int fifo_fd = open(splitters[idx].fifo_path, O_RDONLY);
        if(fifo_fd == -1)
        {
            printf("An error occured with open(%s) -> %s\n", splitters[idx].fifo_path, strerror(errno));
            exit(0);
        }
        splitters[idx].fifo_fd = fifo_fd;
    }
}


int GetTotalNumberOfSorters(SplitterChild* splitters, int size)
{
    int total = 0;
    for (int idx = 0; idx < size; idx++)
    {
        total += splitters[idx].numOfSorters;
    }

    return total;
}

void waitForSplitters(int k)
{
    int status = 0;
    int n = k;
    while (n > 0) {
        long pid = wait(&status);
        if (pid > 0)
        {
            //printf("main:: Child splitter with PID %ld exited with status 0x%x.\n", (long)pid, status);
            --n;
        }
    }
}

/*
* 
read the explainNote.txt for more information
about the need of use of my_read

*/
int my_read(int __fd, void *__buf, size_t __nbytes)
{
    while(1==1)
    {
        int ret = read(__fd, __buf, __nbytes);
        if(ret == -1)
        {
            int _errno = errno;
            if(_errno == EINTR)
            {
               // printf("EINTR\n");
                continue;
            }
            
            
            printf("An error occured with my_read -> %s\n",  strerror(_errno));
            exit(EXIT_FAILURE);
        }

        return ret;
    }
}

int main(int argc, char* argv[])
{
    
    if (argc == 1)
    {
        printf("No flags were inserted!\n");
        return 0;
    }
    int k = -1;
    const char* filename;
    const char* sorting1;
    const char* sorting2;
    getFlags(argc, argv, &k, &filename, &sorting1, &sorting2);
    



    //must receive a signal before the sorter processes terminate!!
    setupSignalHandler();

    //get number of voters
    int numOfVoters = getNumberOfVoters(filename);

    //make our childs (splitters)
    pid_t rootPID = getpid();
    SplitterChild* splitters = makeSplitters(k, numOfVoters, filename, sorting1, sorting2, rootPID);


    //it is time to open the files from the other side, not read them yet!
    openPipesForRead(splitters, k);





    //ROOT READY TO RECEIVE FINAL DATA FOR THE FINAL SORTING
    //MAKE AN ARRAY TO LOAD THE VOTERS FROM THE PIPES
    VoterArray arrayOfFinalVoters;
    arrayOfFinalVoters.voters = calloc(sizeof(Voter), numOfVoters);
    arrayOfFinalVoters.size = numOfVoters;


    //get total number of sorters
    int total = GetTotalNumberOfSorters(splitters, k);
    TimeSorterData* totalTimingData = calloc(sizeof(TimeSorterData), total);

    int timeIdx = 0;
    int count = 0;
    for (int idx = 0; idx < k; idx++)
    {
        //READ VOTERS FROM SPLITTER:
        for (int j = 0; j < splitters[idx].endReading; j++)
        {
            my_read(splitters[idx].fifo_fd, &arrayOfFinalVoters.voters[count], sizeof(Voter));
            //printf("from splitter<%d>: %d, %s\n", idx, arrayOfFinalVoters.voters[count].id, arrayOfFinalVoters.voters[count].lastName);
            count++;
        }
        
        for (int h = 0; h < splitters[idx].numOfSorters; h++)
        {
            my_read(splitters[idx].fifo_fd, totalTimingData[timeIdx].buffer, sizeof(totalTimingData[timeIdx].buffer));
            my_read(splitters[idx].fifo_fd, totalTimingData[timeIdx].RtimeTaken, sizeof(totalTimingData[timeIdx].RtimeTaken));
            my_read(splitters[idx].fifo_fd, totalTimingData[timeIdx].CtimeTaken, sizeof(totalTimingData[timeIdx].CtimeTaken));


            //printf("from splitter<%d>: %s, %s, %s\n", idx, totalTimingData[timeIdx].buffer, totalTimingData[timeIdx].RtimeTaken, totalTimingData[timeIdx].CtimeTaken);

            timeIdx++;
        }


    }


    //printf("waitForSplitters\n");
    //wait for children to end!
    waitForSplitters(k);


     //CLOSE PIPRES
    for (int idx = 0; idx < k; idx++)
    {
        close(splitters[idx].fifo_fd);
        unlink(splitters[idx].fifo_path);
    }





    //SORT THE VOTERS USING QUICKSORT
    quick_sort(&arrayOfFinalVoters, 0, arrayOfFinalVoters.size - 1);

    //PRINT ALL SORTED VOTERS
    for (int idx = 0; idx < numOfVoters; idx++)
    {
        PrintVoter(&arrayOfFinalVoters.voters[idx]);
    }

    //PRINT TIMING DATA
    for (int idx = 0; idx < total; idx++)
    {
        printf("Run time for path %s was %s sec(REAL time), although we used the CPU for %s (CPU time).\n", totalTimingData[idx].buffer, totalTimingData[idx].RtimeTaken, totalTimingData[idx].CtimeTaken);
    }

    printf("\n\n");
    printf("Root process has received %d SIGUSR1, %d SIGUSR2\n", usr1_signals, usr2_signals);


    //now de-allocate data in the heap

    //first of all,
    //de-allocate array of splitters

    free(splitters);
    splitters = NULL;

    //VoterArray arrayOfFinalVoters;
    //arrayOfFinalVoters.voters = calloc(sizeof(Voter), numOfVoters);
    //arrayOfFinalVoters.size = numOfVoters;
    free(arrayOfFinalVoters.voters);
    arrayOfFinalVoters.voters = NULL;


    //TimeSorterData* totalTimingData = calloc(sizeof(TimeSorterData), total);
    free(totalTimingData);
    totalTimingData = NULL;


    return 0;
}



//fix start-end for splitter
SplitterChild* makeSplitters(int k, int numOfVoters, const char* filename, const char* sorting1, const char* sorting2, pid_t rootPID)
{
    SplitterChild* array = calloc(sizeof(SplitterChild),  k);

    //create k children
    //before make them, find the piece each one of them has

    int modulus = numOfVoters % k;
    int quantum = numOfVoters / k;
    
    for(int idx=0; idx<k; idx++)
    {
        array[idx].numOfSorters = k - idx;

        if(idx == 0)
        {
            array[idx].startReading = idx;
            array[idx].endReading = modulus + quantum;
        }
        else
        {
            array[idx].startReading = (idx *quantum)+modulus;
            array[idx].endReading = quantum;
        }

        sprintf(array[idx].fifo_path, "./splitter_path%d", idx);
        

        //CREATE THE PIPE BETWEEN THE ROOT AND THE SPLITTER!
        if (mkfifo(array[idx].fifo_path, 0666) == -1)
        {
            printf("An error occured with mkfifo(%s) -> %s\n", array[idx].fifo_path, strerror(errno));
            exit(EXIT_FAILURE);
        }

        //CREATE A SPLITTER
        array[idx].id = fork();
        if (array[idx].id == 0)
        {
            // We are the child.
            char str_startIdx[12]={0};
            char str_endIdx[12]={0};
            char str_id[12] = { 0 };
            char str_numOfSorters[12] = { 0 };
            char str_rootPID[12] = { 0 };

            sprintf(str_startIdx, "%d", array[idx].startReading);
            sprintf(str_endIdx, "%d", array[idx].endReading);
            sprintf(str_id, "%d", idx);
            sprintf(str_numOfSorters, "%d", array[idx].numOfSorters);
            sprintf(str_rootPID, "%d", rootPID);

            execl("splitter", "splitter", filename, str_id, str_numOfSorters, str_startIdx, str_endIdx, array[idx].fifo_path, sorting1, sorting2, str_rootPID, (char*)NULL);
            perror("execl");    // Print an error message if execl fails.
            exit(-1);           // Terminate the child process if execl fails.
        }
        if(array[idx].id < 0)
        {
            // Forking failed.
            perror("fork()");
            exit(EXIT_FAILURE);
        }
    }



    return array;
}

