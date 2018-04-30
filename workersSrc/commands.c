#include "commands.h"

extern char * id;

extern int dirCount;
extern dirInfo * directories;

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

    writeToPipe("deadline");    //Ask if we are within the deadline
    readFromPipe(msgbuf);       //Read response
    if(strcmp(msgbuf,"yes") == 0)
        sendSearchResults();
    freeSearchInfo(searchResults);
    for(int i=0; i<termCount; i++)
        free(searchTerms[i]);
    free(searchTerms);
}

void searchForWord(char * searchTerm){
    char msgbuf[MSGSIZE+1] = {0};
    sprintf(msgbuf,"%d:search:%s",(int)time(NULL),searchTerm);
    for(int i=0; i<dirCount; i++){
        for(int j=0; j<directories[i].fileCount; j++){
            PostingListHead * pl = getPostingList(searchTerm,directories[i].files[j].trie);
            if(pl != NULL){
                PostingListNode * plNode = pl->next;
                while(plNode != NULL){
                    resultsCount += addSearchResult(plNode->id,&directories[i].files[j],&searchResults);
                    if(strstr(msgbuf, directories[i].files[j].fileName) == NULL){
                        strcat(msgbuf,":");
                        strcat(msgbuf,directories[i].files[j].fileName);
                    }
                    plNode = plNode->next;
                }
            }
        }
    }
    fprintf(myLog,"%s\n",msgbuf);
}

void sendSearchResults(){
    char msgbuf[MSGSIZE+1] = {0};
    char * temp;
    SearchInfo * node = searchResults;
    int count = 0;
    while(node != NULL){
        temp = searchInfoToString(node);
        strcpy(msgbuf,temp);
        free(temp);
        writeToPipe(msgbuf);
        node = node->next;
        count++;
    }
    strcpy(msgbuf,"noMoreResults");
    writeToPipe(msgbuf);
}
