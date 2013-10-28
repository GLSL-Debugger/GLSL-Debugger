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
#include "mesa/program/hash_table.h"

#include "glslang/Interface/CodeTools.h"


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
	if( !globals )
		globals = new global_print_tracker;

   foreach_iter(exec_list_iterator, iter, *instructions) {
      ir_instruction *ir = (ir_instruction *)iter.get();
	  if (ir->ir_type == ir_type_variable) {
		ir_variable *var = static_cast<ir_variable*>(ir);
		if ((strstr(var->name, "gl_") == var->name)
			  && !var->invariant)
			continue;
	  }

	  ir->accept(this);
      if (ir->ir_type != ir_type_function)
		ralloc_asprintf_append (&buffer, ";\n");
   }
}


void ir_output_traverser_visitor::indent(void)
{
   for (int i = 0; i < indentation; i++)
      ralloc_asprintf_append (&buffer, "  ");
}

void ir_output_traverser_visitor::print_var_name (ir_variable* v)
{
    long id = (long)hash_table_find (globals->var_hash, v);
	if (!id && v->mode == ir_var_temporary)
	{
        id = ++globals->var_counter;
        hash_table_insert (globals->var_hash, (void*)id, v);
	}
    if (id)
    {
        if (v->mode == ir_var_temporary)
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
   const char *const cent = (ir->centroid) ? "centroid " : "";
   const char *const inv = (ir->invariant) ? "invariant " : "";
   const char *const mode[3][ir_var_mode_count] =
   {
	{ "", "uniform ", "in ",        "out ",     "in ", "out ", "inout ", "", "", "" },
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
   if ((this->mode == EShLangNone && ir->mode != ir_var_uniform))
   {
     long id = (long)hash_table_find (globals->var_hash, ir);
     if (id == 0)
     {
       id = ++globals->var_counter;
       hash_table_insert (globals->var_hash, (void*)id, ir);
     }
   }

   check_initializer(ir, this);

   // keep invariant declaration for builtin variables
   if (strstr(ir->name, "gl_") == ir->name) {
      ralloc_asprintf_append (&buffer, "%s", inv);
      print_var_name (ir);
      return;
   }

   ralloc_asprintf_append (&buffer, "%s%s%s%s",
	  cent, inv, interp[ir->interpolation], mode[decormode][ir->mode]);
   print_precision (ir, ir->type);
   buffer = print_type(buffer, ir->type, false);
   ralloc_asprintf_append (&buffer, " ");
   print_var_name (ir);
   buffer = print_type_post(buffer, ir->type, false);

	if (ir->constant_value &&
		ir->mode != ir_var_shader_in &&
		ir->mode != ir_var_shader_out &&
		ir->mode != ir_var_function_in &&
		ir->mode != ir_var_function_out &&
		ir->mode != ir_var_function_inout)
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
   //
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
	   foreach_iter(exec_list_iterator, iter, ir->parameters) {
		  ir_variable *const inst = (ir_variable *) iter.get();

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
   indentation++;

	// insert postponed global assigments
	if (strcmp(ir->function()->name, "main") == 0)
	{
		assert (!globals->main_function_done);
		globals->main_function_done = true;
		foreach_iter(exec_list_iterator, it, globals->global_assignements)
		{
			ir_instruction* as = ((ga_entry *)it.get())->ir;
			as->accept(this);
			ralloc_asprintf_append(&buffer, ";\n");
		}
	}

   foreach_iter(exec_list_iterator, iter, ir->body) {
      ir_instruction *const inst = (ir_instruction *) iter.get();

      indent();
      inst->accept(this);
	  ralloc_asprintf_append (&buffer, ";\n");
   }
   indentation--;
   indent();
   ralloc_asprintf_append (&buffer, "}\n");
}


void ir_output_traverser_visitor::visit(ir_function *ir)
{
   bool found_non_builtin_proto = false;

   foreach_iter(exec_list_iterator, iter, *ir) {
      ir_function_signature *const sig = (ir_function_signature *) iter.get();
      if (!sig->is_builtin)
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
      foreach_iter(exec_list_iterator, iter, *ir) {
         ir_instruction* sig = (ir_instruction*)iter.get();
         // Double only path signature
         if( sig->debug_state != ir_dbg_state_path )
            continue;

         indent();
         sig->accept(this);
         ralloc_asprintf_append (&buffer, "\n");
      }
      this->cgOptions = option;
   }

   foreach_iter(exec_list_iterator, iter, *ir) {
      ir_function_signature *const sig = (ir_function_signature *) iter.get();

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
	//"normalize",
	"exp",
	"log",
	"exp2",
	"log2",
	"int",		// f2i
	"int",		// f2u
	"float",	// i2f
	"bool",		// f2b
	"float",	// b2f
	"bool",		// i2b
	"int",		// b2i
	"float",	// u2f
	"int",		// i2u
	"int",		// u2i
	"float",	// bit i2f
	"int",		// bit f2i
	"float",	// bit u2f
	"int",		// bit f2u
	"any",
	"trunc",
	"ceil",
	"floor",
	"fract",
	"roundEven",
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
	"unpackHalf2x16_splitX_TODO",
	"unpackHalf2x16_splitY_TODO",
	"bitfieldReverse",
	"bitCount",
	"findMSB",
	"findLSB",
	"noise",
	"+",
	"-",
	"*",
	"/",
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
	"uboloadTODO",
	"vectorExtract_TODO",
	"fma",
//	"clamp",
//	"mix",
	"bfi_TODO",
	"bitfield_extract_TODO",
	"vector_insert_TODO",
	"bitfield_insert_TODO",
	"vectorTODO",
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
		sampler_uv_dim = 3;
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
	if (!dstIndex && lhsType->matrix_columns <= 1 && lhsType->vector_elements > 1 && write_mask != (1<<lhsType->vector_elements)-1)
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
		ralloc_asprintf_append(&buffer, "//"); // for the ; that will follow (ugly, I know)
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
		bool skip_assign = false;
		ir_dereference_variable* lhsDeref = ir->lhs->as_dereference_variable();
		ir_dereference_variable* rhsDeref = rhsOp->operands[0]->as_dereference_variable();
		if (lhsDeref && rhsDeref)
		{
			if (lhsDeref->var == rhsDeref->var)
				skip_assign = true;
		}

		if (!skip_assign)
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
			foreach_iter(exec_list_iterator, iter, *ir) {
				ir_instruction * const inst = (ir_instruction *)iter.get();

				if( !first )
					ralloc_asprintf_append( &it->buffer, ", " );

				if( i == lastInParameter ){
					if( !getSideEffectsDebugParameter( ir, lastInParameter ) ){
						/* No special care necessary, just add it before */
						ralloc_asprintf_append (&it->buffer, "(");
						cgAddDbgCode( CG_TYPE_RESULT, &it->buffer, it->cgOptions,
								it->cgbl, it->vl, it->dbgStack, 0 );
						// FIXME: WTF? WHAT LANGUAGE ORIGIANAL USED?
						ralloc_asprintf_append (&it->buffer, ", ");
						inst->accept( it );
						ralloc_asprintf_append (&it->buffer, ")");
					}else{
						/* Copy to temporary, debug, and copy back */
						ralloc_asprintf_append (&it->buffer, "(");
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
						ralloc_asprintf_append (&it->buffer, ")");
					}
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
			foreach_iter(exec_list_iterator, iter, *ir) {
				ir_instruction * const inst = (ir_instruction *)iter.get();
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
		foreach_iter(exec_list_iterator, iter, *ir) {
			ir_instruction * const inst = (ir_instruction *)iter.get();
			if( !first )
				ralloc_asprintf_append( &it->buffer, ", " );
			inst->accept( it );
			first = false;
		}
		ralloc_asprintf_append( &it->buffer, ")" );
	}
	// TODO: will be available in next mesa
//	else if( it->mode == EShLangGeometry
//			&& strcmp( node->getName().c_str(), EMIT_VERTEX_SIG ) == 0
//			&& !( node->isUserDefined() ) ){
//		/* Special case for geometry shaders "EmitVertex()" */
//
//		switch( oit->cgOptions ){
//			case DBG_CG_GEOMETRY_MAP:
//				oit->debugProgram += getFunctionName( node->getName() );
//				processAggregateChildren( node, it, "(", ", ", ")" );
//				oit->debugProgram += ";\n";
//				outputIndentation( oit, oit->depth );
//				cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//						oit->cgbl, oit->vl, oit->dbgStack, 0 );
//				break;
//			case DBG_CG_GEOMETRY_CHANGEABLE:
//				/* Check if changeable in scope here */
//				if( oit->cgbl && oit->vl ){
//					scopeList *sl = node->getScope();
//					scopeList::iterator sit;
//
//					int id;
//					bool allInScope = true;
//
//					for( id = 0; id < oit->cgbl->numChangeables; id++ ){
//						bool inScope = false;
//
//						/* builtins are always valid */
//						ShVariable *var = findShVariableFromId( oit->vl,
//								oit->cgbl->changeables[id]->id );
//						if( !var ){
//							dbgPrint( DBGLVL_WARNING,
//									"CodeGen - unkown changeable, stop debugging\n" );
//							return false;
//						}
//
//						if( var->builtin ){
//							inScope = true;
//						}else{
//							/* parse the actual scope */
//							for( sit = sl->begin(); sit != sl->end(); sit++ ){
//								if( ( *sit ) == oit->cgbl->changeables[id]->id ){
//									inScope = true;
//								}
//							}
//						}
//
//						if( !inScope ){
//							allInScope = false;
//							break;
//						}
//					}
//
//					if( allInScope ){
//						cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//								oit->cgbl, oit->vl, oit->dbgStack,
//								CG_GEOM_CHANGEABLE_IN_SCOPE );
//					}else{
//						cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//								oit->cgbl, oit->vl, oit->dbgStack,
//								CG_GEOM_CHANGEABLE_NO_SCOPE );
//					}
//					oit->debugProgram += ";\n";
//					outputIndentation( oit, oit->depth );
//				}
//
//				/* Add original function call */
//				oit->debugProgram += getFunctionName( node->getName() );
//				processAggregateChildren( node, it, "(", ", ", ")" );
//				break;
//			case DBG_CG_VERTEX_COUNT:
//				cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//						oit->cgbl, oit->vl, oit->dbgStack, 0,
//						oit->parseContext->resources->geoOutputType );
//				break;
//			case DBG_CG_COVERAGE:
//			case DBG_CG_CHANGEABLE:
//			case DBG_CG_SELECTION_CONDITIONAL:
//			case DBG_CG_LOOP_CONDITIONAL:
//				oit->sequenceNoOperation = true;
//				break;
//			default:
//				oit->debugProgram += getFunctionName( node->getName() );
//				processAggregateChildren( node, it, "(", ", ", ")" );
//				break;
//		}
//	}else if( oit->language == EShLangGeometry
//			&& strcmp( node->getName().c_str(), END_PRIMITIVE_SIG ) == 0
//			&& !( node->isUserDefined() ) ){
//		/* Special case for geometry shaders "EndPrimitive()" */
//		switch( oit->cgOptions ){
//			case DBG_CG_GEOMETRY_MAP:
//				oit->debugProgram += getFunctionName( node->getName() );
//				processAggregateChildren( node, it, "(", ", ", ")" );
//				oit->debugProgram += ";\n";
//				outputIndentation( oit, oit->depth );
//				cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//						oit->cgbl, oit->vl, oit->dbgStack, 1 );
//				break;
//			case DBG_CG_VERTEX_COUNT:
//				cgAddDbgCode( CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//						oit->cgbl, oit->vl, oit->dbgStack, 1,
//						oit->parseContext->resources->geoOutputType );
//				break;
//			case DBG_CG_COVERAGE:
//			case DBG_CG_CHANGEABLE:
//			case DBG_CG_SELECTION_CONDITIONAL:
//			case DBG_CG_LOOP_CONDITIONAL:
//				oit->sequenceNoOperation = true;
//				break;
//			default:
//				oit->debugProgram += getFunctionName( node->getName() );
//				processAggregateChildren( node, it, "(", ", ", ")" );
//				break;
//		}
//	}
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
   foreach_iter(exec_list_iterator, iter, *ir) {
      ir_instruction *const inst = (ir_instruction *) iter.get();
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


void
ir_output_traverser_visitor::visit(ir_if *ir)
{
	bool copyCondition;
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
                        cgAddDbgCode(CG_TYPE_RESULT, &buffer, cgOptions, cgbl,
                        				vl, dbgStack, 0);
                        ralloc_asprintf_append (&buffer, ";\n");
                        indent();
                        break;
                    case ir_dbg_if_condition_passed:
                    case ir_dbg_if_if:
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
   indentation++;

   if( !ir->then_instructions.is_empty() && ir->debug_target &&
		   this->cgOptions == DBG_CG_SELECTION_CONDITIONAL &&
		   ir->debug_state_internal == ir_dbg_if_condition_passed) {
		/* Add code to colorize condition */
	   indent();
       cgAddDbgCode( CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, true);
       ralloc_asprintf_append (&buffer, ";\n");
   }

   foreach_iter(exec_list_iterator, iter, ir->then_instructions) {
      ir_instruction *const inst = (ir_instruction *) iter.get();

      indent();
      inst->accept(this);
      ralloc_asprintf_append (&buffer, ";\n");
   }

   indentation--;
   indent();
   ralloc_asprintf_append (&buffer, "}");

   if (!ir->else_instructions.is_empty())
   {
		if( ir->debug_target && this->cgOptions == DBG_CG_SELECTION_CONDITIONAL
				&& ir->debug_state_internal == ir_dbg_if_condition_passed ){
			/* Add code to colorize condition */
			indent();
			cgAddDbgCode( CG_TYPE_RESULT, &buffer, cgOptions, cgbl, vl, dbgStack, true );
			ralloc_asprintf_append( &buffer, ";\n" );
		}

	   ralloc_asprintf_append (&buffer, " else {\n");
	   indentation++;

	   foreach_iter(exec_list_iterator, iter, ir->else_instructions) {
		  ir_instruction *const inst = (ir_instruction *) iter.get();

		  indent();
		  inst->accept(this);
		  ralloc_asprintf_append (&buffer, ";\n");
	   }
	   indentation--;
	   indent();
	   ralloc_asprintf_append (&buffer, "}");
   }
}


void
ir_output_traverser_visitor::visit(ir_loop *ir)
{
	// TODO: loop
	printf("TODO: loop debug output");

	bool noData = (ir->counter == NULL && ir->from == NULL && ir->to == NULL && ir->increment == NULL);
	if (noData) {
		ralloc_asprintf_append (&buffer, "while (true) {\n");
		indentation++;
		foreach_iter(exec_list_iterator, iter, ir->body_instructions) {
			ir_instruction *const inst = (ir_instruction *) iter.get();
			indent();
			inst->accept(this);
			ralloc_asprintf_append (&buffer, ";\n");
		}
		indentation--;
		indent();
		ralloc_asprintf_append (&buffer, "}");
		return;
	}

	bool canonicalFor = (ir->counter && ir->from && ir->to && ir->increment);
	if (canonicalFor)
	{
		ralloc_asprintf_append (&buffer, "for (");
		ir->counter->accept (this);
		ralloc_asprintf_append (&buffer, " = ");
		ir->from->accept (this);
		ralloc_asprintf_append (&buffer, "; ");
		print_var_name (ir->counter);

		// IR cmp operator is when to terminate loop; whereas GLSL for loop syntax
		// is while to continue the loop. Invert the meaning of operator when outputting.
		const char* termOp = NULL;
		switch (ir->cmp) {
		case ir_binop_less: termOp = ">="; break;
		case ir_binop_greater: termOp = "<="; break;
		case ir_binop_lequal: termOp = ">"; break;
		case ir_binop_gequal: termOp = "<"; break;
		case ir_binop_equal: termOp = "!="; break;
		case ir_binop_nequal: termOp = "=="; break;
		default: assert(false);
		}
		ralloc_asprintf_append (&buffer, " %s ", termOp);
		ir->to->accept (this);
		ralloc_asprintf_append (&buffer, "; ");
		// IR already has instructions that modify the loop counter in the body
		//print_var_name (ir->counter);
		//ralloc_asprintf_append (&buffer, " = ");
		//print_var_name (ir->counter);
		//ralloc_asprintf_append (&buffer, "+(");
		//ir->increment->accept (this);
		//ralloc_asprintf_append (&buffer, ")");
		ralloc_asprintf_append (&buffer, ") {\n");
		indentation++;
		foreach_iter(exec_list_iterator, iter, ir->body_instructions) {
			ir_instruction *const inst = (ir_instruction *) iter.get();
			indent();
			inst->accept(this);
			ralloc_asprintf_append (&buffer, ";\n");
		}
		indentation--;
		indent();
		ralloc_asprintf_append (&buffer, "}");
		return;
	}


   ralloc_asprintf_append (&buffer, "( TODO loop (");
   if (ir->counter != NULL)
      ir->counter->accept(this);
   ralloc_asprintf_append (&buffer, ") (");
   if (ir->from != NULL)
      ir->from->accept(this);
   ralloc_asprintf_append (&buffer, ") (");
   if (ir->to != NULL)
      ir->to->accept(this);
   ralloc_asprintf_append (&buffer, ") (");
   if (ir->increment != NULL)
      ir->increment->accept(this);
   ralloc_asprintf_append (&buffer, ") (\n");
   indentation++;

   foreach_iter(exec_list_iterator, iter, ir->body_instructions) {
      ir_instruction *const inst = (ir_instruction *) iter.get();

      indent();
      inst->accept(this);
      ralloc_asprintf_append (&buffer, ";\n");
   }
   indentation--;
   indent();
   ralloc_asprintf_append (&buffer, "))\n");
}


void
ir_output_traverser_visitor::visit(ir_loop_jump *ir)
{
   ralloc_asprintf_append (&buffer, "%s", ir->is_break() ? "break" : "continue");
}
