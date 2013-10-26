/*
 * Program.h
 *
 *  Created on: 08.09.2013
 */

#ifndef PROGRAM_INTERFACE_TO_MESA_H_
#define PROGRAM_INTERFACE_TO_MESA_H_

#include "ShaderLang.h"
#include <string>
#include "ir.h"

class exec_list;
class ir_instruction;
class ir_function;
class TIntermTraverser;

void clearTraverseDebugJump(void);
DbgResult* ShaderTraverse( struct gl_shader* shader, int debugOptions, int dbgBh );
bool ShaderVarTraverse( struct gl_shader* shader, ShVariableList *vl );
bool compileShaderCode(struct gl_shader* shader);
bool compileDbgShaderCode(struct gl_shader* shader, ShChangeableList *cgbl,
        ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code);
void TraverseList( exec_list* list, TIntermTraverser* it );
void Traverse( ir_instruction* node, TIntermTraverser* it );


#endif /* PROGRAM_INTERFACE_TO_MESA */
