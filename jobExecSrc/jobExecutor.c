#include "signalHandler.h"
#include <time.h>

pid_t * childPIDs;
int w;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes

int * responses;

void allocateSpace(){
	responses = malloc(w*sizeof(int));
	for(int i=0; i<w; i++) responses[i] = 0;

	out = malloc(w*sizeof(int));		//Output named pipe file descriptors
	in = malloc(w*sizeof(int));			//Input named pipe file descriptors

	childPIDs = malloc(w*sizeof(pid_t));
}

//This functions opens and tests both input and output pipes for every worker
int openAndTestPipes(){
	char msgbuf[MSGSIZE+1] = {0};	//Buffer to be used with named pipes
	for(int i=0; i<w; i++){
		out[i]=openForWriting(outPipes[i]);
		in[i]=openForReading(inPipes[i]);
		usleep(10000);
		writeToChild(i,out[i],"/test");
		readFromPipe(in[i],msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",i);
			return 0;
		}
	}
	for(int i=0; i<w; i++){
		//Reopen the pipe with non blocking argument
		close(in[i]);
		in[i] = open(inPipes[i], O_RDONLY | O_NONBLOCK);
	}
	printf("All workers up and running.\n");
	return 1;
}

int main(int argc, char *argv[]){
	setupSigActions();

	char * docfile;

	//Read and check arguments for validity
	if(manageArguments(argc,argv,&docfile,&w) < 0) exit(3);

	//Allocate space for pipe file descriptors, pipe names, array to keep
	//PIDs of workers etc.
	allocateSpace();

	//Create all the necessary named pipes
	getPipeNames(w,&outPipes,&inPipes);
	for(int i=0; i<w; i++){
		createNamedPipe(outPipes[i]);
		createNamedPipe(inPipes[i]);
	}

	//Create all the workers
	for(int i=0; i<w; i++)
		createReceiver(i);

	if(openAndTestPipes() == 0)
		exit(2);

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
