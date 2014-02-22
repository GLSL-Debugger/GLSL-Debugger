/*
 * ast_debugvar.h
 *
 *  Created on: 12.02.2014
 */

#ifndef AST_DEBUGVAR_TEST_H_
#define AST_DEBUGVAR_TEST_H_

#include "ShaderInput.h"
#include "ShaderLang.h"
#include "glslang/Interface/Visitors/debugvar.h"
#include "Base.h"
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class DebugVarTest: public BaseUnitTest {
public:
	DebugVarTest()
	{
		unit_name = "dbgvar";
	}

	void setUp()
	{
		vl = new ShVariableList;
		vl->numVariables = 0;
		vl->variables = NULL;

		std::string test_files = "shaders/test";
		holder = input.getShader(test_files);
		comparator.loadResults(test_files, unit_name);
	}

	virtual void testShader(int num)
	{
		AstShader* sh = holder->shaders[num];
		if (!sh)
			return;
		ast_debugvar_traverser_visitor it(sh, vl);
		it.visit(sh->head);
		doComparison(sh);
		printf("results\n");
		std::cout << results;
	}

	static CppUnit::TestSuite *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite;
		suiteOfTests->addTest(
				new CppUnit::TestCaller<DebugVarTest>("testVertex",
						&DebugVarTest::testVertex));
		suiteOfTests->addTest(
				new CppUnit::TestCaller<DebugVarTest>("testGeom",
						&DebugVarTest::testGeom));
		suiteOfTests->addTest(
				new CppUnit::TestCaller<DebugVarTest>("testFrag",
						&DebugVarTest::testFrag));
		return suiteOfTests;
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		results << ast_node_names[type];

		ast_expression* expr = node->as_expression();
		if (expr) {
			const char* expr_type;
			if (expr->oper < ast_array_index)
				expr_type = expr->operator_string(expr->oper);
			else
				expr_type = ast_expr_string_ext[expr->oper - ast_array_index];
			results << " (" << expr_type << ")";
		}

		for (int i = 0; i < depth; ++i)
			results << "    ";

		foreach_list(scope_node, &node->scope) {
			scope_item* sc = (scope_item*)scope_node;
			results << " <" << sc->id << "," << sc->name << ">";
		}

		results << "\n";
		return true;
	}

	void tearDown()
	{
		delete vl;
	}

private:
	ShaderHolder* holder;
	ShVariableList* vl;
};

#endif /* AST_DEBUGVAR_TEST_H_ */
