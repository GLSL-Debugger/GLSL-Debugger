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
#include "MShader.h"
#include "glsl/ir.h"
#include "glsl/glsl_symbol_table.h"
#include "glsl/list.h"
#include "Visitors/debugjump.h"
#include "IRScope.h"
#include "CodeTools.h"

#include "mesa-glsl/glslang/Include/ShHandle.h"
#include "mesa-glsl/glslang/Include/intermediate.h"
#include "glsldb/utils/dbgprint.h"


#define MAIN_FUNC_SIGNATURE "main"
#define VERBOSE 10

#define VPRINT(level, ...) { if (level < VERBOSE) \
                                dbgPrint(DBGLVL_COMPILERINFO, __VA_ARGS__); }

//enum OTOperation {
//    OTOpTargetUnset,         // Invalidate actual target
//    OTOpTargetSet,           // Look for new target
//    OTOpPathClear,           // Clear all path nodes, but not targets
//    OTOpPathBuild,           // Construct path from root to targets
//    OTOpReset,               // Reconstruct initial debug state
//    OTOpDone,                // Do no harm, i.e. don't change anything anymore
//    OTOpFinished             // Reached end of program, stop debugging
//};


/*
class TOutputDebugJumpTraverser : public TIntermTraverser {
public:
    TOutputDebugJumpTraverser() :
    	finishedDbgFunction(false)
        { }
    OTOperation operation;
    // Keeps track of function call order
    ir_instruction *root;
    struct gl_shader* shader;
    IRStack parseStack;
    int dbgBehaviour;
    bool finishedDbgFunction;
};
*/

static struct {
	ir_debugjump_traverser_visitor* it;
    DbgResult result;
} g;

//
// This function must be provided to create the actual
// compile object used by higher level code.  It returns
// a subclass of TCompiler.
//
//TCompiler* ConstructTraverseDebugJump(EShLanguage language, int debugOptions, int dbgBh)
//{
//    return new TTraverseDebugJump(language, debugOptions, dbgBh);
//}

//
// Delete the compiler made by ConstructCompiler
//
//void DeleteTraverseDebugJump(TCompiler* compiler)
//{
//    delete compiler;
//}

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

static void addScopeToScopeStack(scopeList *s)
{
	addScopeToScopeStack(g.result.scopeStack, s);
}

//static void setGobalScope(scopeList *s)
//{
//	setDbgScope(g.result.scope, s);
//    /* Add local scope to scope stack */
//    addScopeToScopeStack(s);
//}

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
//
//static DbgRsRange setDbgResultRange(const YYLTYPE& range)
//{
//    DbgRsRange r;
//    r.left.line = range.first_line;
//    r.left.colum = range.first_column;
//    r.right.line = range.last_line;
//    r.right.colum = range.last_column;
//    return r;
//}

//void processDebugable(ir_instruction *node, OTOperation *op)
//// Default handling of a node that can be debugged
//{
//    enum ir_dbg_state newState;
//    VPRINT(3, "processDebugable L:%s Op:%i DbgSt:%i\n",
//    		FormatSourceRange(node->yy_location).c_str(), *op, node->debug_state);
//
//    switch (*op) {
//        case OTOpTargetUnset:
//            switch (node->debug_state) {
//                case ir_dbg_state_target:
//                    node->debug_state = ir_dbg_state_unset;
//                    *op = OTOpTargetSet;
//                    VPRINT(3, "\t ------- unset target --------\n");
//                    g.result.position = DBG_RS_POSITION_UNSET;
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpTargetSet:
//            switch (node->debug_state) {
//                case ir_dbg_state_target:
//                    VPRINT(3, "\t ERROR! found target with DbgStTarget\n");
//                    exit(1);
//                    break;
//                case ir_dbg_state_unset:
//                    node->debug_state = ir_dbg_state_target;
//                    *op = OTOpDone;
//                    VPRINT(3, "\t -------- set target ---------\n");
//                    switch (node->ir_type) {
//                    	case ir_type_assignment:
//                    		g.result.position = DBG_RS_POSITION_ASSIGMENT;
//                    		g.result.range = setDbgResultRange(node->yy_location);
//                    		setGobalScope( get_scope(node) );
//                    		break;
//                    	case ir_type_expression:
//                    	{
//                    		ir_expression* exp = node->as_expression();
//                    		g.result.range = setDbgResultRange(node->yy_location);
//                    		setGobalScope( get_scope(node) );
//                    		if( exp->operation < ir_last_unop ){
//                    			g.result.position = DBG_RS_POSITION_UNARY;
//                    		}else if( exp->operation < ir_last_binop ){
//                    			// TODO: WHY?
//                    			g.result.position = DBG_RS_POSITION_ASSIGMENT;
//                    		}else {
//                    			// Dunno, lol
//                    		}
//                    		break;
//                    	}
////                    } else if (node->getAsBranchNode()) {
////                        g.result.position = DBG_RS_POSITION_BRANCH;
////                        g.result.range = setDbgResultRange(node->getRange());
////                        setGobalScope(node->getScope());
////                    } else if (node->getAsDummy()) {
////                        g.result.position = DBG_RS_POSITION_DUMMY;
////                        g.result.range = setDbgResultRange(node->getRange());
////                        setGobalScope(node->getScope());
////                    }
//                    	default:
//                    		break;
//                    }
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpPathClear:
//            switch (node->debug_state) {
//                case ir_dbg_state_path:
//                    node->debug_state = ir_dbg_state_unset;
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpPathBuild:
//            switch(node->debug_state) {
//                case ir_dbg_state_unset:
//                {
//                    /* Check children for DebugState */
//                    newState = ir_dbg_state_unset;
//                    switch (node->ir_type){
//                    	// Aggregate
//                    	// case ir_type_call:
//                    	case ir_type_function:
//                    	{
//                    		ir_function* f = node->as_function();
//                            foreach_iter( exec_list_iterator, iter, f->signatures ){
//                            	ir_instruction* ir = (ir_instruction *)iter.get();
//                            	VPRINT(6, "getDebugState: %i\n", ir->debug_state);
//                            	// TODO: We just throw away any states, choose only last?
//                            	if( ir->debug_state != ir_dbg_state_unset )
//                            		newState = ir_dbg_state_unset;
//                            }
//                            break;
//                    	}
//                    	case ir_type_variable:
//                    	{
//                    		ir_variable* v = node->as_variable();
//                    		/* Check optional initialization */
//                         	if( v->constant_value &&
//                    			v->constant_value->debug_state != ir_dbg_state_unset )
//                    			newState = ir_dbg_state_path;
//                    		else if( v->constant_initializer &&
//                    				 v->constant_initializer->debug_state != ir_dbg_state_unset )
//                    			newState = ir_dbg_state_path;
//                    		break;
//                    	}
//                    	case ir_type_expression:
//                    	{
//                    		ir_expression* exp = node->as_expression();
//                    		unsigned ops = exp->get_num_operands();
//                    		for( unsigned i = 0; i < ops; ++i ){
//                    			if( exp->operands[i] &&
//                    			    exp->operands[i]->debug_state != ir_dbg_state_unset)
//                    				newState = ir_dbg_state_path;
//                    		}
//                    		break;
//                    	}
//                    	case ir_type_if:
//                    	{
//                    		ir_if* irif = node->as_if();
//            				if( dbg_state_not_match( &irif->then_instructions, ir_dbg_state_unset ) ||
//            					dbg_state_not_match( &irif->else_instructions, ir_dbg_state_unset ) ||
//            					irif->condition->debug_state != ir_dbg_state_unset )
//                    		    newState = ir_dbg_state_path;
//                    		break;
//                    	}
////                    } else if (node->getAsBranchNode()) {
////                        TIntermBranch *bn = node->getAsBranchNode();
////                        if (bn->getExpression() &&
////                            bn->getExpression()->getDebugState() != DbgStNone) {
////                            newState = DbgStPath;
////                        }
//                    	default:
//                    		break;
//                    }
//                    node->debug_state = newState;
//                    break;
//                }
//                default:
//                    break;
//            }
//            break;
//        case OTOpReset:
//            node->debug_state = ir_dbg_state_unset;
//            break;
//        default:
//            break;
//    }
//}


