#include "signalHandler.h"
#include <dirent.h>

extern char * id;
extern int stage;
extern int done;

extern int dirCount;
extern struct dirInfo * directories;

int dirReceived;

void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};

	readFromPipe(msgbuf);
	printf("Worker #%d: Message Received: -%s-\n", atoi(id), msgbuf);

	if(strcmp(msgbuf,"/test") == 0){
		writeToPipe(msgbuf);
		return;
	}

	if(stage == 1){
		dirCount = atoi(msgbuf);
		directories = malloc(dirCount*sizeof(dirInfo));
		stage++;
		dirReceived = 0;
		writeToPipe(msgbuf);
	}
	else if(stage == 2){
		directories[dirReceived].dirName = malloc(strlen(msgbuf)+1);
		strcpy(directories[dirReceived].dirName,msgbuf);
		dirReceived++;
		if(dirReceived == dirCount){
			printf("I'm worker #%d and these are my directories:\n",atoi(id));
			for(int i=0; i<dirCount; i++){
				getFiles(&directories[i]);
				printf("\t-%s-\n",directories[i].dirName);
				for(int j=0; j<directories[i].fileCount; j++){
					printf("\t\t-%s-\n", directories[i].files[j].fileName);
				}
			}
			stage++;
		}
		writeToPipe(msgbuf);
	}
	else if(stage == 3){
		writeToPipe(msgbuf);
		kill(getppid(),SIGUSR1);	//Inform the parent that we responded
	}
}

void sigDone(int signum){
	done = 1;
}

void setupSigActions(){
	struct sigaction sigusr1;
	sigusr1.sa_handler = sigCheckPipe;
	sigemptyset(&sigusr1.sa_mask);
	sigusr1.sa_flags = 0;
	sigaction(SIGUSR1,&sigusr1,NULL);

	struct sigaction sigusr2;
	sigusr2.sa_handler = sigDone;
	sigemptyset (&sigusr2.sa_mask);
	sigusr2.sa_flags = 0;
	sigaction(SIGUSR2,&sigusr2,NULL);
}

int countFiles(char * directory){
    DIR *dir;
    struct dirent *ent;
    int fileCount = 0;
    if((dir = opendir(directory)) == NULL) {
        perror ("worker");
        exit(3);
    }
      /* print all the files and directories within directory */
    while((ent = readdir(dir)) != NULL) {
        fileCount++;
    }
    closedir (dir);
    return fileCount-2; //Don't include current and parent directories
}

void getFiles(struct dirInfo * directory){
    DIR *dir;
    struct dirent *ent;

    directory->fileCount = countFiles(directory->dirName);
    directory->files = malloc(directory->fileCount*sizeof(fileInfo));

    if((dir = opendir(directory->dirName)) == NULL) {
        perror("worker");
        exit(3);
    }

    ent = readdir(dir); //Consume curent and parent directories
    ent = readdir(dir);
    for(int i=0; i<directory->fileCount; i++){
        ent = readdir(dir);
        directory->files[i].fileName = malloc(strlen(directory->dirName)+strlen(ent->d_name)+2);
        directory->files[i].fileName[0] = 0;
        strcat(directory->files[i].fileName,directory->dirName);
        strcat(directory->files[i].fileName,"/");
        strcat(directory->files[i].fileName,ent->d_name);
        strcat(directory->files[i].fileName,"\0");
    }
    closedir (dir);
}
