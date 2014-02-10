#include "glsl/list.h"
#include "Visitors/debugvar.h"
#include "Visitors/debugchange.h"
#include "glsldb/utils/dbgprint.h"

//
//  Generate code from the given parse tree
//
bool ShaderVarTraverse(AstShader* shader, ShVariableList *vl)
{
	exec_list* list = shader->head;

	/* Check for empty parse tree */
	if (list->is_empty())
		return false;

	//
	// Fill exernal variable list and determine scope
	//
	VPRINT(2, "==Processing=Variables======================================\n");
	ast_debugvar_traverser_visitor it(vl);
	it.visit(list);
	VPRINT(2, "==Processing=Variables===done===============================\n");

	// Now check which variables get altered at target and also
	// insert this information at end of sequences to be able to keep track
	// all changes made in functions and branches
	VPRINT(2, "==Processing=Changes========================================\n");
	ir_debugchange_traverser_visitor itChange;
	itChange.visit(list);
	VPRINT(2, "==Processing=Changes===done=================================\n");
	return true;
}

