#include "../include/main.h"

void bubbleSort(VoterArray* arrayOfVoters)
{
    //SORT THE VOTERS USING BUBBLESORT
    for (int pass = 1; pass < arrayOfVoters->size; pass++)
    {
        for (int idx = 0; idx < arrayOfVoters->size - 1; idx++)
        {
            if (strcmp(arrayOfVoters->voters[idx].lastName, arrayOfVoters->voters[idx + 1].lastName) > 0)
            {
                Voter tempVoter = arrayOfVoters->voters[idx];
                arrayOfVoters->voters[idx] = arrayOfVoters->voters[idx + 1];
                arrayOfVoters->voters[idx + 1] = tempVoter;
            }
            else if (strcmp(arrayOfVoters->voters[idx].lastName, arrayOfVoters->voters[idx + 1].lastName) == 0)
            {
                //if the lastname is the same, sort them due to their firstname
                if (strcmp(arrayOfVoters->voters[idx].firstName, arrayOfVoters->voters[idx + 1].firstName) > 0)
                {
                    Voter tempVoter = arrayOfVoters->voters[idx];
                    arrayOfVoters->voters[idx] = arrayOfVoters->voters[idx + 1];
                    arrayOfVoters->voters[idx + 1] = tempVoter;
                }
                else if (strcmp(arrayOfVoters->voters[idx].firstName, arrayOfVoters->voters[idx + 1].firstName) == 0)
                {
                    //and if and the firstname is the same we sort them due to their ID, which is surely unique for each one of them
                    if (arrayOfVoters->voters[idx].id > arrayOfVoters->voters[idx + 1].id)
                    {
                        Voter tempVoter = arrayOfVoters->voters[idx];
                        arrayOfVoters->voters[idx] = arrayOfVoters->voters[idx + 1];
                        arrayOfVoters->voters[idx + 1] = tempVoter;
                    }
                }
            }
        }
    }
}
