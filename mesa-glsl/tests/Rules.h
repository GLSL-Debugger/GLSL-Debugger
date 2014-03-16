/*
 * Rules.h
 *
 *  Created on: 07.03.2014.
 */
#ifndef TEST_RULES_H_
#define TEST_RULES_H_

#include "ast.h"
#include "call_visitor.h"
#include <string>
#include <map>

struct DebugState {
	enum ast_dbg_state state;
	enum ast_dbg_overwrite overwrite;
	int state_internal;
	int iter;
	char* iter_name;
};

typedef std::map<ast_node*, DebugState> StatesMap;

class TestRule: public CallAcceptor {
public:
	enum types {
		tr_none = 0,
		tr_jump = 1,
		tr_save = 2,
		tr_load = 4,
		tr_bhvr = 8,
	};

	void load(int, std::string);

	bool stateAction()
	{
		return type & (tr_save | tr_load);
	}

	bool jumpAction()
	{
		return type & tr_jump;
	}

	bool bhvAction()
	{
		return type & tr_bhvr;
	}

	int getJump()
	{
		return jump_to;
	}

	int getBehaviour()
	{
		return behaviour;
	}


	virtual void apply(StatesMap* state, AstShader* sh)
	{
		if (!type & (tr_load | tr_save))
			return;
		states = state;
		if (type == tr_save)
			states->clear();
		ast_call_visitor call(this);
		call.visit(sh->head);
	}

	virtual bool accept(int, ast_node* node, enum ast_node_type node_type)
	{
		if (type & tr_save) {

			saveState(node, node_type, (*states)[node]);
		} else if (type & tr_load) {
			auto it = states->find(node);
			if (it != states->end())
				loadState(node, node_type, it->second);
		}
		return true;
	}

	void saveState(ast_node* node, enum ast_node_type t, DebugState& state)
	{
		state.state = node->debug_state;
		state.overwrite = node->debug_overwrite;
		state.state_internal = 0;
		state.iter = 0;
		if (state.iter_name)
			free(state.iter_name);
		state.iter_name = NULL;

		switch (t) {
		case AST_SELECTION_STATEMENT: {
			ast_selection_statement* ss = (ast_selection_statement*) node;
			state.state_internal = ss->debug_state_internal;
			break;
		}
		case AST_SWITCH_STATEMENT: {
			ast_switch_statement* sw = (ast_switch_statement*) node;
			state.state_internal = sw->debug_state_internal;
			break;
		}
		case AST_ITERATION_STATEMENT: {
			ast_iteration_statement* sl = (ast_iteration_statement*) node;
			state.state_internal = sl->debug_state_internal;
			state.iter = sl->debug_iter;
			if (sl->debug_iter_name)
				state.iter_name = strdup(sl->debug_iter_name);
			break;
		}
		default:
			break;
		}
	}

	void loadState(ast_node* node, enum ast_node_type t, DebugState& state)
	{
		node->debug_state = state.state;
		node->debug_overwrite = state.overwrite;
		switch (t) {
		case AST_SELECTION_STATEMENT: {
			ast_selection_statement* ss = (ast_selection_statement*) node;
			ss->debug_state_internal =
					static_cast<enum ast_dbg_state_internal_if>(state.state_internal);
			break;
		}
		case AST_SWITCH_STATEMENT: {
			ast_switch_statement* sw = (ast_switch_statement*) node;
			sw->debug_state_internal =
					static_cast<enum ast_dbg_state_internal_switch>(state.state_internal);
			break;
		}
		case AST_ITERATION_STATEMENT: {
			ast_iteration_statement* sl = (ast_iteration_statement*) node;
			sl->debug_state_internal =
					static_cast<enum ast_dbg_state_internal_loop>(state.state_internal);
			sl->debug_iter = state.iter;
			if (sl->debug_iter_name)
				free(sl->debug_iter_name);
			sl->debug_iter_name = NULL;
			if (state.iter_name)
				sl->debug_iter_name = strdup(state.iter_name);
			break;
		}
		default:
			break;
		}
	}

protected:
	StatesMap* states;
	int line;
	int type;
	int jump_to;
	int behaviour;
};

#endif /* TEST_RULES_H_ */
