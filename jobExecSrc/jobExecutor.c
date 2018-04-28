#include "commands.h"

pid_t * childPIDs;
int w;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes
struct workerInfo * workers;
int numberOfDirectories;
char ** allDirectories;

int totalLines;
int totalWords;
int totalLetters;

int * responses;



int main(int argc, char *argv[]){
	setupSigActions();

	totalLines = 0;
	totalWords = 0;
	totalLetters = 0;

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

	//Send directory info to workers
	char msgbuf[MSGSIZE+1];
	for(int i=0; i<w; i++){
		char num[16] = {0};			//String with the number of directories
		sprintf(num, "%d", workers[i].dirCount);
		writeToChild(i,num);
		readFromPipe(i,msgbuf);
		if(strcmp(num,msgbuf) != 0){
			printf("Communication error with worker #%d.\n",i);
			exit(2);
		}
		for(int j=0; j<workers[i].dirCount; j++){
			writeToChild(i,workers[i].directories[j]);
			readFromPipe(i,msgbuf);
			if(strcmp(workers[i].directories[j],msgbuf) != 0){
				printf("Communication error with worker #%d.\n",i);
				exit(2);
			}
		}
	}

	//nonBlockingInputPipes();

	//Receive word count statistics from every worker
	for(int i=0; i<w; i++){
		writeToChild(i,"/wc");
		readFromPipe(i,msgbuf);
		totalLines += atoi(strtok(msgbuf," "));
		totalWords += atoi(strtok(NULL," "));
		totalLetters += atoi(strtok(NULL," "));
	}

	commandInputLoop();

	//While there is a child that hasnt sent a response
	// int sum = 0;
	// while(sum != w){
	// 	sum = 0;
	// 	for(int i=0; i<w; i++){
	// 		sum += responses[i];
	// 	}
	// }
	// for(int i=0; i<w; i++) responses[i] = 0;

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
