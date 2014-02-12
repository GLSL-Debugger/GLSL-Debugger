/*
 * IRScope.h
 *
 *  Created on: 03.10.2013
 */

#ifndef __ASTSCOPE_H_
#define __ASTSCOPE_H_

#include "ShaderLang.h"
#include <list>

typedef std::list<int> scopeList;

scopeList* get_scope( ast_node* );
void set_scope( ast_node*, scopeList* );
ShChangeableList* get_changeable_list( ast_node* );
ShChangeableList* get_changeable_paramerers_list( ast_node* );

void addScopeToScopeStack(DbgRsScope& stack, scopeList *s);
void setDbgScope(DbgRsScope& target, scopeList *s);


#endif /* __ASTSCOPE_H_ */
