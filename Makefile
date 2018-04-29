workers = 4

all:
	gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c workersSrc/commands.c;
	gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c

fifo:
	rm -f /tmp/*

clean:
	rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*

run:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c workersSrc/commands.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c
	@echo -----------------------------------------------------------------;
	@./jobExecutor -w $(workers) -d docfileSmall.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

valgrind:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c workersSrc/commands.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --trace-children=yes ./jobExecutor -w $(workers) -d docfileSmall.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

verbose:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c workersSrc/commands.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --track-origins=yes -v ./jobExecutor -w $(workers) -d docfileSmall.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."
