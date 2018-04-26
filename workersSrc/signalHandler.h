#include "pipes.h"
#include "dirInfo.h"

void sigCheckPipe(int signum);
void sigDone(int signum);
void setupSigActions();

int countFiles(char * directory);
void getFiles(struct dirInfo * directory);
