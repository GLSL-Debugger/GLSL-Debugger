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
#include <stdio.h>
#include <stdlib.h>
#include "glsl/ir.h"
#include "glsl/list.h"
#include "mesa-glsl/glslang/Include/ShHandle.h"
#include "mesa-glsl/glslang/Public/ShaderLang.h"
#include "mesa-glsl/glslang/Include/intermediate.h"

#include "MShader.h"
#include "CodeInsertion.h"
#include "CodeTools.h"
#include "IRScope.h"
#include "Visitors/output.h"
#include "Visitors/stacktraverser.h"

#include "glsldb/utils/dbgprint.h"

#define DBG_TEXT_BEGIN "\x1B[1;31mgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0)\x1B[0;31m"
#define DBG_TEXT_END "\x1B[0m"

#define EMIT_VERTEX_SIG   "EmitVertex("
#define END_PRIMITIVE_SIG "EndPrimitive("

bool appendVersion(struct gl_shader* shader, std::string& program)
{
    if( !shader || shader->Version == 0 )
    	return false;

    char buf[10];
    program += "#version ";
    sprintf(buf, "%i", shader->Version);
    program += buf;
    program += "\n";
    return true;
}


//
//  Generate code from the given parse tree
//
bool compileShaderCode(struct gl_shader* shader)
{
	exec_list* list = shader->ir;
//    bool haveValidObjectCode = true;

    EShLanguage language = EShLangVertex;
    if( shader->Type == GL_FRAGMENT_SHADER )
    	language = EShLangFragment;
    else if( shader->Type == GL_GEOMETRY_SHADER )
		language = EShLangGeometry;

    ir_output_traverser_visitor it(shader, language, DBG_CG_ORIGINAL_SRC, NULL, NULL, NULL);
    it.append_version();
    it.run(list);
//    haveValidObjectCode = true;

    dbgPrint(DBGLVL_COMPILERINFO, "\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"
                   "%s\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",
                   it.buffer);

//    return haveValidObjectCode;
    return true;
}


static void dumpNodeInfo(ir_instruction* node)
{
    dbgPrint(DBGLVL_COMPILERINFO, "(%s) ", FormatSourceRange(node->yy_location).c_str());
    switch( node->ir_type ){
    	case ir_type_call:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION CALL %s",
    		                node->as_call()->callee_name());
    		break;
    	case ir_type_function_signature:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION SIGNATURE %s",
    				node->as_function_signature()->function_name());
    		break;
    	case ir_type_expression:
    	{
    		ir_expression* eir = node->as_expression();
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "EXPRESSION: %s",
    				eir->operation < ir_last_unop ? "UNARY" :
    				eir->operation < ir_last_binop ? "BINARY" :
    				eir->operation < ir_last_triop ? "TERNARY" : "QUAD"	);
    		break;
    	}
    	case ir_type_assignment:
			dbgPrintNoPrefix( DBGLVL_COMPILERINFO, "ASSIGNMENT");
			break;
    	case ir_type_if:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "IF");
    		break;
    	case ir_type_loop:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "LOOP");
    		break;
    	case ir_type_return:
    		dbgPrintNoPrefix( DBGLVL_COMPILERINFO, "RETURN");
    		break;
    	case ir_type_discard:
    	    dbgPrintNoPrefix( DBGLVL_COMPILERINFO, "DISCARD");
    	    break;
    	default:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "unknown");
    		break;
    }
	dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
}

static void dumpDbgStack(IRGenStack *stack)
{
	IRGenStack::iterator iter;

    dbgPrint(DBGLVL_COMPILERINFO, "## STACK #####################################\n");

    for(iter = stack->begin(); iter != stack->end(); iter++) {
    	ir_instruction* ir = (*iter);
        dumpNodeInfo(ir);
        switch ( ir->debug_overwrite ) {
            case DbgOwDebugCode:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_debug> ");
                break;
            case DbgOwOriginalCode:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_original> ");
                break;
            default:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_unset> ");
                break;
        }
        dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
        scopeList *sl = get_scope(ir);

        if (sl) {
            scopeList::iterator sit;
            for (sit = sl->begin(); sit != sl->end(); sit++) {
                dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "%i ", (*sit));
            }
            dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
        } else {
            dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "no scope\n");
        }
    }

    dbgPrint(DBGLVL_COMPILERINFO, "###############################################\n");
}


/*
 * DEBUG INTERFACE
 */

void change_DbgOverwrite(ir_call* cir, enum ir_dbg_overwrite status)
{
    dbgPrint(DBGLVL_COMPILERINFO, "---->>> FUNCTION CALL ir_dbg_ow_%s\n",
    		status == ir_dbg_ow_original ? "original" :
    		status == ir_dbg_ow_debug ? "debug" : "unset" );
    dumpNodeInfo(cir);
    dbgPrint(DBGLVL_COMPILERINFO, "---------------------------------------\n");

    cir->debug_overwrite = status;

    ir_function_signature* funcDec = cir->callee;
    funcDec->debug_overwrite = status;
}

