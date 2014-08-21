#include <stdio.h>
#include <stdlib.h>
#include "lib/mpc.h"
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

int main(int argc, char **argv) {
    mpc_parser_t *Decimal = mpc_new("decimal");
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *PLisp = mpc_new("plisp");

    mpca_lang(MPCA_LANG_DEFAULT,
       "decimal  : /-?[0-9]+\\.[0-9]+/ ;	                               \
        number   : /-?[0-9]+/ ;	                                       \
        operator : '+'|'-'|'*'|'/'|\"mul\"|\"sub\"|\"sum\"|\"div\" ;   \
        expr     : <decimal> | <number> | '(' <operator> <expr>+ ')' ; \
        plisp    : /^/ <operator> <expr>+ /$/ ;                        ",
    Decimal, Number, Operator, Expr, PLisp);
    puts("PLisp v.0.0.0.1."); //Pablo Lisp lol
    puts("Press Ctrl-C to exit.");

    while (1) {
        mpc_result_t r;
        char *input = readline("plisp> ");
        add_history(input);
        if ((mpc_parse("<stdin>", input, PLisp, &r))) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }

    mpc_cleanup(5, Number, Decimal, Operator, Expr, PLisp);
    return 0;
}
