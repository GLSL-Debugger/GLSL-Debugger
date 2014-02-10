/*
 * ast.h
 *
 *  Created on: 05.02.2014
 */

#ifndef AST_TRAVERSE_VISITOR_H_
#define AST_TRAVERSE_VISITOR_H_

enum ast_traverser_status {
	traverse_continue,
	traverse_continue_with_parent,
	traverse_stop,
};

enum ast_traverse_flags {
	traverse_previsit = 1,
	traverse_postvisit = 2,
	traverse_debugvisit = 4
};


class ast_traverse_visitor {
public:
	virtual ast_traverse_visitor() :
			depth(0), flags(traverse_previsit), skipInternal(true)
	{
	}

	virtual ~ast_traverse_visitor()
	{
	}

	virtual void visit(class ast_node *) { assert(!"unhandled error_type"); }
	virtual void visit(class ast_expression *);
	virtual void visit(class ast_expression_bin *);
	virtual void visit(class ast_function_expression *);
	virtual void visit(class ast_array_specifier *);
	virtual void visit(class ast_aggregate_initializer *);
	virtual void visit(class ast_compound_statement *);
	virtual void visit(class ast_declaration *);
	virtual void visit(class ast_struct_specifier *);
	virtual void visit(class ast_type_specifier *);
	virtual void visit(class ast_fully_specified_type *);
	virtual void visit(class ast_declarator_list *);
	virtual void visit(class ast_parameter_declarator *);
	virtual void visit(class ast_function *);
	virtual void visit(class ast_expression_statement *);
	virtual void visit(class ast_case_label *);
	virtual void visit(class ast_case_label_list *);
	virtual void visit(class ast_case_statement *);
	virtual void visit(class ast_case_statement_list *);
	virtual void visit(class ast_switch_body *);
	virtual void visit(class ast_selection_statement *);
	virtual void visit(class ast_switch_statement *);
	virtual void visit(class ast_iteration_statement *);
	virtual void visit(class ast_jump_statement *);
	virtual void visit(class ast_function_definition *);
	virtual void visit(class ast_interface_block *);
	virtual void visit(class ast_gs_input_layout *);

	virtual ast_traverser_status traverse(class ast_node *) { return traverse_stop; }
	virtual ast_traverser_status traverse(class ast_expression *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_expression_bin *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_function_expression *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_array_specifier *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_aggregate_initializer *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_compound_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_declaration *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_struct_specifier *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_type_specifier *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_fully_specified_type *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_declarator_list *) { assert(!"unhandled error_type"); }
	virtual ast_traverser_status traverse(class ast_parameter_declarator *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_function *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_expression_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_case_label *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_case_label_list *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_case_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_case_statement_list *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_switch_body *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_selection_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_switch_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_iteration_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_jump_statement *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_function_definition *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_interface_block *) { return traverse_continue; }
	virtual ast_traverser_status traverse(class ast_gs_input_layout *) { return traverse_continue; }

	int depth;
	ast_traverse_flags flags;
	int depth;
	bool skipInternal;
};

#endif /* AST_TRAVERSE_VISITOR_H_ */
