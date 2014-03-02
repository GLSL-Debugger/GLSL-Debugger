/*
 * debugjump.cpp
 *
 *  Created on: 10.10.2013
 */

#include "debugjump.h"
#include "ShaderLang.h"
#include "glsl/list.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"

#define NOT_DEBUGGABLE_RETURN \
	if( this->operation != OTOpPathClear && \
    	this->operation != OTOpPathBuild && \
    	this->operation != OTOpReset ) return

#define DEFAULT_DEBUGABLE(ir)  \
	if( this->operation == OTOpPathClear || \
    	this->operation == OTOpPathBuild || \
    	this->operation == OTOpReset ) processDebugable(ir, &this->operation);

static void setDbgResultRange(DbgRsRange& r, const YYLTYPE& range)
{
	r.left.line = range.first_line;
	r.left.colum = range.first_column;
	r.right.line = range.last_line;
	r.right.colum = range.last_column;
}

void ast_debugjump_traverser_visitor::setGobalScope(exec_list *s)
{
	setDbgScope(this->result.scope, s, shader);
	/* Add local scope to scope stack*/
	addScopeToScopeStack(this->result.scopeStack, s, shader);
}

void ast_debugjump_traverser_visitor::processDebugable(ast_node *node, OTOperation *op)
// Default handling of a node that can be debugged
{
	VPRINT(3, "process Debugable L:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), *op, node->debug_state);

	switch (*op) {
	case OTOpTargetUnset:
		if (node->debug_state == ast_dbg_state_target) {
			node->debug_state = ast_dbg_state_unset;
			*op = OTOpTargetSet;
			VPRINT( 3, "\t ------- unset target --------\n");
			result.position = DBG_RS_POSITION_UNSET;
		}
		break;
	case OTOpTargetSet:
		switch (node->debug_state) {
		case ast_dbg_state_target:
			assert(!"ERROR! found target with DbgStTarget\n");
			break;
		case ast_dbg_state_unset:
			node->debug_state = ast_dbg_state_target;
			*op = OTOpDone;
			VPRINT(3, "\t -------- set target ---------\n");
//			switch (node->ast_type) {
//			case ast_type_assignment:
//				result.position = DBG_RS_POSITION_ASSIGMENT;
//				setDbgResultRange(result.range, node->yy_location);
//           		setGobalScope(node);
//				break;
//			case ast_type_expression: {
//				ast_expression* exp = node->as_expression();
//				setDbgResultRange(result.range, node->yy_location);
//           		setGobalScope( get_scope(node) );
//				if (exp->operation < ast_last_unop) {
//					result.position = DBG_RS_POSITION_UNARY;
//				} else if (exp->operation < ast_last_binop) {
//					// TODO: WHY?
//					result.position = DBG_RS_POSITION_ASSIGMENT;
//				} else {
//					// Dunno, lol
//				}
//				break;
//			}
//			case ast_type_dummy: {
//				result.position = DBG_RS_POSITION_DUMMY;
//				setDbgResultRange(result.range, node->yy_location);
////                    		setGobalScope( get_scope(node) );
//				break;
//			}
//			default:
//				break;
//			}
//			break;
		default:
			break;
		}
		break;
	case OTOpPathClear:
		if (node->debug_state == ast_dbg_state_path)
			node->debug_state = ast_dbg_state_unset;
		break;
	case OTOpPathBuild:
		switch (node->debug_state) {
		case ast_dbg_state_unset: {
//			/* Check children for DebugState */
//			newState = ast_dbg_state_unset;
//			switch (node->ast_type) {
//			case ast_type_assignment: {
//				ast_assignment* as = node->as_assignment();
//				if (as->rhs->debug_target != ast_dbg_state_unset
//						|| as->lhs->debug_target != ast_dbg_state_unset)
//					newState = ast_dbg_state_path;
//				break;
//			}
//			case ast_type_expression: {
//				ast_expression* exp = node->as_expression();
//				unsigned ops = exp->get_num_operands();
//				for (unsigned i = 0; i < ops; ++i) {
//					if (exp->operands[i] && exp->operands[i]->debug_state != ast_dbg_state_unset)
//						newState = ast_dbg_state_path;
//				}
//				break;
//			}
//			default:
//				break;
//			}
//			node->debug_state = newState;
			break;
		}
		default:
			break;
		}
		break;
	case OTOpReset:
		node->debug_state = ast_dbg_state_unset;
		break;
	default:
		break;
	}
}

void ast_debugjump_traverser_visitor::leave(class ast_declaration* node)
{
	VPRINT(2, "process Declaration L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);
	NOT_DEBUGGABLE_RETURN;
	OTOperation old_oper = this->operation;
	processDebugable(node, &this->operation);
	if (node->debug_state != ast_dbg_state_unset && old_oper != OTOpPathBuild)
		return;

	/* Check optional initialization */
	if (node->initializer && node->initializer->debug_state != ast_dbg_state_unset)
		node->debug_state = ast_dbg_state_path;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_expression* ir)
{
	VPRINT( 2,
			"process Expression L:%s DbgSt:%i\n", FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);

	DEFAULT_DEBUGABLE( ir)
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_swizzle* ir)
{
	DEFAULT_DEBUGABLE( ir)
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_assignment* ir)
{
	VPRINT( 2,
			"process Assignment L:%s DbgSt:%i\n", FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);

	processDebugable(ir, &this->operation);

	if (this->operation == OTOpPathBuild || this->operation == OTOpDone)
		return false;
	else if (this->operation == OTOpTargetSet) {
		if (!(this->dbgBehaviour & DBG_BH_JUMPINTO)) {
			// do not visit children
			// add all changeables of this node to the list
//			copyShChangeableList( &result.cgbls, get_changeable_list( ir ) );
			checkReturns(ir, this);
		} else {
			// visit children
			++this->depth;
			if (ir->rhs)
				ir->rhs->accept(this);

			// Since there cannot be a target in the left side anyway
			// it would be possible to skip traversal
			if (ir->lhs)
				ir->lhs->accept(this);
			--this->depth;

			// if no target was found so far
			// all changeables need to be added to the list
			if (this->operation == OTOpTargetSet) {
//				copyShChangeableList( &result.cgbls, get_changeable_list( ir ) );
				checkReturns(ir, this);
			}
		}
		return false;
	} else if (OTOpTargetUnset) {
		// visit children
		++this->depth;
		if (ir->rhs)
			ir->rhs->accept(this);

		// Since there cannot be a target in the left side anyway
		// it would be possible to skip traversal
		if (ir->lhs)
			ir->lhs->accept(this);
		--this->depth;

		// the old target was found inside left/right branch and
		// a new one is still being searched for
		// -> add only changed variables of this assigment, i.e.
		//    changeables of the left branch
		if (this->operation == OTOpTargetSet) {
//			copyShChangeableList( &result.cgbls, get_changeable_list( ir->lhs ) );
			checkReturns(ir, this);
		}
		return false;
	}

	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_constant* ir)
{
	VPRINT(2,
			"process Constant L:%s DbgSt:%i\n", FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);
	DEFAULT_DEBUGABLE(ir)
	return true;
}


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

void ast_debugjump_traverser_visitor::leave(class ast_expression* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_expression_bin* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_function_expression* node)
{
	ast_expression* identifier = node->subexpressions[0];
	assert(identifier);

	const char* func_name = identifier->primary_expression.identifier;
	ast_function_definition* f = getFunctionByName(func_name);
	assert(f || !"Function not found");

	VPRINT(2, "process Call L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(),
			func_name, node->debug_builtin, this->operation, node->debug_state);

	switch (this->operation) {
	case OTOpTargetUnset: {
		if (node->debug_state != ast_dbg_state_target)
			break;

		if (this->dbgBehaviour == DBG_BH_JUMPINTO) {
			// no changeable has to be copied in first place,
			// as we jump into this function

			VPRINT(2, "\t ---- push %p on stack ----\n", f);
			this->parseStack.push(f);
			this->operation = OTOpTargetSet;

			// add local parameters of called function first
			copyShChangeableListCtx(&result.cgbls, &f->prototype->changeables, shader);
			f->accept(this);

			// if parsing ends up here and a target is still beeing
			// searched, a wierd function was called, but anyway,
			// let's copy the appropriate changeables
			if (this->operation == OTOpTargetSet)
				copyShChangeableListCtx(&result.cgbls, &node->changeables, shader);
		} else {
			node->debug_state = ir_dbg_state_unset;
			this->operation = OTOpTargetSet;
			result.position = DBG_RS_POSITION_UNSET;
			VPRINT( 3, "\t ------- unset target --------\n");

			// if parsing of the subfunction finished right now
			// -> copy only changed parameters to changeables
			// else
			// -> copy all, since user wants to jump over this func
			if (this->finishedDbgFunction == true) {
				copyShChangeableListCtx(&result.cgbls, &node->changeable_params, shader);
				this->finishedDbgFunction = false;
			} else {
				copyShChangeableListCtx(&result.cgbls, &node->changeables, shader);
				checkReturns(f);
			}
		}
		break;
	}
	case OTOpTargetSet: {
		if (node->debug_state == ast_dbg_state_target) {
			assert(!"ERROR! found target with DbgStTarget\n");
		} else if (node->debug_state == ast_dbg_state_unset) {
			if (!node->debug_builtin) {
				node->debug_state = ast_dbg_state_target;
				VPRINT( 3, "\t -------- set target ---------\n");
				result.position = DBG_RS_POSITION_FUNCTION_CALL;
				setDbgResultRange(result.range, node->get_location());
				setGobalScope(&node->scope);
				this->operation = OTOpDone;
			} else {
				checkReturns(f);
			}
		}
		break;
	}
	default:
		processDebugable(node, &this->operation);
		if (this->operation == OTOpPathBuild && node->debug_state == ast_dbg_state_unset)
			if (f->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
		break;
	}
}

void ast_debugjump_traverser_visitor::leave(class ast_expression_statement* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_compound_statement* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_declarator_list* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_case_statement* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_case_statement_list* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_switch_body* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_switch_statement* node)
{
}

void ast_debugjump_traverser_visitor::leave(class ast_function_definition* node)
{
	VPRINT( 2, "process function definition L:%s N:%s Op:%i DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(),
			node->prototype->identifier, this->operation, node->debug_state);

	if (this->operation == OTOpTargetSet) {
		/* This marks the end of a function call */
		assert(node == this->parseStack.top() || !"ERROR! unexpected stack order\n");

		VPRINT( 2, "\t ---- pop %p from stack ----\n", node);
		this->parseStack.pop();
		/* Do not directly jump into next function after
		 * returning from a function */
		this->dbgBehaviour &= ~DBG_BH_JUMPINTO;
		this->finishedDbgFunction = true;

		if (!this->parseStack.empty()) {
			ast_node* top = this->parseStack.top();
			VPRINT( 2, "\t ---- continue parsing at %pk ----\n", top);
			this->operation = OTOpTargetUnset;
			top->accept(this);
		} else {
			VPRINT( 2, "\t ---- stack empty, finished ----\n");
			this->operation = OTOpFinished;
		}
	} else if (this->operation != OTOpTargetUnset){
		processDebugable(node, &this->operation);
		if (this->operation == OTOpPathBuild && node->body) {
			VPRINT(6, "getDebugState: %i\n", node->body->debug_state);
			if (node->body->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
		}
	}
}

void ast_debugjump_traverser_visitor::leave(ast_jump_statement* node)
{
	NOT_DEBUGGABLE_RETURN;
	OTOperation old_oper = this->operation;
	processDebugable(node, &this->operation);
	if (node->debug_state != ast_dbg_state_unset)
		return;
	if (old_oper == OTOpTargetSet){
		result.position = DBG_RS_POSITION_BRANCH;
		setDbgResultRange(result.range, node->get_location());
		setGobalScope(&node->scope);
	} else if (old_oper == OTOpPathBuild) {
		if (node->opt_return_value
				&& node->opt_return_value->debug_state != ast_dbg_state_unset)
			node->debug_state = ast_dbg_state_path;
	}
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

#define SET_OPERATION(op, ds, dsi, pos) 	\
		this->operation = op;				\
		node->debug_state = ds;				\
		node->debug_state_internal = dsi;	\
		result.position = pos;

bool ast_debugjump_traverser_visitor::enter(class ast_selection_statement* node)
{
	VPRINT( 2, "process Selection L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	switch (this->operation) {
	case OTOpTargetUnset:
		switch (node->debug_state) {
		case ast_dbg_state_target:
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
							(node->else_statement & ast_dbg_se_discard)) {
						this->discardPassed = true;
						VPRINT( 6, "passed Discard %i\n", __LINE__);
					}
				}
				return true;
			}
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;
	case OTOpTargetSet:
		if (node->debug_state == ast_dbg_state_unset) {
			switch (node->debug_state_internal) {
			case ast_dbg_if_unset:
				VPRINT( 3, "\t -------- set target ---------\n");
				SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_if_init,
						node->else_statement ?
							DBG_RS_POSITION_SELECTION_IF_ELSE :
							DBG_RS_POSITION_SELECTION_IF)
				setDbgResultRange(result.range, node->get_location());
				setGobalScope(&node->scope);
				return false;
			case ast_dbg_if_condition:
				VPRINT( 3, "\t -------- set target again ---------\n");
				SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_if_condition_passed,
						node->else_statement ?
								DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE :
								DBG_RS_POSITION_SELECTION_IF_CHOOSE)
				setDbgResultRange(result.range, node->get_location());
				setGobalScope(&node->scope);
				return false;
			case ast_dbg_if_then:
			case ast_dbg_if_else: {
				VPRINT( 4, "\t -------- set condition pass ---------\n");
				// Debugging of condition finished! Take care of the
				// non-debugged branch - if there is one - and copy
				// it's changeables!
				ast_node* path = node->debug_state_internal == ast_dbg_if_then ?
								node->then_statement : node->else_statement;
				addShChangeables(path);
				node->debug_state_internal = ast_dbg_if_unset;
				return false;
			}
			default:
				break;
			}
		}
		break;
	case OTOpPathClear:
		/* Conditional is intentionally not visited by post-traverser */
		node->condition->accept(this);
		if (node->debug_state == ast_dbg_state_path)
			node->debug_state = ast_dbg_state_unset;
		return true;
	case OTOpPathBuild:
		if (node->debug_state == ast_dbg_state_unset) {
			/* Check conditional and branches */
			if (dbg_state_not_match(node->then_statement, ast_dbg_state_unset)
					|| dbg_state_not_match(node->else_statement, ast_dbg_state_unset)
					|| node->condition->debug_state != ast_dbg_state_unset)
				node->debug_state = ast_dbg_state_path;
		}
		return false;
	case OTOpReset:
		/* Conditional is intentionally not visited by post-traverser */
		if (node->condition)
			node->condition->accept(this);
		if (node->then_statement)
			node->then_statement->accept(this);
		if (node->else_statement)
			node->else_statement->accept(this);
		node->debug_state = ast_dbg_state_unset;
		node->debug_state_internal = ast_dbg_if_unset;
		return false;
	default:
		break;
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
				dbgPrint( DBGLVL_ERROR, "CodeTools - target but state unset\n");
				exit(1);
				break;
			case ir_dbg_loop_qyr_init:
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
			case ir_dbg_loop_qyr_test:
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
			case ir_dbg_loop_select_flow:
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
			case ir_dbg_loop_qyr_terminal:
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
	case OTOpTargetSet:
		if (node->debug_state == ast_dbg_state_unset) {

			if (node->debug_state_internal == ast_dbg_loop_unset &&
					node->mode == ast_iteration_statement::ast_do_while) {
				/* Process body imediately */
				node->debug_state_internal = ast_dbg_loop_wrk_body;
				return true;
			}

			if (node->debug_state_internal == ast_dbg_loop_unset
					|| (node->debug_state_internal >= ast_dbg_loop_wrk_init
							&& node->debug_state_internal <= ast_dbg_loop_wrk_terminal)) {
				VPRINT( 3, "\t -------- set target ---------\n");
				node->debug_state = ast_dbg_state_target;
				this->operation = OTOpDone;

				switch (node->debug_state_internal) {
				case ast_dbg_loop_unset: {
					if (node->init_statement) {
						node->debug_state_internal = ast_dbg_loop_qyr_init;
						result.position = loop_position(node->mode);
					} else if (node->condition) {
						node->debug_state_internal =ast_dbg_loop_qyr_test;
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
		}
		break;
	case OTOpPathClear:
		if (node->debug_state == ast_dbg_state_path)
			node->debug_state = ast_dbg_state_unset;
		return true;
	case OTOpPathBuild:
		if (node->debug_state == ast_dbg_state_unset) {
			/* Check init, test, terminal, and body */
			if (dbg_state_not_match(node->init_statement, ast_dbg_state_unset)
					|| dbg_state_not_match(node->condition, ast_dbg_state_unset)
					|| dbg_state_not_match(node->rest_expression, ast_dbg_state_unset)
					|| dbg_state_not_match(node->body, ast_dbg_state_unset))
				node->debug_state = ir_dbg_state_path;
		}
		return false;
	case OTOpReset:
		if (node->init_statement)
			node->init_statement->accept(this);
		if (node->condition)
			node->condition->accept(this);
		if (node->rest_expression)
			node->rest_expression->accept(this);
		if (node->body)
			node->body->accept(this);
		node->debug_state = ast_dbg_state_unset;
		node->debug_state_internal = ast_dbg_loop_unset;
		/* Reset loop counter */
		node->debug_iter = 0;
		return false;
	default:
		break;
	}

	return true;
}

