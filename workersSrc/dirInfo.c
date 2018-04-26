#include "dirInfo.h"

extern int dirCount;
extern struct dirInfo * directories;

extern int totalLines;
extern int totalWords;
extern int totalLetters;

void loadDirInfo(){
    for(int i=0; i<dirCount; i++)
        getFiles(&directories[i]);
}

void printDirInfo(){
    for(int i=0; i<dirCount; i++)
        printDirContents(&directories[i]);
}

void printDirContents(dirInfo * directory){
    printf("\t-%s-\n",directory->dirName);
    for(int i=0; i<directory->fileCount; i++){
        printf("\t\t-%s-\n", directory->files[i].fileName);
        for(int j=0; j<directory->files[i].lineCounter; j++)
            printf("\t\t\t-%s-\n", directory->files[i].lines[j]);

    }

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

//Store the fileInfo for every file of a given directory
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
        getLines(&directory->files[i]);
        addLinesToTrie(&directory->files[i],i);
        if(getPostingList("sometimes",directory->files[i].trie) != NULL)
            printf("JUST LEFT OUT DUBAI WITH ALL MY FOLK - OPEN WATER MY LOCATION IS REMOTE (%s)\n",directory->files[i].fileName);
    }
    closedir (dir);
}

void removeNewLine(char ** str){
    for(int i=0; i<strlen(*str); i++){
        if((*str)[i] == '\n'){
            (*str)[i] = 0;
        }
    }
}

int countLines(char * file){
    FILE *stream;
    char *line = NULL;
    size_t len = 0;

    if((stream = fopen(file, "r")) == NULL){
		printf("Cannot open given docfile.");
	    exit(4);
	}

    int lineCounter = 0;
    while(getline(&line, &len, stream) != -1) {
        lineCounter++;
    }

    free(line);
    fclose(stream);
    return lineCounter;
}

void getLines(fileInfo * file){
    int lineCounter = countLines(file->fileName);
    file->lines = malloc(lineCounter*sizeof(char*));

    totalLines += lineCounter;

    FILE *stream;
    char *line = NULL;
    size_t len = 0;

	if((stream = fopen(file->fileName, "r")) == NULL){
		printf("Cannot open given docfile.");
	    exit(4);
	}

    for(int i=0; i<lineCounter; i++){
        getline(&line, &len, stream);
        file->lines[i] = malloc(strlen(line)+1);
        strcpy(file->lines[i],line);
        removeNewLine(&file->lines[i]);
        totalLetters += strlen(line);
    }

    free(line);
    fclose(stream);
    file->lineCounter = lineCounter;
}

void addLinesToTrie(fileInfo * file, int id){
    int wordCount;
    file->trie = NULL;
    for(int i=0; i<file->lineCounter; i++){
        wordCount = addWordsIntoTrie(file->lines[i],id,&file->trie);
        totalWords += wordCount;
    }
}
