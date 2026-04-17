#pragma once
typedef struct voter
{
    int id;
    char lastName[20];
    char firstName[20];
    char postalCode[6];
}Voter;

typedef struct voterArray
{
    int size;
    Voter* voters;
}VoterArray;

typedef struct position
{
    int startIdx;
    int stopIdx;
}Position;




void PrintVoter(const Voter * const voter);
void PrintVoters(VoterArray* voters);
VoterArray LoadVoters(const char* filename, int startVotersIdx, int endVotersIdx);
void quick_sort(VoterArray* arrayOfVoters, int low, int high);
void bubbleSort(VoterArray* arrayOfVoters);