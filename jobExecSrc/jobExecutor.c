#include "signalHandler.h"
#include <time.h>

pid_t * childPIDs;
int w;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes
struct workerInfo * workers;
int numberOfDirectories;
char ** allDirectories;

int * responses;

int main(int argc, char *argv[]){
	setupSigActions();

	//Read and check arguments for validity
	char * docfile;
	if(manageArguments(argc,argv,&docfile,&w) < 0) exit(3);

	numberOfDirectories = getLines(docfile,&allDirectories);
	if(w>numberOfDirectories){
		w = numberOfDirectories;
		printf("The original number of workers is more than the directories in docfile. Number of workers is now set to %d.\n",w);
	}

	//Allocate space for pipe file descriptors, pipe names, array to keep
	//PIDs of workers etc.
	allocateSpace();

	//Decide which directories will be assigned to each worker
	distributeDirectories();

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
	free(responses);
	free(docfile);

	for(int i=0; i<numberOfDirectories; i++){
		free(allDirectories[i]);
	}
	free(allDirectories);

	for(int i=0; i<w; i++){
		for(int j=0; j<workers[i].dirCount; j++){
			free(workers[i].directories[j]);
		}
		free(workers[i].directories);
	}
	free(workers);
	exit(0);

}
