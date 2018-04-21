#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#define MSGSIZE 512

pid_t * childPIDs;
int numberOfWorkers;

int openForReading(char * name){
	return open(name, O_RDONLY);
}

int openForWriting(char * name){
	return open(name, O_WRONLY);
}

void writeToChild(int id, int fd, char * message){
	if(write(fd, message, MSGSIZE+1) == -1){
		perror("sender: error in writing:"); exit(2);
	}
	kill(childPIDs[id],SIGUSR1);	//signal child to read from pipe
}

void readFromPipe(int fd, char * message){
	if(read(fd, message, MSGSIZE+1) < 0){
		perror("sender: problem in reading"); exit(5);
	}
}

void createNamedPipe(char * pipeName){
	if(mkfifo(pipeName, 0666) == -1 && errno!=EEXIST){
		perror("sender: mkfifo");
		exit(6);
	}
}

void createReceiver(char * pipeToReceiver, char * pipeFromReceiver, int id){
	pid_t pid = fork();
	if(pid != 0){
		childPIDs[id] = pid;	//Store child pid in global array
		printf("New child created with pid: %d\n",(int)pid);
		return;
	}
	char * buff[5];
	buff[0] = (char*) malloc(20);
	strcpy(buff[0],"./recv");
	buff[1] = (char*) malloc(strlen(pipeToReceiver)+1);
	strcpy(buff[1],pipeToReceiver);
	buff[2] = (char*) malloc(strlen(pipeFromReceiver)+1);
	strcpy(buff[2],pipeFromReceiver);
	char workerID[10] = {0};
	sprintf(workerID, "%d", id);
	buff[3] = (char*) malloc(strlen(workerID)+1);
	strcpy(buff[3],workerID);
	buff[4] = NULL;
	execvp("./recv", buff);
}

void getName(int id, char ** outPipes, char ** inPipes){
	*outPipes = malloc(64);
	*inPipes = malloc(64);
	strcpy(*outPipes,"/tmp/outPipe");
	strcpy(*inPipes,"/tmp/inPipe");
	char num[10] = {0};
	sprintf(num, "%d", id+1);
	strcat(*outPipes,num);
	strcat(*inPipes,num);
}

void getPipeNames(int pipeCount, char *** outPipes, char *** inPipes){
	*outPipes = malloc(pipeCount*sizeof(char*));
	*inPipes = malloc(pipeCount*sizeof(char*));
	for(int i=0; i<pipeCount; i++){
		getName(i,&(*outPipes)[i],&(*inPipes)[i]);
	}
}

void freePipeNames(int pipeCount, char ** outPipes, char ** inPipes){
	for(int i=0; i<pipeCount; i++){
		free(outPipes[i]);
		free(inPipes[i]);
	}
	free(outPipes);
	free(inPipes);
}

void sigChild(int signum){
	signal(SIGCHLD,sigChild);
	for(int i=0; i<numberOfWorkers; i++){
		if(kill(childPIDs[i],0) != 0){
			printf("#%d child terminated\n",i);
			char * outPipeName;
			char * inPipeName;
			getName(i,&outPipeName,&inPipeName);
			createReceiver(outPipeName,inPipeName,i);
			free(outPipeName);
			free(inPipeName);
		}
	}
}

int main(int argc, char *argv[]){
	signal(SIGCHLD,sigChild);

	int w = atoi(argv[1]);				//Number of workers
	char ** outPipes;					//Output named pipes
	char ** inPipes;					//Input named pipes
	int * out = malloc(w*sizeof(int));	//Output named pipe file descriptors
	int * in = malloc(w*sizeof(int));	//Input named pipe file descriptors

	char msgbuf[MSGSIZE+1];

	childPIDs = malloc(w*sizeof(pid_t));
	numberOfWorkers = w;

	getPipeNames(w,&outPipes,&inPipes);

	for(int i=0; i<w; i++){
		createNamedPipe(outPipes[i]);
		createNamedPipe(inPipes[i]);
	}

	for(int i=0; i<w; i++)
		createReceiver(outPipes[i],inPipes[i],i);

	for(int i=0; i<w; i++){
		out[i]=openForWriting(outPipes[i]);
		in[i]=openForReading(inPipes[i]);
		writeToChild(i,out[i],"/test");
		readFromPipe(in[i],msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",i);
			exit(2);
		}
	}
	printf("All workers up and running.\n");


	signal(SIGCHLD,SIG_DFL);
	for(int i=0; i<w; i++)
		if(kill(childPIDs[i],0) == 0) writeToChild(i,out[i],"/exit");
	freePipeNames(w,outPipes,inPipes);
	free(in);
	free(out);
	free(childPIDs);
	exit(0);

}
