#ifndef _CODE_INSERTION_
#define _CODE_INSERTION_

#include "ShaderLang.h"

enum cgTypes {
	CG_TYPE_NONE,
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


class CodeGen
{
public:
	CodeGen(AstShader*);
	~CodeGen();

	void init(cgTypes type, ShVariable *v, ShVariableList *vl, EShLanguage l);
	void allocateResult(ast_node*, EShLanguage, ShVariableList*, DbgCgOptions);
	/* code generation */
	void getNewName(char **name, ShVariableList *vl, const char *prefix);
	void addDeclaration(cgTypes type, char** prog, EShLanguage l);
	void addDbgCode(cgTypes type, char** prog, DbgCgOptions cgOptions,
	                  ShChangeableList *src, ShVariableList *vl,
	                  int option, int outPrimType = 0x0000);
	void addOutput(cgTypes type, char** prog, EShLanguage l, TQualifier o = EvqTemporary);
	void addInitialization(cgTypes type, cgInitialization init, char** prog, EShLanguage l);
	//void addAssignment(cgTypes type, ShVariable *src);
	void destruct(cgTypes type);

	void setIterNames(ShVariableList *vl);
	void resetLoopIterNames(void);

	const char* getDebugName(const char *input);
private:
	AstShader* shader;
	ShVariable* result;
	ShVariable* condition;
	ShVariable* parameter;
};



#endif
