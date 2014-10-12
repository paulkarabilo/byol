/*
 * lenv.c
 *
 *  Created on: Oct 8, 2014
 *      Author: paul
 */



#include "lval.h"
#include "eval.h"
#include "lenv.h"

lenv *new_lenv() {
    lenv *e = malloc(sizeof(lenv));
    e->parent = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void lenv_del(lenv *e) {
    int i;
    for (i = 0; i < e->count; i++) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    if (e->syms != NULL) {
        free(e->syms);
    }
    if (e->vals != NULL) {
        free(e->vals);
    }
    free(e);
}

lval *lenv_get(lenv *e, lval *k) {
    int i;
    for (i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k->sym) == 0) {
            return lval_copy(e->vals[i]);
        }
    }
    if (e->parent) {
        return lenv_get(e->parent, k);
    } else {
        return new_lval_err("symbol %s not found!", k->sym);
    }
}

lenv *lenv_copy(lenv *e) {
    lenv *n = malloc(sizeof(lenv));
    n->parent = e->parent;
    n->count = e->count;
    n->syms = malloc(sizeof(char *) * n->count);
    n->vals = malloc(sizeof(lval *) * n->count);
    int i;
    for (i = 0; i < n->count; i++) {
        n->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = lval_copy(e->vals[i]);
    }
    return n;
}

void lenv_def(lenv *e, lval *k, lval *v) {
    while (e->parent) {
        e = e->parent;
    }
    lenv_put(e, k, v);
}

void lenv_put_by_key(lenv *e, char *k, lval *v) {
    int i;
    for (i = 0; i < e->count; i++) {
        if (strcmp(e->syms[i], k) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }
    e->count++;
    e->vals = realloc(e->vals, sizeof(lval *) * e->count);
    e->syms = realloc(e->syms, sizeof(lval *) * e->count);

    e->vals[e->count - 1] = lval_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k) + 1);
    strcpy(e->syms[e->count - 1], k);
}

void lenv_put(lenv *e, lval *k, lval *v) {
    lenv_put_by_key(e, k->sym, v);
}

void lenv_add_builtin(lenv *e, char *name, lbuiltin fn) {
    lval *v = new_lval_fn(fn, name);
    lenv_put_by_key(e, name, v);
    lval_del(v);
}

void lenv_add_builtins(lenv *e) {
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail",  builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join",  builtin_join);
    lenv_add_builtin(e, "init", builtin_init);
    lenv_add_builtin(e, "cons", builtin_cons);
    lenv_add_builtin(e, "len", builtin_len);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "=", builtin_put);
    lenv_add_builtin(e, "dump", builtin_dump);
    lenv_add_builtin(e, "exit", builtin_exit);

    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
}