bool prepareTarget(ir_instruction** out, IRGenStack* dbgStack, DbgCgOptions dbgCgOptions,
					ShChangeableList *cgbl, ShVariableList *vl)
{
	ir_instruction* target = NULL;
	switch( dbgCgOptions ){
		case DBG_CG_GEOMETRY_MAP:
		case DBG_CG_VERTEX_COUNT:
			break;
		case DBG_CG_COVERAGE:
		case DBG_CG_SELECTION_CONDITIONAL:
		case DBG_CG_LOOP_CONDITIONAL:
		case DBG_CG_GEOMETRY_CHANGEABLE:
			if( !dbgStack->empty() )
				target = dbgStack->back();
			break;
		case DBG_CG_CHANGEABLE:
		{
			IRGenStack::reverse_iterator rit;

			if( !cgbl )
				return false;

			/* iterate backwards thru stack */
			for( rit = dbgStack->rbegin(); rit != dbgStack->rend(); rit++ ){

				ir_instruction* rir = *rit;
				scopeList *sl = get_scope( rir );
				scopeList::iterator sit;
				bool user_call = (rir->ir_type == ir_type_call
						&& !rir->as_call()->callee->is_builtin);

				/* check if all changeables are in scope */bool allInScope = true;

				for( int id = 0; id < cgbl->numChangeables; id++ ){
					bool inScope = false;

					/* builtins are always valid */
					ShVariable *var = findShVariableFromId( vl,
							cgbl->changeables[id]->id );
					if( !var ){
						dbgPrint( DBGLVL_WARNING,
								"CodeGen - unknown changeable, stop debugging\n" );
						return false;
					}

					if( var->builtin ){
						inScope = true;
					}else{
						/* parse the actual scope */
						for( sit = sl->begin(); sit != sl->end(); sit++ ){
							if( ( *sit ) == cgbl->changeables[id]->id ){
								inScope = true;
								break;
							}
						}
					}

					if( !inScope ){
						/* This is not the place to read out the target */
						if( user_call )
							change_DbgOverwrite( rir->as_call(), ir_dbg_ow_original );
						else
							dumpNodeInfo( rir );

						/* pop stack since we cannot process all changeables here */
						allInScope = false;
						break;
					}
				}

				if( allInScope ){
					/* we are finially finished */
					target = rir;

					if( user_call )
						change_DbgOverwrite( rir->as_call(), ir_dbg_ow_original );
					else
						dumpNodeInfo( *rit );
					break;
				}
			}

			if( !target ){
				dbgPrint( DBGLVL_WARNING,
						"CodeGen - target not in stack, no debugging possible\n" );
				dumpDbgStack( dbgStack );
				return false;
			}else{
				/* iterate trough the rest of the stack */
				for( rit = dbgStack->rbegin(); rit != dbgStack->rend();
						rit++ ){
					ir_instruction* rir = *rit;

					if( rir->ir_type == ir_type_call && !rir->as_call()->callee->is_builtin
							&& rir->debug_overwrite == ir_dbg_ow_unset ){
						change_DbgOverwrite( rir->as_call(), ir_dbg_ow_debug );
					}
				}
				/* DEBUG OUTPUT */
				dumpDbgStack( dbgStack );
			}
		}
			break;
		default:
			break;
	}

	*out = target;
	return true;
}


