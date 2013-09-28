/*
 * Program.h
 *
 *  Created on: 08.09.2013
 */

#ifndef PROGRAM_INTERFACE_TO_MESA_H_
#define PROGRAM_INTERFACE_TO_MESA_H_

#include "ShaderLang.h"

class exec_list;
class ir_instruction;
class TIntermTraverser;

DbgResult* ShaderTraverse( struct gl_shader* shader, int debugOptions, int dbgBh );
void TraverseList( exec_list* list, TIntermTraverser* it );
void Traverse( ir_instruction* node, TIntermTraverser* it );


#endif /* PROGRAM_INTERFACE_TO_MESA */
