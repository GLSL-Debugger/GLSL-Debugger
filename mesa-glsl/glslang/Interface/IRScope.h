/*
 * IRScope.h
 *
 *  Created on: 03.10.2013
 */

#ifndef __IRSCOPE_H_
#define __IRSCOPE_H_

#include "intermediate.h"

class ir_instruction;

scopeList* get_scope( ir_instruction* );
void set_scope( ir_instruction*, scopeList* );
ShChangeableList* get_changeable_list( ir_instruction* );
ShChangeableList* get_changeable_paramerers_list( ir_instruction* );


#endif /* __IRSCOPE_H_ */
