all:
	gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm

fifo:
	rm -f /tmp/*

clean:
	rm -f recv send; rm -f /tmp/in* /tmp/out*

run:
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm;
	@echo -----------------------------------------------------------------;
	@./send;
	@echo -----------------------------------------------------------------;
	@ls -lF /tmp | grep inPipe*;
	@ls -lF /tmp | grep outPipe*;

valgrind:
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -o send sendmessages.c -lm;
	@echo -----------------------------------------------------------------;
	@valgrind ./send;
	@echo -----------------------------------------------------------------;
	@ls -lF /tmp | grep inPipe*;
	@ls -lF /tmp | grep outPipe*;
