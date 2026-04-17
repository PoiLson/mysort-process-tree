#include "main.h"


void PrintVoter(const Voter* const voter)
{
    char buffer[256] = { 0 };
    if(voter != NULL)
    {
        sprintf(buffer, "%-20s%-20s%-10d%-10s\n", voter->lastName, voter->firstName, voter->id, voter->postalCode);
        write(1, buffer, strlen(buffer));
    }
}

void PrintVoters(VoterArray* voters)
{
    char buffer[256] = { 0 };
    for(int i=0; i < voters->size; i++)
    {
        Voter* voter = &voters->voters[i];
        sprintf(buffer, "%-20s%-20s%-10d%-10s\n", voter->lastName, voter->firstName, voter->id, voter->postalCode);
        write(1, buffer, strlen(buffer));
    }
}
VoterArray LoadVoters(const char* filename, int startIndex, int numOfVoters)
{
    int fd = open(filename, O_RDONLY);

    if (fd < 0)
    {
        printf("File could not be opened!\n");
        exit(-1);
    }

    //printf("Num of voters is: %d\n", endVotersIdx);

    if (lseek(fd, (startIndex * sizeof(Voter)), SEEK_SET) == -1)
    {
        perror("lseek");
        close(fd);
        exit(-1);
    }

    VoterArray vArray;
    vArray.voters = calloc(sizeof(Voter), numOfVoters);
    vArray.size = numOfVoters;


    int idx = 0;
    size_t returnValue = read(fd, &vArray.voters[idx], sizeof(Voter));

    while (returnValue > 0 && idx < (numOfVoters - 1))
    {
        idx++;
        returnValue = read(fd, &vArray.voters[idx], sizeof(Voter));
    }

    close(fd);
    return vArray;
}