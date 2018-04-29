#include "signalHandler.h"

extern char * id;
extern int stage;
extern int done;

extern int dirCount;
extern struct dirInfo * directories;

extern int totalLines;
extern int totalWords;
extern int totalLetters;

extern SearchInfo * searchResults;
extern int resultsCount;
extern int deadline;

extern int commandID;

int dirReceived;

extern FILE * myLog;

char msgbuf[MSGSIZE+1] = {0};

void sigCheckPipe(int signum){

    readFromPipe(msgbuf);

    //If this is part of a test respond with the same message
    if(strcmp(msgbuf,"/test") == 0){
        writeToPipe(msgbuf);
        return;
    }

    //Stage 1 is where the worker receives a number, which will be the
    //total number of directories it will be in charge of
    if(stage == 1){
        dirCount = atoi(msgbuf);
        directories = malloc(dirCount*sizeof(dirInfo));
        stage++;
        dirReceived = 0;
        writeToPipe(msgbuf);
    }
    //Stage 2 is where the worker receives the names of each directory
    //And loads each file to memory
    else if(stage == 2){
        directories[dirReceived].dirName = malloc(strlen(msgbuf)+1);
        strcpy(directories[dirReceived].dirName,msgbuf);
        dirReceived++;
        if(dirReceived == dirCount){    //Once we've received every directory
            loadDirInfo();
            stage++;
        }
        writeToPipe(msgbuf);
    }
    //Stage 3 is where the worker send its /wc statistics to the jobExecutor
    else if(stage == 3){
        if(strcmp(msgbuf,"/wc") == 0){
            char response[MSGSIZE+1] = {0};
            sprintf(response,"%d %d %d ",totalLines,totalWords,totalLetters);
            writeToPipe(response);
            stage++;
        }
        else{
            printf("Worker error: Expected message to calculate word count.\n");
        }
    }
    //Stage 4 is where the worker is waiting for a command from the jobExecutor
    else if(stage == 4){
        if(strcmp(msgbuf,"/maxcount") == 0){
            commandID = 1;
        }
        else if(strcmp(msgbuf,"/mincount") == 0){
            commandID = 2;
        }
        else if(strcmp(msgbuf,"/search") == 0){
            commandID = 3;
        }
        else if(strcmp(msgbuf,"/wc") == 0){
            fprintf(myLog,"%d:wc:%d:%d:%d\n",(int)time(NULL),totalLines,totalWords,totalLetters);
        }
    }
}

void sigDone(int signum){
    done = 1;
}

void sigDeadline(int signum){
    deadline = 1;
}

void setupSigActions(){
    struct sigaction sigusr1;
    sigusr1.sa_handler = sigCheckPipe;
    sigemptyset(&sigusr1.sa_mask);
    sigusr1.sa_flags = 0;
    sigaction(SIGUSR1,&sigusr1,NULL);

    struct sigaction sigusr2;
    sigusr2.sa_handler = sigDone;
    sigemptyset (&sigusr2.sa_mask);
    sigusr2.sa_flags = 0;
    sigaction(SIGUSR2,&sigusr2,NULL);

    struct sigaction sigchld;
    sigchld.sa_handler = sigDeadline;
    sigemptyset (&sigchld.sa_mask);
    sigchld.sa_flags = 0;
    sigaction(SIGCHLD,&sigchld,NULL);
}

void searchForWord(char * searchTerm){
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
