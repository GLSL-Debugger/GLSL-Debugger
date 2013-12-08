/*
 * traverse.cpp
 *
 *  Created on: 12.10.2013
 */

#include "traverse.h"
#include "ir.h"
#include "glsl/list.h"
#include "glslang/Interface/MShader.h"
#include <assert.h>


void ir_traverse_visitor::visit(ir_variable* ir)
{
	bool visit = true;

	if( this->preVisit )
		visit = this->visitIr( ir );

	if( visit ){
		++this->depth;
		if( ir->constant_value )
			ir->constant_value->accept( this );
		else if( ir->constant_initializer )
			ir->constant_initializer->accept( this );
		--this->depth;
	}

	if( visit && ( this->postVisit || this->debugVisit ) )
		this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_function_signature* ir)
{
	bool visit = true;

	if( this->preVisit )
		visit = this->visitIr( ir );

	if( visit )
		this->visit( &ir->parameters );

	// FIXME: must we check for visit here, or traverse the body again?
	++this->depth;
	this->visit( &ir->body );
	--this->depth;

	if( this->postVisit )
		visit = this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_function* ir)
{
	bool visit = true;

	if( this->preVisit )
		visit = this->visitIr( ir );

	bool found_non_builtin_proto = false;
	foreach_iter(exec_list_iterator, iter, *ir) {
		ir_function_signature * const sig = (ir_function_signature *)iter.get();
		if( !sig->is_builtin() )
			found_non_builtin_proto = true;
	}

	if( visit && found_non_builtin_proto )
		this->visit( &ir->signatures );

	if( visit && ( this->postVisit || this->debugVisit ) )
		this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_expression* ir)
{
	bool visit = true;

	// visit the node before children if pre-visiting.
	if( ( this->preVisit || this->debugVisit ) )
		visit = this->visitIr( ir );

	// Visit the children, in the right order.
	if( visit ){
		++this->depth;
		int operands = ir->get_num_operands();
		for( int i = 0; i < operands; ++i )
			if( ir->operands[i] )
				ir->operands[i]->accept( this );
		--this->depth;
	}

	// Visit the node after the children, if requested and the traversal
	// hasn't been cancelled yet.
	if( visit && this->postVisit )
		visit = this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_texture* ir)
{
	bool visit = true;
	if( ( this->preVisit || this->debugVisit ) )
		visit = this->visitIr( ir );

	if( visit ){
		// sampler
		ir->sampler->accept( this );

		// texture coordinate
		ir->coordinate->accept( this );

		// lod bias
		if( ir->op == ir_txb )
			ir->lod_info.bias->accept( this );

		// lod
		if( ir->op == ir_txl )
			ir->lod_info.lod->accept( this );

		// grad
		if( ir->op == ir_txd ){
			ir->lod_info.grad.dPdx->accept( this );
			ir->lod_info.grad.dPdy->accept( this );
		}
	}

	if( visit && this->postVisit )
		visit = this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_swizzle* ir)
{
	bool visit = true;
	if( ( this->preVisit || this->debugVisit ) )
		visit = this->visitIr( ir );

	if( visit )
		ir->val->accept( this );

	if( visit && this->postVisit )
		visit = this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_dereference_variable* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_dereference_array* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_dereference_record* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_assignment* ir)
{
	bool visit = true;

	// visit the node before children if pre-visiting.
	if( ( this->preVisit || this->debugVisit ) )
		visit = this->visitIr( ir );

	if( visit ){
		++this->depth;
		if( ir->lhs )
			ir->lhs->accept( this );

		ir_expression* rhsOp = ir->rhs->as_expression();
		if( rhsOp && rhsOp->operation == ir_triop_vector_insert ){
			ir_dereference_variable* lhsDeref = ir->lhs->as_dereference_variable();
			ir_dereference_variable* rhsDeref = rhsOp->operands[0]->as_dereference_variable();
			// skip assignment if lhs and rhs would be the same
			if( lhsDeref && rhsDeref && lhsDeref->var != rhsDeref->var )
				rhsOp->operands[0]->accept( this );
			rhsOp->operands[2]->accept( this );
			rhsOp->operands[1]->accept( this );
		}else{
			if( ir->condition )
				ir->condition->accept( this );
			ir->rhs->accept( this );
		}

		--this->depth;
	}

	// Visit the node after the children, if requested and the traversal
	// hasn't been cancelled yet.
	//
	if( visit && this->postVisit )
		visit = this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_constant* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_call* ir)
{
	bool visit = true;
	if( this->preVisit )
		visit = this->visitIr( ir );

	if( visit ){
		++this->depth;
		foreach_iter(exec_list_iterator, iter, *ir) {
			ir_instruction * const inst = (ir_instruction *)iter.get();
			inst->accept( this );
		}
		--this->depth;
	}

	if( visit && ( this->postVisit || this->debugVisit ) )
		this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_return* ir)
{
    bool visit = true;

    if (this->preVisit)
        visit = this->visitIr( ir );

    if (visit && ir->value) {
        ++this->depth;
        ir->value->accept(this);
        --this->depth;
    }

    if (visit && (this->postVisit || this->debugVisit))
    	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_discard* ir)
{
    bool visit = true;

    if (this->preVisit)
        visit = this->visitIr( ir );

    if (visit && ir->condition) {
        ++this->depth;
        ir->condition->accept(this);
        --this->depth;
    }

    if (visit && (this->postVisit || this->debugVisit))
    	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_if* ir)
{
	bool visit = true;

	if( this->debugVisit ){
		/* Visit node for optional check of condition */
		if( ir->debug_state_internal == ir_dbg_if_unset
				|| ir->debug_state_internal == ir_dbg_if_init
				|| ir->debug_state_internal == ir_dbg_if_condition_passed )
			visit = this->visitIr( ir );

		if( visit && ir->debug_state_internal == ir_dbg_if_condition ){
			++this->depth;
			ir->condition->accept( this );
			--this->depth;
		}

		/* Visit node again for choosing debugged branch */
		if( ir->debug_state_internal == ir_dbg_if_condition )
			visit = this->visitIr( ir );

		if( visit ){
			if( ir->debug_state_internal == ir_dbg_if_then )
				this->visit( &ir->then_instructions );
			if( ir->debug_state_internal == ir_dbg_if_else )
				this->visit( &ir->else_instructions );
		}

		/* Visit node again for preparation of pass */
		if( ir->debug_state_internal == ir_dbg_if_then
				|| ir->debug_state_internal == ir_dbg_if_else )
			visit = this->visitIr( ir );

	}else{
		if( ( this->preVisit || this->debugVisit ) )
			visit = this->visitIr( ir );

		if( visit ){
			++this->depth;
			ir->condition->accept( this );
			this->visit( &ir->then_instructions );
			this->visit( &ir->else_instructions );
			--this->depth;
		}

		if( visit && ( this->postVisit || this->debugVisit ) )
			this->visitIr( ir );
	}
}

