#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include "trie.h"

typedef struct fileInfo{
	char * fileName;
    char ** lines;
    int lineCounter;
    TrieNode * trie;
} fileInfo;

typedef struct dirInfo{
	char * dirName;
	int fileCount;
	struct fileInfo * files;
} dirInfo;

void loadDirInfo();
void printDirInfo();
void printDirContents(dirInfo * directory);
int countFiles(char * directory);
void getFiles(struct dirInfo * directory);

void removeNewLine(char ** str);
int countLines(char * file);
void getLines(fileInfo * file);
void addLinesToTrie(fileInfo * file, int id);