//static bool TraverseAggregate(ir_instruction* raw_node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//
//    switch (raw_node->ir_type) {
//        case ir_type_function:
//        {
//        	ir_function* node = raw_node->as_function();
//        	ir_function_signature* sign = (ir_function_signature*)node->signatures.head;
//            VPRINT(2, "processAggregate L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
//                    FormatSourceRange(node->yy_location).c_str(),
//                    node->name, sign->is_builtin, oit->operation, node->debug_state);
//
//            switch (oit->operation) {
//                case OTOpTargetSet:
//                    /* This marks the end of a function call */
//                    if (node == oit->parseStack.top()) {
//                        VPRINT(2, "\t ---- pop %p from stack ----\n", node);
//                        oit->parseStack.pop();
//                        /* Do not dirctly jump into next function after
//                        * returning from a function */
//                        oit->dbgBehaviour &= ~DBG_BH_JUMPINTO;
//                        oit->finishedDbgFunction = true;
//                        if (!oit->parseStack.empty())
//                        {
//                            VPRINT(2, "\t ---- continue parsing at %pk ----\n",
//                                    oit->parseStack.top());
//                            oit->operation = OTOpTargetUnset;
//                            Traverse(oit->parseStack.top(), oit);
//                        } else {
//                            VPRINT(2, "\t ---- stack empty, finished ----\n");
//                            oit->operation = OTOpFinished;
//                        }
//                    } else {
//                        VPRINT(3, "\t ERROR! unexpected stack order\n");
//                        exit(1);
//                    }
//                    break;
//                case OTOpTargetUnset:
//                    break;
//                default:
//                    processDebugable(node, &oit->operation);
//                    break;
//            }
//            return true;
//        }
//        case ir_type_call:
//        {
//        	ir_call* node = raw_node->as_call();
//        	ir_function_signature* fs = node->callee;
//        	ShChangeableList* cl = &(g.result.cgbls);
//        	ShChangeableList* node_cgbl = get_changeable_list(node);
//
//        	VPRINT(2, "processAggregate L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
//                    FormatSourceRange(node->yy_location).c_str(),
//                    node->callee_name(), node->callee->is_builtin,
//                    oit->operation, node->debug_state);
//
//            VPRINT(2, "process node %s ...\n", node->callee_name());
//            switch (oit->operation) {
//                case OTOpTargetUnset:
//                    switch (node->debug_state) {
//                        case ir_dbg_state_target:
//                            if (oit->dbgBehaviour == DBG_BH_JUMPINTO) {
//                                // no changeable has to be copied in first place,
//                                // as we jump into this function
//
//                                VPRINT(2, "\t ---- push %p on stack ----\n", fs);
//                                oit->parseStack.push(fs);
//                                oit->operation = OTOpTargetSet;
//
//                                // add local parameters of called function first
//                                copyShChangeableList(cl, get_changeable_paramerers_list(fs));
//                                Traverse(fs, oit);
//
//                                // if parsing ends up here and a target is still beeing
//                                // searched, a wierd function was called, but anyway,
//                                // let's copy the appropriate changeables
//                                if (oit->operation == OTOpTargetSet)
//                                	copyShChangeableList(cl, node_cgbl);
//                            } else {
//                                node->debug_state = ir_dbg_state_unset;
//                                oit->operation = OTOpTargetSet;
//                                VPRINT(3, "\t ------- unset target --------\n");
//                                g.result.position = DBG_RS_POSITION_UNSET;
//
//                                // if parsing of the subfunction finished right now
//                                // -> copy only changed parameters to changeables
//                                // else
//                                // -> copy all, since user wants to jump over this func
//                                if (oit->finishedDbgFunction == true) {
//                                	copyShChangeableList(cl, get_changeable_paramerers_list(node));
//                                    oit->finishedDbgFunction = false;
//                                } else {
//                                	copyShChangeableList(cl, node_cgbl);
////                                    // Check if this function call would have emitted a vertex
////                                    if (node->containsEmitVertex()) {
////                                        g.result.passedEmitVertex = true;
////                                        VPRINT(6, "passed Emit %i\n", __LINE__);
////                                    }
//                                	if( containsDiscard( &fs->body ) ){
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                            }
//                            break;
//                        default:
//                            break;
//                    }
//                    break;
//                case OTOpTargetSet:
//                    switch (node->debug_state) {
//                        case ir_dbg_state_target:
//                            VPRINT(3, "\t ERROR! found target with DbgStTarget\n");
//                            exit(1);
//                            break;
//                        case ir_dbg_state_unset:
//                            if (! node->callee->is_builtin) {
//                                node->debug_state = ir_dbg_state_target;
//                                VPRINT(3, "\t -------- set target ---------\n");
//                                g.result.position = DBG_RS_POSITION_FUNCTION_CALL;
//                                g.result.range = setDbgResultRange(node->yy_location);
//                                setGobalScope( get_scope(node) );
//                                oit->operation = OTOpDone;
//                            } else {
////                                if (node->containsEmitVertex()) {
////                                    g.result.passedEmitVertex = true;
////                                    VPRINT(6, "passed Emit %i\n", __LINE__);
////                                }
//                                if ( containsDiscard( &(node->callee->body) ) ) {
//                                    g.result.passedDiscard = true;
//                                    VPRINT(6, "passed Discard %i\n", __LINE__);
//                                }
//                            }
//                            break;
//                        default:
//                            break;
//                    }
//                    break;
//                default:
//                    processDebugable(node, &oit->operation);
//                    break;
//            }
//            return true;
//        }
        //case EOpConstructStruct:
        //    return true;
//        default:
//            switch (oit->operation) {
//                case OTOpPathClear:
//                case OTOpPathBuild:
//                case OTOpReset:
//                    processDebugable(raw_node, &oit->operation);
//                    break;
//                default:
//                    break;
//            }
//            return true;
//    }
//}

//static bool TraverseBinary(ir_expression* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//    ir_instruction* ir = (ir_instruction*)node;
//
//    VPRINT(2, "processBinary L:%s DbgSt:%i\n",
//            FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);
//
//    switch (oit->operation) {
//    	case OTOpPathClear:
//    	case OTOpPathBuild:
//    	case OTOpReset:
//    		processDebugable(ir, &oit->operation);
//    		break;
//    	default:
//    		break;
//    }
//    return true;
//}


