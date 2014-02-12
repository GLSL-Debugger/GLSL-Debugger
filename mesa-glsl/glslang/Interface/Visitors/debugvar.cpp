/*
 * debugvar.cpp
 *
 *  Created on: 20.10.2013
 */

#include "debugvar.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"
#include <algorithm>

bool ast_debugvar_traverser_visitor::traverse(class ast_expression* node)
{
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(class ast_expression_bin* node)
{
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(
		class ast_aggregate_initializer* node)
{
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(class ast_declaration* node)
{
	ShVariable* var = findShVariableFromSource(node);

	assert(var);

	VPRINT(3, "%c%sdeclaration of %s <%i>%c%s\n", ESC_CHAR, ESC_BOLD,
			node->identifier, var->uniqueId, ESC_CHAR, ESC_RESET);

	// Add variable to the global list of all seen variables
	addShVariable(vl, var, 0);

	if(node->initializer){
		node->initializer->accept(this);
		scopeList *sl = get_scope(node->initializer);
		if (sl) {
			/*
			 * Actually do not add declared variable to the list here, because
			 * Code Generation would access the data before it is declared. This should
			 * not be needed anyway, since the data would be uninitialized
			 *
			 // Dont forget to check for double ids
			 scopeList::iterator e = find(sl->begin(), sl->end(), v->getUniqueId());

			 // Finally at it to the list
			 if (e == sl->end()) {
			 sl->push_back(v->getUniqueId());
			 }
			 */
		} else {
			dbgPrint( DBGLVL_ERROR,
					"DebugVar - declaration with initialization failed\n");
			exit(1);
		}
	}

	// Now add the list to the actual scope and proceed
	addToScope(var->uniqueId);
	dumpScope();
	return false;
}

bool ast_debugvar_traverser_visitor::traverse(
		class ast_parameter_declarator* node)
{
	assert(0);
	ShVariable* v = findShVariableFromSource(node);
	VPRINT(3, "%c%sparameter %s <%i> %c%s\n", ESC_CHAR, ESC_BOLD,
			node->identifier, v->uniqueId, ESC_CHAR, ESC_RESET);
	addShVariable(getVariableList(), v, 0);
	addToScope(v->uniqueId);
	dumpScope();
	return false;
}

bool ast_debugvar_traverser_visitor::traverse(class ast_case_statement* node)
{
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(
		class ast_selection_statement* node)
{
	// nothing can be declared here in first place
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(class ast_switch_statement* node)
{
	// nothing can be declared here in first place
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(
		class ast_iteration_statement* node)
{
	// declarations made in the initialization are not in scope of the loop
	set_scope(node, getCopyOfScope());

	// remember end of actual scope, initialization only changes scope of body
	scopeList::iterator end;
	int restoreScope = scope.size();
	if (restoreScope)
		end = --(scope.end());

	depth++;
	// visit optional initialization
	if (node->init_statement)
		node->init_statement->accept(this);

	// visit test, this cannot change scope anyway, so order is unimportant
	if (node->condition)
		node->condition->accept(this);

	// visit optional terminal, this cannot change the scope either
	if (node->rest_expression)
		node->rest_expression->accept(this);

	// visit body
	if (node->body)
		node->body->accept(this);

	depth--;

	// restore global scope list
	if (restoreScope)
		scope.erase(++end, scope.end());
	else
		scope.erase(scope.begin(), scope.end());

	return false;
}

bool ast_debugvar_traverser_visitor::traverse(class ast_jump_statement* node)
{
	set_scope(node, getCopyOfScope());
	return true;
}

bool ast_debugvar_traverser_visitor::traverse(
		class ast_function_definition* node)
{
	set_scope(node, getCopyOfScope());
	const char* name = node->prototype->identifier;
	const char* range = FormatSourceRange(node->get_location()).c_str();

	// FIXME: function without prototype
	VPRINT(3,
			"%c%sbegin function signature %s at %s %c%s\n", ESC_CHAR, ESC_BOLD, name, range, ESC_CHAR, ESC_RESET);

	int restoreScope = scope.size();
	scopeList::iterator end;
	if (restoreScope)
		end = --(scope.end());

	depth++;
	this->visit(&node->prototype->parameters);

	if (node->body)
		node->body->accept(this);
	depth--;

	VPRINT(3,
			"%c%send function signature %s at %s %c%s\n", ESC_CHAR, ESC_BOLD, name, range, ESC_CHAR, ESC_RESET);

	// restore global scope list
	if (restoreScope)
		scope.erase(++end, scope.end());
	else
		scope.erase(scope.begin(), scope.end());

	return false;
}

void ast_debugvar_traverser_visitor::addToScope(int id)
{
	// Double ids can only occur with arrays of undeclared size.
	// Scope is of outer bound, as those variables can be used right after
	// their first definition.

	// search for doubles
	scopeList::iterator e = std::find(scope.begin(), scope.end(), id);

	// only insert if not already in there
	if (e == scope.end())
		scope.push_back(id);
}

void ast_debugvar_traverser_visitor::dumpScope(void)
{
	if (scope.empty())
		return;

	for (scopeList::iterator li = scope.begin(), end = scope.end(); li != end;
			++li) {
		int uid = *li;
		ShVariable* v = findShVariableFromId(vl, uid);
		if (!v) {
			dbgPrint(DBGLVL_ERROR, "DebugVar - <%i,?> ", uid);
			exit(1);
		}

		VPRINT(4, "<%i,%s> ", uid, v->name);
	}
	VPRINT(4, "\n");
}

scopeList* ast_debugvar_traverser_visitor::getCopyOfScope(void)
{
	// Hiding of variables in outer scope by local definitions is
	// implemented here. Out of all variables named the same, only the last
	// one is copied to the list!
	scopeList *copiedList = new scopeList();
	scopeList::reverse_iterator rit = scope.rbegin();

	// FIXME: It looks like some weird things happens here.
	// first we iterate scope here and vl in findShVar..,
	// next we iterate in nameIsAlreadyInList and then we
	// iterate things again in calls there. It must be rewritten somehow.
	while (rit != scope.rend()) {
		// Check if variable with same name is already in copiedList
		ShVariable *v = findShVariableFromId(vl, *rit);
		if (!nameIsAlreadyInList(copiedList, v->name))
			copiedList->push_front(*rit);
		rit++;
	}

	return copiedList;
}

bool ast_debugvar_traverser_visitor::nameIsAlreadyInList(scopeList* l,
		const char* name)
{
	scopeList::iterator it = l->begin();

	while (it != l->end()) {
		ShVariable *v = findShVariableFromId(vl, *it);
		if (v) {
			if (!strcmp(name, v->name))
				return true;
		} else {
			dbgPrint(DBGLVL_ERROR,
					"DebugVar - could not find id %i in scopeList\n", *it);
			exit(1);
		}
		it++;
	}

	return false;
}

