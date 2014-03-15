/*
 * Compiler.h
 *
 *  Created on: 19.02.2014
 */

#ifndef COMPILER_H_
#define COMPILER_H_

#include "ShaderLang.h"
#include "glslang/Interface/Visitors/debugvar.h"
#include "Base.h"


class CompileShaderTest: public SuitedUnitTest<CompileShaderTest> {
public:
	CompileShaderTest()
	{
		unit_name = "compile";
		comparator.loadResults(test_files, unit_name);
	}

	void testShader(int num)
	{
		doComparison(holder->shaders[num]);
	}

	virtual bool accept(int depth, ast_node* node, enum ast_node_type type)
	{
		results << node->debug_sideeffects << " ";
		for (int i = 0; i < depth; ++i)
			results << "    ";
		ast_expression* expr = node->as_expression();
		if (expr) {
			const char* expr_type;
			if (expr->oper < ast_array_index)
				expr_type = expr->operator_string(expr->oper);
			else
				expr_type = ast_expr_string_ext[expr->oper - ast_array_index];
			results << "("  << expr_type << ") ";
		}
		results << ast_node_names[type] << "\n";
		return true;
	}
};

#endif /* COMPILER_H_ */