//static bool TraverseAssignment(ir_assignment* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//    ir_instruction* ir = (ir_instruction*)node;
//    ShChangeableList* cl = &(g.result.cgbls);
//
//    VPRINT(2, "processAssignment L:%s DbgSt:%i\n",
//            FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);
//
//    processDebugable(ir, &oit->operation);
//    switch (oit->operation) {
//    	case OTOpTargetSet:
//    		if (!(oit->dbgBehaviour & DBG_BH_JUMPINTO)) {
//    			// do not visit children
//    			// add all changeables of this node to the list
//    			copyShChangeableList(cl, get_changeable_list(node));
//
//    			// Check if this operation would have emitted a vertex
////    			if (node->containsEmitVertex()) {
////    				g.result.passedEmitVertex = true;
////    				VPRINT(6, "passed Emit %i\n", __LINE__);
////    			}
//    			if ( containsDiscard(node) ) {
//    				g.result.passedDiscard = true;
//    				VPRINT(6, "passed Discard %i\n", __LINE__);
//    			}
//    			return false;
//    		} else {
//    			// visit children
//    			++it->depth;
//    			if (node->rhs)
//    				Traverse(node->rhs, it);
//
//    			// Since there cannot be a target in the left side anyway
//    			// it would be possible to skip traversal
//    			if (node->lhs)
//    				Traverse(node->lhs, it);
//
//    			--it->depth;
//
//    			// if no target was found so far
//    			// all changeables need to be added to the list
//    			if (oit->operation == OTOpTargetSet) {
//    				copyShChangeableList(cl, get_changeable_list(node));
//    				// Check if this operation would have emitted a vertex
////    				if (node->containsEmitVertex()) {
////    					g.result.passedEmitVertex = true;
////    					VPRINT(6, "passed Emit %i\n", __LINE__);
////    				}
//    				if( containsDiscard(node) ){
//    					g.result.passedDiscard = true;
//    					VPRINT(6, "passed Discard %i\n", __LINE__);
//    				}
//    			}
//    			return false;
//    		}
//    	case OTOpTargetUnset:
//    		// visit children
//    		++it->depth;
//    		if (node->rhs)
//    			Traverse(node->rhs, it);
//
//    		// Since there cannot be a target in the left side anyway
//    		// it would be possible to skip traversal
//			if (node->lhs)
//				Traverse(node->lhs, it);
//
//    		--it->depth;
//
//    		// the old target was found inside left/right branch and
//    		// a new one is still being searched for
//    		// -> add only changed variables of this assigment, i.e.
//    		//    changeables of the left branch
//    		if (oit->operation == OTOpTargetSet) {
//    			copyShChangeableList(cl, get_changeable_list(node->lhs));
//    			// Check if this operation would have emitted a vertex
////    			if (node->getLeft()->containsEmitVertex()) {
////    				g.result.passedEmitVertex = true;
////    				VPRINT(6, "passed Emit %i\n", __LINE__);
////    			}
//    			if( containsDiscard(node->lhs) ) {
//    				g.result.passedDiscard = true;
//    				VPRINT(6, "passed Discard %i\n", __LINE__);
//    			}
//    		}
//    		return false;
//    	case OTOpPathClear:
//    	case OTOpReset:
//    		return true;
//    	case OTOpPathBuild:
//    	case OTOpDone:
//    		return false;
//    	default:
//    		return true;
//    }
//}

//static bool TraverseDeclaration(ir_variable* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//    VPRINT(2, "processDeclaration L:%s DbgSt:%i\n",
//		    	FormatSourceRange(node->yy_location).c_str(),
//			node->debug_state );
//
//    switch(oit->operation) {
//        case OTOpPathClear:
//        case OTOpPathBuild:
//        case OTOpReset:
//            processDebugable(node, &oit->operation);
//            break;
//        default:
//            break;
//    }
//
//    return true;
//}

