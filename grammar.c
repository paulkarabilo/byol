/*
 * grammar.c
 *
 *  Created on: Aug 21, 2014
 *      Author: paul
 */

#include "grammar.h"
#include "lib/mpc.h"

static mpc_parser_t *Decimal;
static mpc_parser_t *Number;
static mpc_parser_t *Operator;
static mpc_parser_t *Expr;
static mpc_parser_t *PLisp;

mpc_parser_t *plisp_set_grammar() {
    Decimal = mpc_new("decimal");
    Number = mpc_new("number");
    Operator = mpc_new("operator");
    Expr = mpc_new("expr");
    PLisp = mpc_new("plisp");

    mpca_lang(MPCA_LANG_DEFAULT,
       "decimal  : /-?[0-9]+\\.[0-9]+/ ;	                               \
        number   : /-?[0-9]+/ ;	                                       \
        operator : '+'|'-'|'*'|'/'|\"mul\"|\"sub\"|\"sum\"|\"div\" ;   \
        expr     : <decimal> | <number> | '(' <operator> <expr>+ ')' ; \
        plisp    : /^/ <operator> <expr>+ /$/ ;                        ",
    Decimal, Number, Operator, Expr, PLisp);

    return PLisp;
}

void plisp_cleanup_grammar() {
    mpc_cleanup(5, Number, Decimal, Operator, Expr, PLisp);
}
