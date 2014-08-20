-CFLAGS=-Wall -g

all: 
	gcc $(CFLAGS) main.c -o build/plisp

clean:
	rm -rf build/*