/*
 * output.cpp
 *
 *  Created on: 14.10.2013
 *
 *  This file is based on glsl optimizer code, so put original copyright here too
 *  I'm not sure, part of this code if belongs to intel, but if authors say so,
 *  let it be. Modified code part is distributed in the terms of the main project
 *  license. Do we have it, btw?
 *
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include "glslang/Interface/Visitors/output.h"
#include "glsl/list.h"
#include "glsl/ir_unused_structs.h"
#include "mesa/program/hash_table.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/IRScope.h"


static char* print_type(char* buffer, const glsl_type *t, bool arraySize);
static char* print_type_post(char* buffer, const glsl_type *t, bool arraySize);

struct ga_entry : public exec_node
{
	ga_entry(ir_instruction* ir)
	{
		assert(ir);
		this->ir = ir;
	}
	ir_instruction* ir;
};


struct global_print_tracker {
	global_print_tracker () {
		mem_ctx = ralloc_context(0);
		var_counter = 0;
		var_hash = hash_table_ctor(0, hash_table_pointer_hash, hash_table_pointer_compare);
		main_function_done = false;
	}

	~global_print_tracker() {
		hash_table_dtor (var_hash);
		ralloc_free(mem_ctx);
	}

	inline int push_var( ir_variable* v ) {
		/* We using our local variable tracker while printing variables in
		 * debug code, so we replace original ids with our. It is slower.
		 */
		//int id = ++var_counter;
		ShVariable* var = findShVariableFromSource(v);
		int id = var->uniqueId;
		hash_table_insert (var_hash, (void*)id, v);
		return id;
	}

	unsigned	var_counter;
	hash_table*	var_hash;
	exec_list	global_assignements;
	void* mem_ctx;
	bool	main_function_done;
};


void ir_output_traverser_visitor::append_version()
{
	if(shader && shader->Version )
		ralloc_asprintf_append (&buffer, "#version %i\n", shader->Version);
}

void ir_output_traverser_visitor::run(exec_list* instructions)
{
	// remove unused struct declarations
	do_remove_unused_typedecls(instructions);

	if( !globals )
		globals = new global_print_tracker;

	visit_block(instructions, ";\n", false);
}

void ir_output_traverser_visitor::visit_block(ir_dummy* ir, const char* sep, bool do_indent)
{
	if (!ir->next || ir->block_empty())
		return;

	if (do_indent){
		indentation++;
		indent();
	}
	int end_token = ir_dummy::pair_type(ir->dummy_type);
	if (end_token >= 0) {
		bool first = true;
		foreach_node_safe(node, ir->next) {
			ir_instruction * const inst = (ir_instruction *) node;
			ir_dummy * const dm = inst->as_dummy();
			if (dm && end_token == dm->dummy_type)
				break;
			if (!first && sep){
				ralloc_asprintf_append (&buffer, sep);
				if (do_indent)
					indent();
			}
			inst->accept(this);
			first = false;
		}
	}
	if (do_indent)
		indentation--;
}

void ir_output_traverser_visitor::visit_block(exec_list* instructions,
					 const char* sep, bool do_indent)
{
	int skip_pair = -1;
	if (do_indent)
		indentation++;
	foreach_list(node, instructions) {
		ir_instruction * const inst = (ir_instruction *) node;
		if (!list_iter_check(inst, skip_pair))
			continue;
		if (do_indent)
			indent();
		inst->accept( this );
		if (sep && inst->ir_type != ir_type_function && inst->ir_type != ir_type_dummy &&
		    		  (inst->ir_type != ir_type_assignment || this->mode == EShLangNone))
			ralloc_asprintf_append (&buffer, sep);
	}
	if (do_indent)
		indentation--;
}



void ir_output_traverser_visitor::indent(void)
{
   for (int i = 0; i < indentation; i++)
      ralloc_asprintf_append (&buffer, "  ");
}

void ir_output_traverser_visitor::newline_indent()
{
	if (expression_depth % 4 == 0)
	{
		++indentation;
		ralloc_asprintf_append (&buffer, "\n");
		indent();
	}
}
void ir_output_traverser_visitor::newline_deindent()
{
	if (expression_depth % 4 == 0)
	{
		--indentation;
		ralloc_asprintf_append (&buffer, "\n");
		indent();
	}
}

void ir_output_traverser_visitor::print_var_name (ir_variable* v)
{
    long id = (long)hash_table_find (globals->var_hash, v);
	if (!id && v->data.mode == ir_var_temporary)
		id = globals->push_var(v);
    if (id)
    {
        if (v->data.mode == ir_var_temporary)
            ralloc_asprintf_append (&buffer, "tmpvar_%d", (int)id);
        else
            ralloc_asprintf_append (&buffer, "%s_%d", v->name, (int)id);
    }
	else
	{
		ralloc_asprintf_append (&buffer, "%s", v->name);
	}
}

void ir_output_traverser_visitor::print_precision (ir_instruction* ir, const glsl_type* type)
{
	UNUSED_ARG(ir)
	UNUSED_ARG(type)
	return;
}


static char*
print_type(char* buffer, const glsl_type *t, bool arraySize)
{
	if (t->base_type == GLSL_TYPE_ARRAY) {
		buffer = print_type(buffer, t->fields.array, true);
		if (arraySize)
			ralloc_asprintf_append (&buffer, "[%u]", t->length);
	} else if ((t->base_type == GLSL_TYPE_STRUCT)
			   && (strncmp("gl_", t->name, 3) != 0)) {
		ralloc_asprintf_append (&buffer, "%s", t->name);
	} else {
		ralloc_asprintf_append (&buffer, "%s", t->name);
	}
	return buffer;
}

static char*
print_type_post(char* buffer, const glsl_type *t, bool arraySize)
{
	if (t->base_type == GLSL_TYPE_ARRAY) {
		if (!arraySize)
			ralloc_asprintf_append (&buffer, "[%u]", t->length);
	}
	return buffer;
}


void check_initializer(ir_variable *ir, ir_output_traverser_visitor* it)
{
	if (it->cgOptions != DBG_CG_ORIGINAL_SRC && (
			(ir->constant_value && ir->constant_value->debug_target) ||
			(ir->constant_initializer &&  ir->constant_initializer->debug_target))) {
	       it->dbgTargetProcessed = true;
	       cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions, it->cgbl,
	    		   	   	it->vl, it->dbgStack, 0);
	       ralloc_asprintf_append (&it->buffer, ";\n");
	       it->indent();
	   }
}


