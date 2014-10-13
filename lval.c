/*
 * lval.c
 *
 *  Created on: Aug 26, 2014
 *      Author: paul
 */

#include "lval.h"
#include "lib/mpc.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

lval *new_lval_num(long num) {
    lval *lnum = malloc(sizeof(lval));
    lnum->type = LVAL_NUM;
    lnum->num = num;
    return lnum;
}

lval *new_lval_float(double num) {
    lval *lnum = malloc(sizeof(lval));
    lnum->type = LVAL_FLOAT;
    lnum->fnum = num;
    return lnum;
}

lval *new_lval_err(char *fmt, ...) {
    lval *lerr = malloc(sizeof(lval));
    lerr->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);


    lerr->err = malloc(1024);
    vsnprintf(lerr->err, 1023, fmt, va);
    lerr->err = realloc(lerr->err, strlen(lerr->err) + 1);
    va_end(va);
    return lerr;
}

lval *new_lval_sym(char *sym) {
    lval *lsym = malloc(sizeof(lval));
    lsym->type = LVAL_SYM;
    lsym->sym = malloc(strlen(sym) + 1);
    strcpy(lsym->sym, sym);
    return lsym;
}

lval *new_lval_fn(lbuiltin fn, char *name) {
    lval *lfn = malloc(sizeof(lval));
    lfn->type = LVAL_FN;
    lfn->builtin = fn;
    lfn->sym = malloc(strlen(name) + 1);
    strcpy(lfn->sym, name);
    return lfn;
}

lval *new_lval_lambda(lval *args, lval *body, char *name) {
    lval *lfn = malloc(sizeof(lval));
    lfn->type = LVAL_FN;
    lfn->builtin = NULL;
    lfn->fnenv = new_lenv();
    lfn->fnargs = args;
    lfn->fnbody = body;
    lfn->sym = malloc(strlen(name) + 1);
    strcpy(lfn->sym, name);
    return lfn;
}

lval *new_lval_qexpr() {
    lval *lqexpr = malloc(sizeof(lval));
    lqexpr->type = LVAL_QEXPR;
    lqexpr->count = 0;
    lqexpr->cell = NULL;
    return lqexpr;
}

lval *new_lval_sexpr() {
    lval *lsexpr = malloc(sizeof(lval));
    lsexpr->type = LVAL_SEXPR;
    lsexpr->count = 0;
    lsexpr->cell = NULL;
    return lsexpr;
}

lval *lval_pop(lval *val, int i) {
    lval *x = val->cell[i];
    memmove(&val->cell[i], &val->cell[i+1], sizeof(lval *) * (val->count - i - 1));
    val->count--;
    val->cell = realloc(val->cell, sizeof(lval *) * val->count);
    return x;
}

lval *lval_take(lval *val, int i) {
    lval *res = lval_pop(val, i);
    lval_del(val);
    return res;
}

