/*
 * SymbolTable.cpp
 *
 *  Created on: 06.03.2014.
 */

#include "SymbolTable.h"
#include "ShaderLang.h"
#include "ast.h"


class sh_symbol_table_entry {
public:
DECLARE_RALLOC_CXX_OPERATORS(sh_symbol_table_entry)
	;

	sh_symbol_table_entry(ShVariable *v) :
			v(v), f(0)
	{
	}
	sh_symbol_table_entry(ast_function_definition *f) :
			v(0), f(f)
	{
	}

	ShVariable* v;
	ast_function_definition *f;
};

sh_symbol_table::sh_symbol_table()
{
	this->separate_function_namespace = false;
	this->table = _mesa_symbol_table_ctor();
	this->mem_ctx = ralloc_context(NULL);
}

sh_symbol_table::~sh_symbol_table()
{
	_mesa_symbol_table_dtor(table);
	ralloc_free(mem_ctx);
}

void sh_symbol_table::push_scope()
{
	_mesa_symbol_table_push_scope(table);
}

void sh_symbol_table::pop_scope()
{
	_mesa_symbol_table_pop_scope(table);
}

bool sh_symbol_table::name_declared_this_scope(const char *name)
{
	return _mesa_symbol_table_symbol_scope(table, -1, name) == 0;
}

bool sh_symbol_table::add_variable(ShVariable* v)
{
	if (this->separate_function_namespace) {
		/* In 1.10, functions and variables have separate namespaces. */
		sh_symbol_table_entry *existing = get_entry(v->name);
		if (name_declared_this_scope(v->name)) {
			/* If there's already an existing function (not a constructor!) in
			 * the current scope, just update the existing entry to include 'v'.
			 */
			if (existing->v == NULL){ // && existing->t == NULL) {
				existing->v = v;
				return true;
			}
		} else {
			/* If not declared at this scope, add a new entry.  But if an existing
			 * entry includes a function, propagate that to this block - otherwise
			 * the new variable declaration would shadow the function.
			 */
			sh_symbol_table_entry *entry = new (mem_ctx) sh_symbol_table_entry(v);
			if (existing != NULL)
				entry->f = existing->f;
			int added = _mesa_symbol_table_add_symbol(table, -1, v->name, entry);
			assert(added == 0);
			(void) added;
			return true;
		}
		return false;
	}

	/* 1.20+ rules: */
	sh_symbol_table_entry *entry = new (mem_ctx) sh_symbol_table_entry(v);
	return _mesa_symbol_table_add_symbol(table, -1, v->name, entry) == 0;
}

bool sh_symbol_table::add_function(ast_function_definition *f)
{
	const char* name = f->prototype->identifier;
	if (this->separate_function_namespace && name_declared_this_scope(name)) {
		/* In 1.10, functions and variables have separate namespaces. */
		sh_symbol_table_entry *existing = get_entry(name);
		if ((existing->f == NULL)) { // && (existing->t == NULL)) {
			existing->f = f;
			return true;
		}
	}
	sh_symbol_table_entry *entry = new (mem_ctx) sh_symbol_table_entry(f);
	return _mesa_symbol_table_add_symbol(table, -1, name, entry) == 0;
}

void sh_symbol_table::add_global_function(ast_function_definition *f)
{
	sh_symbol_table_entry *entry = new (mem_ctx) sh_symbol_table_entry(f);
	int added = _mesa_symbol_table_add_global_symbol(table, -1, f->prototype->identifier,
			entry);
	assert(added == 0);
	(void) added;
}

ShVariable *sh_symbol_table::get_variable(const char *name)
{
	sh_symbol_table_entry *entry = get_entry(name);
	return entry != NULL ? entry->v : NULL;
}

ast_function_definition *sh_symbol_table::get_function(const char *name)
{
	sh_symbol_table_entry *entry = get_entry(name);
	return entry != NULL ? entry->f : NULL;
}

sh_symbol_table_entry *sh_symbol_table::get_entry(const char *name)
{
	return (sh_symbol_table_entry *) _mesa_symbol_table_find_symbol(table, -1, name);
}

