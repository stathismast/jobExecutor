#include "signalHandler.h"

extern struct workerInfo * workers;
extern int * in;
extern int w;
extern int responses;
extern int searching;
extern int deadline;

void sigChild(int signum){
	for(int i=0; i<w; i++){
		if(kill(workers[i].pid,0) != 0){
			printf("\n#%d child terminated.\n",i);
			reCreateReceiver(i);
		}
	}
}

//Check every pipe
void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};
	if(searching){
		nonBlockingInputPipes();
		for(int i=0; i<w; i++){
			if(read(in[i], msgbuf, MSGSIZE+1) > 0){
				printf("Message from child #%d: -%s-\n",i,msgbuf);
				if(time(NULL)<=deadline) writeToPipe(i,"yes");
				else writeToPipe(i,"no");
				responses++;
				if(responses == w) searching = 0;
			}
		}
		blockingInputPipes();
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
