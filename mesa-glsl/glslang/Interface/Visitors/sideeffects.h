/*
 * sideeffects.h
 *
 *  Created on: 18.10.2013
 */

#ifndef SIDEEFFECTS_H_
#define SIDEEFFECTS_H_

#include "ast_visitor.h"
#include "traverse.h"

class ast_sideeffects_traverser_visitor : public ast_traverse_visitor {
public:
	ast_sideeffects_traverser_visitor(struct _mesa_glsl_parse_state* _st) :
			state(_st)
	{
		flags = traverse_postvisit;
	}

	virtual ~ast_sideeffects_traverser_visitor()
	{
	}

	virtual void visit(exec_list *l) { ast_traverse_visitor::visit(l); }
	virtual void visit(ast_declarator_list *);
	virtual void visit(ast_compound_statement *);
	virtual void visit(ast_expression_statement *);

	virtual bool traverse(ast_expression *);
	virtual bool traverse(ast_expression_bin *);
	virtual bool traverse(ast_function_expression *);
	virtual bool traverse(ast_case_statement *);
	virtual bool traverse(ast_case_statement_list *);
	virtual bool traverse(ast_switch_body *);
	virtual bool traverse(ast_selection_statement *);
	virtual bool traverse(ast_switch_statement *);
	virtual bool traverse(ast_iteration_statement *);
	virtual bool traverse(ast_jump_statement *);
	virtual bool traverse(ast_function_definition *);
	virtual bool traverse(ast_gs_input_layout *);

protected:
	struct _mesa_glsl_parse_state* state;
};

class ir_sideeffects_traverser_visitor : public ir_traverse_visitor {
public:
	ir_sideeffects_traverser_visitor()
	{
		preVisit = false;
		postVisit = true;
	}

	virtual ~ir_sideeffects_traverser_visitor()
	{
	}

	virtual bool visitIr(ir_function_signature *ir);
	virtual bool visitIr(ir_function *ir);
	virtual bool visitIr(ir_expression *ir);
	virtual bool visitIr(ir_assignment *ir);
	virtual bool visitIr(ir_call *ir);
	virtual bool visitIr(ir_discard *ir);
	virtual bool visitIr(ir_if *ir);
	virtual bool visitIr(ir_loop *ir);
	virtual bool visitIr(ir_emit_vertex *ir);

	int list_sideeffects(exec_list* instructions);
	int block_sideeffects(ir_dummy* first);

};


#endif /* SIDEEFFECTS_H_ */
