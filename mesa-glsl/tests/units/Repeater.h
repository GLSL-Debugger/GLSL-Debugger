/*
 * Repeator.h
 *
 *  Created on: 15.03.2014.
 */
#ifndef TEST_REPEATOR_H_
#define TEST_REPEATOR_H_

#include "Base.h"
#include "glslang/Interface/Program.h"
#include "glslang/Interface/Visitors/debugpath.h"
#include "glslang/Interface/Visitors/debugjump.h"
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/TestResult.h>

#define PRINT_ITER 26

static CPPUNIT_NS::TestResult* tests_eventManager = NULL;

template<typename T, typename O>
class TestRepeater: public SuitedUnitTest<T> {
public:
	friend O;

	TestRepeater()
	{
		test = NULL;
		behaviour = DBG_BH_JUMP_INTO;
		resetDbgResult(dbg_result, false);
		djv = new ast_debugjump_traverser_visitor(dbg_result);
	}

	virtual ~TestRepeater()
	{
		delete djv;

		// Due to cppunit strange behavior it expected test runner
		// to exist after test finished, so we just collect all test
		// repeaters we creating in the test and store it to remove it here
		for (auto ci = repeaters.cbegin(); ci != repeaters.cend(); ++ci)
			delete *ci;
	}

	class RepeatedTestCase: public CPPUNIT_NS::TestCase {
	public:
		RepeatedTestCase(TestRepeater<T, O>* b, std::string name) :
				CPPUNIT_NS::TestCase(name), base(b), current_iter(0), shader(NULL)
		{
		}

		~RepeatedTestCase()
		{
		}

		virtual void prepare(AstShader* sh)
		{
			shader = sh;
			base->reset();
			base->djv->parseStack.clear();
			resetDbgResult(base->dbg_result, true);
			ast_debugpath_traverser_visitor dbgpath;
			dbgpath.run(shader->head, DPOpReset);
		}

		virtual void runTest()
		{
			// Do not clear first time
			if (current_iter)
				base->reset();

			CPPUNIT_ASSERT_MESSAGE("Debug is finished", !base->djv->finished());

			current_iter++;
			base->djv->setUp(shader, base->behaviour);
		}

	protected:
		TestRepeater<T, O>* base;
		int current_iter;
		AstShader* shader;
	};

	void setUp()
	{
		test = new O(this);
	}

	virtual void testShader(int num)
	{
		if (!SuitedUnitTest<T>::holder->shaders[num])
			return;
		test->prepare(SuitedUnitTest<T>::holder->shaders[num]);
		auto repeater = new CPPUNIT_NS::RepeatedTest(test, repeats_count[num]);
		repeater->run(tests_eventManager);
		repeaters.push_back(repeater);
	}

	virtual void applyRules(AstShader* sh)
	{
		BaseUnitTest::comparator.applyRules(sh, behaviour);
	}

protected:
	const int repeats_count[3] = { 1, 25, 34 };
	DbgResult dbg_result;
	RepeatedTestCase* test;
	int behaviour;
	std::vector<CPPUNIT_NS::RepeatedTest*> repeaters;

	// cppunit makes new instance for each test, lol.
	ast_debugjump_traverser_visitor* djv;
};

#endif /* TEST_REPEATOR_H_ */