void ir_output_traverser_visitor::visit(ir_variable *ir)
{
   const char *const cent = (ir->data.centroid) ? "centroid " : "";
   const char *const inv = (ir->data.invariant) ? "invariant " : "";
   const char *const mode[4][ir_var_mode_count] =
   {
	{ "", "uniform ", "in ",        "out ",     "in ", "out ", "inout ", "", "", "" },
	{ "", "uniform ", "attribute ", "varying ", "in ", "out ", "inout ", "", "", "" },
	{ "", "uniform ", "attribute ", "varying ", "in ", "out ", "inout ", "", "", "" },
	{ "", "uniform ", "varying ",   "out ",     "in ", "out ", "inout ", "", "", "" },
   };

   const char *const interp[] = { "", "smooth ", "flat ", "noperspective " };

	int decormode = this->mode;
	// GLSL 1.30 and up use "in" and "out" for everything
	if (this->shader->Version >= 130)
	{
		decormode = 0;
	}

	// give an id to any variable defined in a function that is not an uniform
	if ((this->mode == EShLangNone && ir->data.mode != ir_var_uniform))
	{
		long id = (long)hash_table_find (globals->var_hash, ir);
		if (id == 0)
			id = globals->push_var(ir);
	}

   check_initializer(ir, this);

	// keep invariant declaration for builtin variables
	if (strstr(ir->name, "gl_") == ir->name) {
		ralloc_asprintf_append (&buffer, "%s", inv);
		print_var_name (ir);
		return;
	}

	ralloc_asprintf_append (&buffer, "%s%s%s%s",
							cent, inv, interp[ir->data.interpolation], mode[decormode][ir->data.mode]);
	print_precision (ir, ir->type);
	buffer = print_type(buffer, ir->type, false);
	ralloc_asprintf_append (&buffer, " ");
	print_var_name (ir);
	buffer = print_type_post(buffer, ir->type, false);

	if (ir->constant_value &&
		ir->data.mode != ir_var_shader_in &&
		ir->data.mode != ir_var_shader_out &&
		ir->data.mode != ir_var_function_in &&
		ir->data.mode != ir_var_function_out &&
		ir->data.mode != ir_var_function_inout)
	{
		ralloc_asprintf_append (&buffer, " = ");
		visit (ir->constant_value);
	}
}


void ir_output_traverser_visitor::visit(ir_function_signature *ir)
{
   print_precision (ir, ir->return_type);
   buffer = print_type(buffer, ir->return_type, true);

   const char* fname;
   // Use debug name if we print debuged function's clone
   if( this->cgOptions != DBG_CG_ORIGINAL_SRC &&
         strcmp(ir->function_name(), MAIN_FUNC_SIGNATURE) != 0 &&
         ir->debug_state == ir_dbg_state_path &&
         ir->debug_overwrite != ir_dbg_ow_original )
   {
	   std::string mangled = getMangledName(ir);
	   const char* mname = mangled.c_str();
	   fname = cgGetDebugName(mname, this->shader);
   } else {
	   fname = ir->function_name();
   }

   ralloc_asprintf_append (&buffer, " %s (", fname);

   if (!ir->parameters.is_empty())
   {
	   ralloc_asprintf_append (&buffer, "\n");

	   indentation++;
	   bool first = true;
	   foreach_list(node, &ir->parameters) {
		  ir_variable *const inst = (ir_variable *) node;

		  if (!first)
			  ralloc_asprintf_append (&buffer, ",\n");
		  indent();
		  inst->accept(this);
		  first = false;
	   }
	   indentation--;

	   ralloc_asprintf_append (&buffer, "\n");
	   indent();
   }

   if (ir->body.is_empty())
   {
	   ralloc_asprintf_append (&buffer, ");\n");
	   return;
   }

   ralloc_asprintf_append (&buffer, ")\n");

   indent();
   ralloc_asprintf_append (&buffer, "{\n");

	// insert postponed global assigments
	if (strcmp(ir->function()->name, "main") == 0)
	{
		indentation++;
		assert (!globals->main_function_done);
		globals->main_function_done = true;
		foreach_list(node, &globals->global_assignements)
		{
			ir_instruction* as = ((ga_entry *) node)->ir;
			as->accept(this);
			ralloc_asprintf_append(&buffer, ";\n");
		}
		indentation--;
	}

   visit_block(&ir->body, ";\n");

   indent();
   ralloc_asprintf_append (&buffer, "}\n");
}


void ir_output_traverser_visitor::visit(ir_function *ir)
{
   bool found_non_builtin_proto = false;

   foreach_list(node, &ir->signatures) {
      ir_function_signature *const sig = (ir_function_signature *) node;
      if (!sig->is_builtin())
	 found_non_builtin_proto = true;
   }
   if (!found_non_builtin_proto)
      return;

   EShLanguage oldMode = this->mode;
   this->mode = EShLangNone;

   /* Double function if this is on path.
    * This is done to make sure that only the debugged path is calling
    * a function with inserted debug code */
   if( this->cgOptions != DBG_CG_ORIGINAL_SRC &&
         strcmp(ir->name, MAIN_FUNC_SIGNATURE) != 0 &&
         ir->debug_state == ir_dbg_state_path &&
         ir->debug_overwrite != ir_dbg_ow_original ){
      DbgCgOptions option = this->cgOptions;
      this->cgOptions = DBG_CG_ORIGINAL_SRC;
      // Add original function. The general route function will be debug one.
      foreach_list(node, &ir->signatures) {
         ir_instruction* sig = (ir_instruction*) node;
         // Double only path signature
         if( sig->debug_state != ir_dbg_state_path )
            continue;

         indent();
         sig->accept(this);
         ralloc_asprintf_append (&buffer, "\n");
      }
      this->cgOptions = option;
   }

   foreach_list(node, &ir->signatures) {
      ir_function_signature *const sig = (ir_function_signature *) node;

      indent();
      sig->accept(this);
      ralloc_asprintf_append (&buffer, "\n");
   }

   this->mode = oldMode;

   indent();
}

