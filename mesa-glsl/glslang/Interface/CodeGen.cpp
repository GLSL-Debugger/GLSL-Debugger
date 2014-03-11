//
//Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

#include "glsl/list.h"
#include "mesa-glsl/glslang/Public/ShaderLang.h"
#include "CodeInsertion.h"
#include "CodeTools.h"
#include "AstStack.h"
#include "AstScope.h"
#include "SymbolTable.h"
#include "Visitors/output.h"
#include "Visitors/position_output.h"
#include "glsldb/utils/dbgprint.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>

#define DBG_TEXT_BEGIN "\x1B[1;31mgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0)\x1B[0;31m"
#define DBG_TEXT_END "\x1B[0m"

void printShaderIr(struct gl_shader* shader)
{
	ir_position_output_visitor pov(DBGLVL_COMPILERINFO);

	pov.print_header();
	pov.run(shader->ir);
	dbgPrint(DBGLVL_COMPILERINFO, "\n───────────────────────────────────────────────\n");
}

//
//  Generate code from the given parse tree
//
bool compileShaderCode(AstShader* shader)
{
	exec_list* list = shader->head;

	EShLanguage language = EShLangVertex;
	if (shader->stage == MESA_SHADER_FRAGMENT)
		language = EShLangFragment;
	else if (shader->stage == MESA_SHADER_GEOMETRY)
		language = EShLangGeometry;

	CodeGen cg(shader, NULL, NULL);
	ast_output_traverser_visitor it(cg, shader, NULL, NULL, language, DBG_CG_ORIGINAL_SRC);
	it.append_version();
	it.visit(list);
	it.dump();

	return true;
}

/*
 * DEBUG INTERFACE
 */

static void change_DbgOverwrite(AstShader* sh, ast_function_expression* cir, enum ast_dbg_overwrite status)
{
	dbgPrint(DBGLVL_COMPILERINFO, "---->>> FUNCTION CALL ast_dbg_ow_%s\n",
			status == ast_dbg_ow_original ? "original" :
			status == ast_dbg_ow_debug ? "debug" : "unset");
	dumpNodeInfo(cir);
	dbgPrint(DBGLVL_COMPILERINFO, "---------------------------------------\n");
	cir->debug_overwrite = status;

	const char* name = cir->subexpressions[0]->primary_expression.identifier;
	ast_function_definition* func = sh->symbols->get_function(name);
	func->debug_overwrite = status;
}

static ast_node* prepareTarget(AstShader* shader, DbgCgOptions dbgCgOptions,
		ShChangeableList *cgbl, ShVariableList *vl)
{
	ast_node* target = NULL;
	AstStack* stack = &shader->path;

	switch (dbgCgOptions) {
	case DBG_CG_GEOMETRY_MAP:
	case DBG_CG_VERTEX_COUNT:
		break;
	case DBG_CG_COVERAGE:
	case DBG_CG_SELECTION_CONDITIONAL:
	case DBG_CG_LOOP_CONDITIONAL:
	case DBG_CG_GEOMETRY_CHANGEABLE:
		if (!stack->empty())
			target = stack->base();
		break;
	case DBG_CG_CHANGEABLE: {
		if (!cgbl)
			return NULL;

		std::unordered_set<int> changeables;
		for (int id = 0; id < cgbl->numChangeables; id++)
			changeables.emplace(cgbl->changeables[id]->id);

		/* stack top is path beginning, base is target */
		foreach_stack_reverse(node, stack) {
			ast_function_expression* call = node->as_function_expression();
			bool user_call = (call && !call->debug_builtin && !call->is_constructor());
			bool allInScope = true;

			foreach_list(ch_node, &node->changeables) {
				changeable_item* ch_item = (changeable_item*)ch_node;
				if (changeables.find(ch_item->id) != changeables.end())
					continue;

				ShVariable *var = findShVariableFromId(vl, ch_item->id);
				assert(var || !"CodeGen - unknown changeable, stop debugging");

				if (var->builtin)
					continue;

				//This is not the place to read out the target
				if (user_call)
					change_DbgOverwrite(shader, call, ast_dbg_ow_original);
				else
					dumpNodeInfo(node);

				// pop stack since we cannot process all changeables here
				allInScope = false;
				break;
			}

			if (allInScope) {
				// we are finially finished
				target = node;
				if (user_call)
					change_DbgOverwrite(shader, call, ast_dbg_ow_original);
				else
					dumpNodeInfo(node);
				break;
			}
		}

		if (!target) {
			dbgPrint(DBGLVL_WARNING, "CodeGen - target not in stack, no debugging possible\n");
		} else {
			/* iterate trough the rest of the stack */
			for (ast_node* node = stack->back(); node != NULL; node = stack->next()) {
				ast_function_expression* call = node->as_function_expression();
				if (call && !call->is_constructor() && !call->debug_builtin
						&& call->debug_overwrite == ast_dbg_ow_unset)
					change_DbgOverwrite(shader, call, ast_dbg_ow_debug);
			}
		}

		/* DEBUG OUTPUT */
		dumpDbgStack(stack);
		break;
	}
	default:
		break;
	}

	return target;
}

