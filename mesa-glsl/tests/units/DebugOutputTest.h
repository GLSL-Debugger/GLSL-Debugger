/*
 * DebugOutputTest.h
 *
 *  Created on: 12.03.2014.
 */
#ifndef DEBUGOUTPUTTEST_H_
#define DEBUGOUTPUTTEST_H_

#include "Repeater.h"
#include "glslang/Interface/CodeTools.h"
#include "glslang/Interface/Program.h"


const char* DBG_OW_MODE[ast_dbg_ow_end] = {
	"UnsetOW", "OriginalOw", "DebugOw"
};


class DebugOutputTest;

class DebugOutput: public TestRepeater<DebugOutputTest, DebugOutput>::RepeatedTestCase {
public:
	DebugOutput(TestRepeater<DebugOutputTest, DebugOutput>* b) :
		TestRepeater<DebugOutputTest, DebugOutput>::RepeatedTestCase::RepeatedTestCase(b, "DebugOutput")
	{
	}

	void runTest()
	{
		TestRepeater<DebugOutputTest, DebugOutput>::RepeatedTestCase::runTest();

		exec_list* list = shader->head;
		ast_debugpath_traverser_visitor dbgpath;
		dbgpath.run(list, DPOpPathClear);
		CPPUNIT_ASSERT_MESSAGE("Cannot step into shader", base->djv->step(MAIN_FUNC_SIGNATURE));
		if (!base->djv->finished()) {
			dbgpath.run(list, DPOpPathBuild);
			dbgpath.getPath(base->dbg_result.scopeStack, shader);
		}

		char* src = NULL;
		compileDbgShaderCode(shader, NULL, NULL, DBG_CG_COVERAGE, &src);
		if (!base->djv->finished())
			CPPUNIT_ASSERT_MESSAGE("No code generated", src);
		base->results << "================== Source ==================\n";
		base->results << std::string(src ? src : "no code\n");
		base->results << "============================================\n";

		// Now where actual comparison happens
		base->doComparison(shader, true, current_iter > PRINT_ITER);
		base->applyRules(shader);
	}
};

class DebugOutputTest: public TestRepeater<DebugOutputTest, DebugOutput> {
public:
	DebugOutputTest()
	{
		unit_name = "dbgout";
		comparator.loadResults(test_files, unit_name);
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
};

#endif /* DEBUGOUTPUTTEST_H_ */
