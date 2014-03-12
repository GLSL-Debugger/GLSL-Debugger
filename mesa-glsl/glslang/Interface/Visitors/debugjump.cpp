/*
 * debugjump.cpp
 *
 *  Created on: 10.10.2013
 */

#include "debugjump.h"
#include "glsl/list.h"
#include "glslang/Include/ShaderLang.h"
#include "glslang/Interface/AstScope.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/SymbolTable.h"
#include "glsldb/utils/dbgprint.h"

static void setDbgResultRange(DbgRsRange& r, const YYLTYPE& range)
{
	r.left.line = range.first_line;
	r.left.colum = range.first_column;
	r.right.line = range.last_line;
	r.right.colum = range.last_column;
}

bool ast_debugjump_traverser_visitor::step(const char* name)
{
	/* Initialize parse tree for debugging if necessary */
	operation = OTOpTargetUnset;
	if (parseStack.empty()) {
		ast_function_definition* func = shader->symbols->get_function(name);
		if (!func)
			return false;
		operation = OTOpTargetSet;
		parseStack.push(func);
	}

	/* Advance the debug trace; move DbgStTarget */
	VPRINT(1, "********* jump traverse **********\n");
	parseStack.top()->accept(this);
	return true;
}

void ast_debugjump_traverser_visitor::setGobalScope(exec_list *s)
{
	setDbgScope(result.scope, s, shader);
	/* Add local scope to scope stack*/
	addScopeToScopeStack(this->result.scopeStack, s, shader);
}

void ast_debugjump_traverser_visitor::addShChangeables(ast_node* node)
{
	if (!node)
		return;
	copyShChangeableListCtx(&result.cgbls, &node->changeables, shader);
	checkReturns(node);
}

void ast_debugjump_traverser_visitor::checkReturns(ast_node* node)
{
	// Check if this operation would have emitted a vertex
	if (node->debug_sideeffects & ir_dbg_se_emit_vertex) {
		this->vertexEmited = true;
		VPRINT( 6, "passed Emit %i\n", __LINE__);
	}
	if (node->debug_sideeffects & ir_dbg_se_discard) {
		this->discardPassed = true;
		VPRINT( 6, "passed Discard %i\n", __LINE__);
	}
}

