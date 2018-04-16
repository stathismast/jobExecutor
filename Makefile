all:
	gcc -o recv receivemessages.c; gcc -o send sendmessages.c

clean:
	rm -f recv send
