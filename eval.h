/*
 * eval.h
 *
 *  Created on: Aug 21, 2014
 *      Author: paul
 */

#ifndef _PLISP_EVAL_H_
#define _PLISP_EVAL_H_

#include "lib/mpc.h"
#include "lval.h"
#include "lenv.h"

lval *lval_eval(lenv *e, lval *val);
lval *lval_eval_sexpr(lenv *e, lval *val);
lval *builtin_add(lenv *e, lval *val);
lval *builtin_sub(lenv *e, lval *val);
lval *builtin_mul(lenv *e, lval *val);
lval *builtin_div(lenv *e, lval *val);
lval *builtin_eq(lenv *e, lval *val);
lval *builtin_neq(lenv *e, lval *val);
lval *builtin_gt(lenv *e, lval *val);
lval *builtin_lt(lenv *e, lval *val);
lval *builtin_gte(lenv *e, lval *val);
lval *builtin_lte(lenv *e, lval *val);
lval *builtin_if(lenv *e, lval *val);
lval *builtin_tail(lenv *e, lval *val);
lval *builtin_head(lenv *e, lval *val);
lval *builtin_join(lenv *e, lval *val);
lval *builtin_list(lenv *e, lval *val);
lval *builtin_eval(lenv *e, lval *val);
lval *builtin_init(lenv *e, lval *val);
lval *builtin_len(lenv *e, lval *val);
lval *builtin_cons(lenv *e, lval *val);
lval *builtin_def(lenv *e, lval *val);
lval *builtin_put(lenv *e, lval *val);
lval *builtin_lambda(lenv *e, lval *val);
lval *builtin_dump(lenv *e, lval *val);
lval *builtin_exit(lenv *e, lval *val);

#endif /* _PLISP_EVAL_H_ */
