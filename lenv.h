/*
 * lenv.h
 *
 *  Created on: Oct 8, 2014
 *      Author: paul
 */

#ifndef _PLISP_LENV_H_
#define _PLISP_LENV_H_

struct lenv;
typedef struct lenv lenv;
#include "lval.h"

struct lenv {
    int count;
    char **syms;
    lval **vals;
    lenv *parent;
};

void lenv_add_builtins(lenv *e);
lenv *new_lenv();
void lenv_del(lenv *e);

lenv *lenv_copy(lenv *e);
void lenv_def(lenv *e, lval *k, lval *v);

int lenv_is_running(lenv *e);
lval *lenv_get(lenv *e, lval *k);
void lenv_put(lenv *e, lval *k, lval *v);
#endif /* _PLISP_LENV_H_ */
