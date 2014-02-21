/*
 * Base.h
 *
 *  Created on: 21.02.2014
 */

#ifndef TEST_UNIT_BASE_H_
#define TEST_UNIT_BASE_H_

#include "ast_nodes.h"
#include <cppunit/TestFixture.h>

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

protected:
	std::stringstream results;
	static ShaderInput input;
	ResultComparator comparator;
};

#endif /* TEST_UNIT_BASE_H_ */
