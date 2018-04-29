#include "commands.h"

extern struct workerInfo * workers;

extern int totalLines;
extern int totalWords;
extern int totalLetters;

extern int w;
extern int responses;
extern int searching;
extern int deadline;

//Get a line for 'stdin' and return the string containing that line
char * getCommand(){
    char * command = NULL;
    size_t size;
    while(getline(&command, &size, stdin) == -1){
        clearerr(stdin);
        continue;
    }
    return command;
}

//Check if 'str' is a string with numerals only
int isNumber(char * str){
    if(str == NULL) return 0;
    for(int i=0; i<strlen(str); i++)
        if(str[i] < 48 || str[i] > 57)
            return 0;
    return 1;
}

//Main loop for command input and execution
void commandInputLoop(){
    char * command;
    while(1){
        printf("> ");
        command = getCommand();
        if(command == NULL) continue;
        if(*command == '\n') { free(command); continue; }

        command = strtok(command," \t\n");
        if(command == NULL) { free(command); continue; }

        if(strcmp(command, "/exit") == 0){
            free(command);
            printf("Exiting...\n");
            break;
        }
        else if(strcmp(command, "/maxcount") == 0){
            char * command = strtok(NULL," \t\n");
            maxCount(command);
        }
        else if(strcmp(command, "/mincount") == 0){
            char * command = strtok(NULL," \t\n");
            minCount(command);
        }
        else if(strcmp(command, "/wc") == 0){
            printf("wc: %d %d %d\n", totalLines,totalWords,totalLetters);
            for(int i=0; i<w; i++){
                writeToChild(i,"/wc");
            }
        }
        else if(strcmp(command, "/search") == 0)
            search();

        //If non of the above commands is given, the input is invalid
        else printf("Invalid command.\n");
        free(command);
    }
}

void search(){
    char msgbuf[MSGSIZE+1];
    int termCount = 0;
    char * searchTerms[10] = {NULL};//Array for the first 10 search terms given
    char * term;                    //Temporary 'string' used to store search terms

    for(int i=0; i<10; i++)                                //Read and store every search term
        if((term = strtok(NULL, " \t\n")) != NULL){        //Check the token/term
            searchTerms[i] = malloc(strlen(term)+1);    //Allocate space for it
            memcpy(searchTerms[i], term, strlen(term));    //Store it
            searchTerms[i][strlen(term)] = 0;            //Add a null character at the end of the string
            termCount++;
        } else break;

    sprintf(msgbuf,"%d",termCount);
    for(int i=0; i<w; i++)
        writeToChild(i,"/search");
    for(int i=0; i<w; i++)
        writeToPipe(i,msgbuf);
    for(int i=0; i<w; i++)
        for(int j=0; j<termCount; j++)
            writeToPipe(i,searchTerms[j]);

    deadline = time(NULL) + 2;
    responses = 0;
    searching = 1;
    while(responses != w && deadline > time(NULL))
        sleep(deadline - time(NULL));
    searching = 0;

    for(int i=0; i<w; i++)
        kill(workers[i].pid,SIGCHLD);

    for(int i=0; i<w; i++)
        writeToPipe(i,"deadline");
    for(int i=0; i<w; i++)
        readFromPipe(i,msgbuf);
    for(int i=0; i<termCount; i++)
        free(searchTerms[i]);
    printf("%d out of %d workers responded.\n", responses, w);
}

void maxCount(char * keyword){
    char msgbuf[MSGSIZE+1];
    int * counts = malloc(w*sizeof(int));
    char ** fileNames = malloc(w*sizeof(char*));
    for(int i=0; i<w; i++){
        writeToChild(i,"/maxcount");
        writeToPipe(i,keyword);    //Send message to worker without signaling
    }
    //Store the max count from every worker
    for(int i=0; i<w; i++){
        readFromPipe(i,msgbuf);
        counts[i] = atoi(msgbuf);
    }
    //Store the file that has the max count from every worker
    for(int i=0; i<w; i++){
        readFromPipe(i,msgbuf);
        fileNames[i] = malloc(strlen(msgbuf)+1);
        strcpy(fileNames[i],msgbuf);
    }
    //Find the overall max count
    int max = 0;
    for(int i=1; i<w; i++){
        if(counts[i] > counts[max])
            max = i;
    }
    printf("maxcount for %s: %d: %s\n", keyword, counts[max], fileNames[max]);
    free(counts);
    for(int i=0; i<w; i++){
        free(fileNames[i]);
    }
    free(fileNames);
}

void minCount(char * keyword){
    char msgbuf[MSGSIZE+1];
    int * counts = malloc(w*sizeof(int));
    char ** fileNames = malloc(w*sizeof(char*));
    for(int i=0; i<w; i++){
        writeToChild(i,"/mincount");
        writeToPipe(i,keyword);    //Send message to worker without signaling
    }
    //Store the min count from every worker
    for(int i=0; i<w; i++){
        readFromPipe(i,msgbuf);
        counts[i] = atoi(msgbuf);
    }
    //Store the file that has the min count from every worker
    for(int i=0; i<w; i++){
        readFromPipe(i,msgbuf);
        fileNames[i] = malloc(strlen(msgbuf)+1);
        strcpy(fileNames[i],msgbuf);
    }
    //Find the overall min count
    int min = 0;
    int found = 0;
    for(int i=1; i<w; i++){
        if(!found && counts[i] > 0){
            min = i;
            found = 1;
        }
        else if(counts[i] < counts[min] && counts[i] > 0)
            min = i;
    }
    printf("mincount for %s: %d: %s\n", keyword, counts[min], fileNames[min]);
    free(counts);
    for(int i=0; i<w; i++){
        free(fileNames[i]);
    }
    free(fileNames);
}
