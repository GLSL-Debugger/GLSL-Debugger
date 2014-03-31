
#include "Program.h"
#include "CodeTools.h"
#include "visitors/debugjump.h"
#include "visitors/debugpath.h"
#include "mesa/glsl/list.h"
#include "glsldb/utils/dbgprint.h"
#include <string.h>

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

