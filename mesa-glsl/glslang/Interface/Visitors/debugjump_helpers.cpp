/*
 * debugjump_helpers.cpp
 *
 *  Created on: 17.03.2014.
 */

#include "debugjump.h"
#include "glsl/ast.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"
#include <assert.h>

// In the main debugjump.cpp
extern void setDbgResultRange(DbgRsRange& r, const YYLTYPE& range);

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

			if (this->dbgBehaviour & DBG_BH_JUMP_OVER) {
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
					if (node->then_statement->debug_sideeffects & ast_dbg_se_discard) {
						this->result.passedDiscard = true;
						VPRINT(6, "passed Discard %i\n", __LINE__);
					}
				}
			} else {
				node->debug_state_internal = ast_dbg_if_then;
				// check other branch for discards
				if (node->else_statement &&
						(node->else_statement->debug_sideeffects & ast_dbg_se_discard)) {
					this->result.passedDiscard = true;
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
			setDbgResultRange(result.range, node->condition->get_location());
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


bool ast_debugjump_traverser_visitor::enter(class ast_switch_statement* node)
{
	VPRINT( 2, "process Switch L:%s DbgSt:%i\n",
			FormatSourceRange(node->get_location()).c_str(), node->debug_state);

	if (operation == OTOpTargetUnset) {
		if (node->debug_state != ast_dbg_state_target)
			return true;

		switch (node->debug_state_internal) {
		case ast_dbg_switch_unset:
			assert(!"CodeTools - target but state unset\n");
			break;
		case ast_dbg_switch_init: {
			VPRINT( 3, "\t ------- unset target --------\n");
			SET_OPERATION(OTOpTargetSet, ast_dbg_state_unset, ast_dbg_switch_condition,
					DBG_RS_POSITION_UNSET)

			if (this->dbgBehaviour & DBG_BH_JUMPINTO) {
				// visit condition
				if (node->test_expression)
					node->test_expression->accept(this);

				// there was not target in condition, so copy all changeables;
				// it's unlikely that there is a changeable and no target,
				// but anyway be on the safe side
				if (this->operation == OTOpTargetSet)
					addShChangeables(node->test_expression);
				return false;
			} else {
				/* Finish debugging this selection */
				node->debug_state_internal = ast_dbg_switch_unset;
				/* copy changeables */
				addShChangeables(node->test_expression);
				addShChangeables(node->body);
				return false;
			}
			break;
		}
		case ast_dbg_switch_condition_passed: {
			VPRINT( 3, "\t ------- unset target again --------\n");
			node->debug_state = ast_dbg_state_unset;
			this->operation = OTOpTargetSet;
			result.position = DBG_RS_POSITION_UNSET;

			if (this->dbgBehaviour & DBG_BH_JUMP_OVER) {
				/* Finish debugging this selection */
				node->debug_state_internal = ast_dbg_switch_unset;
				/* copy changeables */
				addShChangeables(node->body);
				return false;
			} else {
				node->debug_state_internal = ast_dbg_switch_branch;
				if (node->body->debug_sideeffects & ast_dbg_se_discard) {
					this->result.passedDiscard = true;
					VPRINT(6, "passed Discard %i\n", __LINE__);
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
		case ast_dbg_switch_unset:
			VPRINT( 3, "\t -------- set target ---------\n");
			SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_switch_init,
					DBG_RS_POSITION_SWITCH)
			setDbgResultRange(result.range, node->get_location());
			setGobalScope(&node->scope);
			return false;
		case ast_dbg_switch_condition:
			VPRINT( 3, "\t -------- set target again ---------\n");
			SET_OPERATION(OTOpDone, ast_dbg_state_target, ast_dbg_switch_condition_passed,
					DBG_RS_POSITION_SWITCH_CHOOSE)
			setDbgResultRange(result.range, node->test_expression->get_location());
			setGobalScope(&node->scope);
			return false;
		case ast_dbg_switch_branch: {
			VPRINT( 4, "\t -------- set condition pass ---------\n");
			// Debugging of condition finished! Take care of the
			// non-debugged branch - if there is one - and copy
			// it's changeables!
			addShChangeables(node->body);
			node->debug_state_internal = ast_dbg_switch_unset;
			// Unset debug branch
			ast_switch_body* body = NULL;
			if (node->body)
				body = node->body->as_switch_body();
			if (body && body->stmts)
				body->stmts->debug_branch = 0;
			return false;
		}
		default:
			break;
		}
	}
	return true;
}

bool ast_debugjump_traverser_visitor::enter(class ast_case_statement_list* node)
{
	VPRINT(2, "process SwitchBody L:%s DbgSt:%i\n", FormatSourceRange(node->get_location()).c_str(),
			node->debug_state);

	if (operation != OTOpTargetSet && operation != OTOpTargetUnset)
		return true;

	// Process only debugged branch
	int branch = this->dbgBehaviour & DBG_BH_SWITCH_BRANCH_LAST;
	if (branch)
		node->debug_branch = branch;

	if (node->debug_branch) {
		int current_branch = 0;
		bool no_break = false;
		foreach_list_typed(ast_node, ast, link, &node->cases) {
			if (++current_branch == node->debug_branch || no_break) {
				ast->accept(this);
				no_break = !(ast->debug_sideeffects & (ast_dbg_se_break | ast_dbg_se_return));
			}
			if (current_branch > DBG_BH_SWITCH_BRANCH_LAST)
				dbgPrint(DBGLVL_WARNING, "Only %i switch branches supported, but %i found\n",
						DBG_BH_SWITCH_BRANCH_LAST, current_branch);
		}
	}
	return false;
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

				if (this->dbgBehaviour & DBG_BH_JUMP_OVER) {
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
			YYLTYPE loc;

			switch (node->debug_state_internal) {
			case ast_dbg_loop_unset: {
				if (node->init_statement) {
					node->debug_state_internal = ast_dbg_loop_qyr_init;
					result.position = loop_position(node->mode);
					loc = node->init_statement->get_location();
				} else if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
					loc = node->condition->get_location();
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					result.loopIteration = node->debug_iter;
					loc = node->get_location();
				}
				break;
			}
			case ast_dbg_loop_wrk_init: {
				if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					loc = node->condition->get_location();
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					loc = node->get_location();
				}
				result.position = DBG_RS_POSITION_LOOP_FOR;
				break;
			}
			case ast_dbg_loop_wrk_test: {
				node->debug_state_internal = ast_dbg_loop_select_flow;
				result.position = DBG_RS_POSITION_LOOP_CHOOSE;
				loc = node->condition->get_location();
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
					loc = node->rest_expression->get_location();
				} else if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
					loc = node->condition->get_location();
					/* Increase the loop counter */
					node->debug_iter++;
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					loc = node->get_location();
					if (node->mode == ast_iteration_statement::ast_do_while)
						result.loopIteration = node->debug_iter - 1;
					else
						result.loopIteration = node->debug_iter;
					/* Increase the loop counter */
					node->debug_iter++;
				}
				break;
			}
			case ast_dbg_loop_wrk_terminal: {
				if (node->condition) {
					node->debug_state_internal = ast_dbg_loop_qyr_test;
					result.position = loop_position(node->mode);
					loc = node->condition->get_location();
				} else {
					node->debug_state_internal = ast_dbg_loop_select_flow;
					result.position = DBG_RS_POSITION_LOOP_CHOOSE;
					loc = node->get_location();
					result.loopIteration = node->debug_iter;
				}
				/* Increase the loop counter */
				node->debug_iter++;
				break;
			}
			default:
				break;
			}
			setDbgResultRange(result.range, loc);
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
