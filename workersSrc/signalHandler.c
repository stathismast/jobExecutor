#include "signalHandler.h"

extern char * id;
extern int stage;
extern int done;

extern int dirCount;
extern char ** directories;

int dirReceived;

void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};

	readFromPipe(msgbuf);
	printf("Worker #%d: Message Received: -%s-\n", atoi(id), msgbuf);

	if(strcmp(msgbuf,"/test") == 0){
		writeToPipe(msgbuf);
		return;
	}

	if(stage == 1){
		dirCount = atoi(msgbuf);
		directories = malloc(dirCount*sizeof(char*));
		stage++;
		dirReceived = 0;
		writeToPipe(msgbuf);
	}
	else if(stage == 2){
		directories[dirReceived] = malloc(strlen(msgbuf)+1);
		strcpy(directories[dirReceived],msgbuf);
		dirReceived++;
		if(dirReceived == dirCount){
			printf("I'm worker #%d and these are my directories:\n",atoi(id));
			for(int i=0; i<dirCount; i++){
				printf("\t-%s-\n",directories[i]);
			}
			stage++;
		}
		writeToPipe(msgbuf);
	}
	else if(stage == 3){
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
