/*
 * eval.c
 *
 *  Created on: Aug 22, 2014
 *      Author: paul
 */

#include "eval.h"

int count_nodes(mpc_ast_t *ast) {
    if (ast->children_num == 0) {
        return 1;
    }
    int total = 1;
    int i;
    for (i = 0; i < ast->children_num; i++) {
        total += count_nodes(ast->children[i]);
    }
    return total;

}

long eval_op(char *op, long x, long y) {
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    return 0;
}

long eval(mpc_ast_t *ast) {
    if (strstr(ast->tag, "number")) {
        return atoi(ast->contents);
    }
    if (strstr(ast->tag, "decimal")) {
        return atof(ast->contents);
    }
    char *op = ast->children[1]->contents;
    long x = eval(ast->children[2]);
    int i = 3;
    while (strstr(ast->children[i]->tag, "expr")) {
        x = eval_op(op, x, eval(ast->children[i]));
        i++;
    }
    return x;
}
