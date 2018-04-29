#include "pipes.h"

extern int w;
extern struct workerInfo * workers;
extern int * out;
extern int * in;
extern char ** outPipes;					//Output named pipes
extern char ** inPipes;					//Input named pipes
extern int * responses;

int openForReading(char * name){
	return open(name, O_RDONLY);
}

int openForWriting(char * name){
	return open(name, O_WRONLY);
}

void writeToChild(int id, char * message){
	char msgbuf[MSGSIZE+1] = {0};
	strcpy(msgbuf,message);
	if(write(out[id], msgbuf, MSGSIZE+1) == -1){
		perror("sender: error in writing:"); exit(2);
	}
	kill(workers[id].pid,SIGUSR1);	//signal child to read from pipe
}

void writeToPipe(int id, char * message){
	char msgbuf[MSGSIZE+1] = {0};
	strcpy(msgbuf,message);
	if(write(out[id], msgbuf, MSGSIZE+1) == -1){
		perror("sender: error in writing:"); exit(2);
	}
}

void readFromPipe(int id, char * message){
	if(read(in[id], message, MSGSIZE+1) < 0){
		perror("sender: problem in reading"); exit(5);
	}
}

void createNamedPipe(char * pipeName){
	unlink(pipeName);
	if(mkfifo(pipeName, 0600) == -1){
		perror("sender: mkfifo");
		exit(6);
	}
}

void allocateSpace(){
	responses = malloc(w*sizeof(int));
	for(int i=0; i<w; i++) responses[i] = 0;

	out = malloc(w*sizeof(int));		//Output named pipe file descriptors
	in = malloc(w*sizeof(int));			//Input named pipe file descriptors

	workers = malloc(w*sizeof(struct workerInfo));
}

//This functions opens and tests both input and output pipes for every worker
int openAndTestPipes(){
	char msgbuf[MSGSIZE+1] = {0};	//Buffer to be used with named pipes
	for(int i=0; i<w; i++){
		out[i]=openForWriting(outPipes[i]);
		in[i]=openForReading(inPipes[i]);
		usleep(10000);
		writeToChild(i,"/test");
		readFromPipe(i,msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",i);
			return 0;
		}
	}
	printf("All workers up and running.\n");
	return 1;
}

void nonBlockingInputPipes(){
	for(int i=0; i<w; i++){
		//Reopen the input pipes with non blocking argument
		close(in[i]);
		in[i] = open(inPipes[i], O_RDONLY | O_NONBLOCK);
	}
}

void createReceiver(int id){
	pid_t pid = fork();
	if(pid != 0){
		workers[id].pid = pid;	//Store child pid in global array
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
		workers[id].pid = pid;	//Store child pid in global array
		printf("New child created with pid: %d\n",(int)pid);
		close(in[id]);
		close(out[id]);
		createNamedPipe(outPipes[id]);
		createNamedPipe(inPipes[id]);
		out[id] = openForWriting(outPipes[id]);
		in[id] = openForReading(inPipes[id]);
		usleep(10000);
		writeToChild(id,"/test");
		char msgbuf[MSGSIZE+1] = {0};
		readFromPipe(id,msgbuf);
		if(strcmp(msgbuf,"/test") != 0){
			printf("Communication error with worker #%d.\n",id);
			exit(2);
		}
		char num[16] = {0};			//String with the number of directories
		sprintf(num, "%d", workers[id].dirCount);
		writeToChild(id,num);
		readFromPipe(id,msgbuf);
		if(strcmp(num,msgbuf) != 0){
			printf("Communication error with worker #%d.\n",id);
			exit(2);
		}
		for(int j=0; j<workers[id].dirCount; j++){
			writeToChild(id,workers[id].directories[j]);
			readFromPipe(id,msgbuf);
			if(strcmp(workers[id].directories[j],msgbuf) != 0){
				printf("Communication error with worker #%d.\n",id);
				exit(2);
			}
		}
		writeToChild(id,"/wc");
		readFromPipe(id,msgbuf);
		printf("Worker #%d back up and running.\n> ",id);
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