//static bool TraverseSelection(bool /* preVisit */, TIntermSelection* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//    TIntermSelection *sn = node->getAsSelectionNode();
//
//    VPRINT(2, "processSelection L:%s DbgSt:%i\n",
//            FormatSourceRange(node->getRange()).c_str(),
//            node->getDebugState());
//
//    switch (oit->operation) {
//        case OTOpTargetUnset:
//            switch (sn->getDebugState()) {
//                case DbgStTarget:
//                    switch (sn->getDbgInternalState()) {
//                        case DBG_STATE_SELECTION_UNSET:
//                            dbgPrint(DBGLVL_ERROR, "CodeTools - target but state unset\n");
//                            exit(1);
//                            break;
//                        case DBG_STATE_SELECTION_INIT:
//                            VPRINT(3, "\t ------- unset target --------\n");
//                            sn->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            sn->setDbgInternalState(DBG_STATE_SELECTION_CONDITION);
//                            g.result.position = DBG_RS_POSITION_UNSET;
//                            if (oit->dbgBehaviour & DBG_BH_JUMPINTO) {
//                                // visit condition
//                                node->getCondition()->traverse(it);
//
//                                if (oit->operation == OTOpTargetSet) {
//                                    // there was not target in condition, so copy all
//                                    // changeables; it's unlikely that there is a
//                                    // changeable and no target, but anyway be on the
//                                    // safe side
//                                    copyShChangeableList(&(g.result.cgbls), node->getCondition()->getCgbList());
//
//                                    // Check if condition emitted a vertex
//                                    if (node->getCondition()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getCondition()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                return false;
//                            } else {
//#if 0    /* Jump only over condition */
//                                // user did not want to check condition, so just
//                                // copy all changeables to result
//                                copyShChangeableList(&(g.result.cgbls),
//                                        node->getCondition()->getCgbList());
//#else    /* Jump over whole condition */
//                                /* Finish debugging this selection */
//                                sn->setDbgInternalState(DBG_STATE_SELECTION_UNSET);
//                                /* copy changeables */
//                                copyShChangeableList(&(g.result.cgbls), node->getCondition()->getCgbList());
//                                // Check if condition emitted a vertex
//                                if (node->getCondition()->containsEmitVertex()) {
//                                    g.result.passedEmitVertex = true;
//                                    VPRINT(6, "passed Emit %i\n", __LINE__);
//                                }
//                                if (node->getCondition()->containsDiscard()) {
//                                    g.result.passedDiscard = true;
//                                    VPRINT(6, "passed Discard %i\n", __LINE__);
//                                }
//
//                                if (node->getTrueBlock()) {
//                                    copyShChangeableList(&(g.result.cgbls), node->getTrueBlock()->getCgbList());
//                                    // Check if true block emitted a vertex
//                                    if (node->getTrueBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTrueBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (node->getFalseBlock()) {
//                                    copyShChangeableList(&(g.result.cgbls), node->getFalseBlock()->getCgbList());
//                                    // Check if false block emitted a vertex
//                                    if (node->getFalseBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getFalseBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//
//#endif
//                                return false;
//                            }
//                            break;
//                        case DBG_STATE_SELECTION_CONDITION_PASSED:
//                            VPRINT(3, "\t ------- unset target again --------\n");
//                            sn->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            g.result.position = DBG_RS_POSITION_UNSET;
//
//                            if (node->getFalseBlock()) {
//                                /* IF ELSE construct */
//                                if (oit->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER) {
//                                    /* Finish debugging this selection */
//                                    sn->setDbgInternalState(DBG_STATE_SELECTION_UNSET);
//                                    /* copy changeables */
//                                    copyShChangeableList(&(g.result.cgbls), node->getTrueBlock()->getCgbList());
//                                    copyShChangeableList(&(g.result.cgbls), node->getFalseBlock()->getCgbList());
//                                    /* Check if condition emitted a vertex */
//                                    if (node->getTrueBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTrueBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                    if (node->getFalseBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getFalseBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                    return false;
//                                } else if (oit->dbgBehaviour & DBG_BH_FOLLOW_ELSE) {
//                                    sn->setDbgInternalState(DBG_STATE_SELECTION_ELSE);
//                                    // check other branch for discards
//                                    if (node->getTrueBlock() &&
//                                        node->getTrueBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                } else {
//                                    sn->setDbgInternalState(DBG_STATE_SELECTION_IF);
//                                    // check other branch for discards
//                                    if (node->getFalseBlock() &&
//                                        node->getFalseBlock()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                            } else {
//                                /* IF only construct */
//                                if (oit->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER) {
//                                    /* Finish debugging this selection */
//                                    sn->setDbgInternalState(DBG_STATE_SELECTION_UNSET);
//                                    /* copy changeables */
//                                    if (node->getTrueBlock()) {
//                                        copyShChangeableList(&(g.result.cgbls), node->getTrueBlock()->getCgbList());
//                                        /* Check if condition emitted a vertex */
//                                        if (node->getTrueBlock()->containsEmitVertex()) {
//                                            g.result.passedEmitVertex = true;
//                                            VPRINT(6, "passed Emit %i\n", __LINE__);
//                                        }
//                                        if (node->getTrueBlock()->containsDiscard()) {
//                                            g.result.passedDiscard = true;
//                                            VPRINT(6, "passed Discard %i\n", __LINE__);
//                                        }
//                                    }
//                                    return false;
//                                } else {
//                                    sn->setDbgInternalState(DBG_STATE_SELECTION_IF);
//                                }
//                            }
//                            return true;
//                        default:
//                            break;
//                    }
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpTargetSet:
//            switch (sn->getDebugState()) {
//                case DbgStNone:
//                    switch (sn->getDbgInternalState()) {
//                        case DBG_STATE_SELECTION_UNSET:
//                            VPRINT(3, "\t -------- set target ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            sn->setDbgInternalState(DBG_STATE_SELECTION_INIT);
//                            if (node->getAsSelectionNode()->getFalseBlock()) {
//                                g.result.position =
//                                    DBG_RS_POSITION_SELECTION_IF_ELSE;
//                                g.result.range = setDbgResultRange(node->getRange());
//                                setGobalScope(node->getScope());
//                            } else {
//                                g.result.position =
//                                    DBG_RS_POSITION_SELECTION_IF;
//                                g.result.range = setDbgResultRange(node->getRange());
//                                setGobalScope(node->getScope());
//                            }
//                            return false;
//                        case DBG_STATE_SELECTION_CONDITION:
//                            VPRINT(3, "\t -------- set target again ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            sn->setDbgInternalState(
//                                    DBG_STATE_SELECTION_CONDITION_PASSED);
//                            if (node->getAsSelectionNode()->getFalseBlock()) {
//                                g.result.position = DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE;
//                            } else {
//                                g.result.position = DBG_RS_POSITION_SELECTION_IF_CHOOSE;
//                            }
//                            g.result.range = setDbgResultRange(node->getRange());
//                            setGobalScope(node->getScope());
//                            return false;
//                        case DBG_STATE_SELECTION_IF:
//                        case DBG_STATE_SELECTION_ELSE:
//                            VPRINT(4, "\t -------- set condition pass ---------\n");
//                            // Debugging of condition finished! Take care of the
//                            // non-debugged branch - if there is one - and copy
//                            // it's changeables!
//                            if (sn->getDbgInternalState() == DBG_STATE_SELECTION_IF)
//                            {
//                                if (node->getAsSelectionNode()->getFalseBlock()) {
//                                    copyShChangeableList(&(g.result.cgbls), node->getFalseBlock()->getCgbList());
//                                    /* Check if false block emitted a vertex */
//                                    if (node->getFalseBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                }
//                            } else {
//                                if (node->getAsSelectionNode()->getTrueBlock()) {
//
//                                    copyShChangeableList(&(g.result.cgbls), node->getTrueBlock()->getCgbList());
//                                    /* Check if true block emitted a vertex */
//                                    if (node->getTrueBlock()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                }
//                            }
//                            sn->setDbgInternalState(DBG_STATE_SELECTION_UNSET);
//                            return false;
//                        default:
//                            break;
//                    }
//                    break;
//                case DbgStTarget:
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpPathClear:
//            /* Conditional is intentionally not visited by post-traverser */
//            if (node->getCondition()) {
//                node->getCondition()->traverse(oit);
//            }
//            if (node->getDebugState() == DbgStPath) {
//                node->setDebugState(DbgStNone);
//            }
//            return true;
//        case OTOpPathBuild:
//            if (node->getDebugState() == DbgStNone) {
//                /* Check conditional and branches */
//                TIntermSelection *sn = node->getAsSelectionNode();
//                if ( (sn->getTrueBlock() &&
//                    sn->getTrueBlock()->getDebugState() != DbgStNone) ||
//                    (sn->getFalseBlock() &&
//                    sn->getFalseBlock()->getDebugState() != DbgStNone) ||
//                    (sn->getCondition() &&
//                    sn->getCondition()->getDebugState() != DbgStNone))
//                {
//                    node->setDebugState(DbgStPath);
//                }
//            }
//            return false;
//        case OTOpReset:
//            /* Conditional is intentionally not visited by post-traverser */
//            if (node->getCondition()) {
//                node->getCondition()->traverse(oit);
//            }
//            if (node->getTrueBlock()) {
//                node->getTrueBlock()->traverse(it);
//            }
//            if (node->getFalseBlock()) {
//                node->getFalseBlock()->traverse(it);
//            }
//            node->setDebugState(DbgStNone);
//            node->setDbgInternalState(DBG_STATE_SELECTION_UNSET);
//            return false;
//        default:
//            break;
//    }
//
//    return true;
//}

//static DbgRsTargetPosition setPositionLoop(LoopT t)
//{
//    switch (t) {
//        case LOOP_WHILE:
//            return DBG_RS_POSITION_LOOP_WHILE;
//        case LOOP_FOR:
//            return DBG_RS_POSITION_LOOP_FOR;
//        case LOOP_DO:
//            return DBG_RS_POSITION_LOOP_DO;
//        default:
//            dbgPrint(DBGLVL_ERROR, "CodeTools - setPositionLoop invalid loop type: %i\n", (int)t);
//            exit(1);
//            break;
//    }
//
//    // Never happens. Only warning
//    return DBG_RS_POSITION_DUMMY;
//}

