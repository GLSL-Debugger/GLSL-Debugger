/*
 * IRScope.cpp
 *
 *  Created on: 03.10.2013
 */

#include "ir.h"
#include "IRScope.h"
#include <map>
#include "glsldb/utils/dbgprint.h"

namespace {
	typedef std::map< ir_instruction*, scopeList* > ScopeMap;
	typedef std::map< ir_instruction*, ShChangeableList* > ChangeableListMap;
	typedef std::map< exec_list*, ShChangeableList* > ChangeableExecListMap;
	ScopeMap scopes;
	ChangeableExecListMap changeable_exec_lists;
	ChangeableListMap changeable_lists;
	ChangeableListMap changeable_param_lists;
}



scopeList* get_scope( ir_instruction* ir )
{
	ScopeMap::iterator it = scopes.find(ir);
	if( it != scopes.end() )
		return it->second;
	return NULL;
}

void set_scope( ir_instruction* ir, scopeList* list )
{
	scopes[ir] = list;
}

ShChangeableList* get_changeable_list( exec_list* list )
{
	ChangeableExecListMap::iterator it = changeable_exec_lists.find(list);
	if( it != changeable_exec_lists.end() )
		return it->second;

	// Create new
	ShChangeableList* l = new ShChangeableList();
	l->changeables = NULL;
	l->numChangeables = 0;
	changeable_exec_lists[list] = l;
	return l;
}

// TODO: There a leak
ShChangeableList* get_changeable_list( ir_instruction* ir )
{
	ChangeableListMap::iterator it = changeable_lists.find(ir);
	if( it != changeable_lists.end() )
		return it->second;

	// Create new
	ShChangeableList* l = new ShChangeableList();
	l->changeables = NULL;
	l->numChangeables = 0;
	changeable_lists[ir] = l;
	return l;
}

ShChangeableList* get_changeable_paramerers_list( ir_instruction* ir )
{
	ChangeableListMap::iterator it = changeable_param_lists.find(ir);
	if( it != changeable_param_lists.end() )
		return it->second;

	// Create new
	ShChangeableList* l = new ShChangeableList();
	l->changeables = NULL;
	l->numChangeables = 0;
	changeable_param_lists[ir] = l;
	return l;
}

void addScopeToScopeStack(DbgRsScope& stack, scopeList *s)
{
    int i;

    if (!s) {
        return;
    }

    scopeList::iterator si = s->begin();

    while (si != s->end()) {
        for (i=0; i<stack.numIds; i++) {
            if (*si == stack.ids[i]) {
                goto NEXTINSCOPE;
            }
        }


        stack.numIds++;
        stack.ids = (int*) realloc(stack.ids, stack.numIds*sizeof(int));
        stack.ids[stack.numIds-1] = *si;

NEXTINSCOPE:
        si++;
    }
}

void setDbgScope(DbgRsScope& scope, scopeList *s)
{
    if (!s) {
        dbgPrint(DBGLVL_ERROR, "no scopeList\n");
        exit(1);
        return;
    }

    scopeList::iterator si = s->begin();

    VPRINT(3, "SET GLOBAL SCOPE LIST: ");

    while (si != s->end()) {
        scope.numIds++;
        scope.ids = (int*) realloc(scope.ids, scope.numIds*sizeof(int));
        scope.ids[scope.numIds-1] = *si;
        si++;

        VPRINT(3, "%i ", *si);
    }
    VPRINT(3, "\n");


}