static const char *const operator_glsl_strs[] = {
   "~",
   "!",
   "-",
   "abs",
   "sign",
   "1.0/",
   "inversesqrt",
   "sqrt",
   "exp",
   "log",
   "exp2",
   "log2",
   "int",	// f2i
   "int",	// f2u
   "float",	// i2f
   "bool",	// f2b
   "float",	// b2f
   "bool",	// i2b
   "int",	// b2i
   "float",	// u2f
   "int",	// i2u
   "int",	// u2i
   "float",	// bit i2f
   "int",	// bit f2i
   "float",	// bit u2f
   "int",	// bit f2u
   "any",
   "trunc",
   "ceil",
   "floor",
   "fract",
   "round_even",
   "sin",
   "cos",
   "sin", // reduced
   "cos", // reduced
   "dFdx",
   "dFdy",
   "packSnorm2x16",
   "packSnorm4x8",
   "packUnorm2x16",
   "packUnorm4x8",
   "packHalf2x16",
   "unpackSnorm2x16",
   "unpackSnorm4x8",
   "unpackUnorm2x16",
   "unpackUnorm4x8",
   "unpackHalf2x16",
   "unpackHalf2x16_split_x_TODO",
   "unpackHalf2x16_split_y_TODO",
   "bitfield_reverse",
   "bitCount",
   "findMSB",
   "findLSB",
   "noise",
   "+",
   "-",
   "*",
   "imul_high_TODO",
   "/",
   "carry_TODO",
   "borrow_TODO",
   "mod",
   "<",
   ">",
   "<=",
   ">=",
   "equal",
   "notEqual",
   "==",
   "!=",
   "<<",
   ">>",
   "&",
   "^",
   "|",
   "&&",
   "^^",
   "||",
   "dot",
   "min",
   "max",
   "pow",
   "packHalf2x16_split_TODO",
   "bfm_TODO",
   "ubo_load_TODO",
   "ldexp_TODO",
   "vector_extract_TODO",
   "fma",
   "lrp",
   "csel_TODO",
   "bfi_TODO",
   "bitfield_extract_TODO",
   "vector_insert_TODO",
   "bitfield_insert_TODO",
   "vector_TODO",
};

static const char *const operator_vec_glsl_strs[] = {
	"lessThan",
	"greaterThan",
	"lessThanEqual",
	"greaterThanEqual",
	"equal",
	"notEqual",
};


static bool is_binop_func_like(ir_expression_operation op, const glsl_type* type)
{
	if (op == ir_binop_equal ||
		op == ir_binop_nequal ||
		op == ir_binop_mod ||
		(op >= ir_binop_dot && op <= ir_binop_pow))
		return true;
	if (type->is_vector() && (op >= ir_binop_less && op <= ir_binop_nequal))
	{
		return true;
	}
	return false;
}

void ir_output_traverser_visitor::visit(ir_expression *ir)
{
	++this->expression_depth;
	newline_indent();
    if (ir->debug_target && this->cgOptions != DBG_CG_ORIGINAL_SRC) {
    	dbgTargetProcessed = true;
        cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
        // FIXME: WAT?
        ralloc_asprintf_append (&buffer, ", ");
    }

	if (ir->get_num_operands() == 1) {
		if (ir->operation >= ir_unop_f2i && ir->operation < ir_unop_any) {
			buffer = print_type(buffer, ir->type, true);
			ralloc_asprintf_append(&buffer, "(");
		} else if (ir->operation == ir_unop_rcp) {
			ralloc_asprintf_append (&buffer, "(1.0/(");
		} else {
			ralloc_asprintf_append (&buffer, "%s(", operator_glsl_strs[ir->operation]);
		}
		if (ir->operands[0])
			ir->operands[0]->accept(this);
		ralloc_asprintf_append (&buffer, ")");
		if (ir->operation == ir_unop_rcp) {
			ralloc_asprintf_append (&buffer, ")");
		}
	}
	else if (ir->operation == ir_binop_vector_extract)
	{
		// a[b]

		if (ir->operands[0])
			ir->operands[0]->accept(this);
		ralloc_asprintf_append (&buffer, "[");
		if (ir->operands[1])
			ir->operands[1]->accept(this);
		ralloc_asprintf_append (&buffer, "]");
	}
	else if (is_binop_func_like(ir->operation, ir->type))
	{
		if (ir->operation == ir_binop_mod)
		{
			ralloc_asprintf_append (&buffer, "(");
			buffer = print_type(buffer, ir->type, true);
			ralloc_asprintf_append (&buffer, "(");
		}
		if (ir->type->is_vector() && (ir->operation >= ir_binop_less && ir->operation <= ir_binop_nequal))
			ralloc_asprintf_append (&buffer, "%s (", operator_vec_glsl_strs[ir->operation-ir_binop_less]);
		else
			ralloc_asprintf_append (&buffer, "%s (", operator_glsl_strs[ir->operation]);

		if (ir->operands[0])
			ir->operands[0]->accept(this);
		ralloc_asprintf_append (&buffer, ", ");
		if (ir->operands[1])
			ir->operands[1]->accept(this);
		ralloc_asprintf_append (&buffer, ")");
		if (ir->operation == ir_binop_mod)
            ralloc_asprintf_append (&buffer, "))");
	}
	else if (ir->get_num_operands() == 2)
	{
		ralloc_asprintf_append (&buffer, "(");
		if (ir->operands[0])
			ir->operands[0]->accept(this);

		ralloc_asprintf_append (&buffer, " %s ", operator_glsl_strs[ir->operation]);

		if (ir->operands[1])
			ir->operands[1]->accept(this);
		ralloc_asprintf_append (&buffer, ")");
	}
	else
	{
		// ternary op
		ralloc_asprintf_append (&buffer, "%s (", operator_glsl_strs[ir->operation]);
		if (ir->operands[0])
			ir->operands[0]->accept(this);
		ralloc_asprintf_append (&buffer, ", ");
		if (ir->operands[1])
			ir->operands[1]->accept(this);
		ralloc_asprintf_append (&buffer, ", ");
		if (ir->operands[2])
			ir->operands[2]->accept(this);
		ralloc_asprintf_append (&buffer, ")");
	}

	newline_deindent();
	--this->expression_depth;
}

// [glsl_sampler_dim]
static const char* tex_sampler_dim_name[] = {
	"1D", "2D", "3D", "Cube", "Rect", "Buf", "External",
};
static int tex_sampler_dim_size[] = {
	1, 2, 3, 3, 2, 2, 2,
};

