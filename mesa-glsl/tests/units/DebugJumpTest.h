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

const char* DBG_NAMES[ast_dbg_state_end] = {
	"UnsetDbg", "PathDbg", "TargetDbg"
};

class DebugJumpTest: public SuitedUnitTest<DebugJumpTest> {
public:
	DebugJumpTest()
	{
		unit_name = "dbgjump";
		comparator.loadResults(test_files, unit_name);
		djv = new ast_debugjump_traverser_visitor(dbg_result);
		last_shader = -1;
	}

	virtual void testShader(int num)
	{
		AstShader* sh = holder->shaders[num];
		if (!sh)
			return;

		results.clear();
		exec_list* list = sh->head;
		ast_debugpath_traverser_visitor dbgpath;

		if (last_shader != num) {
			results << "================== Reset path ==================\n";
			djv->parseStack.clear();
			dbgpath.run(list, DPOpReset);
			doComparison(sh, false);
		}

		djv->setUp(sh, DBG_BH_JUMPINTO);
		dbgpath.run(list, DPOpPathClear);
		results << "================== Clear path ==================\n";
		doComparison(sh, false);
		CPPUNIT_ASSERT_MESSAGE("Cannot step into shader", djv->step(MAIN_FUNC_SIGNATURE));
		results << "===================== Step =====================\n";
		if (!djv->finished()) {
			doComparison(sh, false);
			dbgpath.run(list, DPOpPathBuild);
			results << "================== Build path ==================\n";
		}

		// Now where actual comparison happens
		doComparison(sh, true, true);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		int length = printType(node, type);
		if (!node->changeables.is_empty()) {
			printIndent(length, depth);
			results << DBG_NAMES[node->debug_state] << " " << node->debug_target;
		}
		results << "\n";
		return true;
	}

protected:
	int last_shader;
	ast_debugjump_traverser_visitor* djv;
	DbgResult dbg_result;

};


#endif /* DEBUGJUMPTEST_H_ */
