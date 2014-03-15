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
#include "Visitors/debugpath.h"
#include "glsldb/utils/dbgprint.h"


static struct {
	ast_debugjump_traverser_visitor* it;
    DbgResult result;
} g;


//
// Some helper functions for easier scope handling
//
static void freeResult(DbgResult& r)
{
	if (r.scope.ids)
		ralloc_free(r.scope.ids);
	if (r.scopeStack.ids)
		ralloc_free(r.scopeStack.ids);

	if (!r.cgbls.changeables)
		return;

    for (int i=0; i < r.cgbls.numChangeables; i++) {
        ShChangeable *c;
        if ((c = r.cgbls.changeables[i])) {
            for( int j=0; j<c->numIndices; j++)
                free(c->indices[j]);
            ralloc_free(c->indices);
            ralloc_free(c);
        }
    }
    ralloc_free(r.cgbls.changeables);
}


void clearTraverseDebugJump(void)
{
    delete g.it;
    g.it = NULL;
}

void resetDbgResult(DbgResult& r, bool initialized)
{
	if (initialized)
		freeResult(r);
	memset(&r, '\0', sizeof(DbgResult));
}

static void resetGlobals()
{
	static bool initialized = false;
	resetDbgResult(g.result, initialized);
	initialized = true;

	if (!g.it)
		g.it = new ast_debugjump_traverser_visitor(g.result);
}

static DbgResult* endTraverse(enum DbgRsStatuses status)
{
	g.result.status = status;
	return &g.result;
}

//
//  Generate code from the given parse tree
//
DbgResult* ShaderTraverse(AstShader* shader, int debugOptions, int dbgBehaviour)
{
	UNUSED_ARG(debugOptions)

	resetGlobals();

	/* Check for empty parse tree */
	if (!shader)
		return endTraverse(DBG_RS_STATUS_ERROR);

	g.it->setUp(shader, dbgBehaviour);
	exec_list* list = shader->head;
	ast_node* root = exec_node_data(ast_node, shader->head, link);

	/* Check for finished parsing */
	if (dbgBehaviour != DBG_BH_RESET && root->debug_state == ast_dbg_state_end) {
		VPRINT(1, "!!! debugging already finished !!!\n");
		return endTraverse(DBG_RS_STATUS_FINISHED);
	}

	ast_debugpath_traverser_visitor dbgpath;

	/* In case of a reset clear DbgStates and empty stack */
	if (dbgBehaviour == DBG_BH_RESET) {
		g.it->parseStack.clear();
		dbgpath.run(list, DPOpReset);
		return NULL;
	}

	/* Clear debug path, i.e remove all DbgStPath */
	dbgpath.run(list, DPOpPathClear);

	if (!g.it->step(MAIN_FUNC_SIGNATURE))
		return endTraverse(DBG_RS_STATUS_ERROR);

	if (g.it->finished()) {
		/* Debugging finished at the end of the code */
		root->debug_state = ast_dbg_state_end;
		return endTraverse(DBG_RS_STATUS_FINISHED);
	}

	/* Build up new debug path; all DbgStPath */
	dbgpath.run(list, DPOpPathBuild);
	VPRINT(1, "********* Copy scope **********\n");
	dbgpath.getPath(g.result.scopeStack, shader);

	return endTraverse(DBG_RS_STATUS_OK);
}

