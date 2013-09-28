/*
 * Shader.h
 *
 *  Created on: 20.09.2013
 */

#ifndef MESA_INTERFACE_SHADER_H_
#define MESA_INTERFACE_SHADER_H_

#include "ir.h"
#include "ShaderLang.h"

struct exec_list;


void appendToChangeableList( ShChangeableList* table, exec_list* symbols, enum ir_node_type = ir_type_max );


#endif /* MESA_INTERFACE_SHADER_H_ */
