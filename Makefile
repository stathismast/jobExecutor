workersSource = 4

all:
	gcc -g -o worker workersSrc/worker.c;
	gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c -lm

fifo:
	rm -f /tmp/*

clean:
	rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*

run:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@./jobExecutor -w $(workersSource) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

valgrind:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --trace-children=yes ./jobExecutor -w $(workersSource) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

verbose:
	@echo "Compiling..."
	@gcc -g -o worker workersSrc/worker.c;
	@gcc -g -o jobExecutor jobExecSrc/jobExecutor.c jobExecSrc/pipes.c jobExecSrc/ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes -v ./jobExecutor -w $(workersSource) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f worker jobExecutor; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."