void ir_output_traverser_visitor::visit(ir_texture *ir)
{
	// TODO: debug
	if( ir->debug_target )
		printf("TODO: Texture as debug target");

	glsl_sampler_dim sampler_dim = (glsl_sampler_dim)ir->sampler->type->sampler_dimensionality;
	const bool is_shadow = ir->sampler->type->sampler_shadow;
	const glsl_type* uv_type = ir->coordinate->type;
	const int uv_dim = uv_type->vector_elements;
	int sampler_uv_dim = tex_sampler_dim_size[sampler_dim];
	if (is_shadow)
		sampler_uv_dim += 1;
	const bool is_proj = (uv_dim > sampler_uv_dim);

    // texture function name
    //ACS: shadow lookups and lookups with dimensionality included in the name were deprecated in 130
    if(shader->Version < 130)
    {
        ralloc_asprintf_append (&buffer, "%s", is_shadow ? "shadow" : "texture");
        ralloc_asprintf_append (&buffer, "%s", tex_sampler_dim_name[sampler_dim]);
    }
    else
    {
        ralloc_asprintf_append (&buffer, "texture");
    }

	if (is_proj)
		ralloc_asprintf_append (&buffer, "Proj");
	if (ir->op == ir_txl)
		ralloc_asprintf_append (&buffer, "Lod");
	if (ir->op == ir_txd)
		ralloc_asprintf_append (&buffer, "Grad");

//	if (state->es_shader)
//	{
//		if ( (is_shadow && state->EXT_shadow_samplers_enable) ||
//			(ir->op == ir_txl && state->EXT_shader_texture_lod_enable) )
//		{
//			ralloc_asprintf_append (&buffer, "EXT");
//		}
//	}
//
//	if(ir->op == ir_txd)
//	{
//		if(state->es_shader && state->EXT_shader_texture_lod_enable)
//			ralloc_asprintf_append (&buffer, "EXT");
//		else if(!state->es_shader && state->ARB_shader_texture_lod_enable)
//			ralloc_asprintf_append (&buffer, "ARB");
//	}

	ralloc_asprintf_append (&buffer, " (");

	// sampler
	ir->sampler->accept(this);
	ralloc_asprintf_append (&buffer, ", ");

	// texture coordinate
	ir->coordinate->accept(this);

	// lod bias
	if (ir->op == ir_txb)
	{
		ralloc_asprintf_append (&buffer, ", ");
		ir->lod_info.bias->accept(this);
	}

	// lod
	if (ir->op == ir_txl)
	{
		ralloc_asprintf_append (&buffer, ", ");
		ir->lod_info.lod->accept(this);
	}

	// grad
	if (ir->op == ir_txd)
	{
		ralloc_asprintf_append (&buffer, ", ");
		ir->lod_info.grad.dPdx->accept(this);
		ralloc_asprintf_append (&buffer, ", ");
		ir->lod_info.grad.dPdy->accept(this);
	}

	/*

   if (ir->offset != NULL) {
      ir->offset->accept(this);
   }

   if (ir->op != ir_txf) {
      if (ir->projector)
	 ir->projector->accept(this);
      else
	 ralloc_asprintf_append (&buffer, "1");

      if (ir->shadow_comparitor) {
	 ralloc_asprintf_append (&buffer, " ");
	 ir->shadow_comparitor->accept(this);
      } else {
	 ralloc_asprintf_append (&buffer, " ()");
      }
   }

   ralloc_asprintf_append (&buffer, " ");
   switch (ir->op)
   {
   case ir_tex:
      break;
   case ir_txb:
      ir->lod_info.bias->accept(this);
      break;
   case ir_txl:
   case ir_txf:
      ir->lod_info.lod->accept(this);
      break;
   case ir_txd:
      ralloc_asprintf_append (&buffer, "(");
      ir->lod_info.grad.dPdx->accept(this);
      ralloc_asprintf_append (&buffer, " ");
      ir->lod_info.grad.dPdy->accept(this);
      ralloc_asprintf_append (&buffer, ")");
      break;
   };
	 */
   ralloc_asprintf_append (&buffer, ")");
}


void ir_output_traverser_visitor::visit(ir_swizzle *ir)
{
   // TODO: debug
   if( ir->debug_target )
		printf("TODO: Swizzle as debug target");

   const unsigned swiz[4] = {
      ir->mask.x,
      ir->mask.y,
      ir->mask.z,
      ir->mask.w,
   };

	if (ir->val->type == glsl_type::float_type || ir->val->type == glsl_type::int_type)
	{
		if (ir->mask.num_components != 1)
		{
			buffer = print_type(buffer, ir->type, true);
			ralloc_asprintf_append (&buffer, "(");
		}
	}

	ir->val->accept(this);

	if (ir->val->type == glsl_type::float_type || ir->val->type == glsl_type::int_type)
	{
		if (ir->mask.num_components != 1)
		{
			ralloc_asprintf_append (&buffer, ")");
		}
		return;
	}

   ralloc_asprintf_append (&buffer, ".");
   for (unsigned i = 0; i < ir->mask.num_components; i++) {
		ralloc_asprintf_append (&buffer, "%c", "xyzw"[swiz[i]]);
   }
}


void ir_output_traverser_visitor::visit(ir_dereference_variable *ir)
{
   ir_variable *var = ir->variable_referenced();
   print_var_name (var);
}


void ir_output_traverser_visitor::visit(ir_dereference_array *ir)
{
   ir->array->accept(this);
   ralloc_asprintf_append (&buffer, "[");
   ir->array_index->accept(this);
   ralloc_asprintf_append (&buffer, "]");
}


void ir_output_traverser_visitor::visit(ir_dereference_record *ir)
{
   ir->record->accept(this);
   ralloc_asprintf_append (&buffer, ".%s", ir->field);
}


void ir_output_traverser_visitor::emit_assignment_part (ir_dereference* lhs, ir_rvalue* rhs, unsigned write_mask, ir_rvalue* dstIndex)
{
	lhs->accept(this);

	if (dstIndex)
	{
		// if dst index is a constant, then emit a swizzle
		ir_constant* dstConst = dstIndex->as_constant();
		if (dstConst)
		{
			const char* comps = "xyzw";
			char comp = comps[dstConst->get_int_component(0)];
			ralloc_asprintf_append (&buffer, ".%c", comp);
		}
		else
		{
			ralloc_asprintf_append (&buffer, "[");
			dstIndex->accept(this);
			ralloc_asprintf_append (&buffer, "]");
		}
	}

	char mask[5];
	unsigned j = 0;
	const glsl_type* lhsType = lhs->type;
	const glsl_type* rhsType = rhs->type;
	if (!dstIndex && lhsType->matrix_columns <= 1 && lhsType->vector_elements > 1
			&& write_mask != (unsigned)(1<<lhsType->vector_elements)-1)
	{
		for (unsigned i = 0; i < 4; i++) {
			if ((write_mask & (1 << i)) != 0) {
				mask[j] = "xyzw"[i];
				j++;
			}
		}
		lhsType = glsl_type::get_instance(lhsType->base_type, j, 1);
	}
	mask[j] = '\0';
	bool hasWriteMask = false;
	if (mask[0])
	{
		ralloc_asprintf_append (&buffer, ".%s", mask);
		hasWriteMask = true;
	}

	ralloc_asprintf_append (&buffer, " = ");

	bool typeMismatch = !dstIndex && (lhsType != rhsType);
	const bool addSwizzle = hasWriteMask && typeMismatch;
	if (typeMismatch)
	{
		if (!addSwizzle)
			buffer = print_type(buffer, lhsType, true);
		ralloc_asprintf_append (&buffer, "(");
	}

	rhs->accept(this);

	if (typeMismatch)
	{
		ralloc_asprintf_append (&buffer, ")");
		if (addSwizzle)
			ralloc_asprintf_append (&buffer, ".%s", mask);
	}
}

