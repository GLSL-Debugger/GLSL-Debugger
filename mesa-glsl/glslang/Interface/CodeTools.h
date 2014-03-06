#ifndef _CODE_TOOLS_
#define _CODE_TOOLS_

#include "ast.h"
#include "ShaderHolder.h"
#include "ir.h"
#include <string>


#define MAIN_FUNC_SIGNATURE "main"
#define EMIT_VERTEX_SIGNATURE "EmitVertex"
#define END_PRIMITIVE_SIGNATURE "EndPrimitive"

struct exec_list;

__inline std::string FormatSourceRange(const YYLTYPE& range)
{
    char locText[128];

    sprintf(locText, "%4d:%3d - %4d:%3d", range.first_line, range.first_column - 1,
                                        range.last_line, range.last_column - 1);
    return std::string(locText);
}

long strToSwizzleIdx(const char*);
std::string getMangledName(ir_function_signature* func);
char* getFunctionName(const char* in);
int getFunctionDebugParameter(ir_function_signature* node);
ir_instruction* getIRDebugParameter(exec_list* list, int pnum);
ir_instruction* getSideEffectsDebugParameter(ir_call* ir, int pnum);

bool list_iter_check(ir_instruction* const ir, int& state);
bool dbg_state_not_match(ast_node* node, enum ast_dbg_state state);

#endif

