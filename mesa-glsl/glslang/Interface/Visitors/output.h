/*
 * output.h
 *
 *  Created on: 14.10.2013
 */

#ifndef __OUTPUT_TRAVERSER_H_
#define __OUTPUT_TRAVERSER_H_

struct global_print_tracker;

#include <assert.h>
#include "glsl/ir_visitor.h"
#include "ShaderLang.h"
#include "glslang/Interface/CodeInsertion.h"

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
};


#endif /* __OUTPUT_TRAVERSER_H_ */
