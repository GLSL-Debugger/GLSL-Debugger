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
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class DebugVarTest: public CppUnit::TestFixture {
public:
	DebugVarTest()
	{
	}

	void testAll()
	{
		ShaderHolder* holder = input.getShader("shaders/test");
		ast_debugvar_traverser_visitor it(vl);
		for (unsigned i = 0; i < holder->num_shaders; ++i) {
			it.visit(holder->shaders[i]->head);
		}
	}

	static CppUnit::TestSuite *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite;
		suiteOfTests->addTest(
				new CppUnit::TestCaller<DebugVarTest>("testAll",
						&DebugVarTest::testAll));
		return suiteOfTests;
	}

	void setUp()
	{
		vl = new ShVariableList;
	}

	void tearDown()
	{
		delete vl;
	}

protected:
	ShaderInput input;
	ShVariableList* vl;
	exec_list* list;
};

#endif /* AST_DEBUGVAR_TEST_H_ */
