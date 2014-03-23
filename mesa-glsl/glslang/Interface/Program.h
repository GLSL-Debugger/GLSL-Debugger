/*
 * Program.h
 *
 *  Created on: 08.09.2013
 */

#ifndef PROGRAM_INTERFACE_TO_MESA_H_
#define PROGRAM_INTERFACE_TO_MESA_H_

#include "ShaderLang.h"

struct AstShader;
struct gl_context;

void clearTraverseDebugJump(void);
void resetDbgResult(DbgResult& r, bool initialized);
DbgResult* ShaderTraverse(AstShader* shader, int debugOptions, int dbgBh);
bool ShaderVarTraverse(AstShader* shader, ShVariableList *vl);
bool compileShaderCode(AstShader* shader);
bool compileDbgShaderCode(AstShader* shader, ShChangeableList *cgbl, ShVariableList *vl,
		DbgCgOptions dbgCgOptions, char** code);

void compile_shader_to_ast(struct gl_context *ctx, struct AstShader *shader, int debug);

#endif /* PROGRAM_INTERFACE_TO_MESA */
