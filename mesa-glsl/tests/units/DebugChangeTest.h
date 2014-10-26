/*
 * DebugChangeTest.h
 *
 *  Created on: 23.02.2014
 */

#ifndef DEBUGCHANGETEST_H_
#define DEBUGCHANGETEST_H_

#include "ShaderInput.h"
#include "interface/Shader.h"
#include "interface/visitors/debugchange.h"
#include "interface/AstScope.h"
#include "Base.h"


class DebugChangeTest: public SuitedUnitTest<DebugChangeTest> {
public:
	DebugChangeTest()
	{
		unit_name = "dbgchange";
		comparator.loadResults(test_files, unit_name);
	}

	virtual void testShader(int num)
	{
		AstShader* sh = holder->shaders[num];
		if (!sh)
			return;
		ast_debugchange_traverser_visitor it(sh);
		it.visit(sh->head);
		doComparison(sh);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		int length = printType(node, type);
		if (!node->changeables.is_empty()) {
			printIndent(length, depth);
			foreach_in_list(changeable_item, sc, &node->changeables) {
				ShVariable* var = findShVariable(sc->id);
				const char* name = var ? var->name : "undefined";
				results << " <" << sc->id << "," << name << ">";
			}
		}

		results << "\n";
		return true;
	}

};


#endif /* DEBUGCHANGETEST_H_ */