void ir_output_traverser_visitor::visit(ir_assignment *ir)
{
	// assignments in global scope are postponed to main function
	if (this->mode != EShLangNone)
	{
		assert (!this->globals->main_function_done);
		this->globals->global_assignements.push_tail (new(this->globals->mem_ctx) ga_entry(ir));
		return;
	}

    if (ir->debug_target && this->cgOptions != DBG_CG_ORIGINAL_SRC) {
    	dbgTargetProcessed = true;
        cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
        ralloc_asprintf_append (&buffer, "; ");
    }

	// if RHS is ir_triop_vector_insert, then we have to do some special dance. If source expression is:
	//   dst = vector_insert (a, b, idx)
	// then emit it like:
	//   dst = a;
	//   dst.idx = b;
	ir_expression* rhsOp = ir->rhs->as_expression();
	if (rhsOp && rhsOp->operation == ir_triop_vector_insert)
	{
		// skip assignment if lhs and rhs would be the same
		ir_dereference_variable* lhsDeref = ir->lhs->as_dereference_variable();
		ir_dereference_variable* rhsDeref = rhsOp->operands[0]->as_dereference_variable();
		if (!lhsDeref || !rhsDeref || lhsDeref->var != rhsDeref->var)
		{
			emit_assignment_part(ir->lhs, rhsOp->operands[0], ir->write_mask, NULL);
			ralloc_asprintf_append(&buffer, "; ");
		}
		emit_assignment_part(ir->lhs, rhsOp->operands[1], ir->write_mask, rhsOp->operands[2]);
		return;
	}

   if (ir->condition)
   {
      ir->condition->accept(this);
	  ralloc_asprintf_append (&buffer, " ");
   }

	emit_assignment_part (ir->lhs, ir->rhs, ir->write_mask, NULL);
}

static char* print_float (char* buffer, float f)
{
	// Kind of roundabout way, but this is to satisfy two things:
	// * MSVC and gcc-based compilers differ a bit in how they treat float
	//   widht/precision specifiers. Want to match for tests.
	// * GLSL (early version at least) require floats to have ".0" or
	//   exponential notation.
	char tmp[64];
	snprintf(tmp, 64, "%.6g", f);

	char* posE = NULL;
	posE = strchr(tmp, 'e');
	if (!posE)
		posE = strchr(tmp, 'E');

	#if _MSC_VER
	// While gcc would print something like 1.0e+07, MSVC will print 1.0e+007 -
	// only for exponential notation, it seems, will add one extra useless zero. Let's try to remove
	// that so compiler output matches.
	if (posE != NULL)
	{
		if((posE[1] == '+' || posE[1] == '-') && posE[2] == '0')
		{
			char* p = posE+2;
			while (p[0])
			{
				p[0] = p[1];
				++p;
			}
		}
	}
	#endif

	ralloc_strcat (&buffer, tmp);

	// need to append ".0"?
	if (!strchr(tmp,'.') && (posE == NULL))
		ralloc_strcat(&buffer, ".0");
	return buffer;
}

void ir_output_traverser_visitor::visit(ir_constant *ir)
{
	const glsl_type* type = ir->type;

    if (ir->debug_target && this->cgOptions != DBG_CG_ORIGINAL_SRC) {
    	dbgTargetProcessed = true;
        cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
        // FIXME: WAT?
        ralloc_asprintf_append (&buffer, ", ");
    }

	if (type == glsl_type::float_type)
	{
		buffer = print_float (buffer, ir->value.f[0]);
		return;
	}
	else if (type == glsl_type::int_type)
	{
		ralloc_asprintf_append (&buffer, "%d", ir->value.i[0]);
		return;
	}
	else if (type == glsl_type::uint_type)
	{
		ralloc_asprintf_append (&buffer, "%u", ir->value.u[0]);
		return;
	}

   const glsl_type *const base_type = ir->type->get_base_type();

   buffer = print_type(buffer, type, true);
   ralloc_asprintf_append (&buffer, "(");

   if (ir->type->is_array()) {
      for (unsigned i = 0; i < ir->type->length; i++)
      {
	 if (i != 0)
	    ralloc_asprintf_append (&buffer, ", ");
	 ir->get_array_element(i)->accept(this);
      }
   } else if (ir->type->is_record()) {
      bool first = true;
      foreach_list(node, &ir->components){
	  if (!first)
	    ralloc_asprintf_append (&buffer, ", ");
	  first = false;
	  ir_constant* inst = (ir_constant*) node;
	  inst->accept(this);
      }
   } else {
      bool first = true;
      for (unsigned i = 0; i < ir->type->components(); i++) {
	 if (!first)
	    ralloc_asprintf_append (&buffer, ", ");
	 first = false;
	 switch (base_type->base_type) {
	 case GLSL_TYPE_UINT:  ralloc_asprintf_append (&buffer, "%u", ir->value.u[i]); break;
	 case GLSL_TYPE_INT:   ralloc_asprintf_append (&buffer, "%d", ir->value.i[i]); break;
	 case GLSL_TYPE_FLOAT: buffer = print_float(buffer, ir->value.f[i]); break;
	 case GLSL_TYPE_BOOL:  ralloc_asprintf_append (&buffer, "%d", ir->value.b[i]); break;
	 default: assert(0);
	 }
      }
   }
   ralloc_asprintf_append (&buffer, ")");
}

