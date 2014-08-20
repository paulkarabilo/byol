-CFLAGS=-Wall -g

all: 
	gcc $(CFLAGS) main.c -o plisp

clean:
	rm -rf main