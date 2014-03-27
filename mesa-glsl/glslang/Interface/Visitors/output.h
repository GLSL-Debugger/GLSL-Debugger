/*
 * output.h
 *
 *  Created on: 14.10.2013
 */

#ifndef __OUTPUT_TRAVERSER_H_
#define __OUTPUT_TRAVERSER_H_

struct global_print_tracker;

#include "glsl/ast_visitor.h"
#include "ShaderLang.h"
#include "glslang/Interface/CodeInsertion.h"

class ast_output_traverser_visitor: public ast_traverse_visitor
{
public:
	ast_output_traverser_visitor(CodeGen& _cg, AstShader* _shader, ShVariableList* _vl,
			ShChangeableList* _cgbls, EShLanguage _mode, DbgCgOptions _dbgopts) :
			cg(_cg), shader(_shader), vl(_vl), cgbls(_cgbls), mode(_mode), cgOptions(_dbgopts)
	{
		buffer = NULL;
		no_brakets = false;
		dbgTargetProcessed = false;
	}

	virtual ~ast_output_traverser_visitor()
	{
	}

	void dump();
	void get_code(char **);
	void append_header();
	void indent(void);

	virtual void visit(exec_list *);
	virtual void visit(class ast_node *);
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

	virtual bool enter(class ast_function_expression *);

protected:
	void output_extensions(const struct sh_extension *);
	void output_qualifier(const struct ast_type_qualifier *);
	void output_sequence(exec_list *, const char *, const char *,
									  const char *, bool i = false);
	bool geom_call(ast_function_expression *);
	void selection_body(ast_node *, bool, bool, bool conditional = false);
	void loop_debug_prepare(ast_iteration_statement *);
	void loop_debug_condition(ast_iteration_statement *);
	void loop_debug_terminal(ast_iteration_statement *);
	void loop_debug_end(ast_iteration_statement *);

	char* buffer;
	CodeGen& cg;
	AstShader* shader;
	ShVariableList *vl;
	ShChangeableList *cgbls;
	EShLanguage mode;
	DbgCgOptions cgOptions;
	AstStack return_types;
	// FIXME: it is wrong way to do it
	AstStack switches;
	bool dbgTargetProcessed;
	bool no_brakets;
};


#endif /* __OUTPUT_TRAVERSER_H_ */
