/*
 * DebugJumpTest.h
 *
 *  Created on: 05.03.2014.
 */

#ifndef DEBUGJUMPTEST_H_
#define DEBUGJUMPTEST_H_

#include "Repeater.h"
#include "interface/CodeTools.h"


const char* DBG_NAMES[ast_dbg_state_end] = {
	"UnsetDbg", "PathDbg", "TargetDbg", "CallDbg"
};

class DebugJumpTest;

class DebugJump: public TestRepeater<DebugJumpTest, DebugJump>::RepeatedTestCase {
public:
	DebugJump(TestRepeater<DebugJumpTest, DebugJump>* b) :
		TestRepeater<DebugJumpTest, DebugJump>::RepeatedTestCase::RepeatedTestCase(b, "DebugJump")
	{
	}

	void prepare(AstShader* sh)
	{
		TestRepeater<DebugJumpTest, DebugJump>::RepeatedTestCase::prepare(sh);
		base->results << "================== Reset path ==================\n";
		base->doComparison(shader, false);
	}

	void runTest()
	{
		TestRepeater<DebugJumpTest, DebugJump>::RepeatedTestCase::runTest();

		exec_list* list = shader->head;
		ast_debugpath_traverser_visitor dbgpath;
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
		base->doComparison(shader, true);
		base->applyRules(shader);
	}
};


class DebugJumpTest: public TestRepeater<DebugJumpTest, DebugJump> {
public:
	DebugJumpTest()
	{
		unit_name = "dbgjump";
		comparator.loadResults(test_files, unit_name);
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
		                 << dbg_result.range.right.line << ")"
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
};


#endif /* DEBUGJUMPTEST_H_ */
