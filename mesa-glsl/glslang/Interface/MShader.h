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

bool containsDiscard( ir_instruction* );
bool containsDiscard( exec_list* );


#endif /* MESA_INTERFACE_SHADER_H_ */
