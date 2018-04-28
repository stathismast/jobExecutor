workers = 4

all:
	gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c;
	gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c -lm

fifo:
	rm -f /tmp/*

clean:
	rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*

run:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c -lm
	@echo -----------------------------------------------------------------;
	@./jobExecutor -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

valgrind:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --trace-children=yes ./jobExecutor -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

verbose:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c workersSrc/pipes.c workersSrc/signalHandler.c workersSrc/dirInfo.c workersSrc/trie.c workersSrc/searchInfo.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c jobExecSrc/signalHandler.c jobExecSrc/commands.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --track-origins=yes -v ./jobExecutor -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."
