/*
 * debugvar.cpp
 *
 *  Created on: 20.10.2013
 */

#include "debugvar.h"
#include "Shader.h"
#include "CodeTools.h"
#include "AstScope.h"
#include "glsldb/utils/dbgprint.h"
#include <set>
#include <algorithm>
#undef NDEBUG
#include <assert.h>


class ScopeSaver {
public:
	ScopeSaver(exec_list* s)
	{
		scope = s;
		last = s->get_tail();
	}

	~ScopeSaver()
	{
		// restore scope list
		while (!scope->is_empty() && scope->get_tail() != last){
			exec_node *n = scope->get_tail();
			n->remove();
			delete n;
		}
	}

private:
	exec_node* last;
	exec_list* scope;
};

ast_debugvar_traverser_visitor::ast_debugvar_traverser_visitor(AstShader* _sh,
		ShVariableList* _vl) :
		vl(_vl), shader(_sh)
{
	for(int i = 0; i < _sh->globals.numVariables; ++i) {
		ShVariable* var = _sh->globals.variables[i];
		addShVariableCtx(vl, var, var->builtin, _sh);
		addToScope(var);
	}
}

bool ast_debugvar_traverser_visitor::enter(class ast_expression* node)
{
	copyScopeTo(node);

	if (node->oper == ast_identifier)
		assert(node->debug_id >= 0 || !"All variables must have id");

	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_expression_bin* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_function_expression* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_aggregate_initializer* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_compound_statement* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_declaration* node)
{
	ShVariable* var = findShVariable(node->debug_id);
	assert(var);

	VPRINT(3, "%c%sdeclaration of %s <%i>%c%s\n", ESC_CHAR, ESC_BOLD,
			node->identifier, var->uniqueId, ESC_CHAR, ESC_RESET);

	// Add variable to the global list of all seen variables
	addShVariableCtx(vl, var, var->builtin, shader);

	if (node->initializer) {
		node->initializer->accept(this);
		exec_list* scope = &node->initializer->scope;
		(void)scope;
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
	}

	// Now add the list to the actual scope and proceed
	addToScope(var);
	dumpScope();
	return false;
}

bool ast_debugvar_traverser_visitor::enter(class ast_parameter_declarator* node)
{
	// No variable when void
	if (node->is_void)
		return false;

	ShVariable* var = findShVariable(node->debug_id);
	assert(var);

	VPRINT(3, "%c%sparameter %s <%i> %c%s\n", ESC_CHAR, ESC_BOLD,
			node->identifier, var->uniqueId, ESC_CHAR, ESC_RESET);
	addShVariableCtx(vl, var, var->builtin, shader);
	addToScope(var);
	dumpScope();
	return false;
}

bool ast_debugvar_traverser_visitor::enter(class ast_struct_specifier *)
{
	// Struct specification is not declaration
	// Do not add fields as variables
	return false;
}

bool ast_debugvar_traverser_visitor::enter(class ast_case_statement* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_case_statement_list* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_switch_body* node)
{
	copyScopeTo(node);
	return true;
}


bool ast_debugvar_traverser_visitor::enter(class ast_selection_statement* node)
{
	// nothing can be declared here in first place
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_switch_statement* node)
{
	// nothing can be declared here in first place
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_iteration_statement* node)
{
	// declarations made in the initialization are not in scope of the loop
	copyScopeTo(node);

	// remember end of actual scope, initialization only changes scope of body
	// it will be restored on object destruction
	ScopeSaver ss(&scope);

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

	return false;
}

bool ast_debugvar_traverser_visitor::enter(class ast_jump_statement* node)
{
	copyScopeTo(node);
	return true;
}

bool ast_debugvar_traverser_visitor::enter(class ast_function_definition* node)
{
	copyScopeTo(node);
	const char* name = node->prototype->identifier;
	std::string range =	FormatSourceRange(node->get_location());

	// FIXME: function without prototype
	VPRINT(3, "%c%sbegin function signature %s at %s %c%s\n",
			ESC_CHAR, ESC_BOLD, name, range.c_str(), ESC_CHAR, ESC_RESET);

	// Save & restore global scope
	ScopeSaver ss(&scope);

	depth++;
	this->visit(&node->prototype->parameters);

	if (node->body)
		node->body->accept(this);
	depth--;

	VPRINT(3, "%c%send function signature %s at %s %c%s\n",
			ESC_CHAR, ESC_BOLD, name, range.c_str(), ESC_CHAR, ESC_RESET);

	return false;
}

void ast_debugvar_traverser_visitor::leave(class ast_expression_statement* node)
{
	copyScopeTo(node);
}


void ast_debugvar_traverser_visitor::addToScope(ShVariable* var)
{
	// Double ids can only occur with arrays of undeclared size.
	// Scope is of outer bound, as those variables can be used right after
	// their first definition.

	// search for doubles and return if found
	foreach_in_list(scope_item, item, &scope){
		if (item->id == var->uniqueId)
			return;
	}

	scope_item* item = new(shader) scope_item(var->uniqueId, var->name);
	scope.push_tail(item);
}

void ast_debugvar_traverser_visitor::dumpScope(exec_list* dscope)
{
	if (dscope->is_empty())
		return;

	foreach_in_list(scope_item, item, dscope){
		VPRINT(4, "<%i,%s> ", item->id, item->name);
	}

	VPRINT(4, "\n");
}

void ast_debugvar_traverser_visitor::copyScopeTo(ast_node* dst)
{
	std::set<std::string> in_list;

	// Use reverse to get last variable with the same name.
	foreach_in_list_reverse(scope_item, item, &scope) {
		std::string name = item->name;
		// Name already in list
		if (in_list.find(std::string(name)) != in_list.end())
			continue;
		in_list.insert(std::string(name));
		dst->scope.push_head(item->clone(shader));
	}
}