void lval_del(lval *v) {
    int i;
    switch (v->type) {
        case LVAL_SYM:
            free(v->sym);
            break;
        case LVAL_FN:
            free(v->sym);
            if (!v->builtin) {
                lenv_del(v->fnenv);
                lval_del(v->fnargs);
                lval_del(v->fnbody);
            }
            break;
        case LVAL_ERR:
            free(v->err);
            break;
        case LVAL_QEXPR:
        case LVAL_SEXPR:
            for (i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
        default:
            break;
    }
    free(v);
}

lval *lval_add(lval *v, lval *x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval *) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval *lval_copy(lval *src) {
    lval *dest = malloc(sizeof(lval));
    dest->type = src->type;

    switch (src->type) {
        case LVAL_FN:
            dest->sym = malloc(strlen(src->sym) + 1);
            strcpy(dest->sym, src->sym);
            if (src->builtin) {
                dest->builtin = src->builtin;
            } else {
                dest->builtin = NULL;
                dest->fnenv = lenv_copy(src->fnenv);
                dest->fnargs = lval_copy(src->fnargs);
                dest->fnbody = lval_copy(src->fnbody);
            }
            break;
        case LVAL_NUM:
            dest->num = src->num;
            break;
        case LVAL_FLOAT:
            dest->fnum = src->fnum;
            break;

        case LVAL_ERR:
            dest->err = malloc(strlen(src->err) + 1);
            strcpy(dest->err, src->err);
            break;
        case LVAL_SYM:
            dest->sym = malloc(strlen(src->sym) + 1);
            strcpy(dest->sym, src->sym);
            break;

        case LVAL_SEXPR:
        case LVAL_QEXPR:
            dest->count = src->count;
            dest->cell = malloc(sizeof(lval *) * dest->count);
            int i;
            for (i = 0; i < dest->count; i++) {
                dest->cell[i] = lval_copy(src->cell[i]);
            }
            break;
    }

    return dest;
}

int lval_eq(lval *x, lval *y) {
    if (x->type != x->type) {
        return 0;
    } else {
        switch (x->type) {
            case LVAL_NUM:
                return x->num == y->num;
                break;
            case LVAL_FLOAT:
                return x->fnum == y->fnum;
                break;
            case LVAL_ERR:
                return strcmp(x->err, y->err) == 0;
                break;
            case LVAL_SYM:
                return strcmp(x->sym, y->sym) == 0;
                break;
            case LVAL_FN:
                if (x->builtin || y->builtin) {
                    return x->builtin == y->builtin;
                } else {
                    return lval_eq(x->fnargs, y->fnargs) &&
                            lval_eq(x->fnbody, y->fnbody);
                }
                break;
            case LVAL_QEXPR:
            case LVAL_SEXPR:
                if (x->count != y->count) {
                    return 0;
                }
                int i;
                for (i = 0; i < x->count; i++) {
                    if (!lval_eq(x->cell[i], y->cell[i])) {
                        return 0;
                    }
                }
                return 1;
                break;
        }
    }
}

lval *lval_read_num(mpc_ast_t *t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? new_lval_num(x) : new_lval_err("Invalid number");
}

lval *lval_read_float(mpc_ast_t *t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE ? new_lval_float(x) : new_lval_err("Invalid number");
}

lval *lval_read(mpc_ast_t *t) {

    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }
    if (strstr(t->tag, "float")) {
        return lval_read_float(t);
    }

    if (strstr(t->tag, "symbol")) {
        return new_lval_sym(t->contents);
    }

    lval *x = NULL;

    if (strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr")) {
        x = new_lval_sexpr();
    }

    if (strstr(t->tag, "qexpr")) {
        x = new_lval_qexpr();
    }
    int i;
    for (i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

char *ltype_name(lval_type type) {
    switch (type) {
        case LVAL_NUM: return "Integer";
        case LVAL_FLOAT: return "Float";
        case LVAL_ERR: return "Error";
        case LVAL_FN: return "Function";
        case LVAL_QEXPR: return "Q-Expression";
        case LVAL_SEXPR: return "S-Expression";
        case LVAL_SYM: return "Symbol";
        default: return "Unknown type";
    }
}

void lval_expr_print(lval *val, char open, char close) {
    putchar(open);
    int i;
    for (i = 0; i < val->count; i++) {
        lval_print(val->cell[i]);

        if (i != (val->count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

void lval_print(lval *val) {
    switch (val->type) {
        case LVAL_NUM:
            printf("%li", val->num);
            break;
        case LVAL_FLOAT:
            printf("%f", val->fnum);
            break;
        case LVAL_ERR:
            printf("ERROR: %s", val->err);
            break;
        case LVAL_SYM:
            printf("%s", val->sym);
            break;
        case LVAL_FN:
            if (val->builtin) {
                printf("<function %s>", val->sym);
            } else {
                printf("(\\ ");
                lval_print(val->fnargs);
                putchar(' ');
                lval_print(val->fnbody);
                putchar(')');
            }
            break;
        case LVAL_SEXPR:
            lval_expr_print(val, '(', ')');
            break;
        case LVAL_QEXPR:
            lval_expr_print(val, '{', '}');
            break;
    }
}

void lval_println(lval *val) {
    lval_print(val);
    putchar('\n');
}
