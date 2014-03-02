/*
 * AstScope.cpp
 *
 *  Created on: 03.10.2013
 */

#include "AstScope.h"
#include "glsldb/utils/dbgprint.h"

extern ShChangeable * copyShChangeableCtx(ShChangeable *c, void* mem_ctx);


scope_item* scope_item::clone(void *mem_ctx)
{
	return new(mem_ctx) scope_item(id, name);
}

changeable_item* changeable_item::clone(void * mem_ctx)
{
	ShChangeable* ch_copy = copyShChangeableCtx(changeable, mem_ctx);
	return new(mem_ctx) changeable_item(ch_copy);
}

static bool idInStack(DbgRsScope& stack, int id)
{
	for (int i = 0; i < stack.numIds; i++)
		if (id == stack.ids[i])
			return true;
	return false;
}

static void addToScope(DbgRsScope& scope, scope_item* item, void* mem_ctx)
{
	if (idInStack(scope, item->id))
	    return;

    scope.numIds++;
    scope.ids = (int*) reralloc_array_size(mem_ctx, scope.ids, sizeof(int), scope.numIds);
    scope.ids[scope.numIds-1] = item->id;
}

void addScopeToScopeStack(DbgRsScope& stack, exec_list *scope, void* mem_ctx)
{
    if (!scope)
        return;
    foreach_list(node, scope)
    	addToScope(stack, (scope_item*) node, mem_ctx);
}

void setDbgScope(DbgRsScope& scope, exec_list *s, void* mem_ctx)
{
	assert(s || !"no scopeList");
	VPRINT(3, "SET GLOBAL SCOPE LIST: ");

	foreach_list(node, s) {
		scope_item* item = (scope_item*) node;
		addToScope(scope, item, mem_ctx);
        VPRINT(3, "<%i,%s> ", item->id, item->name);
	}

    VPRINT(3, "\n");
}
