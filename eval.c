/*
 * eval.c
 *
 *  Created on: Aug 22, 2014
 *      Author: paul
 */

#include "lval.h"
#include "eval.h"

lval *lval_pop(lval *val, int i) {
    lval *x = (lval *)val->cell[i];
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

lval *builtin_op(lval *val, char *op) {
    int i;
    for (i = 0; i < val->count; i++) {
        lval *cell = val->cell[i];
        if (cell->type != LVAL_NUM && cell->type != LVAL_FLOAT) {
            lval_del(val);
            return new_lval_err("Cannot operate on non-number");
        }
    }

    lval *x = lval_pop(val, 0);

    if (strcmp(op, "-") == 0 && val->count == 0) {
        x->num = -x->num;
    }

    while (val->count > 0) {
        lval *y = lval_pop(val, 0);
        if (strcmp(op, "+") == 0) {
            x->num += y->num;
        }
        if (strcmp(op, "-") == 0) {
            x->num -= y->num;
        }
        if (strcmp(op, "*") == 0) {
            x->num *= y->num;
        }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = new_lval_err("Division by zero");
                break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(val);
    return x;
}

lval *lval_eval_sexpr(lval *val) {
    int i;
    for (i = 0; i < val->count; i++) {
        val->cell[i] = lval_eval(val->cell[i]);
    }
    for (i = 0; i < val->count; i++) {
        lval *cell = val->cell[i];
        if (cell->type == LVAL_ERR) {
            return lval_take(val, i);
        }
    }
    if (val->count == 0) {
        return val;
    }

    if (val->count == 1) {
        return lval_take(val, 0);
    }

    lval *f = lval_pop(val, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f);
        lval_del(val);
        return new_lval_err("S-Expression does not start with a symbol");
    }

    lval *result = builtin_op(val, f->sym);
    lval_del(f);
    return result;
}

lval *lval_eval(lval *val) {
    if (val->type == LVAL_SEXPR) {
        return lval_eval_sexpr(val);
    }
    return val;
}