static void clearPath(AstShader* shader)
{
	/* Reset overwrite modes */
	AstStack* stack = &shader->path;
	for (ast_node* node = stack->back(); node != NULL; node = stack->next()) {
		node->debug_overwrite = ast_dbg_ow_unset;
		ast_function_expression* call = node->as_function_expression();
		if (call && !call->debug_builtin) {
			const char* name = call->subexpressions[0]->primary_expression.identifier;
			ast_function_definition* func = shader->symbols->get_function(name);
			func->debug_overwrite = ast_dbg_ow_unset;
		}
	}
	stack->clear();
}

bool compileDbgShaderCode(AstShader* shader, ShChangeableList *cgbl, ShVariableList *vl,
		DbgCgOptions dbgCgOptions, char** code)
{
	if (dbgCgOptions == DBG_CG_ORIGINAL_SRC) {
		return compileShaderCode(shader);
	}

	exec_list* list = shader->head;

	/* Check for empty parse tree */
	if (list->is_empty())
		return false;

	EShLanguage language = EShLangVertex;
	if (shader->stage == MESA_SHADER_FRAGMENT)
		language = EShLangFragment;
	else if (shader->stage == MESA_SHADER_GEOMETRY)
		language = EShLangGeometry;

	CodeGen cg(shader, vl, cgbl);
	/* Set DbgIterName for loop */
	cg.setIterNames();

	/* Find target, mark it and prepare neccessary dbgTemporaries */
	ast_node* target = prepareTarget(shader, dbgCgOptions, cgbl, vl);
	if (!target)
		return false;
	else
		dbgPrint(DBGLVL_COMPILERINFO, "TARGET is %i:%i %i:%i\n",
				target->location.first_line, target->location.first_column,
				target->location.last_line, target->location.last_column);

	/* Prepare debug temporary registers */
	cg.allocateResult(target, language, dbgCgOptions);

	ast_output_traverser_visitor it(cg, shader, vl, cgbl, language, dbgCgOptions);
	it.append_version();

	/* I have some problems with locale-dependent %f interpretation in printf
	 * Not sure, whose fault it is, qt or some line of code in debugger initialization.
	 * I added this crutch to resolve it, but it must be eventually rewritten.
	 * Well... fuck you, something.
	 * P.S. You cannot have Gujarati comments in generated shaders now.
	 */
	char* old_locale = setlocale(LC_NUMERIC, NULL);
	setlocale(LC_NUMERIC, "POSIX");

	/* 2. Pass:
	 * - do the actual code generation
	 */
	it.visit(list);
	/* restore locale */
	setlocale(LC_NUMERIC, old_locale);

	it.dump();
	it.get_code(code);

	/* Cleanup */
	dumpDbgStack(&shader->path);
	clearPath(shader);

	return true;
}

