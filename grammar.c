/*
 * grammar.c
 *
 *  Created on: Aug 21, 2014
 *      Author: paul
 */

#include "grammar.h"
#include "lib/mpc.h"

static mpc_parser_t *Float;
static mpc_parser_t *Number;
static mpc_parser_t *Symbol;
static mpc_parser_t *Qexpr;
static mpc_parser_t *Sexpr;
static mpc_parser_t *Expr;
static mpc_parser_t *PLisp;

mpc_parser_t *plisp_set_grammar() {
    Float = mpc_new("float");
    Number = mpc_new("number");
    Symbol = mpc_new("symbol");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    PLisp = mpc_new("plisp");

    mpca_lang(MPCA_LANG_DEFAULT,
       "float  : /-?[0-9]+\\.[0-9]+/ ;	                           \
        number   : /-?[0-9]+/ ;	                                       \
        symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;   \
        sexpr    : '(' <expr>* ')';                                    \
        qexpr    : '{' <expr>* '}';                                    \
        expr     : <float> | <number> | <sexpr> | <qexpr> | <symbol>;            \
        plisp    : /^/ <expr>* /$/ ;                        ",
    Float, Number, Symbol, Sexpr, Qexpr, Expr, PLisp);

    return PLisp;
}

void plisp_cleanup_grammar() {
    mpc_cleanup(7, Float, Number, Symbol, Sexpr, Qexpr, Expr, PLisp);
}
