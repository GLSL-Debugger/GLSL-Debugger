/*
 * Base.h
 *
 *  Created on: 21.02.2014
 */

#ifndef TEST_UNIT_BASE_H_
#define TEST_UNIT_BASE_H_

#include "ast_nodes.h"
#include "call_visitor.h"
#include "glslang/Interface/ShaderHolder.h"
#include <cppunit/TestFixture.h>
#include "ShaderInput.h"
#include <string>
#include <sstream>

class BaseUnitTest: public CppUnit::TestFixture {
public:
	BaseUnitTest()
	{
	}

	virtual bool accept(int, ast_node*, enum ast_node_type)
	{
		return true;
	}

	virtual void reset()
	{
		results.clear();
	}

	virtual void testVertex()
	{
		testShader(MESA_SHADER_VERTEX);
	}

	virtual void testGeom()
	{
		testShader(MESA_SHADER_GEOMETRY);
	}

	virtual void testFrag()
	{
		testShader(MESA_SHADER_FRAGMENT);
	}

	virtual void testShader(int) {}

	virtual void doComparison(AstShader* sh)
	{
		if (!sh)
			return;

		ast_call_visitor v(this);
		v.visit(sh->head);

		// I'm afraid of this code.
		// Wrong architecture chosen for this.
		std::string s;
		comparator.setCurrent(sh->name, unit_name);
		while (std::getline(results, s))
			comparator.compareNext(s);
	}

protected:
	std::string unit_name;
	std::stringstream results;
	static ShaderInput input;
	ResultComparator comparator;
};

#endif /* TEST_UNIT_BASE_H_ */
