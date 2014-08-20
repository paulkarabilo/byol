CFLAGS=-Wall -g
LFLAGS=-ledit

all: 
	gcc $(CFLAGS) main.c $(LFLAGS) -o build/plisp

clean:
	rm -rf build/*