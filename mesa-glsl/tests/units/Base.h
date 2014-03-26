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
#include "Format.h"
#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "ShaderInput.h"
#include <string>
#include <sstream>
#include <iomanip>

class BaseUnitTest: public CallAcceptor, public CppUnit::TestFixture {
public:
	BaseUnitTest()
	{
	}

	virtual int printType(ast_node* node, enum ast_node_type type)
	{
		int length = strlen(ast_node_names[type]);
		results << ast_node_names[type];

		ast_expression* expr = node->as_expression();
		if (expr) {
			const char* expr_type;
			if (expr->oper < ast_array_index)
				expr_type = expr->operator_string(expr->oper);
			else
				expr_type = ast_expr_string_ext[expr->oper - ast_array_index];
			results << " (" << expr_type << ")";
			length += strlen(expr_type) + 3;
		}

		return length;
	}

	virtual void printIndent(int length, int depth)
	{
		results << std::setfill(' ') << std::setw(40-length) << " ";
		for (int i = 0; i < depth; ++i)
			results << "    ";
	}

	virtual void reset()
	{
		results.str(std::string());
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

	virtual void doComparison(AstShader* sh, bool do_actual_cmp=true, bool print=false)
	{
		if (!sh)
			return;

		ast_call_visitor v(this);
		v.visit(sh->head);

		if (do_actual_cmp)
			prepareCmp(sh);

		if (print)
			std::cout << results.str();

		if (!do_actual_cmp)
			return;

		if (!comparator.nameMatch(sh->name, unit_name))
			comparator.setCurrent(sh->name, unit_name);
		else
			comparator.nextFile();

		std::string s;
		while (std::getline(results, s))
			comparator.compareNext(s);
	}

	virtual void prepareCmp(AstShader*)
	{

	}

	virtual void applyRules(AstShader* sh) {
		comparator.applyRules(sh);
	}

protected:
	std::string unit_name;
	std::stringstream results;
	static ShaderInput input;
	ResultComparator comparator;

};


template<typename T>
class SuitedUnitTest: public BaseUnitTest
{
public:
	static CppUnit::TestSuite *suite()
	{
		CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite;
		test_files = "shaders/test";
		holder = input.getShader(test_files);

		// FIXME: This creates instance for each test

		if (holder->shaders[0])
			suiteOfTests->addTest(new CppUnit::TestCaller<T>("testVertex", &T::testVertex));

		if (holder->shaders[1])
			suiteOfTests->addTest(new CppUnit::TestCaller<T>("testGeom", &T::testGeom));

		if (holder->shaders[2])
			suiteOfTests->addTest(new CppUnit::TestCaller<T>("testFrag", &T::testFrag));

		return suiteOfTests;
	}

protected:
	static std::string test_files;
	static ShaderHolder* holder;
};

template<typename T>
std::string SuitedUnitTest<T>::test_files;
template<typename T>
ShaderHolder* SuitedUnitTest<T>::holder;

#endif /* TEST_UNIT_BASE_H_ */
