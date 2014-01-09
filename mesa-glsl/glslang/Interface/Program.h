/*
 * Program.h
 *
 *  Created on: 08.09.2013
 */

#ifndef PROGRAM_INTERFACE_TO_MESA_H_
#define PROGRAM_INTERFACE_TO_MESA_H_

#include "ShaderLang.h"

struct gl_shader;

void clearTraverseDebugJump(void);
DbgResult* ShaderTraverse(struct gl_shader* shader, int debugOptions, int dbgBh);
bool ShaderVarTraverse(struct gl_shader* shader, ShVariableList *vl);
void printShaderIr(struct gl_shader* shader);
bool compileShaderCode(struct gl_shader* shader);
bool compileDbgShaderCode(struct gl_shader* shader, ShChangeableList *cgbl,
		ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code);


#endif /* PROGRAM_INTERFACE_TO_MESA */
