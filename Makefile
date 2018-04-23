workers = 4

all:
	gcc -g -o recv receivemessages.c; gcc -g -o send sendmessages.c pipes.c ioManager.c -lm

fifo:
	rm -f /tmp/*

clean:
	rm -f recv send; rm -f /tmp/in* /tmp/out*

run:
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -g -o send sendmessages.c pipes.c ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@./send -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

valgrind:
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -g -o send sendmessages.c pipes.c ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --trace-children=yes ./send -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."

verbose:
	@echo "Compiling..."
	@gcc -g -o recv receivemessages.c; gcc -g -o send sendmessages.c pipes.c ioManager.c -lm
	@echo -----------------------------------------------------------------;
	@valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes --track-origins=yes ./send -w $(workers) -d docfile.txt;
	@echo -----------------------------------------------------------------;
	@echo "Cleaning up..."
	@rm -f recv send; rm -f /tmp/in* /tmp/out*;
	@echo "Pipes removed."
