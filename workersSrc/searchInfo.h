#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirInfo.h"

#ifndef SEARCHINFO_H
#define SEARCHINFO_H

typedef struct SearchInfo{
    int line;
    fileInfo * file;
    struct SearchInfo* next;
} SearchInfo;

#endif // SEARCHINFO_H

SearchInfo * newSearchInfo(int line, fileInfo * file);
void freeSearchInfo(SearchInfo * list);
int addSearchResult(int line, fileInfo * file, SearchInfo ** list);
void printSearchResults(SearchInfo * list);
