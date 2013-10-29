/*
 * scopestack.cpp
 *
 *  Created on: 27.10.2013
 */

#include "scopestack.h"
#include "glsldb/utils/dbgprint.h"
#include "glslang/Interface/IRScope.h"
#include "glslang/Interface/CodeTools.h"



bool ir_scopestack_traverse_visitor::visitIr(ir_variable* ir)
{
    VPRINT(-2, "processDeclaration L:%s DbgSt:%i Passed:%i\n",
                FormatSourceRange(ir->yy_location).c_str(),
                ir->debug_state, this->passedTarget);

    if (this->passedTarget)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_function_signature* ir)
{
	VPRINT( 2, "process Signature L:%s N:%s Blt:%i DbgSt:%i Passed:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->function_name(),
			ir->is_builtin, ir->debug_state, this->passedTarget );

	if( ir->debug_state != ir_dbg_state_unset )
		this->passedTarget = false;

	if( this->passedTarget || ir->debug_state == ir_dbg_state_unset )
		return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if( ir->debug_state == ir_dbg_state_target ){
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_expression* ir)
{
    VPRINT(-2, "processExpression L:%s DbgSt:%i Passed:%i\n",
            FormatSourceRange(ir->yy_location).c_str(),
            ir->debug_state, this->passedTarget);

    if (this->passedTarget)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_swizzle* ir)
{
    VPRINT(-2, "processSwizzle L:%s DbgSt:%i Passed:%i\n",
            FormatSourceRange(ir->yy_location).c_str(),
            ir->debug_state, this->passedTarget);

    if (this->passedTarget)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_assignment* ir)
{
    VPRINT(-2, "processAssignment L:%s DbgSt:%i Passed:%i\n",
            FormatSourceRange(ir->yy_location).c_str(),
            ir->debug_state, this->passedTarget);

    if (this->passedTarget)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_call* ir)
{
	VPRINT( 2, "processAggregate L:%s N:%s Blt:%i DbgSt:%i Passed:%i\n",
			FormatSourceRange(ir->yy_location).c_str(), ir->callee_name(),
			ir->callee->is_builtin, ir->debug_state, this->passedTarget );

	if( ir->debug_state != ir_dbg_state_unset )
		this->passedTarget = false;

	if( this->passedTarget || ir->debug_state == ir_dbg_state_unset )
		return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if( ir->debug_state == ir_dbg_state_target ){
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_return* ir)
{
	if (this->passedTarget)
		return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_discard* ir)
{
	if (this->passedTarget)
		return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_if* ir)
{
    VPRINT(-2, "processSelection L:%s DbgSt:%i Passed:%i\n",
            FormatSourceRange(ir->yy_location).c_str(),
            ir->debug_state, this->passedTarget);

    if (this->passedTarget || ir->debug_state == ir_dbg_state_unset)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    switch (ir->debug_state_internal) {
        case ir_dbg_if_condition:
        case ir_dbg_if_condition_passed:
            if (ir->condition)
            	ir->condition->accept(this);
            break;
        case ir_dbg_if_if:
            if (ir->condition)
            	ir->condition->accept(this);
            this->visit(&ir->then_instructions);
            break;
        case ir_dbg_if_else:
            if (ir->condition)
            	ir->condition->accept(this);
            this->visit(&ir->else_instructions);
            break;
        case ir_dbg_if_unset:
        case ir_dbg_if_init:
        case ir_dbg_if_passed:
            break;
        default:
        	return true;
    }

    return false;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_loop* ir)
{
    VPRINT(-2, "processLoop L:%s DbgSt:%i Passed:%i\n",
    		FormatSourceRange(ir->yy_location).c_str(),
    		ir->debug_state, this->passedTarget);

    if (this->passedTarget || ir->debug_state == ir_dbg_state_unset)
        return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }


    switch (ir->debug_state_internal) {
        case ir_dbg_loop_unset:
        case ir_dbg_loop_qyr_init:
        case ir_dbg_loop_passed:
            break;
        case ir_dbg_loop_wrk_init:
        case ir_dbg_loop_qyr_test:
        	if( ir->from )
        		ir->from->accept(this);
            break;
        case ir_dbg_loop_wrk_test:
        case ir_dbg_loop_select_flow:
        case ir_dbg_loop_qyr_terminal:
        	if( ir->from )
        		ir->from->accept(this);
        	if( ir->counter )
        		ir->counter->accept(this);
            break;
        case ir_dbg_loop_wrk_body:
        	if( ir->from )
        		ir->from->accept(this);
        	if( ir->counter )
        		ir->counter->accept(this);
        	this->visit(&ir->body_instructions);
            break;
        case ir_dbg_loop_wrk_terminal:
        	if( ir->from )
        		ir->from->accept(this);
        	if( ir->counter )
        		ir->counter->accept(this);
        	if( ir->to )
        		ir->to->accept(this);
            break;
        default:
        	return true;
    }

    return false;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_loop_jump* ir)
{
	if (this->passedTarget)
		return false;

    addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

    if (ir->debug_state == ir_dbg_state_target) {
        this->passedTarget = true;
        return false;
    }

    return true;
}

bool ir_scopestack_traverse_visitor::visitIr(ir_list_dummy* ir)
{
	if (this->passedTarget)
		return false;

	addScopeToScopeStack( this->result.scopeStack, get_scope(ir) );

	if( ir->debug_state == ir_dbg_state_target )
		this->passedTarget = true;

    return false;
}
