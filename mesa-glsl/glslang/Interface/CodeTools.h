#ifndef _CODE_TOOLS_
#define _CODE_TOOLS_

#include "ast.h"
#include "ShaderHolder.h"
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

void dumpNodeInfo(ast_node* node);
void dumpDbgStack(AstStack *stack);

long strToSwizzleIdx(const char *);
bool partof(ast_node* target, ast_node* node);
std::string getMangledName(ast_function_definition *, AstShader*);
char* getFunctionName(const char *);
int getFunctionDebugParameter(ast_function_definition *);
ast_node* getSideEffectsDebugParameter(ast_function_expression *, int);

bool dbg_state_not_match(ast_node* node, enum ast_dbg_state state);

#endif