// Default handling of a node that can be debugged
// Returns old operation
void ast_debugjump_traverser_visitor::processDebugable(ast_node *node)
{
	VPRINT(3, "process Debugable L:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), operation, node->debug_state);

	if (operation == OTOpTargetUnset) {
		if (node->debug_state == ast_dbg_state_target) {
			node->debug_state = ast_dbg_state_unset;
			operation = OTOpTargetSet;
			VPRINT( 3, "\t ------- unset target --------\n");
			result.position = DBG_RS_POSITION_UNSET;
		}
	} else if (operation == OTOpTargetSet) {
		assert(node->debug_state != ast_dbg_state_target || !"ERROR! found target with DbgStTarget\n");
		if (node->debug_state == ast_dbg_state_unset) {
			node->debug_state = ast_dbg_state_target;
			operation = OTOpDone;
			VPRINT(3, "\t -------- set target ---------\n");
			if (node->as_expression())
				setTarget(node->as_expression());
			else if (node->as_jump_statement())
				setTarget(node->as_jump_statement());
		}
	}
}

void ast_debugjump_traverser_visitor::setTarget(class ast_expression* node)
{
	switch (node->oper) {
	/* binary */
	case ast_assign:
	case ast_mul_assign:
	case ast_div_assign:
	case ast_mod_assign:
	case ast_add_assign:
	case ast_sub_assign:
	case ast_ls_assign:
	case ast_rs_assign:
	case ast_and_assign:
	case ast_xor_assign:
	case ast_or_assign: {
		result.position = DBG_RS_POSITION_ASSIGMENT;
		setDbgResultRange(result.range, node->get_location());
		setGobalScope(&node->scope);
		break;
	}
	case ast_pre_inc:
	case ast_pre_dec:
	case ast_post_inc:
	case ast_post_dec: {
		result.position = DBG_RS_POSITION_UNARY;
		setDbgResultRange(result.range, node->get_location());
		setGobalScope(&node->scope);
		break;
	}
	default:
		break;
	}
}

void ast_debugjump_traverser_visitor::setTarget(class ast_jump_statement* node)
{
	result.position = DBG_RS_POSITION_BRANCH;
	setDbgResultRange(result.range, node->get_location());
	setGobalScope(&node->scope);
}

//void ast_debugjump_traverser_visitor::setTarget(class ast_dummy* node)
//{
//	result.position = DBG_RS_POSITION_DUMMY;
//	setDbgResultRange(result.range, node->get_location());
//	setGobalScope(&node->scope);
//}

void ast_debugjump_traverser_visitor::visit(class ast_selection_statement* node)
{
	bool visit = true;
	/* Visit node for optional check of condition */
	if (node->debug_state_internal == ast_dbg_if_unset
			|| node->debug_state_internal == ast_dbg_if_init
			|| node->debug_state_internal == ast_dbg_if_condition_passed)
		visit = this->enter(node);

	if (visit && node->debug_state_internal == ast_dbg_if_condition && node->condition)
		node->condition->accept(this);

	/* Visit node again for choosing debugged branch */
	if (node->debug_state_internal == ast_dbg_if_condition)
		visit = this->enter(node);

	if (visit) {
		++depth;
		if (node->debug_state_internal == ast_dbg_if_then && node->then_statement)
			node->then_statement->accept(this);
		if (node->debug_state_internal == ast_dbg_if_else && node->else_statement)
			node->else_statement->accept(this);
		--depth;
	}

	/* Visit node again for preparation of pass */
	if (node->debug_state_internal == ast_dbg_if_then
			|| node->debug_state_internal == ast_dbg_if_else)
		visit = this->enter(node);
}

void ast_debugjump_traverser_visitor::visit(class ast_iteration_statement* node)
{
	bool visit = true;
	/* Visit node first */
	if (node->debug_state_internal == ast_dbg_loop_unset
			|| node->debug_state_internal == ast_dbg_loop_qyr_init)
		visit = this->enter(node);

	if (visit && node->debug_state_internal == ast_dbg_loop_wrk_init && node->init_statement)
		node->init_statement->accept(this);

	/* Visit node again for test */
	if (node->debug_state_internal == ast_dbg_loop_wrk_init
			|| node->debug_state_internal == ast_dbg_loop_qyr_test)
		visit = this->enter(node);

	if (visit && node->debug_state_internal == ast_dbg_loop_wrk_test && node->condition)
		node->condition->accept(this);

	/* Visit node again for flow selection */
	if (node->debug_state_internal == ast_dbg_loop_wrk_test
			|| node->debug_state_internal == ast_dbg_loop_select_flow)
		visit = this->enter(node);

	++depth;
	if (visit && node->debug_state_internal == ast_dbg_loop_wrk_body && node->body)
		node->body->accept(this);
	--depth;

	/* Visit node again for terminal */
	if (node->debug_state_internal == ast_dbg_loop_wrk_body
			|| node->debug_state_internal == ast_dbg_loop_qyr_terminal)
		visit = this->enter(node);

	if (visit && node->debug_state_internal == ast_dbg_loop_wrk_terminal
			&& node->rest_expression)
		node->rest_expression->accept(this);

	/* Visit node again for terminal */
	if (node->debug_state_internal == ast_dbg_loop_wrk_terminal)
		visit = this->enter(node);
}

bool ast_debugjump_traverser_visitor::enter(class ast_expression* node)
{
	VPRINT(2, "process Expression L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	switch (node->oper) {
	/* binary */
	case ast_assign:
	case ast_mul_assign:
	case ast_div_assign:
	case ast_mod_assign:
	case ast_add_assign:
	case ast_sub_assign:
	case ast_ls_assign:
	case ast_rs_assign:
	case ast_and_assign:
	case ast_xor_assign:
	case ast_or_assign: {
		processDebugable(node);
		if (operation == OTOpTargetSet) {
			if (!(dbgBehaviour & DBG_BH_JUMPINTO)) {
				// do not visit children
				// add all changeables of this node to the list
				VPRINT(2, "----> copy changeables\n");
				addShChangeables(node);
			} else {
				// visit children
				++depth;
				// Traverse from right to left
				for (int i = 1; i >= 0; --i)
					node->subexpressions[i]->accept(this);
				--depth;

				// if no target was found so far
				// all changeables need to be added to the list
				if (operation == OTOpTargetSet)
					addShChangeables(node);
			}
		} else if (operation == OTOpTargetUnset) {
			// visit children
			++depth;
			for (int i = 1; i >= 0; --i)
				node->subexpressions[i]->accept(this);
			--depth;

			// the old target was found inside left/right branch and
			// a new one is still being searched for
			// -> add only changed variables of this assigment, i.e.
			//    changeables of the left branch
			if (operation == OTOpTargetSet)
				addShChangeables(node->subexpressions[0]);
		} else if (operation != OTOpDone) {
			return true;
		}
		return false;
		break;
	}
	case ast_conditional:  // TODO: ast_selection_statement
		assert(!"not implemented");
		break;
	default:
		break;
	}

	return true;
}

void ast_debugjump_traverser_visitor::leave(class ast_expression* node)
{
	switch (node->oper) {
	case ast_pre_inc:
	case ast_pre_dec:
	case ast_post_inc:
	case ast_post_dec:
		processDebugable(node);
		if (operation == OTOpTargetSet) {
			if (!(dbgBehaviour & DBG_BH_JUMPINTO)) {
				// user didn't want to debug further
				// copy all changeables
				VPRINT(2, "----> copy changeables\n");
				addShChangeables(node);
			} else {
				// user wants to debug children
				++depth;
				// TODO: Check is this error to do it in leave
				node->subexpressions[0]->accept(this);
				--depth;
				// if the target was not inside operand, all changeables
				// need to be copied
				if (operation == OTOpTargetSet)
					addShChangeables(node);
			}
		} else if (operation == OTOpTargetUnset) {
			// visit operand
			++depth;
			node->subexpressions[0]->accept(this);
			--depth;

			// This should never happen, but anyway maybe in the future
			// if the old target was inside operand but not the new one, add
			// changeables to global list
			if (operation == OTOpTargetSet)
				addShChangeables(node);
		}
		break;
	default:
		break;
	}
}

void ast_debugjump_traverser_visitor::leave(class ast_function_expression* node)
{
	// TODO:
	if (node->is_constructor())
		return;

	ast_expression* identifier = node->subexpressions[0];
	assert(identifier);

	const char* func_name = identifier->primary_expression.identifier;
	ast_function_definition* funcDef = shader->symbols->get_function(func_name);
	assert(!funcDef || node->debug_builtin || !"Function not found");

	VPRINT(2, "process Call L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(),
			func_name, node->debug_builtin, this->operation, node->debug_state);

	if (operation == OTOpTargetUnset) {
		if (node->debug_state != ast_dbg_state_target)
			return;

		if (funcDef && (this->dbgBehaviour & DBG_BH_JUMPINTO)) {
			// no changeable has to be copied in first place,
			// as we jump into this function

			VPRINT(2, "\t ---- push %p on stack ----\n", funcDef);
			this->parseStack.push(funcDef);
			this->operation = OTOpTargetSet;

			// add local parameters of called function first
			copyShChangeableListCtx(&result.cgbls, &funcDef->prototype->changeables, shader);
			funcDef->accept(this);

			// if parsing ends up here and a target is still beeing
			// searched, a wierd function was called, but anyway,
			// let's copy the appropriate changeables
			if (this->operation == OTOpTargetSet)
				copyShChangeableListCtx(&result.cgbls, &node->changeables, shader);
		} else {
			processDebugable(node);

			// if parsing of the subfunction finished right now
			// -> copy only changed parameters to changeables
			// else
			// -> copy all, since user wants to jump over this func
			if (this->finishedDbgFunction) {
				copyShChangeableListCtx(&result.cgbls, &node->changeable_params, shader);
				this->finishedDbgFunction = false;
			} else {
				copyShChangeableListCtx(&result.cgbls, &node->changeables, shader);
				if (funcDef)
					checkReturns(funcDef);
			}
		}
	} else if (operation == OTOpTargetSet) {
		if (node->debug_state == ast_dbg_state_target)
			assert(!"ERROR! found target with DbgStTarget\n");

		if (node->debug_state == ast_dbg_state_unset) {
			node->debug_state = ast_dbg_state_target;
			VPRINT( 3, "\t -------- set target ---------\n");
			result.position = DBG_RS_POSITION_FUNCTION_CALL;
			setDbgResultRange(result.range, node->get_location());
			setGobalScope(&node->scope);
			this->operation = OTOpDone;
		}
	}
}


void ast_debugjump_traverser_visitor::leave(class ast_case_statement*)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_case_statement_list*)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_switch_statement*)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_function_definition* node)
{
	VPRINT( 2, "process function definition L:%s N:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(),
			node->prototype->identifier, operation, node->debug_state);

	if (operation != OTOpTargetSet)
		return;

	/* This marks the end of a function call */
	assert(node == parseStack.top() || !"ERROR! unexpected stack order\n");

	VPRINT( 2, "\t ---- pop %p from stack ----\n", node);
	parseStack.pop();
	/* Do not directly jump into next function after
	 * returning from a function */
	dbgBehaviour &= ~DBG_BH_JUMPINTO;
	finishedDbgFunction = true;

	if (!this->parseStack.empty()) {
		ast_node* top = parseStack.top();
		VPRINT( 2, "\t ---- continue parsing at %pk ----\n", top);
		operation = OTOpTargetUnset;
		top->accept(this);
	} else {
		VPRINT( 2, "\t ---- stack empty, finished ----\n");
		operation = OTOpFinished;
	}
}

