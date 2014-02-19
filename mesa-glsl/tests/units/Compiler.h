/*
 * Compiler.h
 *
 *  Created on: 19.02.2014
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#include "ShaderInput.h"
#include "ShaderLang.h"
#include "glslang/Interface/Visitors/debugvar.h"
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class CompileShaderTest: public CppUnit::TestFixture {
public:
	CompileShaderTest()
	{
	}

	void testAll()
	{
		ShaderHolder* holder = input.getShader("shaders/test");
		(void)holder;
	}

	static CppUnit::TestSuite *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite;
		suiteOfTests->addTest(
				new CppUnit::TestCaller<CompileShaderTest>("testAll",
						&CompileShaderTest::testAll));
		return suiteOfTests;
	}

protected:
	ShaderInput input;
};


#endif /* COMPILER_H_ */
