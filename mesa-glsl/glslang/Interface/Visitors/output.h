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
		dbgTargetProcessed = false;
	}

	virtual ~ast_output_traverser_visitor()
	{
	}

	void dump();
	void get_code(char **);
	void append_header();
	void indent(void);

	virtual void visit(exec_list *l) { ast_traverse_visitor::visit(l); }
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
	virtual void visit(class ast_gs_input_layout *);


	virtual bool enter(class ast_function_expression *);


protected:
	void output_qualifier(const struct ast_type_qualifier *);
	void output_sequence(exec_list *, const char *, const char *,
									  const char *, bool i = false);
	bool geom_call(ast_function_expression *);
	void selection_body(ast_selection_statement *, ast_node *, int);
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
	bool dbgTargetProcessed;
};

/*
class ir_output_traverser_visitor : public ir_visitor {
public:
	ir_output_traverser_visitor(struct gl_shader* shader_, EShLanguage mode_,
			DbgCgOptions dbgopts, ShVariableList *list, ShChangeableList *t, IRGenStack *s) :
			globals(0), cgOptions(dbgopts), vl(list), cgbl(t), dbgStack(s),
			dbgTargetProcessed(false)

	{
		buffer = NULL;
		indentation = 0;
		shader = shader_;
		mode = mode_;
	}

	virtual ~ir_output_traverser_visitor()
	{
	}

	void append_version();
	void run(exec_list* list);

	void indent(void);
	void newline_indent(void);
	void newline_deindent(void);
	void print_var_name (ir_variable* v);
	void print_precision (ir_instruction* ir, const glsl_type* type);

	virtual void visit(ir_variable *);
	virtual void visit(ir_function_signature *);
	virtual void visit(ir_function *);
	virtual void visit(ir_expression *);
	virtual void visit(ir_texture *);
	virtual void visit(ir_swizzle *);
	virtual void visit(ir_dereference_variable *);
	virtual void visit(ir_dereference_array *);
	virtual void visit(ir_dereference_record *);
	virtual void visit(ir_assignment *);
	virtual void visit(ir_constant *);
	virtual void visit(ir_call *);
	virtual void visit(ir_return *);
	virtual void visit(ir_discard *);
	virtual void visit(ir_if *);
	virtual void visit(ir_loop *);
	virtual void visit(ir_loop_jump *);
	virtual void visit(ir_typedecl_statement *);
	virtual void visit(ir_emit_vertex *);
	virtual void visit(ir_end_primitive *);
	virtual void visit(ir_dummy*);

	void visit_block(ir_dummy* first, const char* sep, bool do_indent = false);
	void visit_block(exec_list* instructions, const char* sep, bool do_indent = true);


	void emit_assignment_part (ir_dereference* lhs, ir_rvalue* rhs, unsigned write_mask, ir_rvalue* dstIndex);

	int indentation;
	int expression_depth;
	char* buffer;
	struct gl_shader* shader;
	global_print_tracker* globals;
	EShLanguage mode;
	DbgCgOptions cgOptions;
	ShVariableList *vl;
	ShChangeableList *cgbl;
	IRGenStack *dbgStack;
	bool dbgTargetProcessed;
};*/


#endif /* __OUTPUT_TRAVERSER_H_ */
