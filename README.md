Efstathios-Andreas Mastorakis - sdi1500092

# miniSearch
Second Project for SysPro 2018

Compilation:
make: To compile the both the jobExecutor and the worker
make clean: To delete the executables and .o files

You can also choose to compile any of the included files using the Makefiles as
such: make <directory>/<file>.c

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Brief description of the basic features and funcionality:

INITILIZING THE WORKERS
When the job executor is run the first id does is set up its signal handlers,
one for when a child is terminated and another for when a worker sends a
message to a pipe. Next, the job executor manages the command line arguments
and decides which directories should be assigned to each worker.
Then, it creates the named pipes and the workers. After that it opens and tests
the pipes, sends the directory names to the workers, waits for them to load
their files. After that it asks each worker for its /wc statistics and
adds them together in advance.

After all the above the job executor is ready to receive commands from the user

PIPES
The jobExecutor has two pipes (one for input, one for output) for each worker.
Similarly, each worker has two pipes towards and from the jobExecutor.

WORKERS
Each worker stores information for every directory that is assigned to it and
every file in those directories.
For each different files it creates a Trie that stores information like:
    -all the words that are in the file
    -in what line every word is in
    -how many times each word is in each line
    -how many times each word is in the whole file

MAXCOUNT/MINCOUNT
To calculate the maxcount/mincount for a given word, the worker checks the Trie
of each file to find the file that has the given word the most/least times.

SEARCH
To search the lines that include a given word the worker goes through the Trie
of each file and if it finds a line that has the given word it adds it to a
global list. That list only stores unique entries to avoid duplicates in when
printing the results.

DEADLINE
Once a worker is done searching, it asks the jobExecutor if it was within the
deadline or not. If it was on time, the jobExecutor send a positive message and
receives the results to prints them out. If the worker was not on time, the
jobExecutor will send a negative response and no more messages will be sent.
The jobExecutor waits for the workers to finish but if the time runs out and
there is atleast one worker that hasn't responded, the jobExecutor will send
a signal to that worker to stop it from continuing to search.

SENDING SEARCH RESULTS TO JOBEXECUTOR
If a string that contains a search result is bigger than our buffer, the worker
will send that string in chunks, one by one, to fit our buffer size.

WORKER RECOVERY
When a worker is unexpectedly terminated the jobExecutor will try to replace it.
A new worker will be created and will be assigned the directories that
originally belonged to the terminated process. After that, the jobExecutor and
the new worker will go through the initilization process as described above.

SCRIPTS
For the most/least frequently found scripts, if there are more than one
most/least found words, only one of them will be printed. All else is as
requested.

DATASET
Together with the source code I include 2 datasets. One smaller and one bigger.
To use either one just add docfileBig.txt or docfileSmall.txt as the docfile
argument when running the jobExecutor.

Note:
If a worker is terminated the log file it made is replaced by that of the new
worker that was created in its stead.
