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

int in;
int out;
char * inPipe;
char * outPipe;
char * id;

int done;

void readFromPipe(char * msgbuf){
	if(read(in, msgbuf, MSGSIZE+1) < 0){
		perror("receiver: problem in reading"); exit(5);
	}
}

void writeToPipe(char * msgbuf){
	if(write(out, msgbuf, MSGSIZE+1) == -1){
		perror("receiver: error in writing"); exit(2);
	}
}

void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};

	readFromPipe(msgbuf);
	printf("%d. Message Received: -%s-\n", atoi(id), msgbuf);

	if(strcmp(msgbuf,"/test") == 0)
		writeToPipe(msgbuf);

	if(strcmp(msgbuf,"yeah") == 0){
		writeToPipe(msgbuf);
		kill(getppid(),SIGUSR1);	//Inform the parent that we responded
	}
}

void sigDone(int signum){
	done = 1;
}

void setupSigActions(){
	struct sigaction sigusr1;
	sigusr1.sa_handler = sigCheckPipe;
	sigemptyset(&sigusr1.sa_mask);
	sigusr1.sa_flags = 0;
	sigaction(SIGUSR1,&sigusr1,NULL);

	struct sigaction sigusr2;
	sigusr2.sa_handler = sigDone;
	sigemptyset (&sigusr2.sa_mask);
	sigusr2.sa_flags = 0;
	sigaction(SIGUSR2,&sigusr2,NULL);
}

void manageArguments(int argc, char *argv[]){
	inPipe = malloc(strlen(argv[1])+1);
	strcpy(inPipe,argv[1]);
	outPipe = malloc(strlen(argv[2])+1);
	strcpy(outPipe,argv[2]);
	id = malloc(strlen(argv[3])+1);
	strcpy(id,argv[3]);
}

void openPipes(){
	if((in=open(inPipe, O_RDONLY)) < 0){
		perror("receiver: inPipe open problem"); exit(3);
	}
	if((out=open(outPipe, O_WRONLY)) < 0){
		perror("receiver: outPipe open problem"); exit(3);
	}
	close(out);
	if((out=open(outPipe, O_WRONLY | O_NONBLOCK)) < 0){
		perror("receiver: outPipe open problem"); exit(3);
	}
}

int main(int argc, char *argv[]){
	setupSigActions();
	manageArguments(argc,argv);
	openPipes();

	done = 0;

	while(!done){
		pause();
	}
	
	free(inPipe);
	free(outPipe);
	free(id);
}
