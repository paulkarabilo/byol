/*
 * eval.c
 *
 *  Created on: Aug 22, 2014
 *      Author: paul
 */

#include "lval.h"
#include "eval.h"
#include "lenv.h"


lval *builtin_head(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 1, "head");
    LASSERT_FN_TYPE(val, val->cell[0]->type, LVAL_QEXPR, "head");
    LASSERT_FN_NEMPTY(val, val->cell[0], "head");

    lval *v = lval_take(val, 0);
    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }
    return v;
}

lval *builtin_tail(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 1, "tail");
    LASSERT_FN_TYPE(val, val->cell[0]->type, LVAL_QEXPR, "head");
    LASSERT_FN_NEMPTY(val, val->cell[0], "head");

    lval *v = lval_take(val, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval *builtin_list(lenv *e, lval *val) {
    val->type = LVAL_QEXPR;
    return val;
}

lval *builtin_eval(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 1, "eval");
    LASSERT_FN_TYPE(val, val->cell[0]->type, LVAL_QEXPR, "eval");
    lval *x = lval_take(val, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval *lval_join(lval *x, lval *y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }
    lval_del(y);
    return x;
}

lval *builtin_join(lenv *e, lval *val) {
    int i;
    for (i = 0; i < val->count; i++) {
        LASSERT_FN_TYPE(val, val->cell[i]->type, LVAL_QEXPR, "join");
    }

    lval *x = lval_pop(val, 0);

    while (val->count) {
        x = lval_join(x, lval_pop(val, 0));
    }

    lval_del(val);
    return x;
}

lval *builtin_len(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 1, "len");
    LASSERT_FN_TYPE(val, val->cell[0]->type, LVAL_QEXPR, "len");
    lval *x = new_lval_num(val->cell[0]->count);
    lval_del(val);
    return x;
}

lval *builtin_cons(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 2, "cons");
    LASSERT_FN_TYPE(val, val->cell[1]->type, LVAL_QEXPR, "cons");
    lval *x = lval_pop(val, 0);
    lval *y = lval_take(val, 0);

    y->cell = realloc(y->cell, sizeof(lval *) * (y->count + 1));
    memmove(&y->cell[1], &y->cell[0], sizeof(lval *) * y->count);
    y->count++;
    y->cell[0] = x;
    return y;
}

lval *builtin_init(lenv *e, lval *val) {
    LASSERT_FN_NARGS(val, val->count, 1, "init");
    LASSERT_FN_TYPE(val, val->cell[0]->type, LVAL_QEXPR, "init");
    LASSERT_FN_NEMPTY(val, val->cell[0], "init");
    lval *x = lval_take(val, 0);
    lval_del(lval_pop(x, x->count - 1));
    return x;
}

lval *builtin_dump(lenv *e, lval *val) {
    printf("Plisp global mem dump: \n");
    int i;
    for (i = 0; i < e->count; i++) {
        printf("<@%p> %s [%i bytes]: ",
                e->vals[i],
                e->syms[i],
                sizeof(e->vals[i]));
        lval_println(e->vals[i]);
    }
    lval_del(val);
    return new_lval_sexpr();
}

lval *builtin_exit(lenv *e, lval *val) {
    lval_del(val);
    exit(0);
    return new_lval_sexpr();
}

//TODO: refactor these two basterds
lval *builtin_op_float(lval *val, char *op) {
    lval *x = lval_pop(val, 0);

    NUM2FLOAT(x);

    if (strcmp(op, "-") == 0 && val->count == 0) {
        x->fnum = -x->fnum;
    }

    while (val->count > 0) {
        lval *y = lval_pop(val, 0);
        NUM2FLOAT(y);
        if (strcmp(op, "+") == 0) {
            x->fnum += y->fnum;
        }
        if (strcmp(op, "-") == 0) {
            x->fnum -= y->fnum;
        }
        if (strcmp(op, "*") == 0) {
            x->fnum *= y->fnum;
        }
        if (strcmp(op, "/") == 0) {
            if (y->fnum == 0) {
                lval_del(x);
                lval_del(y);
                x = new_lval_err("Division by zero");
                break;
            }
            x->fnum /= y->fnum;
        }

        lval_del(y);
    }

    lval_del(val);
    return x;
}

