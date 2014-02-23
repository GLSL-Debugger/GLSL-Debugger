/*
 * AstScope.cpp
 *
 *  Created on: 03.10.2013
 */

#include "AstScope.h"
#include "glsldb/utils/dbgprint.h"

extern ShChangeable * copyShChangeable(ShChangeable *c);


scope_item* scope_item::clone(void *mem_ctx)
{
	return new(mem_ctx) scope_item(id, name);
}

changeable_item* changeable_item::clone(void * mem_ctx)
{
	ShChangeable* ch_copy = copyShChangeable(changeable);
	return new(mem_ctx) changeable_item(ch_copy);
}

static bool idInStack(DbgRsScope& stack, int id)
{
	for (int i = 0; i < stack.numIds; i++)
		if (id == stack.ids[i])
			return true;
	return false;
}

void addScopeToScopeStack(DbgRsScope& stack, exec_list *scope)
{
    if (!scope)
        return;

    foreach_list(node, scope){
    	scope_item* item = (scope_item*)node;
    	if (idInStack(stack, item->id))
    		continue;

		stack.numIds++;
		stack.ids = (int*) realloc(stack.ids, stack.numIds*sizeof(int));
		stack.ids[stack.numIds-1] = item->id;
    }
}

void setDbgScope(DbgRsScope& scope, exec_list *s)
{
	assert(s || !"no scopeList");
	VPRINT(3, "SET GLOBAL SCOPE LIST: ");

	foreach_list(node, s) {
		scope_item* item = (scope_item*) node;
        scope.numIds++;
        scope.ids = (int*) realloc(scope.ids, scope.numIds*sizeof(int));
        scope.ids[scope.numIds-1] = item->id;
        VPRINT(3, "<%i,%s> ", item->id, item->name);
	}

    VPRINT(3, "\n");
}
