#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "lib/mpc.h"
#include "grammar.h"
#include "lval.h"
#include "eval.h"
#include "lenv.h"

#ifdef _WIN32

#include <string.h>
static char buffer[2048];
char *linenoise(char *prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char *cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(cpy) - 1 = '\0'];
	return cpy
}

void linenoiseHistoryAdd(char *unused) {}

#else
#include "lib/linenoise.h"
#endif

static char *input;
lenv *global;

int main(int argc, char **argv) {
    mpc_parser_t *plisp = plisp_set_grammar();
    puts("PLisp v.0.0.0.1."); //Pablo Lisp lol
    puts("Press Ctrl-C to exit.");
    global = new_lenv();
    lenv_add_builtins(global);
    while ((input = linenoise("mylsp> ")) != NULL) {
        if (input[0] != '\0') {
        	mpc_result_t r;
			linenoiseHistoryAdd(input);
			if ((mpc_parse("<stdin>", input, plisp, &r))) {
				lval *val = lval_eval(global, lval_read(r.output));
				lval_println(val);
				lval_del(val);
				mpc_ast_delete(r.output);
			} else {
				mpc_err_print(r.error);
				mpc_err_delete(r.error);
			}
        }
        free(input);
    }

    lenv_del(global);
    plisp_cleanup_grammar();
    return 0;
}
