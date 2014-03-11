#ifndef _CODE_INSERTION_
#define _CODE_INSERTION_

#include "ShaderLang.h"
#include "ShaderHolder.h"

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
	CodeGen(AstShader*, ShVariableList*, ShChangeableList*);
	~CodeGen();

	void init(cgTypes type, ShVariable *v, EShLanguage l);
	void allocateResult(ast_node*, EShLanguage, DbgCgOptions);
	/* code generation */
	void getNewName(char **name, const char *prefix);
	void addDeclaration(cgTypes type, char** prog, EShLanguage l);
	void addDbgCode(cgTypes type, char** prog, DbgCgOptions cgOptions,
	                  int option, int outPrimType = 0x0000);
	void addOutput(cgTypes type, char** prog, EShLanguage l);
	void addInitialization(cgTypes type, cgInitialization init, char** prog);
	//void addAssignment(cgTypes type, ShVariable *src);
	void destruct(cgTypes type);

	void setIterNames();
	void resetLoopIterNames(void);

	const char* getDebugName(const char *input);

private:
	AstShader* shader;
	ShVariableList *vl;
	ShChangeableList *cgbls;
	ShVariable* result;
	ShVariable* condition;
	ShVariable* parameter;
};



#endif
