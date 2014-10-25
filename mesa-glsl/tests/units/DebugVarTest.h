/*
 * ast_debugvar.h
 *
 *  Created on: 12.02.2014
 */

#ifndef AST_DEBUGVAR_TEST_H_
#define AST_DEBUGVAR_TEST_H_

#include "ShaderInput.h"
#include "ShaderLang.h"
#include "interface/visitors/debugvar.h"
#include "interface/AstScope.h"
#include "Base.h"


class DebugVarTest: public SuitedUnitTest<DebugVarTest> {
public:
	DebugVarTest()
	{
		unit_name = "dbgvar";
		comparator.loadResults(test_files, unit_name);
		vl = NULL;
	}

	void setUp()
	{
		vl = new ShVariableList;
		vl->numVariables = 0;
		vl->variables = NULL;
	}

	virtual void testShader(int num)
	{
		AstShader* sh = holder->shaders[num];
		if (!sh)
			return;
		ast_debugvar_traverser_visitor it(sh, vl);
		it.visit(sh->head);
		doComparison(sh);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		int length = printType(node, type);
		if (!node->scope.is_empty()) {
			printIndent(length, depth);
			foreach_in_list(scope_item, sc, &node->scope) {
				results << " <" << sc->id << "," << sc->name << ">";
			}
		}

		results << "\n";
		return true;
	}

	void tearDown()
	{
		delete vl;
	}

private:
	ShVariableList* vl;
};

#endif /* AST_DEBUGVAR_TEST_H_ */
