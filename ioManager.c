#include "ioManager.h"

// void getDirectories(char *** directories){
//
// }

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
