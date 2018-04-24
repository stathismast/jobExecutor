#include "pipes.h"

extern pid_t * childPIDs;
extern int * out;
extern int * in;
extern char ** outPipes;					//Output named pipes
extern char ** inPipes;					//Input named pipes

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

		// printf("About to signal child with pid: %d\n",childPIDs[id]);
	int retval = kill(childPIDs[id],SIGUSR1);	//signal child to read from pipe
			// printf("Signal retval was: %d\n",retval);
}

void readFromPipe(int fd, char * message){
	if(read(fd, message, MSGSIZE+1) < 0){
		perror("sender: problem in reading"); exit(5);
	}
}

void createNamedPipe(char * pipeName){
	unlink(pipeName);		//Delete named pipe if it already exists
	if(mkfifo(pipeName, 0600) == -1){
		perror("sender: mkfifo");
		exit(6);
	}
}

void createReceiver(int id){
	pid_t pid = fork();
	if(pid != 0){
		childPIDs[id] = pid;	//Store child pid in global array
		printf("New child created with pid: %d\n",(int)pid);
		return;
	}
	char * buff[5];
	buff[0] = (char*) malloc(20);
	strcpy(buff[0],"./worker");
	buff[1] = (char*) malloc(strlen(outPipes[id])+1);
	strcpy(buff[1],outPipes[id]);
	buff[2] = (char*) malloc(strlen(inPipes[id])+1);
	strcpy(buff[2],inPipes[id]);
	char workerID[10] = {0};
	sprintf(workerID, "%d", id);
	buff[3] = (char*) malloc(strlen(workerID)+1);
	strcpy(buff[3],workerID);
	buff[4] = NULL;
	execvp("./worker", buff);
}

void reCreateReceiver(int id){
	pid_t pid = fork();
	if(pid != 0){
		childPIDs[id] = pid;	//Store child pid in global array
		printf("New child created with pid: %d\n",(int)pid);
		createNamedPipe(outPipes[id]);
		createNamedPipe(inPipes[id]);
		out[id] = openForWriting(outPipes[id]);
		in[id] = openForReading(inPipes[id]);
		writeToChild(id,out[id],"/test");
		char msgbuf[MSGSIZE+1] = {0};
		readFromPipe(in[id],msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",id);
			exit(2);
		}
		return;
	}
	char * buff[5];
	buff[0] = (char*) malloc(20);
	strcpy(buff[0],"./worker");
	buff[1] = (char*) malloc(strlen(outPipes[id])+1);
	strcpy(buff[1],outPipes[id]);
	buff[2] = (char*) malloc(strlen(inPipes[id])+1);
	strcpy(buff[2],inPipes[id]);
	char workerID[10] = {0};
	sprintf(workerID, "%d", id);
	buff[3] = (char*) malloc(strlen(workerID)+1);
	strcpy(buff[3],workerID);
	buff[4] = NULL;
	execvp("./worker", buff);
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
