#include "signalHandler.h"

extern struct workerInfo * workers;
extern int * in;
extern int w;
extern int responses;
extern int searching;
extern int deadline;

//Handler for when a child is terminated
void sigChild(int signum){
    int status;
    wait(&status);
    for(int i=0; i<w; i++){
        if(kill(workers[i].pid,0) != 0){
            printf("\n#%d worker terminated.\n",i);
            reCreateReceiver(i);
            if(searching) responses++;
        }
    }
}

//Handler to check each pipe to see if a worker sent a message
//This only called when we are running a /search command
//If a child responds we increase the value of the 'responses' variable
//When all the workers have responded we change the value of 'searching' to 0
void sigCheckPipe(int signum){
    char msgbuf[MSGSIZE+1] = {0};
    if(searching){
        nonBlockingInputPipes();
        for(int i=0; i<w; i++){
            if(read(in[i], msgbuf, MSGSIZE+1) > 0){
                if(time(NULL)<=deadline) writeToPipe(i,"yes");
                else writeToPipe(i,"no");
                responses++;
                if(responses == w) searching = 0;
            }
        }
        blockingInputPipes();
    }
}

//Set ip handler for SIGCHLD and SIGUSR1
void setupSigActions(){
    struct sigaction sigchld;
    sigchld.sa_handler = sigChild;
    sigemptyset (&sigchld.sa_mask);
    sigchld.sa_flags = 0;
    sigaction(SIGCHLD,&sigchld,NULL);

    struct sigaction sigusr1;
    sigusr1.sa_handler = sigCheckPipe;
    sigemptyset(&sigusr1.sa_mask);
    sigusr1.sa_flags = 0;
    sigaction(SIGUSR1,&sigusr1,NULL);
}
