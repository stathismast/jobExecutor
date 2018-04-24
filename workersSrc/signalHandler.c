#include "signalHandler.h"

extern char * id;
extern int done;

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
