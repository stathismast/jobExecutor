#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if((stream = fopen(file, "r")) == NULL)
        exit(4);

    int lineCounter = 0;
    while(getline(&line, &len, stream) != -1) {
        lineCounter++;
    }

    free(line);
    fclose(stream);
    return lineCounter;
}

int getLines(char * file, char *** directories){
    int lineCounter = countLines(file);
    *directories = malloc(lineCounter*sizeof(char*));

    FILE *stream;
    char *line = NULL;
    size_t len = 0;

    if((stream = fopen("docfile.txt", "r")) == NULL)
        exit(4);

    for(int i=0; i<lineCounter; i++){
        getline(&line, &len, stream);
        (*directories)[i] = malloc(strlen(line)+1);
        strcpy((*directories)[i],line);
        removeNewLine(&(*directories)[i]);
    }

    free(line);
    fclose(stream);
    return lineCounter;
}

int main(void){
    char ** directories;
    int lineCounter = getLines("docfile.txt",&directories);

    for(int i=0; i<lineCounter; i++){
        printf("%d. -%s-\n", i+1, directories[i]);
    }

    for(int i=0; i<lineCounter; i++){
        free(directories[i]);
    }
    free(directories);
}
