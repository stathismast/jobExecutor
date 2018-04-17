#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MSGSIZE 512

int main(int argc, char *argv[]){
	int fd, i, nwrite;
	char msgbuf[MSGSIZE+1];

	char * fifo = malloc(strlen(argv[1])+1);
	strcpy(fifo,argv[1]);

	if (argc>2) { printf("Usage: receivemessage & \n"); exit(1); }
	if ( (fd=open(fifo, O_RDWR)) < 0){
		perror("receiver: fifo open problem"); exit(3);
		}
	for (;;){
		if ( read(fd, msgbuf, MSGSIZE+1) < 0) {
			perror("problem in reading"); exit(5);
			}
		printf("Message Received: -%s-\n", msgbuf);
		if(strcmp(msgbuf,"/exit") == 0) return 0;
		}
}
