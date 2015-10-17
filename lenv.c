#include <limits.h>
#include "lval.h"
#include "eval.h"
#include "lenv.h"

int hash(lenv* e, char* key) {
	unsigned long int val = 0;
	int i = 0;
	while (val < ULONG_MAX && i < strlen(key)) {
		val = val << 8;
		val += key[i];
		i++;
	}
	return val % e->size;
}

lenv *new_lenv(int size) {
	int i;
    lenv *e = malloc(sizeof(lenv));
    e->table = malloc(size * sizeof(lenv_entry));
    for (i = 0; i < size; i++) {
    	e->table[i] = 0;
    }
    e->parent = NULL;
    e->size = size;
    return e;
}

lenv_entry* new_entry (char* key, lval* val) {
	lenv_entry* entry = malloc(sizeof(lenv_entry));
	entry->key = strdup(key);
	entry->val = lval_copy(val);
	entry->next = NULL;
	return entry;
}

lenv_entry* lenv_entry_copy(lenv_entry* src) {
	lenv_entry* dest = new_entry(src->key, src->val);
	if (src->next) {
		dest->next = lenv_entry_copy(src->next);
	}
	return dest;
}

void lenv_entry_del (lenv_entry* entry) {
	if (entry != NULL) {
		free(entry->key);
		lval_del(entry->val);
		lenv_entry* next = entry->next;
		free(entry);
		if (next != NULL) {
			lenv_entry_del(next);
		}
	}
}

void lenv_del(lenv *e) {
    int i;
    for (i = 0; i < e->size; i++) {
    	if (e->table[i] != NULL) {
    		lenv_entry_del(e->table[i]);
    	}
    }
    if (e->table != NULL) {
        free(e->table);
    }
    free(e);
}

lval *lenv_get(lenv *e, lval *k) {
    lenv_entry* entry = e->table[hash(e, k->sym)];
    while (entry != NULL) {
    	if (entry->key != NULL && strcmp(k->sym, entry->key) == 0) {
    		return lval_copy(entry->val);
    	}
    	entry = entry->next;
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
    n->size = e->size;
    n->table = malloc(sizeof(lenv_entry) * n->size);
    int i;
    for (i = 0; i < n->size; i++) {
    	n->table[i] = lenv_entry_copy(e->table[i]);
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
    int bin = hash(e, k);
    lenv_entry* next = e->table[bin];
    short found = 0;

    while (next != NULL) {
    	if (next->key != NULL && strcmp(k, next->key) == 0) {
    		lval_del(next->val);
    		next->val = lval_copy(v);
    		found = 1;
    		break;
    	}
    	next = next->next;
    }

    if (found == 0) {
    	lenv_entry* entry = new_entry(k, v);
    	lenv_entry* first = e->table[bin];
		e->table[bin] = entry;
		if (first != NULL) {
			entry->next = first;
		}

    }
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
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_neq);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, ">=", builtin_gte);
    lenv_add_builtin(e, "<=", builtin_lte);
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "=", builtin_put);
    lenv_add_builtin(e, "dump", builtin_dump);
    lenv_add_builtin(e, "exit", builtin_exit);

    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
}
