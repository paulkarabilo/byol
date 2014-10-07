#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "lib/mpc.h"
#include "grammar.h"
#include "lval.h"
#include "eval.h"

#ifdef _WIN32

#include <string.h>
#define MAX_BUF 2048
static char buffer[MAX_BUF]
char *readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, MAX_BUF, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char *input);

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

static char is_running = 1;
static char *input;

void sig_handler(int signum) {
    is_running = 0;
    free(input);
    plisp_cleanup_grammar();
    exit(0);
}

int main(int argc, char **argv) {
    mpc_parser_t *plisp = plisp_set_grammar();
    puts("PLisp v.0.0.0.1."); //Pablo Lisp lol
    puts("Press Ctrl-C to exit.");

    signal(SIGINT, sig_handler);
    while (is_running) {
        mpc_result_t r;
        input = readline("plisp> ");
        add_history(input);
        if ((mpc_parse("<stdin>", input, plisp, &r))) {
            lval *val = lval_eval(lval_read(r.output));
            lval_println(val);
            lval_del(val);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }

    plisp_cleanup_grammar();
    return 0;
}