//static bool TraverseLoop(bool, TIntermLoop* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//    TIntermLoop *ln = node->getAsLoopNode();
//
//    VPRINT(1, "processLoop L:%s DbgSt:%i\n",
//            FormatSourceRange(node->getRange()).c_str(),
//            node->getDebugState());
//
//    switch (oit->operation) {
//        case OTOpTargetUnset:
//            switch (ln->getDebugState()) {
//                case DbgStTarget:
//                    switch (ln->getDbgInternalState()) {
//                        case DBG_STATE_LOOP_UNSET:
//                            dbgPrint(DBGLVL_ERROR, "CodeTools - target but state unset\n");
//                            exit(1);
//                            break;
//                        case DBG_STATE_LOOP_QYR_INIT:
//                            VPRINT(3, "\t ------- unset target --------\n");
//                            ln->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            ln->setDbgInternalState(DBG_STATE_LOOP_WRK_INIT);
//                            g.result.position = DBG_RS_POSITION_UNSET;
//                            if (oit->dbgBehaviour & DBG_BH_JUMPINTO) {
//                                // visit initialization
//                                node->getInit()->traverse(it);
//
//                                if (oit->operation == OTOpTargetSet) {
//                                    // there was not target in condition, so copy all
//                                    // changeables; it's unlikely that there is a
//                                    // changeable and no target, but anyway be on the
//                                    // safe side
//                                    copyShChangeableList(&(g.result.cgbls), node->getInit()->getCgbList());
//                                    /* Check if init emitted a vertex */
//                                    if (node->getInit()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getInit()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                            } else {
//#if 0    /* Jump only over loop */
//                                // user did not want to check initialization, so just
//                                // copy all changeables to result
//                                copyShChangeableList(&(g.result.cgbls),
//                                        node->getInit()->getCgbList());
//#else    /* Jump over whole loop */
//                                /* Finish debugging this loop */
//                                ln->setDbgInternalState(DBG_STATE_LOOP_UNSET);
//                                ln->setDbgIter(0);
//
//                                /* Copy all changeables from condition, test, body, terminal */
//                                copyShChangeableList(&(g.result.cgbls),
//                                        node->getInit()->getCgbList());
//                                /* Check if init emitted a vertex */
//                                if (node->getInit()->containsEmitVertex()) {
//                                    g.result.passedEmitVertex = true;
//                                    VPRINT(6, "passed Emit %i\n", __LINE__);
//                                }
//                                if (node->getInit()->containsDiscard()) {
//                                    g.result.passedDiscard = true;
//                                    VPRINT(6, "passed Discard %i\n", __LINE__);
//                                }
//                                if (ln->getTest()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTest()->getCgbList());
//                                    /* Check if test emitted a vertex */
//                                    if (ln->getTest()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTest()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getBody()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getBody()->getCgbList());
//                                    /* Check if body emitted a vertex */
//                                    if (ln->getBody()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getBody()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getTerminal()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTerminal()->getCgbList());
//                                    /* Check if terminal emitted a vertex */
//                                    if (ln->getTerminal()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTerminal()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//#endif
//
//                            }
//                            return false;
//                        case DBG_STATE_LOOP_QYR_TEST:
//                            VPRINT(3, "\t ------- unset target --------\n");
//                            ln->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            ln->setDbgInternalState(DBG_STATE_LOOP_WRK_TEST);
//                            g.result.position = DBG_RS_POSITION_UNSET;
//                            if (oit->dbgBehaviour & DBG_BH_JUMPINTO) {
//                                // visit test
//                                node->getTest()->traverse(it);
//
//                                if (oit->operation == OTOpTargetSet) {
//                                    copyShChangeableList(&(g.result.cgbls), node->getTest()->getCgbList());
//                                    /* Check if condition emitted a vertex */
//                                    if (node->getTest()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTest()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                            } else {
//#if 0    /* Jump only over test */
//                                // do not visit test
//                                copyShChangeableList(&(g.result.cgbls),
//                                        node->getTest()->getCgbList());
//#else    /* Jump over whole loop */
//                                /* Finish debugging this loop */
//                                ln->setDbgInternalState(DBG_STATE_LOOP_UNSET);
//                                ln->setDbgIter(0);
//
//                                /* Copy all changeables from test, body, terminal */
//                                if (ln->getTest()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTest()->getCgbList());
//                                    /* Check if test emitted a vertex */
//                                    if (ln->getTest()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTest()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getBody()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getBody()->getCgbList());
//                                    /* Check if body emitted a vertex */
//                                    if (ln->getBody()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getBody()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getTerminal()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTerminal()->getCgbList());
//                                    /* Check if terminal emitted a vertex */
//                                    if (ln->getTerminal()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTerminal()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//#endif
//                            }
//                            return false;
//                        case DBG_STATE_LOOP_SELECT_FLOW:
//                            VPRINT(3, "\t ------- unset target again --------\n");
//                            ln->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            g.result.position = DBG_RS_POSITION_UNSET;
//
//                            if (oit->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER) {
//                                /* Finish debugging this loop */
//                                ln->setDbgInternalState(DBG_STATE_LOOP_UNSET);
//                                ln->setDbgIter(0);
//
//                                /* Copy all changeables from
//                                * test, body, terminal */
//                                if (ln->getTest()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTest()->getCgbList());
//                                    /* Check if test emitted a vertex */
//                                    if (ln->getTest()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTest()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getBody()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getBody()->getCgbList());
//                                    /* Check if body emitted a vertex */
//                                    if (ln->getBody()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getBody()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getTerminal()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTerminal()->getCgbList());
//                                    /* Check if terminal emitted a vertex */
//                                    if (ln->getTerminal()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTerminal()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                return false;
//                            } else if (oit->dbgBehaviour & DBG_BH_LOOP_NEXT_ITER) {
//                                /* Perform one iteration without further debugging
//                                *  - target stays the same
//                                *  - increase loop iter counter
//                                *  - change traverse to be finished
//                                *  - prepare result
//                                */
//
//                                /* Reset target */
//                                node->setDebugState(DbgStTarget);
//                                ln->setDbgInternalState(DBG_STATE_LOOP_SELECT_FLOW);
//
//                                /* Increase iteration */
//                                ln->addDbgIter(1);
//
//                                /* Finish debugging */
//                                oit->operation = OTOpDone;
//
//                                /* Build result struct */
//                                g.result.position = DBG_RS_POSITION_LOOP_CHOOSE;
//                                switch (ln->getLoopType()) {
//                                    case LOOP_FOR:
//                                    case LOOP_WHILE:
//                                        g.result.loopIteration = ln->getDbgIter();
//                                        break;
//                                    case LOOP_DO:
//                                        g.result.loopIteration = ln->getDbgIter() - 1;
//                                        break;
//                                }
//                                g.result.range = setDbgResultRange(node->getRange());
//                                if (ln->getTest()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTest()->getCgbList());
//                                    /* Check if test emitted a vertex */
//                                    if (ln->getTest()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTest()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getBody()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getBody()->getCgbList());
//                                    /* Check if body emitted a vertex */
//                                    if (ln->getBody()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getBody()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                                if (ln->getTerminal()) {
//                                    copyShChangeableList(&(g.result.cgbls), ln->getTerminal()->getCgbList());
//                                    /* Check if terminal emitted a vertex */
//                                    if (ln->getTerminal()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTerminal()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//
//                                setGobalScope(node->getScope());
//                            } else {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_WRK_BODY);
//                            }
//                            return true;
//                        case DBG_STATE_LOOP_QYR_TERMINAL:
//                            VPRINT(3, "\t ------- unset target --------\n");
//                            ln->setDebugState(DbgStNone);
//                            oit->operation = OTOpTargetSet;
//                            ln->setDbgInternalState(DBG_STATE_LOOP_WRK_TERMINAL);
//                            g.result.position = DBG_RS_POSITION_UNSET;
//                            if (oit->dbgBehaviour & DBG_BH_JUMPINTO) {
//                                // visit terminal
//                                node->getTerminal()->traverse(it);
//
//                                if (oit->operation == OTOpTargetSet) {
//                                    copyShChangeableList(&(g.result.cgbls), node->getTerminal()->getCgbList());
//                                    /* Check if terminal emitted a vertex */
//                                    if (node->getTerminal()->containsEmitVertex()) {
//                                        g.result.passedEmitVertex = true;
//                                        VPRINT(6, "passed Emit %i\n", __LINE__);
//                                    }
//                                    if (node->getTerminal()->containsDiscard()) {
//                                        g.result.passedDiscard = true;
//                                        VPRINT(6, "passed Discard %i\n", __LINE__);
//                                    }
//                                }
//                            } else {
//                                // do not visit terminal
//                                copyShChangeableList(&(g.result.cgbls), node->getTerminal()->getCgbList());
//                                /* Check if test emitted a vertex */
//                                if (node->getTerminal()->containsEmitVertex()) {
//                                    g.result.passedEmitVertex = true;
//                                    VPRINT(6, "passed Emit %i\n", __LINE__);
//                                }
//                                if (node->getTerminal()->containsDiscard()) {
//                                    g.result.passedDiscard = true;
//                                    VPRINT(6, "passed Discard %i\n", __LINE__);
//                                }
//                            }
//                            return false;
//                        default:
//                            break;
//                    }
//                default:
//                    break;
//            }
//            break;
//        case OTOpTargetSet:
//            switch (ln->getDebugState()) {
//                case DbgStNone:
//                    switch (ln->getDbgInternalState()) {
//                        case DBG_STATE_LOOP_UNSET:
//                            switch (ln->getLoopType()) {
//                                case LOOP_FOR:
//                                case LOOP_WHILE:
//                                    VPRINT(3, "\t -------- set target ---------\n");
//                                    node->setDebugState(DbgStTarget);
//                                    oit->operation = OTOpDone;
//                                    if (ln->getInit()) {
//                                        ln->setDbgInternalState(
//                                                DBG_STATE_LOOP_QYR_INIT);
//                                        g.result.position =
//                                            setPositionLoop(ln->getLoopType());
//                                    } else if (ln->getTest()) {
//                                        ln->setDbgInternalState(
//                                                DBG_STATE_LOOP_QYR_TEST);
//                                        g.result.position =
//                                            setPositionLoop(ln->getLoopType());
//                                    } else {
//                                        ln->setDbgInternalState(
//                                                DBG_STATE_LOOP_SELECT_FLOW);
//                                        g.result.position =
//                                            DBG_RS_POSITION_LOOP_CHOOSE;
//                                        g.result.loopIteration = ln->getDbgIter();
//                                    }
//                                    g.result.range = setDbgResultRange(node->getRange());
//                                    setGobalScope(node->getScope());
//                                    return false;
//                                case LOOP_DO:
//                                    /* Process body imediately */
//                                    ln->setDbgInternalState(DBG_STATE_LOOP_WRK_BODY);
//                                    return true;
//                            }
//                            break;
//                        case DBG_STATE_LOOP_WRK_INIT:
//                            VPRINT(3, "\t -------- set target ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            if (ln->getTest()) {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_QYR_TEST);
//                            } else {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_SELECT_FLOW);
//                            }
//                            g.result.position = DBG_RS_POSITION_LOOP_FOR;
//                            g.result.range = setDbgResultRange(node->getRange());
//                            setGobalScope(node->getScope());
//                            return false;
//                        case DBG_STATE_LOOP_WRK_TEST:
//                            VPRINT(3, "\t -------- set target ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            ln->setDbgInternalState(DBG_STATE_LOOP_SELECT_FLOW);
//                            g.result.position = DBG_RS_POSITION_LOOP_CHOOSE;
//                            switch (ln->getLoopType()) {
//                                case LOOP_FOR:
//                                case LOOP_WHILE:
//                                    g.result.loopIteration = ln->getDbgIter();
//                                    break;
//                                case LOOP_DO:
//                                    g.result.loopIteration = ln->getDbgIter() - 1;
//                                    break;
//                            }
//                            g.result.range = setDbgResultRange(node->getRange());
//                            setGobalScope(node->getScope());
//                            return false;
//                        case DBG_STATE_LOOP_WRK_BODY:
//                            VPRINT(3, "\t -------- set target ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            if (ln->getTerminal()) {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_QYR_TERMINAL);
//                                g.result.position =
//                                    setPositionLoop(ln->getLoopType());
//                            } else if (ln->getTest()) {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_QYR_TEST);
//                                g.result.position =
//                                    setPositionLoop(ln->getLoopType());
//                                /* Increase the loop counter */
//                                ln->addDbgIter(1);
//                            } else {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_SELECT_FLOW);
//                                g.result.position = DBG_RS_POSITION_LOOP_CHOOSE;
//                                switch (ln->getLoopType()) {
//                                    case LOOP_FOR:
//                                    case LOOP_WHILE:
//                                        g.result.loopIteration = ln->getDbgIter();
//                                        break;
//                                    case LOOP_DO:
//                                        g.result.loopIteration = ln->getDbgIter() - 1;
//                                        break;
//                                }
//                                /* Increase the loop counter */
//                                ln->addDbgIter(1);
//                            }
//                            g.result.range = setDbgResultRange(node->getRange());
//                            setGobalScope(node->getScope());
//                            return false;
//                        case DBG_STATE_LOOP_WRK_TERMINAL:
//                            VPRINT(3, "\t -------- set target ---------\n");
//                            node->setDebugState(DbgStTarget);
//                            oit->operation = OTOpDone;
//                            if (ln->getTest()) {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_QYR_TEST);
//                                g.result.position =
//                                    setPositionLoop(ln->getLoopType());
//                            } else {
//                                ln->setDbgInternalState(DBG_STATE_LOOP_SELECT_FLOW);
//                                g.result.position = DBG_RS_POSITION_LOOP_CHOOSE;
//                                g.result.loopIteration = ln->getDbgIter();
//                            }
//                            /* Increase the loop counter */
//                            ln->addDbgIter(1);
//
//                            g.result.range = setDbgResultRange(node->getRange());
//                            setGobalScope(node->getScope());
//                            return false;
//                        default:
//                            break;
//                    }
//                case DbgStTarget:
//                    break;
//                default:
//                    break;
//            }
//            break;
//        case OTOpPathClear:
//            if (node->getDebugState() == DbgStPath) {
//                node->setDebugState(DbgStNone);
//            }
//            return true;
//        case OTOpPathBuild:
//            if (node->getDebugState() == DbgStNone) {
//                /* Check init, test, terminal, and body */
//                if ( (ln->getInit() &&
//                    ln->getInit()->getDebugState() != DbgStNone) ||
//                    (ln->getTest() &&
//                    ln->getTest()->getDebugState() != DbgStNone) ||
//                    (ln->getTerminal() &&
//                    ln->getTerminal()->getDebugState() != DbgStNone) ||
//                    (ln->getBody() &&
//                    ln->getBody()->getDebugState() != DbgStNone) )
//                {
//                    node->setDebugState(DbgStPath);
//                }
//            }
//            return false;
//        case OTOpReset:
//            if (node->getInit()) {
//                node->getInit()->traverse(it);
//            }
//            if (node->getTest()) {
//                node->getTest()->traverse(it);
//            }
//            if (node->getTerminal()) {
//                node->getTerminal()->traverse(it);
//            }
//            if (node->getBody()) {
//                node->getBody()->traverse(it);
//            }
//            node->setDebugState(DbgStNone);
//            node->setDbgInternalState(DBG_STATE_LOOP_UNSET);
//            /* Reset loop counter */
//            ln->setDbgIter(0);
//            return false;
//        default:
//            break;
//    }
//
//    return true;
//}

