#include "signalHandler.h"

extern pid_t * childPIDs;
extern int w;
extern int * in;
extern int * responses;

void sigChild(int signum){
	for(int i=0; i<w; i++){
		if(kill(childPIDs[i],0) != 0){
			printf("#%d child terminated.\n",i);
			reCreateReceiver(i);
		}
	}
}

//Check every pipe
void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};
	for(int i=0; i<w; i++){
		if(read(in[i], msgbuf, MSGSIZE+1) > 0){
			printf("Message from child #%d: -%s-\n",i,msgbuf);
			if(strcmp(msgbuf,"yeah") == 0) responses[i] = 1;
		}
	}
}

void setupSigActions(){
	struct sigaction sigchld;
	sigchld.sa_handler = sigChild;
	sigemptyset (&sigchld.sa_mask);
	sigchld.sa_flags = 0;
	sigaction(SIGCHLD,&sigchld,NULL);
	struct sigaction sigusr1;
	sigusr1.sa_handler = sigCheckPipe;
	sigemptyset(&sigusr1.sa_mask);
	sigusr1.sa_flags = 0;
	sigaction(SIGUSR1,&sigusr1,NULL);
}
