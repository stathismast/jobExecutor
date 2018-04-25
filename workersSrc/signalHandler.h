#include "pipes.h"


typedef struct fileInfo{
	char * fileName;
} fileInfo;

typedef struct dirInfo{
	char * dirName;
	int fileCount;
	struct fileInfo * files;
} dirInfo;


void sigCheckPipe(int signum);
void sigDone(int signum);
void setupSigActions();

int countFiles(char * directory);
void getFiles(struct dirInfo * directory);
