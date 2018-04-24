#include "ioManager.h"
#include <time.h>

pid_t * childPIDs;
int numberOfWorkers;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes

int * responses;

void sigChild(int signum){
	for(int i=0; i<numberOfWorkers; i++){
		if(kill(childPIDs[i],0) != 0){
			printf("#%d child terminated.\n",i);
			reCreateReceiver(i);
		}
	}
}

//Check every pipe
void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};
	for(int i=0; i<numberOfWorkers; i++){
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

int main(int argc, char *argv[]){

	setupSigActions();

	char * docfile;
	int w;

	if(manageArguments(argc,argv,&docfile,&w) < 0) exit(3);

	out = malloc(w*sizeof(int));		//Output named pipe file descriptors
	in = malloc(w*sizeof(int));			//Input named pipe file descriptors

	responses = malloc(w*sizeof(int));
	for(int i=0; i<w; i++) responses[i] = 0;

	char msgbuf[MSGSIZE+1] = {0};

	childPIDs = malloc(w*sizeof(pid_t));
	numberOfWorkers = w;

	getPipeNames(w,&outPipes,&inPipes);

	for(int i=0; i<w; i++){
		createNamedPipe(outPipes[i]);
		createNamedPipe(inPipes[i]);
	}

	for(int i=0; i<w; i++)
		createReceiver(i);

	for(int i=0; i<w; i++){
		out[i]=openForWriting(outPipes[i]);
		in[i]=openForReading(inPipes[i]);
		usleep(10000);
		writeToChild(i,out[i],"/test");
		readFromPipe(in[i],msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",i);
			exit(2);
		}
	}
	for(int i=0; i<w; i++){
		//Reopen the pipe with non blocking argument
		close(in[i]);
		in[i] = open(inPipes[i], O_RDONLY | O_NONBLOCK);
	}
	printf("All workers up and running.\n");

	//Inform children that they are ready to go
	//Equivilent of promting them to execute a command such as /search
	for(int i=0; i<w; i++){
		writeToChild(i,out[i],"yeah");
	}

	//Whild there is a child that hasnt sent a response
	int sum = 0;
	while(sum != w){
		sum = 0;
		for(int i=0; i<w; i++){
			sum += responses[i];
		}
	}

	signal(SIGCHLD,SIG_DFL);
	for(int i=0; i<w; i++)
		kill(childPIDs[i],SIGUSR2);
	for(int i=0; i<w; i++)
		unlink(outPipes[i]);
	for(int i=0; i<w; i++)
		unlink(inPipes[i]);
	freePipeNames(w,outPipes,inPipes);
	free(in);
	free(out);
	free(childPIDs);
	free(responses);
	free(docfile);
	exit(0);

}
