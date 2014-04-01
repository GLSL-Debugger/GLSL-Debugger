/*
 * ShaderLang.h
 *
 *  Created on: 23.02.2014
 */

#ifndef PRIVATESHADERLANG_H_
#define PRIVATESHADERLANG_H_

#include "ShaderLang.h"

class exec_list;
class ast_node;
class ast_type_qualifier;
class ir_variable;

// Changeagles
ShChangeable* createShChangeableCtx(int id, void* mem_ctx);
ShChangeableIndex* createShChangeableIndexCtx(ShChangeableType type, long index, void* mem_ctx);
void copyAstChangeableList(exec_list *clout, exec_list *clin, exec_list* only, void* mem_ctx);
void addShChangeableCtx(ShChangeableList *cl, ShChangeable *c, void* mem_ctx);
ShChangeable * copyShChangeableCtx(ShChangeable *c, void* mem_ctx);
void addShIndexToChangeableCtx(ShChangeable *c, ShChangeableIndex *idx, void* mem_ctx);
void copyShChangeableToListCtx(ShChangeableList *cl, ShChangeable *c, void* mem_ctx);
void copyShChangeableListCtx(ShChangeableList *clout, exec_list *clin, void* mem_ctx);

// Variables
ShVariable* findShVariable(int id);
ShVariable* findShVariableFromId(ShVariableList *vl, int id);
ShVariable* findFirstShVariableFromName(ShVariableList *vl, const char *name);

ShVariable* copyShVariableCtx(ShVariable *src, void* mem_ctx);
void addShVariableCtx(ShVariableList *vl, ShVariable *v, int builtin, void *mem_ctx);
void addAstShVariable(ast_node*, ShVariable*);
ShVariable* astToShVariable(ast_node* decl, variableQualifier qualifier,
		variableVaryingModifier modifier, const struct glsl_type* decl_type,
		void* mem_ctx);

variableQualifier qualifierFromAst(const ast_type_qualifier* qualifier, bool is_parameter);
variableQualifier qualifierFromIr(ir_variable* var);
variableVaryingModifier modifierFromAst(const ast_type_qualifier* qualifier);
variableVaryingModifier modifierFromIr(const ir_variable* var);





#endif /* PRIVATESHADERLANG_H_ */
