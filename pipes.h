#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

#define MSGSIZE 512

int openForReading(char * name);
int openForWriting(char * name);
void writeToChild(int id, int fd, char * message);
void readFromPipe(int fd, char * message);
void createNamedPipe(char * pipeName);
void createReceiver(int id);
void reCreateReceiver(int id);
void getName(int id, char ** outPipes, char ** inPipes);
void getPipeNames(int pipeCount, char *** outPipes, char *** inPipes);
void freePipeNames(int pipeCount, char ** outPipes, char ** inPipes);
