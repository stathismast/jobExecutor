#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

int countFiles(char * directory){
    DIR *dir;
    struct dirent *ent;
    int fileCount = 0;
    if((dir = opendir("dir1")) == NULL) {
        perror ("worker:");
        exit(3);
    }
      /* print all the files and directories within directory */
    while((ent = readdir(dir)) != NULL) {
        fileCount++;
    }
    closedir (dir);
    return fileCount-2; //Don't include current and parent directories
}

int getFiles(char * directory, char *** files){
    DIR *dir;
    struct dirent *ent;

    int fileCount = countFiles(directory);
    *files = malloc(fileCount*sizeof(char*));

    if((dir = opendir(directory)) == NULL) {
        perror("worker:");
        exit(3);
    }

    ent = readdir(dir); //Consume curent and parent directories
    ent = readdir(dir);
    for(int i=0; i<fileCount; i++){
        ent = readdir(dir);
        (*files)[i] = malloc(strlen(directory)+strlen(ent->d_name)+2);
        (*files)[i][0] = 0;
        strcat((*files)[i],directory);
        strcat((*files)[i],"/");
        strcat((*files)[i],ent->d_name);
        strcat((*files)[i],"\0");
    }
    closedir (dir);
    return fileCount;
}

int main(void){

    char * directory = malloc(10);
    strcpy(directory,"dir1");
    char ** files;

    int fileCount = getFiles(directory,&files);

    for(int i=0; i<fileCount; i++){
        printf("-%s-\n",files[i]);
    }

    for(int i=0; i<fileCount; i++){
        free(files[i]);
    }
    free(files);
    free(directory);
}