void ir_traverse_visitor::visit(ir_loop* ir)
{
	bool visit = true;

	if( this->debugVisit ){
		/* Visit node first */
		if( ir->debug_state_internal == ir_dbg_loop_unset
				|| ir->debug_state_internal == ir_dbg_loop_qyr_init )
			visit = this->visitIr( ir );

		if( ir->debug_state_internal == ir_dbg_loop_wrk_init ){
			++this->depth;
			if( ir->from )
				ir->from->accept( this );
			--this->depth;
		}

		/* Visit node again for test */
		if( ir->debug_state_internal == ir_dbg_loop_wrk_init
				|| ir->debug_state_internal == ir_dbg_loop_qyr_test )
			visit = this->visitIr( ir );

		if( visit && ir->debug_state_internal == ir_dbg_loop_wrk_test ){
			++this->depth;
			if( ir->counter )
				ir->counter->accept( this );
			--this->depth;
		}

		/* Visit node again for flow selection */
		if( ir->debug_state_internal == ir_dbg_loop_wrk_test
				|| ir->debug_state_internal == ir_dbg_loop_select_flow )
			visit = this->visitIr( ir );

		if( visit && ir->debug_state_internal == ir_dbg_loop_wrk_body ){
			++this->depth;
			this->visit( &ir->body_instructions );
			--this->depth;
		}

		/* Visit node again for terminal */
		if( ir->debug_state_internal == ir_dbg_loop_wrk_body
				|| ir->debug_state_internal == ir_dbg_loop_qyr_terminal )
			visit = this->visitIr( ir );

		if( visit && ir->debug_state_internal == ir_dbg_loop_wrk_terminal ){
			++this->depth;
			if( ir->to )
				ir->to->accept( this );
			--this->depth;
		}

		/* Visit node again for terminal */
		if( ir->debug_state_internal == ir_dbg_loop_wrk_terminal )
			visit = this->visitIr( ir );

	}else{
		if( ( this->preVisit || this->debugVisit ) )
			visit = this->visitIr( ir );

		if( visit ){
			++this->depth;
			if( ir->counter )
				ir->counter->accept( this );
			this->visit( &ir->body_instructions );
			if( ir->from )
				ir->from->accept( this );
			if( ir->to )
				ir->to->accept( this );
			--this->depth;
		}

		if( visit && this->postVisit )
			this->visitIr( ir );
	}
}

void ir_traverse_visitor::visit(ir_loop_jump* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_typedecl_statement *)
{
    // This is additional type to output struct declarations. Do nothing.
}

void ir_traverse_visitor::visit(ir_emit_vertex *ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_end_primitive *ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(ir_list_dummy* ir)
{
	this->visitIr( ir );
}

void ir_traverse_visitor::visit(exec_list* instructions)
{
	foreach_iter(exec_list_iterator, iter, *instructions) {
		ir_instruction * const inst = (ir_instruction *)iter.get();
		if( !depth && skipInternal && inst->ir_type == ir_type_variable ){
			ir_variable *var = inst->as_variable();
			if( ( strstr( var->name, "gl_" ) == var->name ) && !var->invariant )
				continue;
		}
		inst->accept( this );
	}
}
