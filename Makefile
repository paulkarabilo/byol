CFLAGS=-Wall -g
LFLAGS=-ledit -lm

all: 
	gcc $(CFLAGS) main.c eval.c lval.c grammar.c lib/mpc.c $(LFLAGS) -o build/plisp

clean:
	rm -rf build/*