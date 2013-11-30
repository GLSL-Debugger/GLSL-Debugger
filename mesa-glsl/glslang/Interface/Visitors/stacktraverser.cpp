/*
 * stacktraverser.cpp
 *
 *  Created on: 09.10.2013
 */

#include "glsl/list.h"
#include "stacktraverser.h"
#include "glslang/Interface/MShader.h"
#include "glsldb/utils/dbgprint.h"



bool ir_stack_traverser_visitor::visitIr(ir_function_signature* ir)
{
	if( ir->debug_state == ir_dbg_state_target ){
		dbgPrint( DBGLVL_ERROR,
				"CodeGen - found function signature as target while building stack\n" );
		exit( 1 );
	}

	if( ir->debug_state != ir_dbg_state_path )
		return false;

	return true;
}

bool ir_stack_traverser_visitor::visitIr(ir_expression* ir)
{
	if( ir->debug_state == ir_dbg_state_path ){
		/* add node to stack and process children */
		/* unaries don't have an own scope, as the never change it
		 * thus we don't put it on the stack for simplicity
		 * even if it would work
		 * but we continue regularily with its child
		 */
		if( ir->operation > ir_last_unop )
			this->dbgStack.push_back( ir );
		return true;
	}

	/* add node to stack and finish */
    if (ir->debug_state == ir_dbg_state_target)
    	this->dbgStack.push_back(ir);
    return false;
}

bool ir_stack_traverser_visitor::visitIr(ir_assignment* ir)
{
	if( ir->debug_state == ir_dbg_state_path ){
		this->dbgStack.push_back( ir );
		return true;
	}

	/* add node to stack and finish */
    if (ir->debug_state == ir_dbg_state_target)
    	this->dbgStack.push_back(ir);

    return false;
}

bool ir_stack_traverser_visitor::visitIr(ir_call* ir)
{
	if( ir->debug_state == ir_dbg_state_target ){
		/* add to list and parse function */
		this->dbgStack.push_back( ir );
		ir->callee->accept( this );
		return false;
	}else if( ir->debug_state == ir_dbg_state_path ){
		/* add to list and parse parameter */
		this->dbgStack.push_back( ir );
		return true;
	}

	return false;
}


bool ir_stack_traverser_visitor::visitIr(ir_if* ir)
{
	if( ir->debug_state == ir_dbg_state_target ){
		if( ir->debug_state_internal == ir_dbg_if_init
				|| ir->debug_state_internal == ir_dbg_if_condition_passed )
			this->dbgStack.push_back( ir );
		else{
			dbgPrint( DBGLVL_ERROR, "CodeGen - ir_if as target has invalid internal state\n" );
			exit( 1 );
		}
	}else if( ir->debug_state == ir_dbg_state_path ){
		switch( ir->debug_state_internal ){
			case ir_dbg_if_condition:
				this->dbgStack.push_back( ir );
				ir->condition->accept(this);
				break;
			case ir_dbg_if_then:
				this->dbgStack.push_back( ir );
				this->visit(&ir->then_instructions);
				break;
			case ir_dbg_if_else:
				this->dbgStack.push_back( ir );
				this->visit(&ir->else_instructions);
				break;
			default:
				dbgPrint( DBGLVL_ERROR, "CodeGen - if_ir as path has invalid internal state\n" );
				exit( 1 );
		}
	}

	return false;
}

bool ir_stack_traverser_visitor::visitIr(ir_loop* ir)
{
	/* Clear old name for dbgLoopIter */
	char** iter_name = dbg_iter_name(ir);
	if( *iter_name != NULL ){
		free(*iter_name);
		*iter_name = NULL;
	}

	if( ir->debug_state == ir_dbg_state_target ){
		if( ir->debug_state_internal == ir_dbg_loop_qyr_init ){
			this->dbgStack.push_back( ir );
		}else if( ir->debug_state_internal == ir_dbg_loop_qyr_test ||
				  ir->debug_state_internal == ir_dbg_loop_select_flow ||
				  ir->debug_state_internal == ir_dbg_loop_qyr_terminal ){
			this->dbgStack.push_back( ir );
			cgSetLoopIterName( iter_name, this->vl );
		}else{
			dbgPrint( DBGLVL_ERROR, "CodeGen - loop target has invalid internal state\n" );
			exit( 1 );
		}
	}else if( ir->debug_state == ir_dbg_state_path ){
		switch( ir->debug_state_internal ){
			case ir_dbg_loop_wrk_init:
				this->dbgStack.push_back( ir );
				cgSetLoopIterName( iter_name, this->vl );
				if( ir->from )
					ir->from->accept(this);
				return false;
			case ir_dbg_loop_wrk_test:
				this->dbgStack.push_back( ir );
				cgSetLoopIterName( iter_name, this->vl );
				ir->increment->accept(this);
				return false;
			case ir_dbg_loop_wrk_body:
				this->dbgStack.push_back( ir );
				cgSetLoopIterName( iter_name, this->vl );
				this->visit(&ir->body_instructions);
				return false;
			case ir_dbg_loop_wrk_terminal:
				this->dbgStack.push_back( ir );
				cgSetLoopIterName( iter_name, this->vl );
				ir->to->accept(this);
				return false;
			default:
				dbgPrint( DBGLVL_ERROR,
						"CodeGen - loop path has invalid internal state\n" );
				exit( 1 );
		}
	}

	return false;
}

bool ir_stack_traverser_visitor::visitIr(ir_list_dummy *ir)
{
	/* add node to stack and finish */
    if (ir->debug_state == ir_dbg_state_target)
    	this->dbgStack.push_back(ir);
    return false;
}
