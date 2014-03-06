/*
 * DebugJumpTest.h
 *
 *  Created on: 05 марта 2014 г.
 */

#ifndef DEBUGJUMPTEST_H_
#define DEBUGJUMPTEST_H_

#include "Base.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/Visitors/debugpath.h"
#include "glslang/Interface/Visitors/debugjump.h"
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/TestResult.h>


const char* DBG_NAMES[ast_dbg_state_end] = {
	"UnsetDbg", "PathDbg", "TargetDbg"
};

const int REPEATS_COUNT[3] = { 1, 2, 1 };

#define PRINT_ITER 2



class DebugJumpTest: public SuitedUnitTest<DebugJumpTest> {
public:
	friend class DebugJump;

	DebugJumpTest()
	{
		test = NULL;
		unit_name = "dbgjump";
		comparator.loadResults(test_files, unit_name);
		djv = new ast_debugjump_traverser_visitor(dbg_result);
	}

	~DebugJumpTest()
	{
		delete djv;
	}

	static void setResult(CPPUNIT_NS::TestResult* r)
	{
		result = r;
	}


	class DebugJump: public CPPUNIT_NS::TestCase {
	public:
		DebugJump(DebugJumpTest* b) :
				base(b), current_iter(0), shader(NULL)
		{
		}

		~DebugJump()
		{
		}

		void prepare(AstShader* sh)
		{
			shader = sh;
			base->results.str(std::string());
			base->results.clear();
			base->results << "================== Reset path ==================\n";
			base->djv->parseStack.clear();
			ast_debugpath_traverser_visitor dbgpath;
			dbgpath.run(shader->head, DPOpReset);
			base->doComparison(shader, false);
		}

		void runTest()
		{
			// Do not clear first time
			if (current_iter){
				base->results.str(std::string());
				base->results.clear();
			}

			exec_list* list = shader->head;
			ast_debugpath_traverser_visitor dbgpath;
			current_iter++;
			base->djv->setUp(shader, DBG_BH_JUMPINTO);
			dbgpath.run(list, DPOpPathClear);
			base->results << "================== Clear path ==================\n";
			base->doComparison(shader, false);
			CPPUNIT_ASSERT_MESSAGE("Cannot step into shader", base->djv->step(MAIN_FUNC_SIGNATURE));
			base->results << "===================== Step =====================\n";
			if (!base->djv->finished()) {
				base->doComparison(shader, false);
				dbgpath.run(list, DPOpPathBuild);
				base->results << "================== Build path ==================\n";
			}

			// Now where actual comparison happens
			base->doComparison(shader, true, current_iter == PRINT_ITER);
		}

	protected:
		DebugJumpTest* base;
		int current_iter;
		AstShader* shader;
	};


	void setUp()
	{
		test = new DebugJump(this);
	}

	virtual void testShader(int num)
	{
		if (!holder->shaders[num])
			return;
		test->prepare(holder->shaders[num]);
		CPPUNIT_NS::RepeatedTest repeator(test, REPEATS_COUNT[num]);
		repeator.run(result);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		int length = printType(node, type);
		if (node->debug_state) {
			printIndent(length, depth);
			results << DBG_NAMES[node->debug_state];
		}
		results << "\n";
		return true;
	}

protected:
	DbgResult dbg_result;
	DebugJump* test;
	static CPPUNIT_NS::TestResult* result;

	// cppunit makes new instance for each test, lol.
	ast_debugjump_traverser_visitor* djv;
};

CPPUNIT_NS::TestResult* DebugJumpTest::result;

#endif /* DEBUGJUMPTEST_H_ */
