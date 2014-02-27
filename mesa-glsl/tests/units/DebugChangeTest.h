/*
 * DebugChangeTest.h
 *
 *  Created on: 23.02.2014
 */

#ifndef DEBUGCHANGETEST_H_
#define DEBUGCHANGETEST_H_

#include "ShaderInput.h"
#include "glslang/Include/ShaderLang.h"
#include "glslang/Interface/Visitors/debugchange.h"
#include "glslang/Interface/AstScope.h"
#include "Base.h"
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

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
		doComparison(sh, false);
		std::cout << results.str();
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		int length = printType(node, type);
		if (!node->changeables.is_empty()) {
			printIndent(length, depth);
			foreach_list(scope_node, &node->changeables) {
				changeable_item* sc = (changeable_item*)scope_node;
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