bool process_debug_call(ir_call* ir, ir_output_traverser_visitor* it){
	if( it->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target ){

		it->dbgTargetProcessed = true;

		/* This function call acts as target */

		/* Check if we can use a parameter for debugging */

		int lastInParameter = getFunctionDebugParameter( ir->callee );

		if( lastInParameter >= 0 ){
			/* we found a usable parameter */
			ralloc_asprintf_append (&it->buffer, "%s (", ir->callee_name());
			int i = 0;
			bool first = true;
			foreach_list(node, &ir->actual_parameters) {
				ir_instruction * const inst = (ir_instruction *) node;

				if( !first )
					ralloc_asprintf_append( &it->buffer, ", " );

				if( i == lastInParameter ){
					ralloc_asprintf_append (&it->buffer, "(");
					if( !getSideEffectsDebugParameter( ir, lastInParameter ) ){
						/* No special care necessary, just add it before */
						cgAddDbgCode( CG_TYPE_RESULT, &it->buffer, it->cgOptions,
								it->cgbl, it->vl, it->dbgStack, 0 );
						// FIXME: WTF? WHAT LANGUAGE ORIGIANAL USED?
						ralloc_asprintf_append (&it->buffer, ", ");
						inst->accept( it );
					}else{
						/* Copy to temporary, debug, and copy back */
						cgAddDbgCode( CG_TYPE_PARAMETER, &it->buffer, it->cgOptions,
														it->cgbl, it->vl, it->dbgStack, 0 );
						ralloc_asprintf_append (&it->buffer, " = (");
						inst->accept( it );
						ralloc_asprintf_append (&it->buffer, "), ");
						cgAddDbgCode( CG_TYPE_RESULT,& it->buffer, it->cgOptions,
								it->cgbl, it->vl, it->dbgStack, 0 );
						ralloc_asprintf_append (&it->buffer, ", ");
						cgAddDbgCode( CG_TYPE_PARAMETER, &it->buffer, it->cgOptions,
										it->cgbl, it->vl, it->dbgStack, 0 );
					}
					ralloc_asprintf_append (&it->buffer, ")");
				}else
					inst->accept( it );

				first = false;
				++i;
			}

			ralloc_asprintf_append (&it->buffer, ")");
		}else{
			/* no usable parameter, so debug before function call */
			ralloc_asprintf_append (&it->buffer, "(");
			cgAddDbgCode( CG_TYPE_RESULT, &it->buffer, it->cgOptions,
					it->cgbl, it->vl, it->dbgStack, 0 );
			ralloc_asprintf_append (&it->buffer, ", %s (", ir->callee_name());
			bool first = true;
			foreach_list(node, &ir->actual_parameters) {
				ir_instruction * const inst = (ir_instruction *) node;
				if( !first )
					ralloc_asprintf_append( &it->buffer, ", " );
				inst->accept( it );
				first = false;
			}
			ralloc_asprintf_append( &it->buffer, ") )" );
		}
		return true;
	}else if( it->cgOptions != DBG_CG_ORIGINAL_SRC
			&& ir->debug_state == ir_dbg_state_target
			&& ir->debug_overwrite != ir_dbg_ow_original ){
		/* This call leads to the actual prosition of debugging */
		ralloc_asprintf_append (&it->buffer, ", %s (",
				cgGetDebugName( ir->callee_name(), it->shader ));
		bool first = true;
		foreach_list(node, &ir->actual_parameters) {
			ir_instruction * const inst = (ir_instruction *) node;
			if( !first )
				ralloc_asprintf_append( &it->buffer, ", " );
			inst->accept( it );
			first = false;
		}
		ralloc_asprintf_append( &it->buffer, ")" );
	}
	return false;
}

void
ir_output_traverser_visitor::visit(ir_call *ir)
{
	// calls in global scope are postponed to main function
	if (this->mode != EShLangNone)
	{
		assert (!this->globals->main_function_done);
		this->globals->global_assignements.push_tail (new(this->globals->mem_ctx) ga_entry(ir));
		ralloc_asprintf_append(&buffer, "//"); // for the ; that will follow (ugly, I know)
		return;
	}

	if (ir->return_deref)
	{
		visit(ir->return_deref);
		ralloc_asprintf_append (&buffer, " = ");
	}

   if( process_debug_call(ir, this) )
      return;

   ralloc_asprintf_append (&buffer, "%s (", ir->callee_name());
   bool first = true;
   foreach_list(node, &ir->actual_parameters) {
      ir_instruction *const inst = (ir_instruction *) node;
	  if (!first)
		  ralloc_asprintf_append (&buffer, ", ");
      inst->accept(this);
	  first = false;
   }
   ralloc_asprintf_append (&buffer, ")");
}


void
ir_output_traverser_visitor::visit(ir_return *ir)
{
  // TODO: debug
  if( ir->debug_state == ir_dbg_state_target )
    printf("TODO: Return as debug target");

   ralloc_asprintf_append (&buffer, "return");

   ir_rvalue *const value = ir->get_value();
   if (value) {
      ralloc_asprintf_append (&buffer, " ");
      value->accept(this);
   }
}


void
ir_output_traverser_visitor::visit(ir_discard *ir)
{
   ralloc_asprintf_append (&buffer, "discard");

   // TODO: debug
   if( ir->debug_target )
     printf("TODO: Discard as debug target. Why it occurred, btw?");

   if (ir->condition != NULL) {
      ralloc_asprintf_append (&buffer, " TODO ");
      ir->condition->accept(this);
   }
}

static void print_selection_instructions(exec_list* list, ir_if* ir,
					ir_output_traverser_visitor* it, int debug_option)
{
   if (ir->debug_target && it->cgOptions == DBG_CG_SELECTION_CONDITIONAL
      && ir->debug_state_internal == ir_dbg_if_condition_passed)	{
      it->indentation++;
      /* Add code to colorize condition */
      it->indent();
      //DBG_CG_COVERAGE
      cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions,
                   it->cgbl, it->vl, it->dbgStack, debug_option);
      ralloc_asprintf_append(&it->buffer, ";\n");
      it->indentation--;
   }

   it->visit_block(list, ";\n");
   it->indent();
}

