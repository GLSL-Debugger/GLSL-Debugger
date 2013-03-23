#ifndef _CODE_INSERTION_
#define _CODE_INSERTION_

#include "../Public/ShaderLang.h"
#include "../Include/Common.h"
#include "../Include/intermediate.h"

enum cgTypes {
    CG_TYPE_RESULT,
    CG_TYPE_CONDITION,
    CG_TYPE_PARAMETER,
	CG_TYPE_LOOP_ITERS,
    CG_TYPE_ALL
};

enum cgInitialization {
    CG_INIT_BLACK,
    CG_INIT_WHITE,
    CG_INIT_CHESS,
    CG_INIT_GEOMAP
};

enum cgGeomChangeable {
    CG_GEOM_CHANGEABLE_AT_TARGET = 0,
    CG_GEOM_CHANGEABLE_IN_SCOPE,
    CG_GEOM_CHANGEABLE_NO_SCOPE
};

/* Stack to hold complete dbgPath */
typedef std::vector<TIntermNode*> TIntermNodeStack;

/* helper functions */
char* itoSwizzle(int i);

/* code generation */
void cgGetNewName(char **name, ShVariableList *vl, const char *prefix);
void cgInit(cgTypes type, ShVariable *v, ShVariableList *vl, EShLanguage l);
void cgAddDeclaration(cgTypes type, TString &prog, EShLanguage l);
void cgAddDbgCode(cgTypes type, TString &prog, DbgCgOptions cgOptions,
                  ShChangeableList *src, ShVariableList *vl, 
                  TIntermNodeStack *stack, int option, int outPrimType = 0x0000);
void cgAddOutput(cgTypes type, TString &prog, EShLanguage l, TQualifier o = EvqTemporary);
void cgAddInitialization(cgTypes type, cgInitialization init,
                         TString &prog, EShLanguage l);
void cgAddAssignment(cgTypes type, ShVariable *src);
void cgDestruct(cgTypes type);

void cgInitNameMap(void);
void cgInitLoopIter(void);
const char* cgGetDebugName(const char *input, TIntermNode *root);

void cgSetLoopIterName(char **name, ShVariableList *vl);
void cgResetLoopIterNames(void);

#endif
