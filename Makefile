all:
	gcc -o recv receivemessages.c; gcc -o send sendmessages.c

fifo:
	rm -f /tmp/myfifo*

clean:
	rm -f recv send
