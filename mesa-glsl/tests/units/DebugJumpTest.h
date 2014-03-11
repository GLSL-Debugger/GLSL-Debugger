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

#define PRINT_ITER 8
const int REPEATS_COUNT[3] = { 1, PRINT_ITER, 1 };


class DebugJumpTest: public SuitedUnitTest<DebugJumpTest> {
public:
	friend class DebugJump;

	DebugJumpTest()
	{
		test = NULL;
		unit_name = "dbgjump";
		behaviour = DBG_BH_JUMPINTO;
		comparator.loadResults(test_files, unit_name);
		djv = new ast_debugjump_traverser_visitor(dbg_result);
	}

	~DebugJumpTest()
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


	class DebugJump: public CPPUNIT_NS::TestCase {
	public:
		DebugJump(DebugJumpTest* b) : CPPUNIT_NS::TestCase("DebugJump"),
				base(b), current_iter(0), shader(NULL)
		{
		}

		~DebugJump()
		{
		}

		void prepare(AstShader* sh)
		{
			shader = sh;
			base->reset();
			base->results << "================== Reset path ==================\n";
			base->djv->parseStack.clear();
			ast_debugpath_traverser_visitor dbgpath;
			dbgpath.run(shader->head, DPOpReset);
			base->doComparison(shader, false);
		}

		void runTest()
		{
			// Do not clear first time
			if (current_iter)
				base->reset();

			exec_list* list = shader->head;
			ast_debugpath_traverser_visitor dbgpath;
			current_iter++;
			base->djv->setUp(shader, base->behaviour);
			dbgpath.run(list, DPOpPathClear);
			base->results << "================== Clear path ==================\n";
			base->doComparison(shader, false);
			CPPUNIT_ASSERT_MESSAGE("Cannot step into shader", base->djv->step(MAIN_FUNC_SIGNATURE));
			base->results << "===================== Step =====================\n";
			if (!base->djv->finished()) {
				base->doComparison(shader, false);
				dbgpath.run(list, DPOpPathBuild);
				dbgpath.getPath(base->dbg_result.scopeStack, shader);
				base->results << "================== Build path ==================\n";
			}

			// Now where actual comparison happens
			base->doComparison(shader, true, current_iter == PRINT_ITER);
			base->applyRules(shader);
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
		auto repeater = new CPPUNIT_NS::RepeatedTest(test, REPEATS_COUNT[num]);
		repeater->run(result);
		repeaters.push_back(repeater);
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

	virtual void prepareCmp(AstShader*)
	{
		std::stringstream dr;
		dr << "Status: " << DBG_STATUSES[dbg_result.status] << " "
		   << "Pos: " << DBG_POSITIONS[dbg_result.position] << "\n"
		   << "Range: (" << dbg_result.range.left.colum << ":"
		                 << dbg_result.range.left.line << " - "
		                 << dbg_result.range.right.colum << ":"
		                 << dbg_result.range.right.line << ") "
		   << "\nScope:\n";
		for (int i = 0; i < dbg_result.scope.numIds; ++i) {
			int id = dbg_result.scope.ids[i];
			dr << " " << formatVariable(id);
		}
		dr << "\nChangeables:\n";
		for (int i = 0; i < dbg_result.cgbls.numChangeables; ++i) {
			ShChangeable* ch = dbg_result.cgbls.changeables[i];
			dr << " " << formatChangeable(ch);
		}
		if (dbg_result.cgbls.numChangeables)
			dr << "\n";
		dr << "LoopIter: " << dbg_result.loopIteration << " "
		   << "EmitVertex: " << dbg_result.passedEmitVertex << " "
		   << "Discard: " << dbg_result.passedDiscard << "\n";

		results << dr.str();
	}

	virtual void applyRules(AstShader* sh)
	{
		comparator.applyRules(sh, behaviour);
	}

protected:
	DbgResult dbg_result;
	DebugJump* test;
	int behaviour;
	std::vector<CPPUNIT_NS::RepeatedTest*> repeaters;
	static CPPUNIT_NS::TestResult* result;

	// cppunit makes new instance for each test, lol.
	ast_debugjump_traverser_visitor* djv;
};

CPPUNIT_NS::TestResult* DebugJumpTest::result;

#endif /* DEBUGJUMPTEST_H_ */