//static bool TraverseUnary(ir_expression* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//    ir_instruction* ir = (ir_instruction*)node;
//
//    VPRINT(2, "processUnary L:%d DbgSt:%i\n",
//            FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);
//
//    switch(oit->operation) {
//    	case OTOpPathClear:
//    	case OTOpPathBuild:
//    	case OTOpReset:
//    		processDebugable(ir, &oit->operation);
//    		break;
//    	default:
//    		break;
//    }
//    return true;
//}



//static bool TraverseBranch(bool /* previsit*/, TIntermBranch* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//    if (node->getExpression()) {
//        processDebugable(node, &oit->operation);
//    }
//
//    return true;
//}

//static void TraverseDummy(TIntermDummy* node, TIntermTraverser* it)
//{
//    TOutputDebugJumpTraverser* oit = static_cast<TOutputDebugJumpTraverser*>(it);
//
//    VPRINT(2, "processDummy L:%d DbgSt:%i\n",
//            FormatSourceRange(node->getRange()).c_str(),
//            node->getDebugState());
//
//    processDebugable(node, &oit->operation);
//}


void clearTraverseDebugJump(void)
{
    delete g.it;
    g.it = NULL;
}



class TScopeStackTraverser : public TIntermTraverser {
public:
    TScopeStackTraverser() : passedTarget(false) { };
    bool passedTarget;
};

