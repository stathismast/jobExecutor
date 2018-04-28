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

int dirReceived;

extern FILE * myLog;

void sigCheckPipe(int signum){
	char msgbuf[MSGSIZE+1] = {0};

	readFromPipe(msgbuf);
	// printf("Worker #%d: Message Received: -%s-\n", atoi(id), msgbuf);

	if(strcmp(msgbuf,"/test") == 0){
		writeToPipe(msgbuf);
		return;
	}

	if(stage == 1){
		dirCount = atoi(msgbuf);
		directories = malloc(dirCount*sizeof(dirInfo));
		stage++;
		dirReceived = 0;
		writeToPipe(msgbuf);
	}
	else if(stage == 2){
		directories[dirReceived].dirName = malloc(strlen(msgbuf)+1);
		strcpy(directories[dirReceived].dirName,msgbuf);
		dirReceived++;
		if(dirReceived == dirCount){	//Once we've received evert directory
			loadDirInfo();
		    printf("I'm worker #%d and these are my directories:\n",atoi(id));
			printDirInfo();
			stage++;
		}
		writeToPipe(msgbuf);
	}
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
	else if(stage == 4){
		if(strcmp(msgbuf,"/maxcount") == 0){
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
		else if(strcmp(msgbuf,"/mincount") == 0){
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
		else if(strcmp(msgbuf,"/search") == 0){
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
				searchForWord(searchTerms[i]);
			printf("Results from #%s: %d\n",id,resultsCount);
			printSearchResults(searchResults);
			freeSearchInfo(searchResults);
			for(int i=0; i<termCount; i++)
				free(searchTerms[i]);
			free(searchTerms);
			writeToPipe("done");
		}
		else if(strcmp(msgbuf,"/wc") == 0){
			fprintf(myLog,"%d:wc:%d:%d:%d\n",(int)time(NULL),totalLetters,totalWords,totalLines);
		}
		else {
			writeToPipe(msgbuf);
			kill(getppid(),SIGUSR1);	//Inform the parent that we responded
		}
	}
}

void sigDone(int signum){
	done = 1;
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
}

void searchForWord(char * searchTerm){
	char msgbuf[MSGSIZE+1];
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