void ast_debugjump_traverser_visitor::leave(ast_jump_statement* node)
{
	processDebugable(node);
}


#define SET_OPERATION(op, ds, dsi, pos) 	\
		this->operation = op;				\
		node->debug_state = ds;				\
		node->debug_state_internal = dsi;	\
		result.position = pos;

bool ast_debugjump_traverser_visitor::enter(class ast_selection_statement* node)
{
	VPRINT( 2, "process Selection L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	if (operation == OTOpTargetUnset) {
		if (node->debug_state != ast_dbg_state_target)
			return true;

		switch (node->debug_state_internal) {
		case ast_dbg_if_unset:
			assert(!"CodeTools - target but state unset\n");
			break;
		case ast_dbg_if_init: {
			VPRINT( 3, "\t ------- unset target --------\n");
			SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_if_condition,
					DBG_RS_POSITION_UNSET)

			if (this->dbgBehaviour & DBG_BH_JUMPINTO) {
				// visit condition
				if (node->condition)
					node->condition->accept(this);

				// there was not target in condition, so copy all changeables;
				// it's unlikely that there is a changeable and no target,
				// but anyway be on the safe side
				if (this->operation == OTOpTargetSet)
					addShChangeables(node->condition);
				return false;
			} else {
				/* Finish debugging this selection */
				node->debug_state_internal = ast_dbg_if_unset;
				/* copy changeables */
				addShChangeables(node->condition);
				addShChangeables(node->then_statement);
				addShChangeables(node->else_statement);
				return false;
			}
			break;
		}
		case ast_dbg_if_condition_passed: {
			VPRINT( 3, "\t ------- unset target again --------\n");
			node->debug_state = ast_dbg_state_unset;
			this->operation = OTOpTargetSet;
			result.position = DBG_RS_POSITION_UNSET;

			if (this->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER) {
				/* Finish debugging this selection */
				node->debug_state_internal = ast_dbg_if_unset;
				/* copy changeables */
				addShChangeables(node->then_statement);
				addShChangeables(node->else_statement);
				return false;
			} else if (this->dbgBehaviour & DBG_BH_FOLLOW_ELSE) {
				if (!node->else_statement) {
					// It looks like it was this weird way before. Not sure
					// this branch will ever execute
					node->debug_state_internal = ast_dbg_if_then;
				} else {
					node->debug_state_internal = ast_dbg_if_else;
					// check other branch for discards
					assert(node->then_statement->debug_sideeffects);
					if (node->then_statement->debug_sideeffects & ast_dbg_se_discard) {
						this->discardPassed = true;
						VPRINT(6, "passed Discard %i\n", __LINE__);
					}
				}
			} else {
				node->debug_state_internal = ast_dbg_if_then;
				// check other branch for discards
				if (node->else_statement &&
						(node->else_statement->debug_sideeffects & ast_dbg_se_discard)) {
					this->discardPassed = true;
					VPRINT( 6, "passed Discard %i\n", __LINE__);
				}
			}
			return true;
		}
		default:
			break;
		}
	} else if (operation == OTOpTargetSet) {
		if (node->debug_state != ast_dbg_state_unset)
			return true;

		switch (node->debug_state_internal) {
		case ast_dbg_if_unset:
			VPRINT( 3, "\t -------- set target ---------\n");
			SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_if_init,
					node->else_statement ? DBG_RS_POSITION_SELECTION_IF_ELSE : DBG_RS_POSITION_SELECTION_IF)
			setDbgResultRange(result.range, node->get_location());
			setGobalScope(&node->scope);
			return false;
		case ast_dbg_if_condition:
			VPRINT( 3, "\t -------- set target again ---------\n");
			SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_if_condition_passed,
					node->else_statement ? DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE : DBG_RS_POSITION_SELECTION_IF_CHOOSE)
			setDbgResultRange(result.range, node->get_location());
			setGobalScope(&node->scope);
			return false;
		case ast_dbg_if_then:
		case ast_dbg_if_else: {
			VPRINT( 4, "\t -------- set condition pass ---------\n");
			// Debugging of condition finished! Take care of the
			// non-debugged branch - if there is one - and copy
			// it's changeables!
			ast_node* path =
					node->debug_state_internal == ast_dbg_if_then ?
							node->then_statement : node->else_statement;
			addShChangeables(path);
			node->debug_state_internal = ast_dbg_if_unset;
			return false;
		}
		default:
			break;
		}
	}

	return true;
}


