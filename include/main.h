#define _POSIX_C_SOURCE 200809L //for sigaction()
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/times.h> /* times () */
#include <errno.h>


#include "voter.h"
#include "utlities.h"
#include "coordinator.h"
#include "splitter.h"