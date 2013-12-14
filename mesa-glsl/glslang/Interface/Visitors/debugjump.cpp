/*
 * debugjump.cpp
 *
 *  Created on: 10.10.2013
 */

#include "debugjump.h"
#include "ShaderLang.h"
#include "glsl/list.h"
#include "glslang/Interface/MShader.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"


#define DEFAULT_DEBUGABLE(ir)  \
	if( this->operation == OTOpPathClear || \
    	this->operation == OTOpPathBuild || \
    	this->operation == OTOpReset ) processDebugable(ir, &this->operation);


static void setDbgResultRange(DbgRsRange& r, const YYLTYPE& range)
{
	r.left.line = range.first_line;
	r.left.colum = range.first_column;
	r.right.line = range.last_line;
	r.right.colum = range.last_column;
}

void ir_debugjump_traverser_visitor::setGobalScope(scopeList *s)
{
    setDbgScope(this->result.scope, s);
    /* Add local scope to scope stack */
    addScopeToScopeStack(this->result.scopeStack, s);
}

void ir_debugjump_traverser_visitor::processDebugable(ir_instruction *node, OTOperation *op)
// Default handling of a node that can be debugged
{
    enum ir_dbg_state newState;
    VPRINT(3, "process Debugable L:%s Op:%i DbgSt:%i\n",
    		FormatSourceRange(node->yy_location).c_str(), *op, node->debug_state);

    switch (*op) {
        case OTOpTargetUnset:
			if( node->debug_state == ir_dbg_state_target ){
				node->debug_state = ir_dbg_state_unset;
				*op = OTOpTargetSet;
				VPRINT( 3, "\t ------- unset target --------\n" );
				result.position = DBG_RS_POSITION_UNSET;
			}
            break;
        case OTOpTargetSet:
            switch (node->debug_state) {
                case ir_dbg_state_target:
                    VPRINT(3, "\t ERROR! found target with DbgStTarget\n");
                    exit(1);
                    break;
                case ir_dbg_state_unset:
                    node->debug_state = ir_dbg_state_target;
                    *op = OTOpDone;
                    VPRINT(3, "\t -------- set target ---------\n");
                    switch (node->ir_type) {
                    	case ir_type_assignment:
                    		result.position = DBG_RS_POSITION_ASSIGMENT;
                    		setDbgResultRange(result.range, node->yy_location);
                    		setGobalScope( get_scope(node) );
                    		break;
                    	case ir_type_expression:
                    	{
                    		ir_expression* exp = node->as_expression();
                    		setDbgResultRange(result.range, node->yy_location);
                    		setGobalScope( get_scope(node) );
                    		if( exp->operation < ir_last_unop ){
                    			result.position = DBG_RS_POSITION_UNARY;
                    		}else if( exp->operation < ir_last_binop ){
                    			// TODO: WHY?
                    			result.position = DBG_RS_POSITION_ASSIGMENT;
                    		}else {
                    			// Dunno, lol
                    		}
                    		break;
                    	}
                    	case ir_type_return:
                    	case ir_type_discard:
                    	case ir_type_loop_jump:
                    	{
                    		result.position = DBG_RS_POSITION_BRANCH;
                    		setDbgResultRange(result.range, node->yy_location);
                    		setGobalScope( get_scope(node) );
                    		break;
                    	}
                    	case ir_type_dummy:
                    	{
                    		result.position = DBG_RS_POSITION_DUMMY;
                    		setDbgResultRange(result.range, node->yy_location);
                    		setGobalScope( get_scope(node) );
                    		break;
                    	}
                    	default:
                    		break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case OTOpPathClear:
            if( node->debug_state == ir_dbg_state_path )
            	node->debug_state = ir_dbg_state_unset;
            break;
        case OTOpPathBuild:
            switch(node->debug_state) {
                case ir_dbg_state_unset:
                {
                    /* Check children for DebugState */
                    newState = ir_dbg_state_unset;
                    switch (node->ir_type){
                    	case ir_type_function_signature:
                    	{
                    		ir_function_signature* fs = node->as_function_signature();
							foreach_iter( exec_list_iterator, iter, fs->parameters ) {
								ir_instruction* ir = (ir_instruction *)iter.get();
								VPRINT( 6, "getDebugState: %i\n", ir->debug_state );
								if( ir->debug_state != ir_dbg_state_unset ){
									newState = ir_dbg_state_path;
									break;
								}
							}
							foreach_iter( exec_list_iterator, iter, fs->body ) {
								ir_instruction* ir = (ir_instruction *)iter.get();
								VPRINT( 6, "getDebugState: %i\n", ir->debug_state );
								if( ir->debug_state != ir_dbg_state_unset ){
									newState = ir_dbg_state_path;
									break;
								}
							}
							break;
                    	}
                    	case ir_type_call:
                    	{
                    		ir_call* fs = node->as_call();
                    		if( fs->callee->debug_state != ir_dbg_state_unset )
                    			newState = ir_dbg_state_path;
                    		break;
                    	}
                    	case ir_type_variable:
                    	{
                    		ir_variable* v = node->as_variable();
                    		/* Check optional initialization */
                         	if( v->constant_value &&
                    			v->constant_value->debug_state != ir_dbg_state_unset )
                    			newState = ir_dbg_state_path;
                    		else if( v->constant_initializer &&
                    				 v->constant_initializer->debug_state != ir_dbg_state_unset )
                    			newState = ir_dbg_state_path;
                    		break;
                    	}
                    	case ir_type_assignment:
                    	{
                    		ir_assignment* as = node->as_assignment();
                    		if( as->rhs->debug_target != ir_dbg_state_unset ||
                    				as->lhs->debug_target != ir_dbg_state_unset )
                    			newState = ir_dbg_state_path;
                    		break;
                    	}
                    	case ir_type_expression:
                    	{
                    		ir_expression* exp = node->as_expression();
                    		unsigned ops = exp->get_num_operands();
                    		for( unsigned i = 0; i < ops; ++i ){
                    			if( exp->operands[i] &&
                    			    exp->operands[i]->debug_state != ir_dbg_state_unset)
                    				newState = ir_dbg_state_path;
                    		}
                    		break;
                    	}
                    	case ir_type_if:
                    	{
                    		ir_if* irif = node->as_if();
            				if( dbg_state_not_match( &irif->then_instructions, ir_dbg_state_unset ) ||
            					dbg_state_not_match( &irif->else_instructions, ir_dbg_state_unset ) ||
            					irif->condition->debug_state != ir_dbg_state_unset )
                    		    newState = ir_dbg_state_path;
                    		break;
                    	}
                    	case ir_type_return:
                    	{
                    		ir_return* r = node->as_return();
                    		if( r->value && r->value->debug_state != ir_dbg_state_unset )
                    			newState = ir_dbg_state_path;
                    		break;
                    	}
                    	case ir_type_discard:
                    	{
							ir_discard* d = node->as_discard();
							if( d->condition && d->condition->debug_state != ir_dbg_state_unset )
								newState = ir_dbg_state_path;
							break;
                    	}
                    	case ir_type_dummy:
                    	{
                    		ir_dummy* ird = node->as_dummy();
                    		int end_token = ir_dummy::pair_type(ird->dummy_type);
                    		if (ird && ird->next && end_token >= 0){
                    			foreach_node_safe(node, ird->next){
                    				ir_instruction * const inst = (ir_instruction *)node;
                    				ir_dummy * const dm = inst->as_dummy();
                    				if (dm && end_token == dm->dummy_type)
                    					break;
                    				if (inst->debug_state != ir_dbg_state_unset)
                    					newState = ir_dbg_state_path;
                    			}
                    		}
                    		break;
                    	}
                    	default:
                    		break;
                    }
                    node->debug_state = newState;
                    break;
                }
                default:
                    break;
            }
            break;
        case OTOpReset:
            node->debug_state = ir_dbg_state_unset;
            break;
        default:
            break;
    }
}


bool ir_debugjump_traverser_visitor::visitIr(ir_variable* ir)
{
	VPRINT( 2, "process Declaration L:%s DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );

	DEFAULT_DEBUGABLE( ir )
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_function_signature* ir)
{
//	ir_function_signature* sign = (ir_function_signature*)ir->signatures.head;
	VPRINT( 2, "process Signature L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->function_name(),
			ir->is_builtin(), this->operation, ir->debug_state );

	if( this->operation == OTOpTargetSet ){
		/* This marks the end of a function call */
		if( ir != this->parseStack.top() ){
			VPRINT( 3, "\t ERROR! unexpected stack order\n" );
			exit( 1 );
		}

		VPRINT( 2, "\t ---- pop %p from stack ----\n", ir );
		this->parseStack.pop();
		/* Do not directly jump into next function after
		 * returning from a function */
		this->dbgBehaviour &= ~DBG_BH_JUMPINTO;
		this->finishedDbgFunction = true;
		if( !this->parseStack.empty() ){
			ir_instruction* top_ir = this->parseStack.top();
			VPRINT( 2, "\t ---- continue parsing at %pk ----\n", top_ir );
			this->operation = OTOpTargetUnset;
			top_ir->accept( this );
		}else{
			VPRINT( 2, "\t ---- stack empty, finished ----\n" );
			this->operation = OTOpFinished;
		}
	}else if( this->operation != OTOpTargetUnset )
		processDebugable( ir, &this->operation );

	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_expression* ir)
{
	VPRINT( 2, "process Expression L:%s DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );

	DEFAULT_DEBUGABLE( ir )
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_texture* ir)
{
	DEFAULT_DEBUGABLE( ir )
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_swizzle* ir)
{
	DEFAULT_DEBUGABLE( ir )
	return true;
}

static void checkReturns(ir_instruction* ir, ir_debugjump_traverser_visitor* it)
{
	// Check if this operation would have emitted a vertex
	if( containsEmitVertex( ir ) ){
		it->vertexEmited = true;
		VPRINT( 6, "passed Emit %i\n", __LINE__ );
	}
	if( containsDiscard( ir ) ){
		it->discardPassed = true;
		VPRINT( 6, "passed Discard %i\n", __LINE__ );
	}
}

static void checkReturns(exec_list* ir, ir_debugjump_traverser_visitor* it)
{
	// Check if this operation would have emitted a vertex
	if( containsEmitVertex( ir ) ){
		it->vertexEmited = true;
		VPRINT( 6, "passed Emit %i\n", __LINE__ );
	}
	if( containsDiscard( ir ) ){
		it->discardPassed = true;
		VPRINT( 6, "passed Discard %i\n", __LINE__ );
	}
}

bool ir_debugjump_traverser_visitor::visitIr(ir_assignment* ir)
{
	VPRINT( 2, "process Assignment L:%s DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );

	processDebugable( ir, &this->operation );

	if( this->operation == OTOpPathBuild || this->operation == OTOpDone )
		return false;
	else if( this->operation == OTOpTargetSet ){
		if( !( this->dbgBehaviour & DBG_BH_JUMPINTO ) ){
			// do not visit children
			// add all changeables of this node to the list
			copyShChangeableList( &result.cgbls, get_changeable_list( ir ) );
			checkReturns( ir, this );
		}else{
			// visit children
			++this->depth;
			if( ir->rhs )
				ir->rhs->accept( this );

			// Since there cannot be a target in the left side anyway
			// it would be possible to skip traversal
			if( ir->lhs )
				ir->lhs->accept( this );
			--this->depth;

			// if no target was found so far
			// all changeables need to be added to the list
			if( this->operation == OTOpTargetSet ){
				copyShChangeableList( &result.cgbls, get_changeable_list( ir ) );
				checkReturns( ir, this );
			}
		}
		return false;
	}else if( OTOpTargetUnset ){
		// visit children
		++this->depth;
		if( ir->rhs )
			ir->rhs->accept( this );

		// Since there cannot be a target in the left side anyway
		// it would be possible to skip traversal
		if( ir->lhs )
			ir->lhs->accept( this );
		--this->depth;

		// the old target was found inside left/right branch and
		// a new one is still being searched for
		// -> add only changed variables of this assigment, i.e.
		//    changeables of the left branch
		if( this->operation == OTOpTargetSet ){
			copyShChangeableList( &result.cgbls, get_changeable_list( ir->lhs ) );
			checkReturns( ir, this );
		}
		return false;
	}

	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_constant* ir)
{
    VPRINT(2, "process Constant L:%s DbgSt:%i\n",
		    	FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );
    DEFAULT_DEBUGABLE(ir)
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_call* ir)
{
	ir_function_signature* fs = ir->callee;
	ShChangeableList* node_cgbl = get_changeable_list( ir );

	VPRINT( 2, "process Call L:%s N:%s Blt:%i Op:%i DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->callee_name(),
			ir->callee->is_builtin(), this->operation, ir->debug_state );

	//VPRINT( 2, "process node %s ...\n", ir->callee_name() );
	switch( this->operation ){
		case OTOpTargetUnset:
		{
			if( ir->debug_state != ir_dbg_state_target )
				break;
			if( this->dbgBehaviour == DBG_BH_JUMPINTO ){
				// no changeable has to be copied in first place,
				// as we jump into this function

				VPRINT( 2, "\t ---- push %p on stack ----\n", fs );
				this->parseStack.push( fs );
				this->operation = OTOpTargetSet;

				// add local parameters of called function first
				copyShChangeableList( &result.cgbls,
						get_changeable_paramerers_list( fs ) );
				fs->accept( this );

				// if parsing ends up here and a target is still beeing
				// searched, a wierd function was called, but anyway,
				// let's copy the appropriate changeables
				if( this->operation == OTOpTargetSet )
					copyShChangeableList( &result.cgbls, node_cgbl );
			}else{
				ir->debug_state = ir_dbg_state_unset;
				this->operation = OTOpTargetSet;
				VPRINT( 3, "\t ------- unset target --------\n" );
				result.position = DBG_RS_POSITION_UNSET;

				// if parsing of the subfunction finished right now
				// -> copy only changed parameters to changeables
				// else
				// -> copy all, since user wants to jump over this func
				if( this->finishedDbgFunction == true ){
					copyShChangeableList( &result.cgbls,
							get_changeable_paramerers_list( ir ) );
					this->finishedDbgFunction = false;
				}else{
					copyShChangeableList( &result.cgbls, node_cgbl );
					checkReturns( &fs->body, this );
				}
			}
			break;
		}
		case OTOpTargetSet:
		{
			if( ir->debug_state == ir_dbg_state_target ){
				VPRINT( 3, "\t ERROR! found target with DbgStTarget\n" );
				exit( 1 );
			}else if( ir->debug_state == ir_dbg_state_unset ){
				if( !ir->callee->is_builtin() ){
					ir->debug_state = ir_dbg_state_target;
					VPRINT( 3, "\t -------- set target ---------\n" );
					result.position = DBG_RS_POSITION_FUNCTION_CALL;
					setDbgResultRange( result.range, ir->yy_location );
					setGobalScope( get_scope( ir ) );
					this->operation = OTOpDone;
				}else
					checkReturns( &( ir->callee->body ), this );
			}
			break;
		}
		default:
			processDebugable( ir, &this->operation );
			break;
	}

	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_return* ir)
{
	if( ir->value )
		DEFAULT_DEBUGABLE(ir)
	return true;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_discard* ir)
{
	if( ir->condition )
		DEFAULT_DEBUGABLE(ir)
	return true;
}

static void addShChangeablesFromList(ir_debugjump_traverser_visitor* it,
		exec_list* listin)
{
	foreach_iter(exec_list_iterator, iter, *listin) {
		ir_instruction * const inst = (ir_instruction *)iter.get();
		copyShChangeableList( &it->result.cgbls, get_changeable_list( inst ) );
		checkReturns( inst, it );
	}
}

static inline void addShChangeables(ir_debugjump_traverser_visitor* it,
		ir_instruction* ir)
{
	if( !ir )
		return;

	copyShChangeableList( &it->result.cgbls, get_changeable_list( ir ) );
	checkReturns( ir, it );
}

bool ir_debugjump_traverser_visitor::visitIr(ir_if* ir)
{
	VPRINT( 2, "process Selection L:%s DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );

	switch( this->operation ){
		case OTOpTargetUnset:
			switch( ir->debug_state ){
				case ir_dbg_state_target:
					switch( ir->debug_state_internal ){
						case ir_dbg_if_unset:
							dbgPrint( DBGLVL_ERROR,
									"CodeTools - target but state unset\n" );
							exit( 1 );
							break;
						case ir_dbg_if_init:
						{
							VPRINT( 3, "\t ------- unset target --------\n" );
							ir->debug_state = ir_dbg_state_unset;
							this->operation = OTOpTargetSet;
							result.position = DBG_RS_POSITION_UNSET;

							ir->debug_state_internal = ir_dbg_if_condition;
							ir_rvalue* condition = ir->condition;

							if( this->dbgBehaviour & DBG_BH_JUMPINTO ){
								// visit condition
								condition->accept( this );
								if( this->operation == OTOpTargetSet ){
									// there was not target in condition, so copy all
									// changeables; it's unlikely that there is a
									// changeable and no target, but anyway be on the
									// safe side
									addShChangeables( this, condition );
								}
								return false;
							}else{
								/* Finish debugging this selection */
								ir->debug_state_internal = ir_dbg_if_unset;
								/* copy changeables */
								addShChangeables( this, condition );
								addShChangeablesFromList( this, &ir->then_instructions );
								addShChangeablesFromList( this, &ir->else_instructions );
								return false;
							}
							break;
						}
						case ir_dbg_if_condition_passed:
						{
							VPRINT( 3, "\t ------- unset target again --------\n" );
							ir->debug_state = ir_dbg_state_unset;
							this->operation = OTOpTargetSet;
							result.position = DBG_RS_POSITION_UNSET;

							if( this->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER ){
								/* Finish debugging this selection */
								ir->debug_state_internal = ir_dbg_if_unset;
								/* copy changeables */
								addShChangeablesFromList( this, &ir->then_instructions );
								addShChangeablesFromList( this, &ir->else_instructions );
								return false;
							}else if( this->dbgBehaviour & DBG_BH_FOLLOW_ELSE ){
								if( ir->else_instructions.is_empty() ){
									// It looks like it was this weird way before. Not sure
									// this branch will ever execute
									ir->debug_state_internal = ir_dbg_if_then;
								}else{
									ir->debug_state_internal = ir_dbg_if_else;
									// check other branch for discards
									if( containsDiscard( &ir->then_instructions ) ){
										this->discardPassed = true;
										VPRINT( 6, "passed Discard %i\n", __LINE__ );
									}
								}
							}else{
								ir->debug_state_internal = ir_dbg_if_then;
								// check other branch for discards
								if( containsDiscard( &ir->else_instructions ) ){
									this->discardPassed = true;
									VPRINT( 6, "passed Discard %i\n", __LINE__ );
								}
							}
							return true;
						}
						default:
							break;
					}
					break;
				default:
					break;
			}
			break;
		case OTOpTargetSet:
			if( ir->debug_state == ir_dbg_state_unset ){
				switch( ir->debug_state_internal ){
					case ir_dbg_if_unset:
						VPRINT( 3, "\t -------- set target ---------\n" );
						ir->debug_state = ir_dbg_state_target;
						this->operation = OTOpDone;
						ir->debug_state_internal = ir_dbg_if_init;
						if( !ir->else_instructions.is_empty() )
							result.position = DBG_RS_POSITION_SELECTION_IF_ELSE;
						else
							result.position = DBG_RS_POSITION_SELECTION_IF;
						setDbgResultRange( result.range, ir->yy_location );
						setGobalScope( get_scope( ir ) );
						return false;
					case ir_dbg_if_condition:
						VPRINT( 3, "\t -------- set target again ---------\n" );
						ir->debug_state = ir_dbg_state_target;
						this->operation = OTOpDone;
						ir->debug_state_internal = ir_dbg_if_condition_passed;
						if( !ir->else_instructions.is_empty() )
							result.position = DBG_RS_POSITION_SELECTION_IF_ELSE_CHOOSE;
						else
							result.position = DBG_RS_POSITION_SELECTION_IF_CHOOSE;
						setDbgResultRange( result.range, ir->yy_location );
						setGobalScope( get_scope( ir ) );
						return false;
					case ir_dbg_if_then:
					case ir_dbg_if_else:
					{
						VPRINT( 4, "\t -------- set condition pass ---------\n" );
						// Debugging of condition finished! Take care of the
						// non-debugged branch - if there is one - and copy
						// it's changeables!
						exec_list* path = ir->debug_state_internal == ir_dbg_if_then ?
									&ir->then_instructions : &ir->else_instructions;
						addShChangeablesFromList( this, path );
						ir->debug_state_internal = ir_dbg_if_unset;
						return false;
					}
					default:
						break;
				}
			}
			break;
		case OTOpPathClear:
			/* Conditional is intentionally not visited by post-traverser */
			ir->condition->accept( this );
			if( ir->debug_state == ir_dbg_state_path )
				ir->debug_state = ir_dbg_state_unset;
			return true;
		case OTOpPathBuild:
			if( ir->debug_state == ir_dbg_state_unset ){
				/* Check conditional and branches */
				if( dbg_state_not_match( &ir->then_instructions, ir_dbg_state_unset )
						|| dbg_state_not_match( &ir->else_instructions,
								ir_dbg_state_unset )
						|| ir->condition->debug_state != ir_dbg_state_unset )
					ir->debug_state = ir_dbg_state_path;
			}
			return false;
		case OTOpReset:
			/* Conditional is intentionally not visited by post-traverser */
			ir->condition->accept( this );
			this->visit( &ir->then_instructions );
			this->visit( &ir->else_instructions );
			ir->debug_state = ir_dbg_state_unset;
			ir->debug_state_internal = ir_dbg_if_unset;
			return false;
		default:
			break;
	}

	return true;
}

static inline DbgRsTargetPosition loop_position(enum ir_iteration_modes t)
{
	switch( t ){
		case ir_loop_while:
			return DBG_RS_POSITION_LOOP_WHILE;
		case ir_loop_for:
			return DBG_RS_POSITION_LOOP_FOR;
		case ir_loop_do_while:
			return DBG_RS_POSITION_LOOP_DO;
		default:
			dbgPrint( DBGLVL_ERROR,
					"CodeTools - setPositionLoop invalid loop type: %i\n", (int)t );
			exit( 1 );
			break;
	}

	// Never happens. Only warning
	return DBG_RS_POSITION_DUMMY;
}

bool ir_debugjump_traverser_visitor::visitIr(ir_loop* ir)
{
	VPRINT( 1, "process Loop L:%s DbgSt:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->debug_state );
	ir_if* check = ((ir_instruction*)ir->debug_check_block->next)->as_if();

	switch( this->operation ){
		case OTOpTargetUnset:
			switch( ir->debug_state ){
				case ir_dbg_state_target:
					switch( ir->debug_state_internal ){
						case ir_dbg_loop_unset:
							dbgPrint( DBGLVL_ERROR,
									"CodeTools - target but state unset\n" );
							exit( 1 );
							break;
						case ir_dbg_loop_qyr_init:
							VPRINT( 3, "\t ------- unset target --------\n" );
							this->operation = OTOpTargetSet;
							ir->debug_state = ir_dbg_state_unset;
							ir->debug_state_internal = ir_dbg_loop_wrk_init;
							result.position = DBG_RS_POSITION_UNSET;

							if( this->dbgBehaviour & DBG_BH_JUMPINTO ){
								// visit initialization
								ir->debug_init->accept( this );

								if( this->operation == OTOpTargetSet ){
									// there was not target in condition, so copy all
									// changeables; it's unlikely that there is a
									// changeable and no target, but anyway be on the
									// safe side
									addShChangeables( this, ir->debug_init );
								}
							}else{
								/* Finish debugging this loop */
								ir->debug_state_internal = ir_dbg_loop_unset;
								ir->debug_iter = 0;

								/* Copy all changeables from condition, test, body, terminal */
								addShChangeables( this, ir->debug_init );
								addShChangeables( this, check->condition );
								addShChangeablesFromList( this, &ir->body_instructions );
								addShChangeables( this, ir->debug_terminal );
							}
							return false;
						case ir_dbg_loop_qyr_test:
							VPRINT( 3, "\t ------- unset target --------\n" );
							this->operation = OTOpTargetSet;
							ir->debug_state = ir_dbg_state_unset;
							ir->debug_state_internal = ir_dbg_loop_wrk_test;
							result.position = DBG_RS_POSITION_UNSET;

							if( this->dbgBehaviour & DBG_BH_JUMPINTO ){
								// visit test
								check->condition->accept( this );
								if( this->operation == OTOpTargetSet )
									addShChangeables( this, check->condition );
							}else{
								/* Finish debugging this loop */
								ir->debug_state_internal = ir_dbg_loop_unset;
								ir->debug_iter = 0;

								/* Copy all changeables from test, body, terminal */
								addShChangeables( this, check->condition );
								addShChangeablesFromList( this, &ir->body_instructions );
								addShChangeables( this, ir->debug_terminal );
							}
							return false;
						case ir_dbg_loop_select_flow:
							VPRINT( 3, "\t ------- unset target again --------\n" );
							this->operation = OTOpTargetSet;
							ir->debug_state = ir_dbg_state_unset;
							ir->debug_state_internal = ir_dbg_loop_wrk_test;
							result.position = DBG_RS_POSITION_UNSET;

							if( this->dbgBehaviour & DBG_BH_SELECTION_JUMP_OVER ) {
								/* Finish debugging this loop */
								ir->debug_state_internal = ir_dbg_loop_unset;
								ir->debug_iter = 0;

								/* Copy all changeables from test, body, terminal */
								addShChangeables( this, check->condition );
								addShChangeablesFromList( this, &ir->body_instructions );
								addShChangeables( this, ir->debug_terminal );
								return false;
							} else if( this->dbgBehaviour & DBG_BH_LOOP_NEXT_ITER ) {
								/* Perform one iteration without further debugging
								 *  - target stays the same
								 *  - increase loop iter counter
								 *  - change traverse to be finished
								 *  - prepare result
								 */

								/* Reset target */
								ir->debug_state = ir_dbg_state_target;
								ir->debug_state_internal = ir_dbg_loop_select_flow;

								/* Increase iteration */
								ir->debug_iter++;

								/* Finish debugging */
								this->operation = OTOpDone;

								/* Build result struct */
								this->result.position = DBG_RS_POSITION_LOOP_CHOOSE;
								this->result.loopIteration = ir->debug_iter;
								if( ir->mode == ir_loop_do_while )
									result.loopIteration = ir->debug_iter - 1;
								else
									result.loopIteration = ir->debug_iter;
								setDbgResultRange( result.range, ir->yy_location );

								addShChangeables( this, check->condition );
								addShChangeablesFromList( this, &ir->body_instructions );
								addShChangeables( this, ir->debug_terminal );

								setGobalScope( get_scope( ir ) );
							} else {
								ir->debug_state_internal = ir_dbg_loop_wrk_body;
							}
							return true;
						case ir_dbg_loop_qyr_terminal:
							VPRINT( 3, "\t ------- unset target --------\n" );
							this->operation = OTOpTargetSet;
							ir->debug_state = ir_dbg_state_unset;
							ir->debug_state_internal = ir_dbg_loop_wrk_terminal;
							result.position = DBG_RS_POSITION_UNSET;
							if( this->dbgBehaviour & DBG_BH_JUMPINTO ){
								// visit terminal
								ir->debug_terminal->accept( this );
								if( this->operation == OTOpTargetSet )
									addShChangeables( this, ir->debug_terminal );
							}else{
								// do not visit terminal
								addShChangeables( this, ir->debug_terminal );
							}
							return false;
						default:
							break;
					}
					break;
				default:
					break;
			}
			break;
		case OTOpTargetSet:
			if( ir->debug_state == ir_dbg_state_unset ){
				printf("TODO: This will be fucked up now. Completely new logic must be added." );
				if( ir->debug_state_internal == ir_dbg_loop_unset && ir->mode == ir_loop_do_while ){
					/* Process body imediately */
					ir->debug_state_internal = ir_dbg_loop_wrk_body;
					return true;
				}
				if( ir->debug_state_internal == ir_dbg_loop_unset ||
						( ir->debug_state_internal >= ir_dbg_loop_wrk_init &&
						  ir->debug_state_internal <= ir_dbg_loop_wrk_terminal ) ){
					VPRINT( 3, "\t -------- set target ---------\n" );
					ir->debug_state = ir_dbg_state_target;
					this->operation = OTOpDone;

					switch( ir->debug_state_internal ){
						case ir_dbg_loop_unset:
						{
							if( ir->debug_init ){
								ir->debug_state_internal = ir_dbg_loop_qyr_init;
								result.position = loop_position( ir->mode );
							}else if( check->condition ){
								ir->debug_state_internal = ir_dbg_loop_qyr_test;
								result.position = loop_position( ir->mode );
							}else{
								ir->debug_state_internal = ir_dbg_loop_select_flow;
								result.position = DBG_RS_POSITION_LOOP_CHOOSE;
								result.loopIteration = ir->debug_iter;
							}
							break;
						}
						case ir_dbg_loop_wrk_init:
						{
							if( check->condition )
								ir->debug_state_internal = ir_dbg_loop_qyr_test;
							else
								ir->debug_state_internal = ir_dbg_loop_select_flow;
							result.position = DBG_RS_POSITION_LOOP_FOR;
							break;
						}
						case ir_dbg_loop_wrk_test:
						{
							ir->debug_state_internal = ir_dbg_loop_select_flow;
							result.position = DBG_RS_POSITION_LOOP_CHOOSE;
							if( ir->mode == ir_loop_do_while )
								result.loopIteration = ir->debug_iter - 1;
							else
								result.loopIteration = ir->debug_iter;
							break;
						}
						case ir_dbg_loop_wrk_body:
						{
							if( ir->debug_terminal ){
								ir->debug_state_internal = ir_dbg_loop_qyr_terminal;
								result.position = loop_position( ir->mode );
							}else if( check->condition ){
								ir->debug_state_internal = ir_dbg_loop_qyr_test;
								result.position = loop_position( ir->mode );
								/* Increase the loop counter */
								ir->debug_iter++;
							}else{
								ir->debug_state_internal = ir_dbg_loop_select_flow;
								result.position = DBG_RS_POSITION_LOOP_CHOOSE;
								if( ir->mode == ir_loop_do_while )
									result.loopIteration = ir->debug_iter - 1;
								else
									result.loopIteration = ir->debug_iter;
								/* Increase the loop counter */
								ir->debug_iter++;
							}
							break;
						}
						case ir_dbg_loop_wrk_terminal:
						{
							if( check->condition ){
								ir->debug_state_internal = ir_dbg_loop_qyr_test;
								result.position = loop_position( ir->mode );
							}else{
								ir->debug_state_internal = ir_dbg_loop_select_flow;
								result.position = DBG_RS_POSITION_LOOP_CHOOSE;
								result.loopIteration = ir->debug_iter;
							}
							/* Increase the loop counter */
							ir->debug_iter++;
							break;
						}
						default:
							break;
					}
					setDbgResultRange( result.range, ir->yy_location );
					setGobalScope( get_scope( ir ) );
					return false;
				}
			}
			break;
		case OTOpPathClear:
			if( ir->debug_state == ir_dbg_state_path )
				ir->debug_state = ir_dbg_state_unset;
			return true;
		case OTOpPathBuild:
			if( ir->debug_state == ir_dbg_state_unset ){
				/* Check init, test, terminal, and body */
				if( ( ir->debug_init && ir->debug_init->debug_state != ir_dbg_state_unset )
						|| ( check->condition && check->condition->debug_state != ir_dbg_state_unset )
						|| ( ir->debug_terminal && ir->debug_terminal->debug_state != ir_dbg_state_unset )
						|| dbg_state_not_match( &ir->body_instructions, ir_dbg_state_unset ) )
					ir->debug_state = ir_dbg_state_path;
			}
			return false;
		case OTOpReset:
			if( ir->debug_init )
				ir->debug_init->accept( this );
			if( check->condition )
				check->condition->accept( this );
			if( ir->debug_terminal )
				ir->debug_terminal->accept( this );
			this->visit( &ir->body_instructions );
			ir->debug_state = ir_dbg_state_unset;
			ir->debug_state_internal = ir_dbg_loop_unset;
			/* Reset loop counter */
			ir->debug_iter = 0;
			return false;
		default:
			break;
	}

	return true;
}



bool ir_debugjump_traverser_visitor::visitIr(ir_dummy* ir)
{
	VPRINT(2, "process Dummy L:%s DbgSt:%i\n",
            FormatSourceRange(ir->yy_location).c_str(), ir->debug_state);
	processDebugable(ir, &this->operation);
	return false;
}