lval *builtin_op(lval *val, char *op) {
    int i;
    int is_float = 0;
    for (i = 0; i < val->count; i++) {
        lval *cell = val->cell[i];
        if (cell->type != LVAL_NUM && cell->type != LVAL_FLOAT) {
            lval_del(val);
            return new_lval_err("Cannot operate on non-number");
        }
        if (cell->type == LVAL_FLOAT) {
            is_float = 1;
        }
    }

    if (is_float) {
        return builtin_op_float(val, op);
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

lval *builtin_add(lenv *e, lval *v) {
    return builtin_op(v, "+");
}

lval *builtin_sub(lenv *e, lval *v) {
    return builtin_op(v, "-");
}

lval *builtin_mul(lenv *e, lval *v) {
    return builtin_op(v, "*");
}

lval *builtin_div(lenv *e, lval *v) {
    return builtin_op(v, "/");
}

lval *builtin_lambda(lenv *e, lval *v) {
    LASSERT_FN_NARGS(v, 2, v->count, "\\");
    LASSERT_FN_TYPE(v, v->cell[0]->type, LVAL_QEXPR, "\\");
    LASSERT_FN_TYPE(v, v->cell[1]->type, LVAL_QEXPR, "\\");

    int i;
    lval *c0 = v->cell[0];
    for (i = 0; i < c0->count; i++) {
        LASSERT_TYPE(v, c0->cell[i]->type, LVAL_SYM, "Cannot define non-symbol.");
    }

    lval *args = lval_pop(v, 0);
    lval *body = lval_pop(v, 0);

    return new_lval_lambda(args, body, "test");
}

lval *builtin_var(lenv *e, lval *v, char *fn) {
    LASSERT_FN_TYPE(v, v->cell[0]->type, LVAL_QEXPR, "var");

    lval *syms = v->cell[0];
    int i;
    for (i = 0; i < syms->count; i++) {
        LASSERT_TYPE(v, syms->cell[i]->type, LVAL_SYM,
                "Function %s cannot define non-symbol", fn);
    }
    LASSERT(v, (syms->count == v->count - 1),
            "Function %s got wrong number of arguments. Expected %i, got %i",
            fn, syms->count, v->count - 1);

    for (i = 0; i < syms->count; i++) {
        if (strcmp(fn, "def") == 0) {
            lenv_def(e, syms->cell[i], v->cell[i + 1]);
        }
        if (strcmp(fn, "=") == 0) {
            lenv_put(e, syms->cell[i], v->cell[i + 1]);
        }
    }

    lval_del(v);
    return new_lval_sexpr();
}

lval *builtin_def(lenv *e, lval *v) {
    return builtin_var(e, v, "def");
}

lval *builtin_put(lenv *e, lval *v) {
    return builtin_var(e, v, "=");
}

lval *lval_call(lenv *e, lval *f, lval *v) {
    if (f->builtin) {
        return f->builtin(e, v);
    }

    int given = v->count;
    int total = f->fnargs->count;

    while (v->count) {
        LASSERT(v, (f->fnargs->count != 0),
                "Function passed too many arguments. Got %i, expected %i",
                given, total);

        lval *sym = lval_pop(f->fnargs, 0);
        lval *val = lval_pop(v, 0);
        lenv_put(f->fnenv, sym, val);
        lval_del(sym);
        lval_del(val);
    }

    lval_del(v);

    if (f->fnargs->count == 0) {
        f->fnenv->parent = e;
        return builtin_eval(f->fnenv,
                lval_add(new_lval_sexpr(), lval_copy(f->fnbody)));
    } else {
        return lval_copy(f);
    }
}

lval *lval_eval_sexpr(lenv *e, lval *val) {
    int i;
    for (i = 0; i < val->count; i++) {
        val->cell[i] = lval_eval(e, val->cell[i]);
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
    if (f->type != LVAL_FN) {
        lval_del(f);
        lval_del(val);
        return new_lval_err("first element is not a function!");
    }

    lval *result = lval_call(e, f, val);
    lval_del(f);
    return result;
}

lval *lval_eval(lenv *e, lval *val) {
    if (val->type == LVAL_SYM) {
        lval *x = lenv_get(e, val);
        lval_del(val);
        return x;
    }
    if (val->type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, val);
    }
    return val;
}