static bool ScopeStackTraverseAggregate( ir_instruction* raw_node, TIntermTraverser* it)
{
    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);

    switch (raw_node->ir_type) {
    	case ir_type_function:
    	{
    		ir_function* node = raw_node->as_function();
    		ir_function_signature* sign = (ir_function_signature*)node->signatures.head;
    		VPRINT(2, "processAggregate L:%s N:%s Blt:%i DbgSt:%i Passed:%i\n",
    				FormatSourceRange(node->yy_location).c_str(),
    				node->name, sign->is_builtin, node->debug_state, sit->passedTarget );

    		if( node->debug_state != ir_dbg_state_unset )
    			sit->passedTarget = false;

    		break;
    	}
    	case ir_type_call:
    	{
    		ir_call* node = raw_node->as_call();
    		VPRINT(2, "processAggregate L:%s N:%s Blt:%i DbgSt:%i Passed:%i\n",
    				FormatSourceRange(node->yy_location).c_str(),
    				node->callee_name(), node->callee->is_builtin,
    				node->debug_state, sit->passedTarget );
    		break;
    	}
    	default:
    		break;
    }

    if( sit->passedTarget || raw_node->debug_state == ir_dbg_state_unset )
        return false;

    addScopeToScopeStack( get_scope(raw_node) );

    if( raw_node->debug_state == ir_dbg_state_target ){
        sit->passedTarget = true;
        return false;
    }

    return true;
}

static bool ScopeStackTraverseExpression( ir_expression* raw_node, TIntermTraverser* it)
{
    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);
    ir_instruction* node = (ir_instruction*)raw_node;

    VPRINT(-2, "processExpression Op:%i L:%s DbgSt:%i Passed:%i\n",
    		raw_node->operation, FormatSourceRange(node->yy_location).c_str(),
    		node->debug_state, sit->passedTarget);

    if (sit->passedTarget)
        return false;

    addScopeToScopeStack( get_scope(node) );

    if (node->debug_state == ir_dbg_state_target ) {
        sit->passedTarget = true;
        return false;
    }

    return true;
}

//static bool ScopeStackTraverseSelection(bool /* preVisit */, TIntermSelection* node, TIntermTraverser* it)
//{
//    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);
//
//    VPRINT(-2, "processSelection L:%s DbgSt:%i Passed:%i\n",
//            FormatSourceRange(node->getRange()).c_str(),
//            node->getDebugState(),sit->passedTarget);
//
//    if (sit->passedTarget) {
//        return false;
//    }
//
//    if (node->getDebugState() == DbgStTarget) {
//        addScopeToScopeStack(node->getScope());
//        sit->passedTarget = true;
//        return false;
//    }
//
//    if (node->getDebugState() == DbgStNone) {
//        return false;
//    }
//
//    switch (node->getDbgInternalState()) {
//        case DBG_STATE_SELECTION_UNSET:
//        case DBG_STATE_SELECTION_INIT:
//            addScopeToScopeStack(node->getScope());
//            return false;
//        case DBG_STATE_SELECTION_CONDITION:
//        case DBG_STATE_SELECTION_CONDITION_PASSED:
//            addScopeToScopeStack(node->getScope());
//            if (node->getCondition()) {
//                node->getCondition()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_SELECTION_IF:
//            addScopeToScopeStack(node->getScope());
//            if (node->getCondition()) {
//                node->getCondition()->traverse(it);
//            }
//            if (node->getTrueBlock()) {
//                node->getTrueBlock()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_SELECTION_ELSE:
//            addScopeToScopeStack(node->getScope());
//            if (node->getCondition()) {
//                node->getCondition()->traverse(it);
//            }
//            if (node->getFalseBlock()) {
//                node->getFalseBlock()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_SELECTION_PASSED:
//            addScopeToScopeStack(node->getScope());
//            return false;
//    }
//    return true;
//}

//static bool ScopeStackTraverseLoop(bool, TIntermLoop* node, TIntermTraverser* it)
//{
//    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);
//
//    VPRINT(-2, "processLoop L:%s DbgSt:%i Passed:%i\n",
//            FormatSourceRange(node->getRange()).c_str(),
//            node->getDebugState(),
//            sit->passedTarget);
//
//    if (sit->passedTarget) {
//        return false;
//    }
//
//    if (node->getDebugState() == DbgStTarget) {
//        addScopeToScopeStack(node->getScope());
//        sit->passedTarget = true;
//        return false;
//    }
//
//    if (node->getDebugState() == DbgStNone) {
//        return false;
//    }
//
//    switch (node->getDbgInternalState()) {
//        case DBG_STATE_LOOP_UNSET:
//        case DBG_STATE_LOOP_QYR_INIT:
//            addScopeToScopeStack(node->getScope());
//            return false;
//        case DBG_STATE_LOOP_WRK_INIT:
//        case DBG_STATE_LOOP_QYR_TEST:
//            addScopeToScopeStack(node->getScope());
//            if (node->getInit()) {
//                node->getInit()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_LOOP_WRK_TEST:
//        case DBG_STATE_LOOP_SELECT_FLOW:
//        case DBG_STATE_LOOP_QYR_TERMINAL:
//            addScopeToScopeStack(node->getScope());
//            if (node->getInit()) {
//                node->getInit()->traverse(it);
//            }
//            if (node->getTest()) {
//                node->getTest()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_LOOP_WRK_BODY:
//            addScopeToScopeStack(node->getScope());
//            if (node->getInit()) {
//                node->getInit()->traverse(it);
//            }
//            if (node->getTest()) {
//                node->getTest()->traverse(it);
//            }
//            if (node->getBody()) {
//                node->getBody()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_LOOP_WRK_TERMINAL:
//            addScopeToScopeStack(node->getScope());
//            if (node->getInit()) {
//                node->getInit()->traverse(it);
//            }
//            if (node->getTest()) {
//                node->getTest()->traverse(it);
//            }
//            if (node->getTerminal()) {
//                node->getTerminal()->traverse(it);
//            }
//            return false;
//        case DBG_STATE_LOOP_PASSED:
//            addScopeToScopeStack(node->getScope());
//            return false;
//    }
//    return true;
//}

