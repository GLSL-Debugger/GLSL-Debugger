/*
 * Compiler.h
 *
 *  Created on: 19.02.2014
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#include "ShaderLang.h"
#include "glslang/Interface/Visitors/debugvar.h"
#include "Base.h"
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>


class CompileShaderTest: public BaseUnitTest {
public:
	CompileShaderTest()
	{
		unit_name = "compile";
	}

	void setUp()
	{
		std::string test_files = "shaders/test";
		holder = input.getShader(test_files);
		comparator.loadResults(test_files, unit_name);
	}

	void testShader(int num)
	{
		doComparison(holder->shaders[num]);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		results << node->debug_sideeffects << " ";
		for (int i = 0; i < depth; ++i)
			results << "    ";
		ast_expression* expr = node->as_expression();
		if (expr) {
			const char* expr_type;
			if (expr->oper < ast_array_index)
				expr_type = expr->operator_string(expr->oper);
			else
				expr_type = ast_expr_string_ext[expr->oper - ast_array_index];
			results << "("  << expr_type << ") ";
		}
		results << ast_node_names[type] << "\n";
		return true;
	}

	static CppUnit::TestSuite *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite;
		suiteOfTests->addTest(
				new CppUnit::TestCaller<CompileShaderTest>("testVertex",
						&CompileShaderTest::testVertex));
		suiteOfTests->addTest(
				new CppUnit::TestCaller<CompileShaderTest>("testGeom",
						&CompileShaderTest::testGeom));
		suiteOfTests->addTest(
				new CppUnit::TestCaller<CompileShaderTest>("testFrag",
						&CompileShaderTest::testFrag));
		return suiteOfTests;
	}

private:
	ShaderHolder* holder;

};

#endif /* COMPILER_H_ */
