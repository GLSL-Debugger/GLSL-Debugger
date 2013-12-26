#ifndef _CODE_TOOLS_
#define _CODE_TOOLS_

#include "../Public/ShaderLang.h"
#include "../Include/Common.h"
#include "ir.h"

#define MAIN_FUNC_SIGNATURE "main"

std::string getMangledName(ir_function_signature* func);


__inline std::string FormatSourceRange(const YYLTYPE& range)
{
    char locText[128];

    sprintf(locText, "%4d:%3d - %4d:%3d", range.first_line, range.first_column,
                                        range.last_line, range.last_column);
    return std::string(locText);
}

char* getFunctionName(const char* in);
ir_function* getFunctionBySignature( const char *sig, struct gl_shader* shader );
int getFunctionDebugParameter(ir_function_signature *node);
ir_instruction* getIRDebugParameter(exec_list *list, int pnum);
ir_instruction* getSideEffectsDebugParameter(ir_call *ir, int pnum);

#endif

