#include "searchInfo.h"

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
