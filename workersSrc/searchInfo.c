#include "searchInfo.h"

extern

//Create and return a new SearchInfo
SearchInfo * newSearchInfo(int line, fileInfo * file){
    SearchInfo * node = malloc(sizeof(SearchInfo));
    node->line = line;
    node->file = file;
    node->next = NULL;
    return node;
}

//Deallocate space for a given SearchInfo list
void freeSearchInfo(SearchInfo * list){
    if(list == NULL) return;
    freeSearchInfo(list->next);
    free(list);
}

//Add a new node in SearchInfo list
int addSearchResult(int line, fileInfo * file, SearchInfo ** list){
    int nodeAdded = 0;

    if(*list == NULL){
        (*list) = newSearchInfo(line,file);
        return 1;
    }

    if(strcmp(file->fileName, (*list)->file->fileName) == 0 && line == (*list)->line)
        return 0;

    return addSearchResult(line,file,&(*list)->next);
}

//Print the contents of a SearchInfo list (for debuggin purposes only)
void printSearchResults(SearchInfo * list){
    if(list == NULL) return;

    printf("line %d in file '%s': %s\n", list->line, list->file->fileName, list->file->lines[list->line]);
    printSearchResults(list->next);
}

//Convert a given SearchInfo node to a string
char * searchInfoToString(SearchInfo * list){
    //First we calculate the length of the string
    int length = getNumberOfDigits(list->line);
    length += strlen(list->file->fileName);
    length += strlen(list->file->lines[list->line]) + 20;
    //Allocate space for the string
    char * string = malloc(length);
    sprintf(string, "line %d in file '%s': %s\n", list->line, list->file->fileName, list->file->lines[list->line]);
    return string;
}

//Returns the number of digits of a given integer
int getNumberOfDigits(int i){
	if(i==0) return 1;
	int offset = 1;
	int p = 10;
	while(i%p != i){
		offset++;
		p *= 10;
	}
	return offset;
}
