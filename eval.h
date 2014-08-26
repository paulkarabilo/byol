/*
 * eval.h
 *
 *  Created on: Aug 21, 2014
 *      Author: paul
 */

#ifndef _PLISP_EVAL_H_
#define _PLISP_EVAL_H_

#include "lib/mpc.h"

int count_nodes(mpc_ast_t *ast);
long eval(mpc_ast_t *ast);
#endif /* _PLISP_EVAL_H_ */
