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
            //We have already collected the counts from each worker
            //So we just print the out
            printf("wc: %d %d %d\n", totalLines,totalWords,totalLetters);

            //Inform workers so they can add the /wc command in their logs
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

//Function in charge of executing the /search command
void search(){
    char msgbuf[MSGSIZE+1];
    int termCount = 0;
    char * searchTerms[12] = {NULL};
    char * term;

    //Read and store every search term
    for(int i=0; i<12; i++)
        if((term = strtok(NULL, " \t\n")) != NULL){
            searchTerms[i] = malloc(strlen(term)+1);
            memcpy(searchTerms[i], term, strlen(term));
            searchTerms[i][strlen(term)] = 0;
            termCount++;
        } else break;

    //Store the given deadline
    int pos = 0;
    while(pos<termCount){
        if(strcmp(searchTerms[pos],"-d") == 0) break;
        pos++;
    }
    if(pos != termCount-2){
        printf("Error: Invalid /search command\n");
        for(int i=0; i<termCount; i++)
            free(searchTerms[i]);
        return;
    }
    int dl = atoi(searchTerms[termCount-1]);
    if(dl <= 0){
        printf("Error: Invalid /search command: -d argument must be a number greater than 0\n");
        for(int i=0; i<termCount; i++)
            free(searchTerms[i]);
        return;
    }

    //Inform workers that we are about to run a search command
    for(int i=0; i<w; i++)
        writeToChild(i,"/search");

    //Send the total number of search terms to the workers
    sprintf(msgbuf,"%d",termCount-2);   //String with the number of search terms
    for(int i=0; i<w; i++)
        writeToPipe(i,msgbuf);

    //Send every search term to each worker
    for(int i=0; i<w; i++)
        for(int j=0; j<termCount-2; j++)
            writeToPipe(i,searchTerms[j]);

    //Calculate deadline
    deadline = time(NULL) + dl;
    responses = 0;
    searching = 1;
    //Sleep until EITHER:
    //a. all the workers have responed with their results OR
    //b. we have reached the deadline
    while(responses != w && deadline > time(NULL))
        sleep(deadline - time(NULL));
    searching = 0;

    //If a worker is still searching this signal informs them to stop
    for(int i=0; i<w; i++)
        kill(workers[i].pid,SIGCHLD);

    //Send a message to workers to confirm that they have stopped searching
    for(int i=0; i<w; i++)
        writeToPipe(i,"deadline");
    //Confirmation from workers
    for(int i=0; i<w; i++)
        readFromPipe(i,msgbuf);

    for(int i=0; i<termCount; i++)
        free(searchTerms[i]);
    printf("%d out of %d workers responded.\n", responses, w);
}

//Function in charge of executing the /maxcount command
void maxCount(char * keyword){
    char msgbuf[MSGSIZE+1];
    int * counts = malloc(w*sizeof(int));
    char ** fileNames = malloc(w*sizeof(char*));

    //Inform the workers that we are about to execute a maxcount command
    for(int i=0; i<w; i++){
        writeToChild(i,"/maxcount");
        writeToPipe(i,keyword);    //Send message to worker without signaling
                       //No need to signal because the worker is expecting it
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

    //Print out the result
    if(counts[max] == 0)
        printf("'%s' does not exist in the given dataset\n", keyword);
    else printf("'%s' appears %d times in '%s'\n", keyword, counts[max], fileNames[max]);
    free(counts);
    for(int i=0; i<w; i++){
        free(fileNames[i]);
    }
    free(fileNames);
}

//Function in charge of executing the /mincount command
void minCount(char * keyword){
    char msgbuf[MSGSIZE+1];
    int * counts = malloc(w*sizeof(int));
    char ** fileNames = malloc(w*sizeof(char*));
    for(int i=0; i<w; i++){
        writeToChild(i,"/mincount");
        writeToPipe(i,keyword);    //Send message to worker without signaling
                       //No need to signal because the worker is expecting it
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

    //Print out the result
    if(counts[min] == 0)
        printf("'%s' does not exist in the given dataset\n", keyword);
    else printf("%s appears %d times in '%s'\n", keyword, counts[min], fileNames[min]);
    free(counts);
    for(int i=0; i<w; i++){
        free(fileNames[i]);
    }
    free(fileNames);
}

void getWordCount(){
    char msgbuf[MSGSIZE+1];
    for(int i=0; i<w; i++){
        writeToChild(i,"/wc");
        readFromPipe(i,msgbuf);
        totalLines += atoi(strtok(msgbuf," "));
        totalWords += atoi(strtok(NULL," "));
        totalLetters += atoi(strtok(NULL," "));
    }
}
