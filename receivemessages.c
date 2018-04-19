#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MSGSIZE 512

int main(int argc, char *argv[]){
	int fdA, fdB, i, nwrite;
	char msgbuf[MSGSIZE+1];

	char * fifoA = malloc(strlen(argv[1])+1);
	strcpy(fifoA,argv[1]);
	char * fifoB = malloc(strlen(argv[2])+1);
	strcpy(fifoB,argv[2]);

	if ( (fdA=open(fifoA, O_RDONLY)) < 0){
		perror("receiver: fifoA open problem"); exit(3);
	}
	if ( (fdB=open(fifoB, O_WRONLY)) < 0){
		perror("receiver: fifoB open problem"); exit(3);
	}
	for (;;){
		if ( read(fdA, msgbuf, MSGSIZE+1) < 0) {
			perror("receiver: problem in reading"); exit(5);
			}
		printf("Message Received: -%s-\n", msgbuf);

		if(strcmp(msgbuf,"/exit") == 0) return 0;

		strcpy(msgbuf, "I am your child");
		if(write(fdB, msgbuf, MSGSIZE+1) == -1){
			perror("receiver: error in writing"); exit(2);
		}
	}
}
