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
#define MSGSIZE 512

char *fifo = "/tmp/myfifo";

int main(int argc, char *argv[]){
	int fd, i;
	char msgbuf[MSGSIZE+1];

	pid_t pid;

	if(mkfifo(fifo, 0666) == -1)
		if(errno!=EEXIST){
			perror("sender: mkfifo");
			exit(6);
		}

	pid = fork();

	if(pid == 0){
		char * buff[3];
		buff[0] = (char*) malloc(12);
		strcpy(buff[0],"./recv");
		buff[1] = (char*) malloc(12);
		strcpy(buff[1],fifo);
		buff[2] = NULL;
		execvp("./recv", buff);
	}
	else{
		fd=open(fifo, O_WRONLY);
		strcpy(msgbuf, "it doesnt really matter what i write yeah?");
		if(write(fd, msgbuf, MSGSIZE+1) == -1){
			perror("Error in Writing"); exit(2);
		}
	}

	strcpy(msgbuf, "/exit");	//send exit command to child
	if(write(fd, msgbuf, MSGSIZE+1) == -1){
		perror("Error in Writing"); exit(2);
	}
	exit(0);

}
