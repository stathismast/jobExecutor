#include "ioManager.h"

extern int w;
extern struct workerInfo * workers;
extern int numberOfDirectories;
extern char ** allDirectories;

void removeNewLine(char ** str){
    for(int i=0; i<strlen(*str); i++){
        if((*str)[i] == '\n'){
            (*str)[i] = 0;
        }
    }
}

int countLines(char * file){
    FILE *stream;
    char *line = NULL;
    size_t len = 0;

    if((stream = fopen(file, "r")) == NULL){
		printf("Cannot open given docfile.");
	    exit(4);
	}

    int lineCounter = 0;
    while(getline(&line, &len, stream) != -1) {
        lineCounter++;
    }

    free(line);
    fclose(stream);
    return lineCounter;
}

int getLines(char * file, char *** directories){
    int lineCounter = countLines(file);
    *directories = malloc(lineCounter*sizeof(char*));

    FILE *stream;
    char *line = NULL;
    size_t len = 0;

	if((stream = fopen(file, "r")) == NULL){
		printf("Cannot open given docfile.");
	    exit(4);
	}

    for(int i=0; i<lineCounter; i++){
        getline(&line, &len, stream);
        (*directories)[i] = malloc(strlen(line)+1);
        strcpy((*directories)[i],line);
        removeNewLine(&(*directories)[i]);
    }

    free(line);
    fclose(stream);
    return lineCounter;
}

void distributeDirectories(){
	for(int i=0; i<w; i++)
		workers[i].dirCount = numberOfDirectories/w;

	for(int i=0; i<numberOfDirectories%w; i++)
		workers[i].dirCount++;

	// for(int i=0; i<w; i++)
	// 	printf("%d. %d directorie(s)\n", i, workers[i].dirCount);

	int pos=0;
	for(int i=0; i<w; i++){
		workers[i].directories = malloc(workers[i].dirCount*sizeof(char*));
		for(int j=0; j<workers[i].dirCount; j++){
			workers[i].directories[j] = malloc(strlen(allDirectories[pos])+1);
			strcpy(workers[i].directories[j], allDirectories[pos]);
			pos++;
		}
		// printf("Directories for %d:\n",i);
		// for(int j=0; j<workers[i].dirCount; j++){
		// 	printf("\t%s\n",workers[i].directories[j]);
		// }
	}
}

//Manages arguments given on execution and checks for errors
int manageArguments(int argc, char *argv[], char ** docfile, int * numWorkers){
	int gotInputFile = 0;		//Is true if '-d' argument is given
	int argumentError = 0;		//Is true if arguments are invalid
	int invalidWArgument = 0;	//Is true if '-w' argument in specific is invalid

	*numWorkers = 4;

	if(argc > 5){	//Too many arguments
		printf("ERROR: Too many arguments.\n");
		printf("Usage: ./jobExecutor -d docfile -w numWorkers\nArguments can be given in any order and only '-d' is necessary. 'numWorkers' defaults to 4.\n");
		return -1;
	}

	//Check for each argument (this part of the code is based on an
	//argument manager I developed for a previous assignment)
	for(int i=1; i<argc; i++)
		if(strcmp(argv[i],"-d") == 0){				//If '-d' is given
			if(argc > i+1 && !gotInputFile){		//If there is another argument after this
				//Allocate space for the file name
				(*docfile) = malloc((strlen(argv[i+1])+1)*sizeof(char));
				strcpy(*docfile,argv[i+1]);			//Store file name
				gotInputFile = 1;					//Set value to '1' to denote that we now have recieved an input file name
				i++;								//Increase 'i' because we consumed the following argument as well
			}
			else{									//More than one '-d' given or there is no argument after '-d'
				argumentError = 1;
				break;
			}
		}
		else if(strcmp(argv[i],"-w") == 0){							//If '-w' is given
		if(argc > i+1){												//If there is another argument after this
				*numWorkers = atoi(argv[i+1]);						//Store value of 'numWorkers'
				if(*numWorkers < 1) {invalidWArgument = 1; break;}	//If '-w' is 0 or negative or not a numeral
				i++;												//Increase 'i' because we consumed the following argument as well
			}
			else{													//There is no argument after '-w'
				argumentError = 1;
				break;
			}
		}
		else{	//Invalid argument given
			argumentError = 1;
		}

	//Print error messages
	if(invalidWArgument){
		printf("ERROR: Invalid '-w numWorkers' argument. 'numWorkers' should be a number greater than 0.\n");
		printf("Usage: ./jobExecutor -d docfile -w numWorkers\nArguments can be given in any order and only '-d' is necessary. 'numWorkers' defaults to 4.\n");
		return -1;
	}
	if(argumentError || !gotInputFile){
		printf("ERROR: Invalid arguments.\n");
		printf("Usage: ./jobExecutor -d docfile -w numWorkers\nArguments can be given in any order and only '-d' is necessary. 'numWorkers' defaults to 4.\n");
		return -1;
	}
}
