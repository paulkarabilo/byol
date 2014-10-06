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

lval *lval_eval(lval *val);
lval *lval_eval_sexpr(lval *val);

#endif /* _PLISP_EVAL_H_ */
