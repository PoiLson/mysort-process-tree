#include "../include/main.h"


void openPipesForRead(SorterChild* sorters, int size)
{
    for (int idx = 0; idx < size; idx++)
    {
        int fifo_fd = open(sorters[idx].fifo_path, O_RDONLY);
        if(fifo_fd == -1)
        {
            printf("An error occured with open(%s) -> %s\n", sorters[idx].fifo_path, strerror(errno));
            exit(0);
        }
        sorters[idx].fifo_fd = fifo_fd;
    }
}



VoterArray ReadVotersFromSorters(SorterChild* sorters, int numOfSorters, int numOfVoters, int splitterID)
{
    //MAKE AN ARRAY TO LOAD THE VOTERS FROM THE PIPES
    VoterArray arrayOfVoters;
    arrayOfVoters.voters = calloc(sizeof(Voter),  numOfVoters);
    arrayOfVoters.size = numOfVoters;

    //it is time to read VOTERS from the sorters through the use of fifo paths!
    int count = 0;
    for (int idx = 0; idx < numOfSorters; idx++)
    {
        for (int j = 0; j < sorters[idx].endVotersIdx; j++)
        {
            read(sorters[idx].fifo_fd, &arrayOfVoters.voters[count++], sizeof(Voter));
            //printf("splitter<%d>sorter<%d>: %d, %s\n",splitterID,idx,arrayOfVoters.voters[count-1].id, arrayOfVoters.voters[count-1].lastName);
        }

        //get also the timing of each sorter
        read(sorters[idx].fifo_fd, sorters[idx].RtimeTaken, sizeof(sorters[idx].RtimeTaken));
        read(sorters[idx].fifo_fd, sorters[idx].CtimeTaken, sizeof(sorters[idx].CtimeTaken));
        //printf("splitter<%d>sorter<%d>: %s, %s\n",splitterID,idx,sorters[idx].RtimeTaken, sorters[idx].CtimeTaken);
    }

    return arrayOfVoters;
}

void waitForSorters(int splitterID, int k)
{
    int status = 0;
    int n = k;
    while (n > 0) {
        long pid = wait(&status);
        if (pid > 0)
        {
            //printf("splitter<%d>:: Child sorter with PID %ld exited with status 0x%x.\n",splitterID, (long)pid, status);
            --n;
        }
    }
}






int main(int argc, char* argv[])
{
    const char* filename = argv[1];
    int splitterID = atoi(argv[2]);
    int numOfSorters = atoi(argv[3]);
    int startReading = atoi(argv[4]);
    int stopReading = atoi(argv[5]);
    const char* fifo_path = argv[6];
    const char* sorting1 = argv[7];
    const char* sorting2 = argv[8];
    pid_t rootPID = atoi(argv[9]);

    
    //OPEN OUR FIFO FOR WRITE
    int fifo_fd = openPipeForWrite(fifo_path);

    //printf("splitter<%d>: filename=%s, numOfSorters=%d, startReading=%d,stopReading= %d, %s\n",splitterID, filename,numOfSorters, startReading, stopReading,fifo_path);


    //make our childs (sorters)
    SorterChild* sorters = makeSorters(splitterID, numOfSorters, filename, stopReading, startReading, sorting1, sorting2, rootPID);


    //it is time to open the files from the other side, not read them yet!
    openPipesForRead(sorters, numOfSorters);
    

    //MAKE AN ARRAY TO LOAD THE VOTERS FROM THE PIPES
    VoterArray arrayOfVoters = ReadVotersFromSorters(sorters, numOfSorters, stopReading, splitterID);


    //printf("splitter<%d>: waitForSorters\n",splitterID);
    //wait for children to end!
    waitForSorters(splitterID, numOfSorters);


    //CLOSE PIPES
    for (int idx = 0; idx < numOfSorters; idx++)
    {
        close(sorters[idx].fifo_fd);
        unlink(sorters[idx].fifo_path);
    }


    //SORT THE VOTERS USING QUICKSORT
    quick_sort(&arrayOfVoters, 0, arrayOfVoters.size - 1);

    //SENT ALL SORTED VOTERS
    for (int idx = 0; idx < stopReading; idx++)
    {
        write(fifo_fd, &arrayOfVoters.voters[idx], sizeof(Voter));
        //PrintVoter(&arrayOfVoters.voters[idx]);
    }
    //now sent to the root the data about the sorters and their timing
    for (int idx = 0; idx < numOfSorters; idx++)
    {
        write(fifo_fd, &sorters[idx].fifo_path[0], sizeof(sorters[idx].fifo_path));
        write(fifo_fd, &sorters[idx].RtimeTaken[0], sizeof(sorters[idx].RtimeTaken));
        write(fifo_fd, &sorters[idx].CtimeTaken[0], sizeof(sorters[idx].CtimeTaken));
    }
    

    
    close(fifo_fd);

    //de-allocate data from heap
    //SorterChild* sorters = calloc(sizeof(SorterChild), k);
    free(sorters);
    sorters = NULL;

    kill(rootPID, SIGUSR1);
    return 0;
}




