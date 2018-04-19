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

void createReceiver(char * pipeToReceiver, char * pipeFromReceiver){
	char * buff[4];
	buff[0] = (char*) malloc(20);
	strcpy(buff[0],"./recv");
	buff[1] = (char*) malloc(strlen(pipeToReceiver)+1);
	strcpy(buff[1],pipeToReceiver);
	buff[2] = (char*) malloc(strlen(pipeFromReceiver)+1);
	strcpy(buff[2],pipeFromReceiver);
	buff[3] = NULL;
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
	char ** outPipes;
	char ** inPipes;
	getPipeNames(2,&outPipes,&inPipes);

	int out[1], in[1], i;
	char msgbuf[MSGSIZE+1];

	pid_t pid;

	createNamedPipe(outPipes[0]);
	createNamedPipe(inPipes[0]);

	pid = fork();

	if(pid == 0){
		createReceiver(outPipes[0],inPipes[0]);
	}
	else{
		out[0]=openForWriting(outPipes[0]);
		in[0]=openForReading(inPipes[0]);
		writeInPipe(out[0],"it doesnt really matter what i write yeah?");
		readFromPipe(in[0],msgbuf);
		printf("Child responded: -%s-\n", msgbuf);
		writeInPipe(out[0],"i hope this works plz");
		readFromPipe(in[0],msgbuf);
		printf("Child responded: -%s-\n", msgbuf);
	}

	writeInPipe(out[0],"/exit");
	freePipeNames(2,outPipes,inPipes);
	exit(0);

}