void
ir_output_traverser_visitor::visit(ir_if *ir)
{
   bool copyCondition = false;
   /* Add debug code */
   if (ir->debug_target) {
      this->dbgTargetProcessed = true;
      if( cgOptions == DBG_CG_COVERAGE ||
          cgOptions == DBG_CG_CHANGEABLE ||
          cgOptions == DBG_CG_GEOMETRY_CHANGEABLE ){
         switch (ir->debug_state_internal) {
            case ir_dbg_if_unset:
               printf("CodeGen - selection status is unset\n");
               exit(1);
               break;
            case ir_dbg_if_init:
            case ir_dbg_if_condition:
               /* Add debug code prior to selection */
               cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
               ralloc_asprintf_append (&buffer, ";\n");
               indent();
               break;
            case ir_dbg_if_condition_passed:
            case ir_dbg_if_then:
            case ir_dbg_if_else:
               /* Add temporary register for condition */
               /* Fix: trigraph initialized condition register
                *      even if trigraph is part of another
                *      statement
                *
               cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
               cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
               outputIndentation(oit, oit->depth);
                */
               copyCondition = true;
               break;
            default:
               break;
         }
      }
   }

   ralloc_asprintf_append (&buffer, "if (");

   /* Add condition */
   if( copyCondition ){
      cgAddDbgCode( CG_TYPE_CONDITION, &buffer, cgOptions, cgbl, vl, dbgStack, 0 );
      ralloc_asprintf_append (&buffer, " = (");
      ir->condition->accept(this);
      ralloc_asprintf_append (&buffer, "), ");

      /* Add debug code */
      cgAddDbgCode( CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0 );
      ralloc_asprintf_append (&buffer, ", ");
      cgAddDbgCode( CG_TYPE_CONDITION, &buffer, cgOptions, cgbl, vl, dbgStack, 0 );
   }else
      ir->condition->accept(this);

   ralloc_asprintf_append (&buffer, ") {\n");
   print_selection_instructions(&ir->then_instructions, ir, this, true);
   ralloc_asprintf_append (&buffer, "}");

   if (!ir->else_instructions.is_empty())
   {
	   ralloc_asprintf_append (&buffer, " else {\n");
       	   print_selection_instructions(&ir->else_instructions, ir, this, false);
	   ralloc_asprintf_append (&buffer, "}");
   }
}

static void
loop_debug_init(ir_loop* ir, ir_output_traverser_visitor* it)
{
	if (it->cgOptions == DBG_CG_ORIGINAL_SRC){
		it->visit_block(ir->debug_init, ";\n");
		return;
	}

	/* Add loop counter */
	if (ir->need_dbgiter())
		ralloc_asprintf_append (&it->buffer, "%s = 0;\n", ir->debug_iter_name);


	/* Add debug temoprary register to copy condition */
	if (ir->debug_target && ir->debug_state_internal == ir_dbg_loop_select_flow) {
		switch (it->cgOptions) {
		case DBG_CG_COVERAGE:
		case DBG_CG_LOOP_CONDITIONAL:
		case DBG_CG_CHANGEABLE:
		case DBG_CG_GEOMETRY_CHANGEABLE:
			it->indent();
			cgInit(CG_TYPE_CONDITION, NULL, it->vl, it->mode);
			cgAddDeclaration(CG_TYPE_CONDITION, &it->buffer, it->mode);
			break;
		default:
			break;
		}
	}

	/* Add debug code prior to loop */
	if (ir->mode == ir_loop_for) {
		if (ir->debug_target &&
				ir->debug_state_internal == ir_dbg_loop_qyr_init) {
			switch (it->cgOptions) {
			case DBG_CG_COVERAGE:
			case DBG_CG_CHANGEABLE:
			case DBG_CG_GEOMETRY_CHANGEABLE:
				cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions, it->cgbl,
								it->vl, it->dbgStack, 0);
				ralloc_asprintf_append (&it->buffer, ";\n");
				break;
			default:
				break;
			}
		} else if (!ir->debug_target &&
				ir->debug_state_internal == ir_dbg_loop_wrk_init) {
			it->indentation++;
			ralloc_asprintf_append (&it->buffer, "{\n");
		}
	}

	if (!ir->debug_check->block_empty()){
		it->visit_block(ir->debug_init, ";\n ", true);
		ralloc_asprintf_append (&it->buffer, ";\n");
	}
	it->indent();
}

static void
loop_debug_condition(ir_loop* ir, ir_output_traverser_visitor* it)
{

	if (it->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target) {
		if (ir->debug_state_internal == ir_dbg_loop_qyr_test) {
			cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions, it->cgbl,
							it->vl, it->dbgStack, 0);
			ralloc_asprintf_append(&it->buffer, ", ");
		} else if (ir->debug_state_internal == ir_dbg_loop_select_flow) {
			/* Copy test */
			cgAddDbgCode(CG_TYPE_CONDITION, &it->buffer, it->cgOptions, it->cgbl,
							it->vl, it->dbgStack, 0);
			ralloc_asprintf_append(&it->buffer, " = ( ");
		}
	}

	/* Add original condition without any modifications */
	ir_rvalue* check = ir->condition();
	DbgCgOptions opts = it->cgOptions;
	it->cgOptions = DBG_CG_ORIGINAL_SRC;
	if (check){
		// Condition here is not-inverted. Invert it again.
		ralloc_asprintf_append(&it->buffer, "!");
		check->accept(it);
	}else{
		ralloc_asprintf_append(&it->buffer, "true");
	}
	it->cgOptions = opts;

	if (it->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target &&
			ir->debug_state_internal == ir_dbg_loop_select_flow) {
		ralloc_asprintf_append(&it->buffer, "), ");
		/* Add debug code */
		cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions, it->cgbl,
						it->vl, it->dbgStack, 0);
		ralloc_asprintf_append(&it->buffer, ", ");
		cgAddDbgCode(CG_TYPE_CONDITION, &it->buffer, it->cgOptions, it->cgbl,
						it->vl, it->dbgStack, 0);
	}
}

static void
loop_debug_terminal(ir_loop* ir, ir_output_traverser_visitor* it)
{
	if (it->cgOptions == DBG_CG_ORIGINAL_SRC)
		return;

	if (ir->mode == ir_loop_for && ir->debug_target
			&& ir->debug_state_internal == ir_dbg_loop_qyr_terminal) {
		cgAddDbgCode(CG_TYPE_RESULT, &it->buffer, it->cgOptions, it->cgbl,
									it->vl, it->dbgStack, 0);
		if (!ir->debug_terminal->block_empty())
			ralloc_asprintf_append(&it->buffer, ", ");
	}

	DbgCgOptions opts = it->cgOptions;
	it->cgOptions = DBG_CG_ORIGINAL_SRC;
	it->visit_block(ir->debug_terminal, ", ");
	it->cgOptions = opts;
}

