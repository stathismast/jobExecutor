#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MSGSIZE 512

void openPipes();
void readFromPipe(char * msgbuf);
void writeToPipe(char * msgbuf);
