#include "pipes.h"

void removeNewLine(char ** str);
int countLines(char * file);
int getLines(char * file, char *** directories);
void distributeDirectories();
int manageArguments(int argc, char *argv[], char ** fileName, int * k);
