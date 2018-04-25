#include "signalHandler.h"
#include <time.h>

pid_t * childPIDs;
int w;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes

struct workerInfo * workers;

int * responses;

int main(int argc, char *argv[]){
	setupSigActions();


	//Read and check arguments for validity
	char * docfile;
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

	//Send short message to all the workers
	for(int i=0; i<w; i++){
		writeToChild(i,"yeah");
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
		kill(workers[i].pid,SIGUSR2);
	for(int i=0; i<w; i++)
		unlink(outPipes[i]);
	for(int i=0; i<w; i++)
		unlink(inPipes[i]);
	freePipeNames(w,outPipes,inPipes);
	free(in);
	free(out);
	free(workers);
	free(responses);
	free(docfile);
	exit(0);

}
