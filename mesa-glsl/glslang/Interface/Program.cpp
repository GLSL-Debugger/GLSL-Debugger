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

#include <string.h>
#include "Program.h"
#include "CodeTools.h"
#include "glsl/ir.h"
#include "glsl/list.h"
#include "Visitors/debugjump.h"
#include "Visitors/scopestack.h"
#include "glsldb/utils/dbgprint.h"


static struct {
	ast_debugjump_traverser_visitor* it;
    DbgResult result;
} g;


//
// Some helper functions for easier scope handling
//
static void initGlobalScope(void)
{
    static int initialized = 0;

    if (!initialized) {
        g.result.scope.numIds = 0;
        g.result.scope.ids = NULL;
        initialized = 1;
    }
}

static void initGlobalScopeStack(void)
{
    static int initialized = 0;

    if (!initialized) {
        g.result.scopeStack.numIds = 0;
        g.result.scopeStack.ids = NULL;
        initialized = 1;
    }
}

static void clearGlobalScope(void)
{
    g.result.scope.numIds = 0;
    free(g.result.scope.ids);
    g.result.scope.ids = NULL;
}

static void clearGlobalScopeStack(void)
{
    g.result.scopeStack.numIds = 0;
    free(g.result.scopeStack.ids);
    g.result.scopeStack.ids = NULL;
}


//
// Functions for keeping track of changes variables
//
static void initGlobalChangeables(void)
{
    static int initialized = 0;

    if (!initialized) {
        g.result.cgbls.numChangeables = 0;
        g.result.cgbls.changeables = NULL;
        initialized = 1;
    }
}

static void clearGlobalChangeables(void)
{
    int i, j;

    for (i=0; i<g.result.cgbls.numChangeables; i++) {
        ShChangeable *c;
        if ((c = g.result.cgbls.changeables[i])) {
            for(j=0; j<c->numIndices; j++) {
                free(c->indices[j]);
            }
            free(c->indices);
            free(c);
        }
    }
    free(g.result.cgbls.changeables);

    g.result.cgbls.numChangeables = 0;
    g.result.cgbls.changeables = NULL;
}


void clearTraverseDebugJump(void)
{
    delete g.it;
    g.it = NULL;
}

static void resetGlobals()
{
	/* Setup scope if neccessary */
	initGlobalScope();
	initGlobalScopeStack();
	initGlobalChangeables();

	g.result.range.left.line = 0;
	g.result.range.left.colum = 0;
	g.result.range.right.line = 0;
	g.result.range.right.colum = 0;

	clearGlobalScope();
	clearGlobalScopeStack();
	clearGlobalChangeables();

	/* Check validity of debug request */
	g.result.status = DBG_RS_STATUS_UNSET;
	g.result.position = DBG_RS_POSITION_UNSET;
	g.result.loopIteration = 0;
	g.result.passedEmitVertex = false;
	g.result.passedDiscard = false;
}

//
//  Generate code from the given parse tree
//
DbgResult* ShaderTraverse(AstShader* shader, int debugOptions, int dbgBehaviour)
{
	UNUSED_ARG(debugOptions)

	resetGlobals();

	/* Check for empty parse tree */
	if (!shader) {
		g.result.status = DBG_RS_STATUS_ERROR;
		g.result.position = DBG_RS_POSITION_UNSET;
		return &g.result;
	}

	if (!g.it)
		g.it = new ast_debugjump_traverser_visitor(g.result);

	g.it->shader = shader;
	g.it->vertexEmited = false;
	g.it->discardPassed = false;
	g.it->dbgBehaviour = dbgBehaviour;

	exec_list* list = shader->head;
	ast_node* root = exec_node_data(ast_node, shader->head, link);

	g.it->preVisit = false;
	g.it->postVisit = false;
	g.it->debugVisit = true;


	/* Check for finished parsing */
	if (dbgBehaviour != DBG_BH_RESET && root->debug_state == ast_dbg_state_end) {
		VPRINT(1, "!!! debugging already finished !!!\n");
		g.result.status = DBG_RS_STATUS_FINISHED;
		return &g.result;
	}

	/* In case of a reset clear DbgStates and empty stack */
	if (dbgBehaviour == DBG_BH_RESET) {
		g.it->operation = OTOpReset;
		while (!(g.it->parseStack.empty()))
			g.it->parseStack.pop();
		VPRINT(1, "********* reset traverse **********\n");
		g.it->visit(list);
		return NULL;
	}

	/* Clear debug path, i.e remove all DbgStPath */
	g.it->operation = OTOpPathClear;
	VPRINT(1, "********* clear path traverse **********\n");
	g.it->visit(list);

	/* Initialize parse tree for debugging if necessary */
	g.it->operation = OTOpTargetUnset;
	if (g.it->parseStack.empty()) {
		ast_function* main = getFunctionByName(MAIN_FUNC_SIGNATURE);
		if (!main) {
			g.result.status = DBG_RS_STATUS_ERROR;
			return &g.result;
		}
		g.it->operation = OTOpTargetSet;
		g.it->parseStack.push(main);
	}

	/* Advance the debug trace; move DbgStTarget */
	VPRINT(1, "********* jump traverse **********\n");
	g.it->parseStack.top()->accept(g.it);

	if (g.it->operation == OTOpFinished) {
		/* Debugging finished at the end of the code */
		root->debug_state = ir_dbg_state_end;
		g.result.status = DBG_RS_STATUS_FINISHED;
		return &g.result;
	} else {
		/* Build up new debug path; all DbgStPath */
		g.it->operation = OTOpPathBuild;
		g.it->preVisit = false;
		g.it->postVisit = true;
		g.it->debugVisit = false;
		VPRINT(1, "********* create path traverse **********\n");
		g.it->visit(list);
	}

	VPRINT(1, "********* traverse scope **********\n");
	ir_scopestack_traverse_visitor itScopeStack(g.result);
	itScopeStack.visit(list);

	g.result.status = DBG_RS_STATUS_OK;
	return &g.result;
}

