#include "main.h"

//function for sorting voters by their lastname or firstname or ID's
int compareVoters(const void* a, const void* b) {
    int lastNameComparison = strcmp(((Voter*)a)->lastName, ((Voter*)b)->lastName);
    if (lastNameComparison != 0) {
        return lastNameComparison;
    }

    int firstNameComparison = strcmp(((Voter*)a)->firstName, ((Voter*)b)->firstName);
    if (firstNameComparison != 0) {
        return firstNameComparison;
    }

    return ((Voter*)a)->id - ((Voter*)b)->id;
}

// Function to partition the array
int partition(VoterArray* arrayOfVoters, int low, int high) {
    Voter pivot = arrayOfVoters->voters[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (compareVoters(&arrayOfVoters->voters[j], &pivot) <= 0) {
            i++;
            Voter temp = arrayOfVoters->voters[i];
            arrayOfVoters->voters[i] = arrayOfVoters->voters[j];
            arrayOfVoters->voters[j] = temp;
        }
    }

    Voter temp = arrayOfVoters->voters[i + 1];
    arrayOfVoters->voters[i + 1] = arrayOfVoters->voters[high];
    arrayOfVoters->voters[high] = temp;

    return i + 1;
}

// Function to perform quicksort
void quick_sort(VoterArray* arrayOfVoters, int low, int high)
{
    if (low < high)
    {
        int pi = partition(arrayOfVoters, low, high);

        quick_sort(arrayOfVoters, low, pi - 1);
        quick_sort(arrayOfVoters, pi + 1, high);
    }
}