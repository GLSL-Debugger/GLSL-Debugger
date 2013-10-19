#ifndef _CODE_TOOLS_
#define _CODE_TOOLS_

#include "../Public/ShaderLang.h"
#include "../Include/Common.h"
#include "../Include/intermediate.h"

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
//bool isChildofMain(TIntermNode *node, TIntermNode *root);
int getFunctionDebugParameter(ir_function_signature *node);
ir_instruction* getIRDebugParameter(exec_list *list, int pnum);
//bool getAtomicDebugParameter(TIntermAggregate *node, int pnum);
ir_instruction* getSideEffectsDebugParameter(ir_call *ir, int pnum);
//bool isPartofMain(TIntermNode *target, TIntermNode *root);
//bool isPartofNode(TIntermNode *target, TIntermNode *node);

#endif

