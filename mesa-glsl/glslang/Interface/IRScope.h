/*
 * IRScope.h
 *
 *  Created on: 03.10.2013
 */

#ifndef __IRSCOPE_H_
#define __IRSCOPE_H_

#include "ShaderLang.h"
#include <list>

typedef std::list<int> scopeList;

class ir_instruction;

scopeList* get_scope( ir_instruction* );
void set_scope( ir_instruction*, scopeList* );
// TODO: actually changeable_list for exec_list* and parameters_list are doubles each other
ShChangeableList* get_changeable_list( exec_list* );
ShChangeableList* get_changeable_list( ir_instruction* );
ShChangeableList* get_changeable_paramerers_list( ir_instruction* );

void addScopeToScopeStack(DbgRsScope& stack, scopeList *s);
void setDbgScope(DbgRsScope& target, scopeList *s);


#endif /* __IRSCOPE_H_ */
