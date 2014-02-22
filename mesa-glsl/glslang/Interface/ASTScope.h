/*
 * IRScope.h
 *
 *  Created on: 03.10.2013
 */

#ifndef __ASTSCOPE_H_
#define __ASTSCOPE_H_

#include "ShaderLang.h"
#include "glsl/list.h"
#include <list>

typedef std::list<int> scopeList;

class scope_item: public exec_node {
public:
	scope_item(int _id, const char* _n) : id(_id), name(_n) {}
	scope_item* clone(void * mem_ctx);

	int id;
	const char* name;
};

ShChangeableList* get_changeable_list(ast_node*);
ShChangeableList* get_changeable_paramerers_list(ast_node*);

void addScopeToScopeStack(DbgRsScope& stack, scopeList *s);
void setDbgScope(DbgRsScope& target, scopeList *s);

#endif /* __ASTSCOPE_H_ */
