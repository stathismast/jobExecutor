#include "dirInfo.h"

#ifndef SEARCHINFO_H
#define SEARCHINFO_H

typedef struct SearchInfo{
    int line;                   //Number of line in which a word was found
    fileInfo * file;            //Pointer to the file the word was found
    struct SearchInfo* next;    //Next element of the list
} SearchInfo;

#endif // SEARCHINFO_H

SearchInfo * newSearchInfo(int line, fileInfo * file);
void freeSearchInfo(SearchInfo * list);
int addSearchResult(int line, fileInfo * file, SearchInfo ** list);
void printSearchResults(SearchInfo * list);