static void
loop_debug_end(ir_loop* ir, ir_output_traverser_visitor* it)
{
	if (it->cgOptions == DBG_CG_ORIGINAL_SRC)
		return;

	it->indentation++;
	if (ir->need_dbgiter()) {
		it->indent();
		ralloc_asprintf_append (&it->buffer, "%s++;\n", ir->debug_iter_name);
	}

	if (ir->mode == ir_loop_for && !ir->debug_target &&
			ir->debug_state_internal == ir_dbg_loop_wrk_init) {
		it->indentation--;
		it->indent();
		ralloc_asprintf_append (&it->buffer, "}\n");
	}
	it->indentation--;

	it->indent();
}

void
ir_output_traverser_visitor::visit(ir_loop *ir)
{
	if (this->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target)
		this->dbgTargetProcessed = true;

	loop_debug_init(ir, this);

	if (ir->mode == ir_loop_for) {
		ralloc_asprintf_append (&buffer, "for (");
		ralloc_asprintf_append (&buffer, "; ");
		loop_debug_condition(ir, this);
		ralloc_asprintf_append (&buffer, "; ");
		loop_debug_terminal(ir, this);
		ralloc_asprintf_append (&buffer, ") {\n");
		visit_block(&ir->body_instructions, ";\n");
		loop_debug_end(ir, this);
		ralloc_asprintf_append (&buffer, "}");
		return;
	}

	// While loops

	if (ir->mode == ir_loop_while) {
		ralloc_asprintf_append (&buffer, "while (");
		loop_debug_condition(ir, this);
		ralloc_asprintf_append (&buffer, ") {\n");
	} else {
		ralloc_asprintf_append (&buffer, "do {\n");
	}

	visit_block(&ir->body_instructions, ";\n");
	visit_block(ir->debug_terminal, ";\n", true);
	loop_debug_end(ir, this);

	if (ir->mode == ir_loop_while) {
		ralloc_asprintf_append (&buffer, "}");
	} else {
		ralloc_asprintf_append (&buffer, "while (");
		loop_debug_condition(ir, this);
		ralloc_asprintf_append (&buffer, ")");
	}
}


void
ir_output_traverser_visitor::visit(ir_loop_jump *ir)
{
   ralloc_asprintf_append (&buffer, "%s", ir->is_break() ? "break" : "continue");
}

void
ir_output_traverser_visitor::visit(ir_dummy* ir)
{
	if (ir->debug_target && this->cgOptions != DBG_CG_ORIGINAL_SRC) {
	    this->dbgTargetProcessed = true;
	    indent();
	    cgAddDbgCode( CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0 );
	    ralloc_asprintf_append( &buffer, ";\n" );
	}
}

void
ir_output_traverser_visitor::visit(ir_typedecl_statement *ir)
{
	const glsl_type *const s = ir->type_decl;
	ralloc_asprintf_append (&buffer, "struct %s {\n", s->name);

	for (unsigned j = 0; j < s->length; j++) {
		ralloc_asprintf_append (&buffer, "  ");
		//if (state->es_shader)
		//	ralloc_asprintf_append (&buffer, "%s", get_precision_string(s->fields.structure[j].precision));
		buffer = print_type(buffer, s->fields.structure[j].type, false);
		ralloc_asprintf_append (&buffer, " %s", s->fields.structure[j].name);
		buffer = print_type_post(buffer, s->fields.structure[j].type, false);
		ralloc_asprintf_append (&buffer, ";\n");
	}
	ralloc_asprintf_append (&buffer, "}");
}

void
ir_output_traverser_visitor::visit(ir_emit_vertex *ir)
{
	if (this->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target) {
		/* Special case for geometry shaders "EmitVertex()" */

		switch (this->cgOptions) {
			case DBG_CG_GEOMETRY_MAP:
				ralloc_asprintf_append (&buffer, "EmitVertex();\n");
				indent();
				cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
				break;
			case DBG_CG_GEOMETRY_CHANGEABLE:
				/* Check if changeable in scope here */
				if( cgbl && vl ){
					scopeList *sl = get_scope(ir);
					scopeList::iterator sit;

					int id;
					bool allInScope = true;

					for( id = 0; id < cgbl->numChangeables; id++ ){
						bool inScope = false;

						/* builtins are always valid */
						ShVariable *var = findShVariableFromId(vl, cgbl->changeables[id]->id );
						if( !var ){
							printf("CodeGen - unkown changeable, stop debugging\n");
							return;
						}

						if( var->builtin ){
							inScope = true;
						}else{
							/* parse the actual scope */
							for( sit = sl->begin(); sit != sl->end(); sit++ ){
								if( ( *sit ) == cgbl->changeables[id]->id ){
									inScope = true;
								}
							}
						}

						if( !inScope ){
							allInScope = false;
							break;
						}
					}

					if( allInScope ){
						cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack,
									 CG_GEOM_CHANGEABLE_IN_SCOPE);
					}else{
						cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack,
									 CG_GEOM_CHANGEABLE_NO_SCOPE);
					}

					ralloc_asprintf_append (&buffer, "\n");
					indent();
				}

				/* Add original function call */
				ralloc_asprintf_append (&buffer, "EmitVertex();\n");
				break;
			case DBG_CG_VERTEX_COUNT:
				cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 0);
				//			oit->parseContext->resources->geoOutputType );
				break;
			case DBG_CG_COVERAGE:
			case DBG_CG_CHANGEABLE:
			case DBG_CG_SELECTION_CONDITIONAL:
			case DBG_CG_LOOP_CONDITIONAL:
				//oit->sequenceNoOperation = true;
				break;
			default:
				ralloc_asprintf_append (&buffer, "EmitVertex();\n");
				break;
		}
	} else {
		ralloc_asprintf_append (&buffer, "emit-vertex-TODO");
	}
}

void
ir_output_traverser_visitor::visit(ir_end_primitive *ir)
{
	/* Special case for geometry shaders "EndPrimitive()" */
	if (this->cgOptions != DBG_CG_ORIGINAL_SRC && ir->debug_target) {
		switch (this->cgOptions) {
			case DBG_CG_GEOMETRY_MAP:
				ralloc_asprintf_append (&buffer, "EndPrimitive();\n");
				indent();
				cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 1);
				break;
			case DBG_CG_VERTEX_COUNT:
				cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, 1);
						//this->parseContext->resources->geoOutputType );
				break;
			case DBG_CG_COVERAGE:
			case DBG_CG_CHANGEABLE:
			case DBG_CG_SELECTION_CONDITIONAL:
			case DBG_CG_LOOP_CONDITIONAL:
				//this->sequenceNoOperation = true;
				break;
			default:
				ralloc_asprintf_append (&buffer, "EndPrimitive();\n");
				break;
		}
	} else {
		ralloc_asprintf_append (&buffer, "end-primitive-TODO");
	}
}
