#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#define MSGSIZE 65

char *fifo = "/tmp/myfifo";

int main(int argc, char *argv[]){
	int fd, i, nwrite;
	char msgbuf[MSGSIZE+1];

	if (argc<2) { printf("Usage: sendamessage ... \n"); exit(1); }

	if ( (fd=open(fifo, O_WRONLY| O_NONBLOCK)) < 0)
		{ perror("fife open error"); exit(1); }

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
	exit(0);
}
		
