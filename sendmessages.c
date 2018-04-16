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
#define MSGSIZE 65

char *fifo = "/tmp/myfifo";

int main(int argc, char *argv[]){
	int fd, i, nwrite;
	char msgbuf[MSGSIZE+1];

	int pid;

	if (argc<2) { printf("Usage: sendamessage ... \n"); exit(1); }

	if ( mkfifo(fifo, 0666) == -1 ){
		if ( errno!=EEXIST ){
			perror("sender: mkfifo");
			exit(6);
		}
	}

	if (pid = fork() == 0){
		char * buff[2];
		printf("This is the child\n");
		buff[0] = (char*) malloc(12);
		strcpy(buff[0],"./recv");
		buff[1];
		execvp("./recv", buff);
	}
	else{

		sleep(1);

		if ( (fd=open(fifo, O_WRONLY| O_NONBLOCK)) < 0)
			{ perror("sender: fifo open error"); exit(1); }

		for (i=1; i<argc; i++){
			if (strlen(argv[i]) > MSGSIZE){
				printf("Message with Prefix %.*s Too long - Ignored\n",10,argv[i]); 
				fflush(stdout);
				continue;
				}
			strcpy(msgbuf, argv[i]);
			if ((nwrite=write(fd, msgbuf, MSGSIZE+1)) == -1)
				{ perror("Error in Writing"); exit(2); }

			}

		printf("PID is %d \n", (int) pid);
		exit(0);
	}
}