//static bool ScopeStackTraverseBranch(bool /* previsit*/, TIntermBranch* node, TIntermTraverser* it)
//{
//    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);
//
//    if (sit->passedTarget) {
//        return false;
//    }
//
//    if (node->getDebugState() == DbgStTarget) {
//        addScopeToScopeStack(node->getScope());
//        sit->passedTarget = true;
//        return false;
//    }
//
//    addScopeToScopeStack(node->getScope());
//    return true;
//}

static bool ScopeStackTraverseDeclaration(ir_variable* node, TIntermTraverser* it)
{
    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);

    VPRINT(-2, "processDeclaration L:%s DbgSt:%i Passed:%i\n",
            FormatSourceRange(node->yy_location).c_str(),
            node->debug_state,
            sit->passedTarget);


    if (sit->passedTarget)
        return false;

    addScopeToScopeStack( get_scope(node) );

    if (node->debug_state == ir_dbg_state_target) {
        sit->passedTarget = true;
        return false;
    }

    return true;
}

//static void ScopeStackTraverseDummy(TIntermDummy* node, TIntermTraverser* it)
//{
//    TScopeStackTraverser* sit = static_cast<TScopeStackTraverser*>(it);
//
//    if (sit->passedTarget) {
//        return;
//    }
//
//    if (node->getDebugState() == DbgStTarget) {
//        addScopeToScopeStack(node->getScope());
//        sit->passedTarget = true;
//        return;
//    }
//
//    addScopeToScopeStack(node->getScope());
//}


//
//  Generate code from the given parse tree
//
DbgResult* ShaderTraverse( struct gl_shader* shader, int debugOptions, int dbgBehaviour )
{
	UNUSED_ARG(debugOptions)

    /* Setup scope if neccessary */
    initGlobalScope();
    initGlobalScopeStack();
    initGlobalChangeables();

    g.result.range.left.line = 0;
    g.result.range.left.colum = 0;
    g.result.range.right.line = 0;
    g.result.range.right.colum = 0;

    /* Check for empty parse tree */
    if (shader == 0) {
        g.result.status = DBG_RS_STATUS_ERROR;
        g.result.position = DBG_RS_POSITION_UNSET;
        clearGlobalScope();
        clearGlobalScopeStack();
        clearGlobalChangeables();
        return &g.result;
    }

    /* Check validity of debug request */

    g.result.status = DBG_RS_STATUS_UNSET;
    g.result.position = DBG_RS_POSITION_UNSET;
    g.result.loopIteration = 0;
    g.result.passedEmitVertex = false;
    g.result.passedDiscard = false;

    clearGlobalScope();
    clearGlobalScopeStack();
    clearGlobalChangeables();

    if (!g.it)
        g.it = new ir_debugjump_traverser_visitor(g.result);

    g.it->vertexEmited = false;
    g.it->discardPassed = false;

    exec_list* list = shader->ir;
    ir_instruction* root = (ir_instruction*)list->head;

    //g.it->root = root;

    g.it->preVisit = false;
    g.it->postVisit = false;
    g.it->debugVisit = true;
//    g.it->rightToLeft = false;

    g.it->dbgBehaviour = dbgBehaviour;

//    g.it->visitAggregate = TraverseAggregate;
//    g.it->visitAssignment = TraverseAssignment;
//    g.it->visitBinary = TraverseBinary;
//    g.it->visitConstantUnion = 0;
////    g.it->visitSelection = TraverseSelection;
////    g.it->visitLoop = TraverseLoop;
//    g.it->visitSymbol = 0;
//    g.it->visitFuncParam = 0;
//    g.it->visitUnary = TraverseUnary;
////    g.it->visitBranch = TraverseBranch;
//    g.it->visitDeclaration = TraverseDeclaration;
//    g.it->visitFuncDeclaration = 0;
//    g.it->visitSpecification = 0;
//    g.it->visitParameter = 0;
//    g.it->visitDummy = TraverseDummy;


    /* Check for finished parsing */
    if (dbgBehaviour != DBG_BH_RESET  &&
        root->debug_state == ir_dbg_state_end) {
        VPRINT(1, "!!! debugging already finished !!!\n");
        g.result.status = DBG_RS_STATUS_FINISHED;
        return &g.result;
    }

    /* In case of a reset clear DbgStates and empty stack */
    if (dbgBehaviour == DBG_BH_RESET) {
        g.it->operation = OTOpReset;
        while (!(g.it->parseStack.empty())) {
            g.it->parseStack.pop();
        }
        VPRINT(1, "********* reset traverse **********\n");
        g.it->visit(list);
        //TraverseList( list, g.it );
        return NULL;
    }

    /* Clear debug path, i.e remove all DbgStPath */
    g.it->operation = OTOpPathClear;
    VPRINT(1, "********* clear path traverse **********\n");
    g.it->visit(list);
//    TraverseList( list, g.it );

    /* Initialize parsetree for debugging if necessary */
    g.it->operation = OTOpTargetUnset;
    if (g.it->parseStack.empty()) {
        ir_instruction* main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, shader);
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


    TScopeStackTraverser itScopeStack;

    itScopeStack.preVisit = true;
    itScopeStack.postVisit = false;
    itScopeStack.debugVisit = false;
    itScopeStack.rightToLeft = false;

    itScopeStack.visitAggregate = ScopeStackTraverseAggregate;
    itScopeStack.visitBinary = ScopeStackTraverseExpression;
    itScopeStack.visitConstantUnion = 0;
//    itScopeStack.visitSelection = ScopeStackTraverseSelection;
//    itScopeStack.visitLoop = ScopeStackTraverseLoop;
    itScopeStack.visitSymbol = 0;
    itScopeStack.visitFuncParam = 0;
    itScopeStack.visitUnary = ScopeStackTraverseExpression;
//    itScopeStack.visitBranch = ScopeStackTraverseBranch;
    itScopeStack.visitDeclaration = ScopeStackTraverseDeclaration;
    itScopeStack.visitFuncDeclaration = 0;
    itScopeStack.visitSpecification = 0;
    itScopeStack.visitParameter = 0;
//    itScopeStack.visitDummy = ScopeStackTraverseDummy;

    TraverseList( list, &itScopeStack );

    g.result.status = DBG_RS_STATUS_OK;
    return &g.result;
}

