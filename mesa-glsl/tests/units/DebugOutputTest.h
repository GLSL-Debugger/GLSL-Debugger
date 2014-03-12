/*
 * DebugOutputTest.h
 *
 *  Created on: 12.03.2014.
 */
#ifndef DEBUGOUTPUTTEST_H_
#define DEBUGOUTPUTTEST_H_

#include "Base.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/Visitors/debugpath.h"
#include "glslang/Interface/Visitors/debugjump.h"
#include "glslang/Interface/Program.h"
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/TestResult.h>


const char* DBG_OW_MODE[ast_dbg_ow_end] = {
	"UnsetOW", "OriginalOw", "DebugOw"
};


class DebugOutputTest: public SuitedUnitTest<DebugOutputTest> {
public:
	friend class DebugOutput;

	DebugOutputTest()
	{
		test = NULL;
		unit_name = "dbgout";
		behaviour = DBG_BH_JUMPINTO;
		comparator.loadResults(test_files, unit_name);
		djv = new ast_debugjump_traverser_visitor(dbg_result);
	}

	~DebugOutputTest()
	{
		delete djv;

		// Due to cppunit strange behavior it expected test runner
		// to exist after test finished, so we just collect all test
		// repeaters we creating in the test and store it to remove it here
		for (auto ci = repeaters.cbegin(); ci != repeaters.cend(); ++ci)
			delete *ci;
	}

	static void setResult(CPPUNIT_NS::TestResult* r)
	{
		result = r;
	}


	class DebugOutput: public CPPUNIT_NS::TestCase {
	public:
		DebugOutput(DebugOutputTest* b) : CPPUNIT_NS::TestCase("DebugOutput"),
				base(b), current_iter(0), shader(NULL)
		{
		}

		~DebugOutput()
		{
		}

		void prepare(AstShader* sh)
		{
			shader = sh;
			base->reset();
			base->djv->parseStack.clear();
			ast_debugpath_traverser_visitor dbgpath;
			dbgpath.run(shader->head, DPOpReset);
		}

		void runTest()
		{
			// Do not clear first time
			if (current_iter)
				base->reset();

			CPPUNIT_ASSERT_MESSAGE("Debug is finished", !base->djv->finished());

			exec_list* list = shader->head;
			ast_debugpath_traverser_visitor dbgpath;
			current_iter++;
			base->djv->setUp(shader, base->behaviour);
			dbgpath.run(list, DPOpPathClear);
			CPPUNIT_ASSERT_MESSAGE("Cannot step into shader", base->djv->step(MAIN_FUNC_SIGNATURE));
			if (!base->djv->finished()) {
				dbgpath.run(list, DPOpPathBuild);
				dbgpath.getPath(base->dbg_result.scopeStack, shader);
			}

			char* src = NULL;
			compileDbgShaderCode(shader, NULL, NULL, DBG_CG_COVERAGE, &src);
			CPPUNIT_ASSERT_MESSAGE("No code generated", src);
			base->results << "================== Source ==================\n";
			base->results << std::string(src);
			base->results << "============================================\n";

			// Now where actual comparison happens
			base->doComparison(shader, true, current_iter < PRINT_ITER);
			base->applyRules(shader);
		}

	protected:
		DebugOutputTest* base;
		int current_iter;
		AstShader* shader;
	};


	void setUp()
	{
		test = new DebugOutput(this);
	}

	virtual void testShader(int num)
	{
		if (!holder->shaders[num])
			return;
		test->prepare(holder->shaders[num]);
		auto repeater = new CPPUNIT_NS::RepeatedTest(test, REPEATS_COUNT[num]);
		repeater->run(result);
		repeaters.push_back(repeater);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		if (node->debug_overwrite) {
			int length = printType(node, type);
			printIndent(length, depth);
			results << DBG_OW_MODE[node->debug_overwrite];
			results << "\n";
		}
		return true;
	}

	virtual void applyRules(AstShader* sh)
	{
		comparator.applyRules(sh, behaviour);
	}

protected:
	DbgResult dbg_result;
	DebugOutput* test;
	int behaviour;
	std::vector<CPPUNIT_NS::RepeatedTest*> repeaters;
	static CPPUNIT_NS::TestResult* result;
	ast_debugjump_traverser_visitor* djv;
};

CPPUNIT_NS::TestResult* DebugOutputTest::result;


#endif /* DEBUGOUTPUTTEST_H_ */
