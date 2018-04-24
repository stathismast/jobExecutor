#include "signalHandler.h"

int in;
int out;
char * inPipe;
char * outPipe;
char * id;

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

	while(!done){
		pause();
	}

	free(inPipe);
	free(outPipe);
	free(id);
}