SorterChild* makeSorters(int splitterID, int k, const char* filename, int totalSize, int startIDX, const char* sorting1, const char* sorting2, pid_t rootPID)
{
        SorterChild* array = calloc(sizeof(SorterChild), k);

        //create k children
        //before make them, find the piece each one of them has

        int modulus = totalSize % k;
        int quantum = totalSize / k;
        int firstChild = modulus + quantum;

        //make an array with the quantums its child should take
        Position* positions = calloc(sizeof(Position), k);

        int positionCounter = 0;
        for (int idx = 0; idx < k; idx++)
        {
            //we are at the first child which takes the bigger segment
            if (idx == 0)
            {
                positions[idx].startIdx = startIDX;
                positions[idx].stopIdx = firstChild;
                positionCounter += firstChild;
            }
            else
            {
                positions[idx].startIdx = positionCounter + startIDX;
                positions[idx].stopIdx = quantum;
                positionCounter += quantum;
            }

        }

        for(int idx = 0; idx < k; idx++)
        {

            array[idx].startVotersIdx = positions[idx].startIdx;
            array[idx].endVotersIdx = positions[idx].stopIdx;
            //printf("makeSorters<splitterID=%d>, idx=%d, startVotersIdx=%d, endVotersIdx=%d\n", splitterID, idx, array[idx].startVotersIdx, array[idx].endVotersIdx);
            sprintf(array[idx].fifo_path, "Splitter%d-Sorter%d_path", splitterID, idx);

            if (idx % 2 == 0) //we are in an even id sorter, bubblesort
                strcpy(array[idx].sortingPath, sorting1);
            else
                strcpy(array[idx].sortingPath, sorting2);


            //CREATE THE PIPE BETWEEN THE ROOT AND THE SPLITTER!
            if (mkfifo(array[idx].fifo_path, 0666) == -1)
            {
                printf("An error occured with mkfifo(%s) -> %s\n", array[idx].fifo_path, strerror(errno));
                exit(EXIT_FAILURE);
            }

            //CREATE A SORTER
            array[idx].id = fork();
            if (array[idx].id == 0)
            {
                // We are the child.
                char str_id[12] = { 0 };
                char startPoint[12] = { 0 };
                char endPoint[12] = { 0 };
                char str_rootPID[12] = { 0 };

                sprintf(str_id, "%d", idx);
                sprintf(startPoint, "%d", array[idx].startVotersIdx);
                sprintf(endPoint, "%d", array[idx].endVotersIdx);
                sprintf(str_rootPID, "%d", rootPID);


                //exec immediately to sorting algorithms!
                execl(array[idx].sortingPath, array[idx].sortingPath, filename, str_id, array[idx].fifo_path, startPoint, endPoint, str_rootPID, (char*)NULL);
                perror("execl"); // Print an error message if execl fails.
                exit(-1); // Terminate the child process if execl fails.
            }
            if (array[idx].id < 0)
            {
                // Forking failed.
                perror("fork()");
                exit(EXIT_FAILURE);
            }
        }



    //de-allocate data from heap
    //Position* positions = calloc(sizeof(Position), k);
    free(positions);
    positions = NULL;

    return array;
}
