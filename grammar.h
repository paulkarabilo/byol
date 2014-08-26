/*
 * grammar.h
 *
 *  Created on: Aug 21, 2014
 *      Author: paul
 */

#ifndef _PLISP_GRAMMAR_H_
#define _PLISP_GRAMMAR_H_
#include "lib/mpc.h"

mpc_parser_t *plisp_set_grammar();
void plisp_cleanup_grammar();


#endif /* _PLISP_GRAMMAR_H_ */
