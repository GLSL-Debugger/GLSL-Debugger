/*
 * Shader.cpp
 *
 *  Created on: 20.09.2013
 */

#include "MShader.h"
#include "glsl/list.h"
#include "glsl/glsl_symbol_table.h"

void appendToChangeableList( ShChangeableList* table, exec_list* symbols,
		enum ir_node_type type )
{

	foreach_list(node, symbols) {
		ir_instruction *const ir = (ir_instruction *) node;
		if( type < ir_type_max && ir->ir_type != type )
			continue;

		switch( ir->ir_type ){
			case ir_type_variable:
			case ir_type_function:
			{
				ShChangeable* cgbl = createShChangeable(-1);
				cgbl->data = ir;
				addShChangeable(table, cgbl);
				break;
			}
			default:
				break;
		}

	}
}
