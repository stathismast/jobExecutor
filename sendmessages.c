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

int openForReading(char * name){
	return open(name, O_RDONLY);
}

int openForWriting(char * name){
	return open(name, O_WRONLY);
}

void writeInPipe(int fd, char * message){
	if(write(fd, message, MSGSIZE+1) == -1){
		perror("sender: error in writing:"); exit(2);
	}
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

void getPipeNames(int pipeCount, char *** outPipes, char *** inPipes){
	*outPipes = malloc(pipeCount*sizeof(char*));
	*inPipes = malloc(pipeCount*sizeof(char*));
	for(int i=0; i<pipeCount; i++){
		(*outPipes)[i] = malloc(64);
		(*inPipes)[i] = malloc(64);
		strcpy((*outPipes)[i],"/tmp/outPipe");
		strcpy((*inPipes)[i],"/tmp/inPipe");
		char num[10] = {0};
		sprintf(num, "%d", i+1);
		strcat((*outPipes)[i],num);
		strcat((*inPipes)[i],num);
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

int main(int argc, char *argv[]){
	int w = atoi(argv[1]);							//Number of workers
	char ** outPipes;					//Output named pipes
	char ** inPipes;					//Input named pipes
	int * out = malloc(w*sizeof(int));	//Output named pipe file descriptors
	int * in = malloc(w*sizeof(int));	//Input named pipe file descriptors

	char msgbuf[MSGSIZE+1];

	getPipeNames(w,&outPipes,&inPipes);

	for(int i=0; i<w; i++){
		createNamedPipe(outPipes[i]);
		createNamedPipe(inPipes[i]);
	}

	for(int i=0; i<w; i++){
		if(fork() == 0)
			createReceiver(outPipes[i],inPipes[i],i);
	}

	for(int i=0; i<w; i++){
		out[i]=openForWriting(outPipes[i]);
		in[i]=openForReading(inPipes[i]);
		writeInPipe(out[i],"message one");
		readFromPipe(in[i],msgbuf);
		printf("Child responded: -%s-\n", msgbuf);
	}

	for(int i=0; i<w; i++)
		writeInPipe(out[i],"/exit");
	freePipeNames(w,outPipes,inPipes);
	free(in);
	free(out);
	exit(0);

}
