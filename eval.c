/*
 * eval.c
 *
 *  Created on: Aug 22, 2014
 *      Author: paul
 */

#include "lval.h"
#include "eval.h"

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

lval *builtin_head(lval *val) {
    LASSERT(val, (val->count == 1), "Function 'head' passed to many arguments!");
    LASSERT(val, (val->cell[0]->type == LVAL_QEXPR), "Function 'head' passed incorrect type!");
    LASSERT(val, (val->cell[0]->count != 0), "Function 'head' passed {}");

    lval *v = lval_take(val, 0);
    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval *builtin_tail(lval *val) {
    LASSERT(val, (val->count == 1), "Function 'tail' passed too many arguments");
    LASSERT(val, (val->cell[0]->type == LVAL_QEXPR), "Function 'head' passed incorrect type!");
    LASSERT(val, (val->cell[0]->count != 0), "Function 'head' passed {}");

    lval *v = lval_take(val, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval *builtin_list(lval *val) {
    val->type = LVAL_QEXPR;
    return val;
}

lval *builtin_eval(lval *val) {
    LASSERT(val, (val->count == 1), "Function 'eval' passed too many arguments");
    LASSERT(val, (val->cell[0]->type == LVAL_QEXPR), "Function 'eval' passed incorrect type");
    lval *x = lval_take(val, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}

lval *lval_join(lval *x, lval *y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }
    lval_del(y);
    return x;
}

lval *builtin_join(lval *val) {
    int i;
    for (i = 0; i < val->count; i++) {
        LASSERT(val, (val->cell[i]->type == LVAL_QEXPR), "Function 'join' passed incorrect type");
    }

    lval *x = lval_pop(val, 0);

    while (val->count) {
        x = lval_join(x, lval_pop(val, 0));
    }

    lval_del(val);
    return x;
}

lval *builtin_len(lval *val) {
    LASSERT(val, (val->count == 1), "Function 'len' passed incorrect number of arguments");
    LASSERT(val, (val->cell[0]->type == LVAL_QEXPR), "Function 'len' passed incorrect type");
    lval *x = new_lval_num(val->cell[0]->count);
    lval_del(val);
    return x;
}

lval *builtin_cons(lval *val) {
    LASSERT(val, (val->count == 2), "Function 'cons' passed incorrect numsber of arguments");
    LASSERT(val, (val->cell[1]->type == LVAL_QEXPR), "Function 'cons' passed incorrect type");
    lval *x = lval_pop(val, 0);
    lval *y = lval_take(val, 0);

    y->cell = realloc(y->cell, sizeof(lval *) * (y->count + 1));
    memmove(&y->cell[1], &y->cell[0], sizeof(lval *) * y->count);
    y->count++;
    y->cell[0] = x;
    return y;
}

lval *builtin_init(lval *val) {
    LASSERT(val, (val->count == 1), "Function 'init' passed incorrect number of arguments");
    LASSERT(val, (val->cell[0]->type == LVAL_QEXPR), "Function 'init' passed incorrect type");
    LASSERT(val, (val->cell[0]->count != 0), "Function 'init' passed {}");
    lval *x = lval_take(val, 0);
    lval_del(lval_pop(x, x->count - 1));
    return x;
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

lval *builtin(lval *val, char *fn) {
    if (strcmp("list", fn) == 0) { return builtin_list(val); }
    if (strcmp("eval", fn) == 0) { return builtin_eval(val); }
    if (strcmp("head", fn) == 0) { return builtin_head(val); }
    if (strcmp("tail", fn) == 0) { return builtin_tail(val); }
    if (strcmp("join", fn) == 0) { return builtin_join(val); }
    if (strcmp("len", fn) == 0) { return builtin_len(val); }
    if (strcmp("cons", fn) == 0) { return builtin_cons(val); }
    if (strcmp("init", fn) == 0) { return builtin_init(val); }
    if (strstr("*+-/", fn)) { return builtin_op(val, fn); }
    lval_del(val);
    return new_lval_err("Unknown function!");
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

    lval *result = builtin(val, f->sym);
    lval_del(f);
    return result;
}

lval *lval_eval(lval *val) {
    if (val->type == LVAL_SEXPR) {
        return lval_eval_sexpr(val);
    }
    return val;
}
