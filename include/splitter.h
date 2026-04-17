typedef struct sorterChild
{
    pid_t id;
    char fifo_path[64];
    int startVotersIdx;
    int endVotersIdx;
    char sortingPath[64];
    int fifo_fd;
    char RtimeTaken[32]; //for real time
    char CtimeTaken[32]; //for CPU time
}SorterChild;

SorterChild* makeSorters(int splitterID, int k, const char* filename, int totalSize, int startIDX, const char* sorting1, const char* sorting2, pid_t rootPID);