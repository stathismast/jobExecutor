#include "pipes.h"

pid_t * childPIDs;
int numberOfWorkers;
int * out;
int * in;
char ** outPipes;					//Output named pipes
char ** inPipes;					//Input named pipes

int * responses;

void sigChild(int signum){
	signal(SIGCHLD,sigChild);
	for(int i=0; i<numberOfWorkers; i++){
		if(kill(childPIDs[i],0) != 0){
			printf("#%d child terminated.\n",i);
			reCreateReceiver(i);
		}
	}
}

//Check every pipe
void sigCheckPipe(int signum){
	signal(SIGUSR1,sigCheckPipe);

	char msgbuf[MSGSIZE+1];
	for(int i=0; i<numberOfWorkers; i++){
		close(in[i]);
		//Reopen the pipe with non blocking argument
		in[i] = open(inPipes[i], O_RDONLY | O_NONBLOCK);
		if(read(in[i], msgbuf, MSGSIZE+1) > 0){
			printf("Message from child #%d: -%s-\n",i,msgbuf);
			if(strcmp(msgbuf,"yeah") == 0) responses[i] = 1;
		}
		close(in[i]);
		//Reopen the pipe with blocking functionallity
		//This might be reduntant at this point seeing as we might only need
		//non blocking Communication from now on
		in[i] = open(inPipes[i], O_RDONLY);
	}
}

int main(int argc, char *argv[]){
	signal(SIGCHLD,sigChild);
	signal(SIGUSR1,sigCheckPipe);

	int w = atoi(argv[1]);				//Number of workers
	out = malloc(w*sizeof(int));		//Output named pipe file descriptors
	in = malloc(w*sizeof(int));			//Input named pipe file descriptors

	responses = malloc(w*sizeof(int));
	for(int i=0; i<w; i++) responses[i] = 0;

	char msgbuf[MSGSIZE+1];

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
	printf("All workers up and running.\n");

	//Inform children that they are ready to go
	//Equivilent of promting them to execute a command such as /search
	for(int i=0; i<w; i++){
		kill(childPIDs[i],SIGUSR2);
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
		if(kill(childPIDs[i],0) == 0) writeToChild(i,out[i],"/exit");
	for(int i=0; i<w; i++)
		unlink(outPipes[i]);
	for(int i=0; i<w; i++)
		unlink(inPipes[i]);
	freePipeNames(w,outPipes,inPipes);
	free(in);
	free(out);
	free(childPIDs);
	free(responses);
	exit(0);

}
