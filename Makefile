workers = 4

all:
	gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm

fifo:
	rm -f /tmp/*

clean:
	rm -f recv send; rm -f /tmp/in* /tmp/out*

run:
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm;
	@echo -----------------------------------------------------------------;
	@./send $(workers);
	@echo -----------------------------------------------------------------;
	@ls -lF /tmp | grep inPipe*;
	@ls -lF /tmp | grep outPipe*;
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

valgrind:
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm;
	@echo -----------------------------------------------------------------;
	@valgrind ./send $(workers);
	@echo -----------------------------------------------------------------;
	@ls -lF /tmp | grep inPipe*;
	@ls -lF /tmp | grep outPipe*;
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."
