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

__inline std::string FormatSourceRange(const YYLTYPE& range)
{
    char locText[128];

    sprintf(locText, "%4d:%3d - %4d:%3d", range.first_line, range.first_column,
                                        range.last_line, range.last_column);
    return std::string(locText);
}


ir_function* getFunctionBySignature( const char *sig, struct gl_shader* shader );
DbgResult* ShaderTraverse( struct gl_shader* shader, int debugOptions, int dbgBh );
bool ShaderVarTraverse( struct gl_shader* shader, ShVariableList *vl );
void TraverseList( exec_list* list, TIntermTraverser* it );
void Traverse( ir_instruction* node, TIntermTraverser* it );


#endif /* PROGRAM_INTERFACE_TO_MESA */
