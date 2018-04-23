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

int fdA;
int fdB;
char * id;

int ready;

void sigCheckPipe(int signum){
	signal(SIGUSR1,sigCheckPipe);

	char msgbuf[MSGSIZE+1];
	if ( read(fdA, msgbuf, MSGSIZE+1) < 0) {
		perror("receiver: problem in reading"); exit(5);
		}
	// printf("%d. Message Received: -%s-\n", atoi(id), msgbuf);

	if(strcmp(msgbuf,"/exit") == 0) exit(0);

	if(write(fdB, msgbuf, MSGSIZE+1) == -1){
		perror("receiver: error in writing"); exit(2);
	}
}

void sigReady(int signum){
	signal(SIGUSR2,sigReady);
	ready = 1;
}

int main(int argc, char *argv[]){
	signal(SIGUSR1,sigCheckPipe);
	signal(SIGUSR2,sigReady);
	ready = 0;

	id = malloc(strlen(argv[3])+1);
	strcpy(id,argv[3]);

	char * fifoA;
	char * fifoB;

	fifoA = malloc(strlen(argv[1])+1);
	strcpy(fifoA,argv[1]);
	fifoB = malloc(strlen(argv[2])+1);
	strcpy(fifoB,argv[2]);

	if ( (fdA=open(fifoA, O_RDONLY)) < 0){
		perror("receiver: fifoA open problem"); exit(3);
	}
	if ( (fdB=open(fifoB, O_WRONLY)) < 0){
		perror("receiver: fifoB open problem"); exit(3);
	}

	//Pause until we get a signal from parent that we are ready to go
	//As if we are promted to run a /search command for example
	while(!ready){
		pause();
	}

	//Send out response to the parent
	char msgbuf[MSGSIZE+1];
	strcpy(msgbuf,"yeah");
	if(write(fdB, msgbuf, MSGSIZE+1) == -1){
		perror("receiver: error in writing"); exit(2);
	}
	kill(getppid(),SIGUSR1);	//Inform the parent that we responded
	ready = 0;


	for (;;){
		pause();
	}
}
