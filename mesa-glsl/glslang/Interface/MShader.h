/*
 * Shader.h
 *
 *  Created on: 20.09.2013
 */

#ifndef MESA_INTERFACE_SHADER_H_
#define MESA_INTERFACE_SHADER_H_

#include <string>
#include "ShaderLang.h"
#include "ir.h"

struct exec_list;

std::string getCodeArraySize( const struct glsl_type* type );
std::string getCodeString( ir_variable* var, bool withQualifier = false,
							EShLanguage l = EShLangFragment );
std::string getCodeString( const struct glsl_type* type );


bool containsDiscard( ir_instruction* );
bool containsDiscard( exec_list* );
bool containsEmitVertex( ir_instruction* );
bool containsEmitVertex( exec_list* );

bool dbg_state_not_match( exec_list*, enum ir_dbg_state );
char** dbg_iter_name( ir_loop* );
ir_list_dummy* list_dummy( exec_list*, ir_function_signature* );

void init_shader();
void clean_shader();



#endif /* MESA_INTERFACE_SHADER_H_ */