bool compileDbgShaderCode(struct gl_shader* shader, ShChangeableList *cgbl,
        ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code)
{
    if (dbgCgOptions == DBG_CG_ORIGINAL_SRC) {
        return compileShaderCode(shader);
    }

    exec_list* list = shader->ir;

    /* Check for empty parse tree */
    if ( list->head->next == NULL || list->tail_pred == NULL )
        return false;

    EShLanguage language = EShLangVertex;
    if( shader->Type == GL_FRAGMENT_SHADER )
    	language = EShLangFragment;
    else if( shader->Type == GL_GEOMETRY_SHADER )
		language = EShLangGeometry;

	cgInitLoopIter();

    /* 1. Pass:
     * - build up debug stack (list of all nodes on the dbgPath)
     * - mark target node where code has to be inserted
     */
	ir_stack_traverser_visitor it1pass(vl);

    ir_function* main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, shader);
    if (!main) {
        dbgPrint(DBGLVL_ERROR, "CodeGen - could not find main function!\n");
        exit(1);
    }
    it1pass.visit(main);

    //Traverse(main, &it1pass);

    /* Find target, mark it and prepare neccessary dbgTemporaries */
    ir_instruction* target = NULL;
	if( !prepareTarget( &target, &(it1pass.dbgStack), dbgCgOptions, cgbl, vl ) )
		return false;

    if (target) {
        dbgPrint(DBGLVL_COMPILERINFO, "TARGET is %i:%i %i:%i\n", target->yy_location.first_line,
                                                     target->yy_location.first_column,
                                                     target->yy_location.last_line,
                                                     target->yy_location.last_column);
        target->debug_target = true;
    }

    /* Prepare debug temporary registers */

    /* Always allocate a result register */
    dbgPrint(DBGLVL_COMPILERINFO, "initialize CG_TYPE_RESULT for %i\n", language);
    {
    	int size = ( dbgCgOptions == DBG_CG_GEOMETRY_MAP ||
    			     dbgCgOptions == DBG_CG_VERTEX_COUNT ) ? 3 :
    			     (dbgCgOptions == DBG_CG_GEOMETRY_CHANGEABLE) ? 2 : 0;
    	if( size ) {
    		ShVariable ret;
    		ret.uniqueId   = -1;
    		ret.builtin    = 0;
    		ret.name       = NULL;
    		ret.size       = size;
    		ret.isMatrix   = 0;
    		ret.isArray    = 0;
    		for (int i=0; i < MAX_ARRAYS; i++) {
    			ret.arraySize[0]  = 0;
    		}
    		ret.structName = NULL;
    		ret.structSize = 0;
    		ret.structSpec = NULL;
    		ret.type       = SH_FLOAT;
    		ret.qualifier  = SH_VARYING_OUT;
    		cgInit(CG_TYPE_RESULT, &ret, vl, language);
    	}else{
    		cgInit(CG_TYPE_RESULT, NULL, vl, language);
    	}
    }

    /* Check if an optional parameter register is neccessary */
    if (target && target->as_call() ) {

        /* Now check if a parameter is used for code insertion */
    	ir_call* cir = target->as_call();
    	ir_function_signature* funcDec = cir->callee;

        int lastInParameter = getFunctionDebugParameter(funcDec);
        if (lastInParameter >=0) {
        	ir_instruction* t = getSideEffectsDebugParameter( cir, lastInParameter );
        	ir_variable* var = t ? t->as_variable() : NULL;
        	if( var )
        		cgInit(CG_TYPE_PARAMETER, findShVariableFromSource(var), vl, language);
        	else {
                dbgPrint(DBGLVL_ERROR, "CodeGen - side effects returned type is invalid\n");
                exit(1);
        	}
        }
    }

    /* Check if a selection condition needs to be copied */
    if (target && (dbgCgOptions == DBG_CG_COVERAGE ||
    			   dbgCgOptions == DBG_CG_CHANGEABLE ||
    			   dbgCgOptions == DBG_CG_GEOMETRY_CHANGEABLE) &&
         target->ir_type == ir_type_if)
    {
        switch ( target->as_if()->debug_state_internal ) {
            case ir_dbg_if_condition_passed:
            case ir_dbg_if_if:
            case ir_dbg_if_else:
                cgInit(CG_TYPE_CONDITION, NULL, vl, language);
                break;
            default:
                break;
        }
    }

    ir_output_traverser_visitor it(shader, language,
					dbgCgOptions, vl, cgbl, &(it1pass.dbgStack));
    it.append_version();

    /* Add declaration of all neccessary types */
    cgAddDeclaration(CG_TYPE_ALL, &it.buffer, language);

    /* Clear the name map holding debugged names */
    cgInitNameMap();

    /* 2. Pass:
     * - do the actual code generation
     */
    it.run(list);

    /* Unset target again */
    if (target)
    	target->debug_target = false;

    cgDestruct(CG_TYPE_ALL);

    /* Cleanup */

    /* Reset overwrite modes */
    for (IRGenStack::iterator stit = it1pass.dbgStack.begin(), end = it1pass.dbgStack.end();
    		 stit != end; stit++) {
    	ir_instruction* stir = *stit;
    	stir->debug_overwrite = ir_dbg_ow_unset;
    	ir_call* cstir = stir->as_call();

        if ( cstir && ! cstir->callee->is_builtin ) {
        	ir_function_signature* funcDec = cstir->callee;
            funcDec->debug_overwrite = ir_dbg_ow_unset;
        }
    }
    dumpDbgStack(&(it1pass.dbgStack));
    it1pass.dbgStack.clear();

    dbgPrint(DBGLVL_COMPILERINFO, "\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"
                   "%s\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",
                   it.buffer);

#ifdef _WIN32
    *code = _strdup(it.buffer);
#else
	*code = strdup(it.buffer);
#endif

	return true;
}


