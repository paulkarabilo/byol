/*
 * lval.h
 *
 *  Created on: Aug 26, 2014
 *      Author: paul
 */

#ifndef _PLVAL_H_

#define _PLVAL_H_

#include <stdarg.h>
#include "lib/mpc.h"


typedef struct lval lval;
#include "lenv.h"

typedef enum {LVAL_NUM, LVAL_FLOAT, LVAL_ERR, LVAL_SYM, LVAL_FN, LVAL_QEXPR, LVAL_SEXPR} lval_type;
typedef lval *(*lbuiltin)(lenv *, lval *);

struct lval {
    lval_type type;
    long num;
    double fnum;
    char *err;
    char *sym;
    lbuiltin builtin;
    lenv *fnenv;
    lval *fnargs;
    lval *fnbody;
    int count;
    struct lval** cell;
};


lval *new_lval_num(long num);
lval *new_lval_float(double num);
lval *new_lval_err(char *fmt, ...);
lval *new_lval_sym(char *sym);
lval *new_lval_fn(lbuiltin fn, char *name);
lval *new_lval_lambda(lval *args, lval *body, char *name);
lval *new_lval_sexpr();
lval *new_lval_qexpr();

lval *lval_pop(lval *v, int i);
lval *lval_take(lval *v, int i);
lval *lval_read(mpc_ast_t *t);
lval *lval_copy(lval *src);
void lval_del(lval *val);
lval *lval_add(lval *v,lval *x);

void lval_printf(lval *val);
void lval_print(lval *val);
void lval_println(lval *val);

char *ltype_name(lval_type type);

#define NUM2FLOAT(v) \
    if (v->type == LVAL_NUM) { \
        v->type = LVAL_FLOAT; \
        v->fnum = v->num; \
        v->num = 0; \
    }

#define FLOAT2NUM(v) \
    if (v->type == LVAL_FLOAT) { \
        v->type = LVAL_NUM; \
        v->num = (int)v->fnum; \
        v->fnum = 0; \
    }

#define LASSERT(args, cond, fmt, ...) if (!(cond)) { lval_del(args); return new_lval_err(fmt, ##__VA_ARGS__); }
#define LASSERT_TYPE(v, t1, t2, fmt, ...) \
        if (!(t1 == t2)) {\
            lval_del(v); \
            return new_lval_err(fmt " Expected %s, got %s", ##__VA_ARGS__, ltype_name((t1)), ltype_name(t2)); \
        }

#define LASSERT_FN_TYPE(v, t1, t2, name) \
    LASSERT_TYPE(v, t1, t2, "Function '" name "' passed incorrect type.")

#define LASSERT_FN_NARGS(v, n, e, name) \
    LASSERT(v, (n == e), "Function '" name "' expected %i arguments, got %i", n, e)

#define LASSERT_FN_NEMPTY(v, l, name) \
    LASSERT(v, (l->count != 0), "Function '" name "' passed {}")
#endif /* _PLVAL_H_ */
