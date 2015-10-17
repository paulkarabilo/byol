CFLAGS=-Wall -g
LFLAGS=-lm

all: 
	gcc $(CFLAGS) main.c eval.c lenv.c lval.c  grammar.c lib/mpc.c lib/linenoise.c $(LFLAGS) -o build/plisp

clean:
	rm -rf build/*