static inline DbgRsTargetPosition loop_position(ast_iteration_statement::ast_iteration_modes t)
{
	switch (t) {
	case ast_iteration_statement::ast_while:
		return DBG_RS_POSITION_LOOP_WHILE;
	case ast_iteration_statement::ast_for:
		return DBG_RS_POSITION_LOOP_FOR;
	case ast_iteration_statement::ast_do_while:
		return DBG_RS_POSITION_LOOP_DO;
	default:
		dbgPrint(DBGLVL_ERROR, "CodeTools - setPositionLoop invalid loop type: %i\n", (int)t);
		assert(!"wrong mode");
		break;
	}

	// Never happens. Only warning
	return DBG_RS_POSITION_DUMMY;
}

bool ast_debugjump_traverser_visitor::enter(class ast_iteration_statement* node)
{
	VPRINT( 1, "process Loop L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	switch (this->operation) {
	case OTOpTargetUnset:
		switch (node->debug_state) {
		case ast_dbg_state_target:
			switch (node->debug_state_internal) {
			case ast_dbg_loop_unset:
				assert(!"CodeTools - target but state unset\n");
				break;
			case ast_dbg_loop_qyr_init:
				VPRINT( 3, "\t ------- unset target --------\n");
				SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_loop_wrk_init,
						DBG_RS_POSITION_UNSET)

				if (this->dbgBehaviour & DBG_BH_JUMPINTO) {
					// visit initialization
					if (node->init_statement)
						node->init_statement->accept(this);

					// there was not target in condition, so copy all changeables;
					// it's unlikely that there is a changeable and no target,
					// but anyway be on the safe side
					if (this->operation == OTOpTargetSet)
						addShChangeables(node->init_statement);
				} else {
					/* Finish debugging this loop */
					node->debug_state_internal = ast_dbg_loop_unset;
					node->debug_iter = 0;

					/* Copy all changeables from condition, test, body, terminal */
					addShChangeables(node->init_statement);
					addShChangeables(node->condition);
					addShChangeables(node->body);
					addShChangeables(node->rest_expression);
				}
				return false;
			case ast_dbg_loop_qyr_test:
				VPRINT( 3, "\t ------- unset target --------\n");
				SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_loop_wrk_test,
										DBG_RS_POSITION_UNSET)

				if (this->dbgBehaviour & DBG_BH_JUMPINTO) {
					// visit test
					if (node->condition)
						node->condition->accept(this);
					if (this->operation == OTOpTargetSet)
						addShChangeables(node->condition);
				} else {
					/* Finish debugging this loop */
					node->debug_state_internal = ast_dbg_loop_unset;
					node->debug_iter = 0;

					/* Copy all changeables from test, body, terminal */
					addShChangeables(node->condition);
					addShChangeables(node->body);
					addShChangeables(node->rest_expression);
				}
				return false;
			case ast_dbg_loop_select_flow:
				VPRINT( 3, "\t ------- unset target again --------\n");
				SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_loop_wrk_test,
										DBG_RS_POSITION_UNSET)

				if (this->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER) {
					/* Finish debugging this loop */
					node->debug_state_internal = ast_dbg_loop_unset;
					node->debug_iter = 0;

					/* Copy all changeables from test, body, terminal */
					addShChangeables(node->condition);
					addShChangeables(node->body);
					addShChangeables(node->rest_expression);
					return false;
				} else if (this->dbgBehaviour & DBG_BH_LOOP_NEXT_ITER) {
					/* Perform one iteration without further debugging
					 *  - target stays the same
					 *  - increase loop iter counter
					 *  - change traverse to be finished
					 *  - prepare result
					 */

					/* Reset target */
					node->debug_state = ast_dbg_state_target;
					node->debug_state_internal = ast_dbg_loop_select_flow;

					/* Increase iteration */
					node->debug_iter++;

					/* Finish debugging */
					this->operation = OTOpDone;

					/* Build result struct */
					this->result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					this->result.loopIteration = node->debug_iter;
					if (node->mode == ast_iteration_statement::ast_do_while)
						result.loopIteration = node->debug_iter - 1;
					else
						result.loopIteration = node->debug_iter;
					setDbgResultRange(result.range, node->get_location());

					addShChangeables(node->condition);
					addShChangeables(node->body);
					addShChangeables(node->rest_expression);
					setGobalScope(&node->scope);
				} else {
					node->debug_state_internal = ast_dbg_loop_wrk_body;
				}
				return true;
			case ast_dbg_loop_qyr_terminal:
				VPRINT( 3, "\t ------- unset target --------\n");
				SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_loop_wrk_terminal,
										DBG_RS_POSITION_UNSET)
				// visit terminal
				if (this->dbgBehaviour & DBG_BH_JUMPINTO)
					if (node->rest_expression)
						node->rest_expression->accept(this);

				if (this->operation == OTOpTargetSet)
					addShChangeables(node->rest_expression);
				return false;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case OTOpTargetSet: {
		if (node->debug_state != ast_dbg_state_unset)
			break;

		if (node->debug_state_internal == ast_dbg_loop_unset
				&& node->mode == ast_iteration_statement::ast_do_while) {
			/* Process body imediately */
			node->debug_state_internal = ast_dbg_loop_wrk_body;
			return true;
		}

		if (node->debug_state_internal == ast_dbg_loop_unset
				|| (node->debug_state_internal >= ast_dbg_loop_wrk_init
						&& node->debug_state_internal <= ast_dbg_loop_wrk_terminal)) {
			VPRINT(3, "\t -------- set target ---------\n");
			node->debug_state = ast_dbg_state_target;
			this->operation = OTOpDone;

			switch (node->debug_state_internal) {
			case ast_dbg_loop_unset: {
				if (node->init_statement) {
					node->debug_state_internal = ast_dbg_loop_qyr_init;
					result.position = loop_position(node->mode);
				} else if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					result.loopIteration = node->debug_iter;
				}
				break;
			}
			case ast_dbg_loop_wrk_init: {
				if (node->condition)
					node->debug_state_internal = ast_dbg_loop_qyr_test;
				else
					node->debug_state_internal = ast_dbg_loop_select_flow;
				result.position = DBG_RS_POSITION_LOOP_FOR;
				break;
			}
			case ast_dbg_loop_wrk_test: {
				node->debug_state_internal = ast_dbg_loop_select_flow;
				result.position = DBG_RS_POSITION_LOOP_CHOOSE;
				if (node->mode == ast_iteration_statement::ast_do_while)
					result.loopIteration = node->debug_iter - 1;
				else
					result.loopIteration = node->debug_iter;
				break;
			}
			case ast_dbg_loop_wrk_body: {
				if (node->rest_expression) {
					node->debug_state_internal = ast_dbg_loop_qyr_terminal;
					result.position = loop_position(node->mode);
				} else if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
					/* Increase the loop counter */
					node->debug_iter++;
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					if (node->mode == ast_iteration_statement::ast_do_while)
						result.loopIteration = node->debug_iter - 1;
					else
						result.loopIteration = node->debug_iter;
					/* Increase the loop counter */
					node->debug_iter++;
				}
				break;
			}
			case ir_dbg_loop_wrk_terminal: {
				if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					result.loopIteration = node->debug_iter;
				}
				/* Increase the loop counter */
				node->debug_iter++;
				break;
			}
			default:
				break;
			}
			setDbgResultRange(result.range, node->get_location());
			setGobalScope(&node->scope);
			return false;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

