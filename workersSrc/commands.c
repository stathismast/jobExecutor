#include "commands.h"

extern char * id;

extern SearchInfo * searchResults;
extern int resultsCount;
extern int deadline;

extern FILE * myLog;

void maxcount(){
    char msgbuf[MSGSIZE+1] = {0};
    readFromPipe(msgbuf);
    char * word = malloc(strlen(msgbuf)+1);
    strcpy(word,msgbuf);
    char * fileName;
    int count = getMaxWordCount(word,&fileName);
    sprintf(msgbuf, "%d", count);
    writeToPipe(msgbuf);
    strcpy(msgbuf,fileName);
    writeToPipe(msgbuf);
    if(count != 0)
        fprintf(myLog,"%d:maxcount:%s:%s:%d\n",(int)time(NULL),word,fileName,count);
    else
        fprintf(myLog,"%d:maxcount:%s\n",(int)time(NULL),word);
    free(word);
    free(fileName);
}

void mincount(){
    char msgbuf[MSGSIZE+1] = {0};
    readFromPipe(msgbuf);
    char * word = malloc(strlen(msgbuf)+1);
    strcpy(word,msgbuf);
    char * fileName;
    int count = getMinWordCount(word,&fileName);
    sprintf(msgbuf, "%d", count);
    writeToPipe(msgbuf);
    strcpy(msgbuf,fileName);
    writeToPipe(msgbuf);
    if(count != 0)
        fprintf(myLog,"%d:mincount:%s:%s:%d\n",(int)time(NULL),word,fileName,count);
    else
        fprintf(myLog,"%d:mincount:%s\n",(int)time(NULL),word);
    free(word);
    free(fileName);
}

void search(){
    char msgbuf[MSGSIZE+1] = {0};
    deadline = 0;
    readFromPipe(msgbuf);
    int termCount = atoi(msgbuf);
    char ** searchTerms = malloc(termCount*sizeof(char*));
    for(int i=0; i<termCount; i++){
        readFromPipe(msgbuf);
        searchTerms[i] = malloc(strlen(msgbuf)+1);
        strcpy(searchTerms[i],msgbuf);
    }
    searchResults = NULL;
    resultsCount = 0;
    for(int i=0; i<termCount; i++)
        if(!deadline) searchForWord(searchTerms[i]);
        else break;
    if(!deadline){
        writeToPipe("deadline");
        kill(getppid(),SIGUSR1);    //Inform the parent that we responded
        readFromPipe(msgbuf);
        if(strcmp(msgbuf,"yes") == 0)
            printSearchResults(searchResults);
    }
    if(!deadline) pause();
    freeSearchInfo(searchResults);
    for(int i=0; i<termCount; i++)
        free(searchTerms[i]);
    free(searchTerms);
    readFromPipe(msgbuf);
    writeToPipe("done");
}
