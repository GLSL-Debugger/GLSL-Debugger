/*
 * AstScope.h
 *
 *  Created on: 03.10.2013
 */

#ifndef AST_SCOPE_H_
#define AST_SCOPE_H_

#include "ShaderLang.h"
#include "mesa/glsl/list.h"


class scope_item: public exec_node {
public:
	scope_item(int _id, const char* _n) : id(_id), name(_n) {}
	scope_item* clone(void * mem_ctx);

	int id;
	const char* name;
};

class changeable_item: public exec_node {
public:
	changeable_item(ShChangeable* _c) : id(_c->id), changeable(_c) {}
	changeable_item* clone(void * mem_ctx);

	int id;
	ShChangeable* changeable;
};


void addScopeToScopeStack(DbgRsScope& stack, exec_list *s, void* mem_ctx);
void setDbgScope(DbgRsScope& target, exec_list *s, void* mem_ctx);

#endif /* AST_SCOPE_H_ */
