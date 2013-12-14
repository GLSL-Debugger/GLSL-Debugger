/*
 * debugvar.cpp
 *
 *  Created on: 20.10.2013
 */

#include "debugvar.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"
#include <algorithm>

static void traverse_block( exec_list* list, ir_debugvar_traverser_visitor* it )
{
	if( !list->is_empty() ){
		scopeList& scope = it->getScope();

		// remember end of actual scope
		scopeList::iterator end;
		int restoreScope = scope.size();
		if( restoreScope )
			end = --( scope.end() );

		it->visit( list );

		// restore global scope list
		if( restoreScope )
			scope.erase( ++end, scope.end() );
		else
			scope.erase( scope.begin(), scope.end() );
	}
}


void ir_debugvar_traverser_visitor::addToScope(int id)
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

void ir_debugvar_traverser_visitor::dumpScope(void)
{
	if( scope.empty() )
		return;

	for( scopeList::iterator li = scope.begin(), end = scope.end(); li != end; ++li ) {
    	int uid = *li;
        ShVariable* v = findShVariableFromId(vl, uid);
        if ( !v ) {
            dbgPrint(DBGLVL_ERROR, "DebugVar - <%i,?> ", uid);
            exit(1);
        }

        VPRINT(4, "<%i,%s> ", uid, v->name);
    }
    VPRINT(4, "\n");
}

scopeList* ir_debugvar_traverser_visitor::getCopyOfScope(void)
{
    // Hiding of variables in outer scope by local definitions is
    // implemented here. Out of all variables named the same, only the last
    // one is copied to the list!
    scopeList *copiedList = new scopeList();
    scopeList::reverse_iterator rit = scope.rbegin();

    while (rit != scope.rend()) {
        // Check if variable with same name is already in copiedList
        ShVariable *v = findShVariableFromId(vl, *rit);
        if (!nameIsAlreadyInList(copiedList, v->name))
            copiedList->push_front(*rit);
        rit++;
    }

    return copiedList;
}

static void process_initializer(ir_constant* ir, ir_debugvar_traverser_visitor* it)
{
	if( !ir )
		return;

	ir->accept(it);
	scopeList *sl = get_scope(ir);
	if( sl ){
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
	}else{
		dbgPrint( DBGLVL_ERROR, "DebugVar - declaration with initialization failed\n" );
		exit( 1 );
	}
}

bool ir_debugvar_traverser_visitor::visitIr(ir_variable* ir)
{
	ShVariable* var = irToShVariable( ir );

	VPRINT(3, "%c%sdeclaration of %s <%i>%c%s\n", ESC_CHAR, ESC_BOLD,
				ir->name, var->uniqueId, ESC_CHAR, ESC_RESET);

    // Add variable to the global list of all seen variables
    addShVariable(vl, var, 0);

    //TODO: initializer
//    process_initializer(ir->constant_value, this);
//    process_initializer(ir->constant_initializer, this);

	// Now add the list to the actual scope and proceed
    // Prevent temporary variables to get in scope
    if( ir->mode != ir_var_temporary ){
    	addToScope( var->uniqueId );
    	dumpScope();
    }else
    	VPRINT(4, "Variable is temporary. Scope not affected.\n");

	return false;
}

static void traverse_func_param( ir_variable* ir, ir_debugvar_traverser_visitor* it )
{
	if( !ir )
		return;

	ShVariable* v = irToShVariable(ir);

	VPRINT(3, "%c%sparameter %s <%i> %c%s\n", ESC_CHAR, ESC_BOLD,
					ir->name, v->uniqueId, ESC_CHAR, ESC_RESET);

	addShVariable( it->getVariableList(), v, 0 );
	it->addToScope( v->uniqueId );
	it->dumpScope();
}

bool ir_debugvar_traverser_visitor::visitIr(ir_function_signature *ir)
{
	set_scope( ir, getCopyOfScope() );
	VPRINT(3, "%c%sbegin function signature %s at %s %c%s\n", ESC_CHAR, ESC_BOLD,
					ir->function_name(), FormatSourceRange(ir->yy_location).c_str(),
					ESC_CHAR, ESC_RESET);

	int restoreScope = scope.size();
	scopeList::iterator end;
	if (restoreScope)
		end = --(scope.end());

	foreach_iter( exec_list_iterator, iter, ir->parameters ){
		traverse_func_param(((ir_instruction*)iter.get())->as_variable(), this);
	}

	foreach_iter( exec_list_iterator, iter, ir->body ){
		ir_instruction* inst = (ir_instruction*)iter.get();
		inst->accept(this);
	}

	VPRINT(3, "%c%send function signature %s at %s %c%s\n", ESC_CHAR, ESC_BOLD,
					ir->function_name(), FormatSourceRange(ir->yy_location).c_str(),
					ESC_CHAR, ESC_RESET);

	// restore global scope list
	if( restoreScope )
		scope.erase( ++end, scope.end() );
	else
		scope.erase( scope.begin(), scope.end() );

	return false;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_expression* ir)
{
	if( ir->operation > ir_last_unop )
		set_scope( ir, getCopyOfScope() );
    return true;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_assignment* ir)
{
	set_scope( ir, getCopyOfScope() );
	return true;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_return* ir)
{
	set_scope( ir, getCopyOfScope() );
	return true;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_discard* ir)
{
	set_scope( ir, getCopyOfScope() );
	return true;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_if* ir)
{
	// nothing can be declared here in first place
	set_scope( ir,  getCopyOfScope() );

	ir->condition->accept(this);

	traverse_block(&ir->then_instructions, this);
	traverse_block(&ir->else_instructions, this);

	return false;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_loop* ir)
{
	// declarations made in the initialization are not in scope of the loop
	set_scope(ir,  getCopyOfScope());

	// remember end of actual scope, initialization only changes scope of body
	scopeList::iterator end;
	int restoreScope = scope.size();
	if( restoreScope )
		end = --( scope.end() );

	// visit optional initialization
	if( ir->debug_init )
		ir->debug_init->accept(this);

	// visit test, this cannot change scope anyway, so order is unimportant
	ir_if* check = ((ir_instruction*)ir->debug_check_block->next)->as_if();
	if( check->condition )
		check->condition->accept(this);

	// visit optional terminal, this cannot change the scope either
	if( ir->debug_terminal )
		ir->debug_terminal->accept(this);

	// visit body
	this->visit(&ir->body_instructions);

	// restore global scope list
	if( restoreScope )
		scope.erase( ++end, scope.end() );
	else
		scope.erase( scope.begin(), scope.end() );

	return false;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_loop_jump* ir)
{
	set_scope( ir, getCopyOfScope() );
	return true;
}

bool ir_debugvar_traverser_visitor::nameIsAlreadyInList(scopeList* l, const char* name)
{
    scopeList::iterator it = l->begin();

    while (it != l->end()) {
        ShVariable *v = findShVariableFromId(vl, *it);
        if (v) {
            if (!strcmp(name, v->name))
                return true;
        } else {
            dbgPrint(DBGLVL_ERROR, "DebugVar - could not find id %i in scopeList\n", *it);
            exit(1);
        }
        it++;
    }

    return false;
}

bool ir_debugvar_traverser_visitor::visitIr(ir_dummy* ir)
{
	set_scope(ir, getCopyOfScope());
	return false;
}
