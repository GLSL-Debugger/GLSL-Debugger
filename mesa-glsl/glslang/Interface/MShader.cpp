/*
 * Shader.cpp
 *
 *  Created on: 20.09.2013
 */

#include "MShader.h"
#include "glsl/list.h"
#include "glsl/glsl_symbol_table.h"
#include "BaseTypes.h"

#include <map>

namespace {
	typedef std::map< ir_loop*, char* > IterNameMap;
	typedef std::map< exec_list*, ir_list_dummy* > DebugInfoMap;
	IterNameMap iter_names;
	DebugInfoMap debug_infos;
//	void* global_ctx;
}


std::string getCodeArraySize( const struct glsl_type* type )
{
	std::string out = "";
	if ( type->is_array() ){
		char buffer[30];
		for (unsigned i = 0; i < type->length; ++i) {
			// FIXME? if (v->getType().getArraySize(i) != 0) {
			sprintf(buffer, "[%i]", type->fields.array[i].length);
			out += buffer;
		}
	}
    return out;
}

std::string getCodeString( ir_variable* var, bool withQualifier, EShLanguage l  )
{
	if( var->invariant )
		return "invariant";

	std::string out = "";

	if( withQualifier ){
		if( var->interpolation )
			out += std::string(interpolation_string(var->interpolation)) + " ";
		if( var->centroid )
			out += "centroid ";
		if( var->mode > ir_var_auto &&  var->mode < ir_var_temporary )
			out += std::string(getQualifierString(var->mode, l)) + " ";
	}

	return out + getCodeString(var->type);
}

std::string getCodeString( const struct glsl_type* type )
{
    std::string out = "";
    char buf[100];

	if ( type->is_array() ) {
         /* ASSUMPTION: arrays are processed after calling this function!!!
         *  p += sprintf(p, "array of ");
         */
    }
    if ( type->is_record() ) { // Struct
    	// FIXME: How the unnamed struct named in mesa? Typename cannot be null
    	out += std::string(type->name);
    } else if ( type->is_matrix() ) {
        /* For compability stick to older spec for square matrixes */
        if ( type->vector_elements == type->matrix_columns )
        	sprintf(buf, "mat%i", type->vector_elements);
        else
        	sprintf(buf, "mat%ix%i", type->vector_elements, type->matrix_columns);
        out += std::string(buf);
    } else if ( type->is_vector() ) {
        if (type->base_type == GLSL_TYPE_INT)
            out += "i";
        else if (type->base_type == GLSL_TYPE_UINT)
        	out += "u";
        else if (type->base_type == GLSL_TYPE_BOOL)
        	out += "b";
        sprintf(buf, "vec%i", type->length);
        out += std::string(buf);
    } else {
    	out += std::string(type->name);
    }

    return out;
}



bool containsDiscard( ir_instruction* ir )
{
	switch( ir->ir_type ){
		case ir_type_discard:
			return true;
			break;
		case ir_type_assignment:
		{
			ir_assignment* f = ir->as_assignment();
			return containsDiscard(f->lhs) || containsDiscard(f->rhs);
			break;
		}
		case ir_type_expression:
		{
			ir_expression* e = ir->as_expression();
			int operands = e->get_num_operands();
			for( int i = 0; i < operands; ++i )
				if( e->operands[i] && containsDiscard(e->operands[i]) )
					return true;
			break;
		}
		case ir_type_call:
		{
			ir_call* c = ir->as_call();
			return containsDiscard( &(c->callee->body) );
			break;
		}
		default:
			break;
	}
	return false;
}

bool containsDiscard( exec_list* list )
{
	foreach_iter( exec_list_iterator, iter, *list ) {
		ir_instruction* ir = (ir_instruction*)iter.get();
		if( containsDiscard( ir ) )
			return true;
	}
	return false;
}

bool containsEmitVertex( ir_instruction* )
{
	// TODO: will be in newest mesa
	return false;
}

bool containsEmitVertex( exec_list* list )
{
	foreach_iter( exec_list_iterator, iter, *list ) {
		ir_instruction* ir = (ir_instruction*)iter.get();
		if( containsEmitVertex( ir ) )
			return true;
	}
	return false;
}


bool dbg_state_not_match(exec_list* list, enum ir_dbg_state state)
{
	foreach_iter(exec_list_iterator, iter, *list) {
		if( ( (ir_instruction *)iter.get() )->debug_state != state )
			return true;
	}
	return false;
}

char** dbg_iter_name( ir_loop* ir )
{
	IterNameMap::iterator it = iter_names.find(ir);
	if( it != iter_names.end() )
		return &it->second;
	char* name = (char*)malloc(1);
	name[0] = '\0';
	iter_names[ir] = name;
	return &iter_names[ir];
}

/*
ir_list_dummy* list_dummy( exec_list* list, ir_function_signature* parent )
{
	if( list->is_empty() )
		return NULL;
	DebugInfoMap::iterator it = debug_infos.find(list);
	if( it != debug_infos.end() )
		return it->second;

	ir_list_dummy* ir = new(global_ctx) ir_list_dummy;
	ir->ir_type = ir_type_list_dummy;
	ir->debug_state = ir_dbg_state_unset;
	ir->debug_overwrite = ir_dbg_ow_unset;
	ir->debug_target = false;

	ir->yy_location.first_column = ir->yy_location.last_column = parent->yy_location.last_column;
	ir->yy_location.first_line = ir->yy_location.last_line = parent->yy_location.last_line;

	debug_infos[list] = ir;
	return ir;
}
*/

void init_shader( )
{
	//global_ctx = ralloc_context(NULL);
}

void clean_shader( )
{
	iter_names.clear();
	debug_infos.clear();
	//ralloc_free(global_ctx);
}


