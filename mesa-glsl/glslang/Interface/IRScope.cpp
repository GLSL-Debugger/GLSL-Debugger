/*
 * IRScope.cpp
 *
 *  Created on: 03.10.2013
 */

#include "ir.h"
#include "IRScope.h"
#include <map>


namespace {
	typedef std::map< ir_instruction*, scopeList* > ScopeMap;
	typedef std::map< ir_instruction*, ShChangeableList* > ChangeableListMap;
	ScopeMap scopes;
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

