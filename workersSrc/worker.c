#include "signalHandler.h"

int in;
int out;
char * inPipe;
char * outPipe;
char * id;

int dirCount;
dirInfo * directories;

int stage;
int done;

void manageArguments(int argc, char *argv[]){
	inPipe = malloc(strlen(argv[1])+1);
	strcpy(inPipe,argv[1]);
	outPipe = malloc(strlen(argv[2])+1);
	strcpy(outPipe,argv[2]);
	id = malloc(strlen(argv[3])+1);
	strcpy(id,argv[3]);
}

int main(int argc, char *argv[]){
	setupSigActions();
	manageArguments(argc,argv);
	openPipes();

	done = 0;
	stage = 1;

	while(!done){
		pause();
	}

	free(inPipe);
	free(outPipe);
	free(id);
	for(int i=0; i<dirCount; i++){
		for(int j=0; j<directories[i].fileCount; j++){
			free(directories[i].files[j].fileName);
			for(int k=0; k<directories[i].files[j].lineCounter; k++){
				free(directories[i].files[j].lines[k]);
			}
			free(directories[i].files[j].lines);
		}
		free(directories[i].dirName);
		free(directories[i].files);
	}
	free(directories);
}
