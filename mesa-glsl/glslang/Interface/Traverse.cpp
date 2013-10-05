/*
 * Traverse.cpp
 *
 *  Created on: 13.09.2013
 */

//
// Traverse the intermediate representation tree, and
// call a node type specific function for each node.
// Done recursively through the member function Traverse().
// Node types can be skipped if their function to call is 0,
// but their subtree will still be traversed.
// Nodes with children can have their whole subtree skipped
// if preVisit is turned on and the type specific function
// returns false.
//
// preVisit, postVisit, debugVisit and rightToLeft control
// what order nodes are visited in.
//

#include "Program.h"
#include "glsl/ir.h"
#include "glsl/list.h"
#include "intermediate.h"

//
// Traversal functions for terminals are straighforward....
//
void TraverseList( exec_list* list, TIntermTraverser* it )
{
	foreach_iter( exec_list_iterator, iter, *list ){
	      ir_instruction* ir = (ir_instruction *)iter.get();
	      Traverse( ir, it );
	}
}

void Traverse( ir_instruction* node, TIntermTraverser* it )
{
	if( ! node )
		return;

	bool visit = true;

#define visit(name, var) \
	if( it->visit##name ) visit = it->visit##name(var, it);

//	node->print();
//	printf("\n");
	switch( node->ir_type ){
		case ir_type_unset:
			break;
		case ir_type_constant:
		{
			ir_constant* c = node->as_constant();
			//visit(Declaration, c);
			break;
		}
		case ir_type_variable:
		{
			// Declaration
			ir_variable* v = node->as_variable();
			visit(Declaration, v);
			break;
		}
		case ir_type_function:
		{
			ir_function* f = node->as_function();
			visit(FuncDeclaration, f)
			if( visit )
				TraverseList( &f->signatures, it );
			break;
		}
		case ir_type_function_signature:
		{
			ir_function_signature* fs = node->as_function_signature();
			visit(Aggregate, fs)
			if( visit ){
				foreach_iter( exec_list_iterator, iter, fs->parameters ){
					ir_variable* ir = ((ir_instruction *)iter.get())->as_variable();
					if( !ir ){
						printf("Suddenly func parameter is not only ir_variable, it is %d. Please report it",
								((ir_instruction*)iter.get())->ir_type);
						exit(1);
					}
					visit(FuncParam, ir)
				}
			}
			// FIXME: Does it need to be processed in aggregate? Now only parameters is processed
			++it->depth;
			TraverseList( &fs->body, it );
			--it->depth;
			break;
		}
		case ir_type_assignment:
		{
			ir_assignment* a = node->as_assignment();
			visit(Assignment, a)
			if( visit ){
				++it->depth;
				if( a->condition ){
					printf("WHOA! A conditional assignment. Not sure how it must be processed, show your example, please.");
					Traverse( a->condition, it );
				}
				if (it->rightToLeft) {
					if( a->rhs )
						Traverse( a->rhs, it );
					if( a->lhs )
						Traverse( a->lhs, it );
				} else {
					if( a->lhs )
						Traverse( a->lhs, it );
					if( a->rhs )
						Traverse( a->rhs, it );
				}
				--it->depth;
			}
			break;
		}
		case ir_type_dereference_variable:
		{
			ir_dereference_variable* dv = node->as_dereference_variable();
			Traverse( dv->variable_referenced(), it );
			break;
		}
		case ir_type_expression:
		{
			ir_expression* e = node->as_expression();
			int operands = e->get_num_operands();
			++it->depth;
			if (it->rightToLeft) {
				for( int i = operands; i > 0; --i )
					if( e->operands[i] )
						Traverse( e->operands[i], it );
			} else {
				for( int i = 0; i < operands; ++i )
					if( e->operands[i] )
						Traverse( e->operands[i], it );
			}
			--it->depth;
			if( e->operation < ir_last_unop ){
				visit(Unary, e);
			}else if( e->operation < ir_last_binop ){
				visit(Binary, e);
			}else if( e->operation < ir_last_triop ){
				printf("TODO: Ternary operation not processed.");
				//visit(Unary, e);
			}else{
				printf("TODO: Quad operation not processed.");
				//visit(Unary, e);
			}
			break;
		}
		default:
			printf("unsupported type\n");
			break;
	}

#undef visit

//
//
//
//
//
//
//		   ir_type_call,
//		   ir_type_constant,
//		   ir_type_dereference_array,
//		   ir_type_dereference_record,
//		   ir_type_dereference_variable,
//		   ir_type_discard,
//		   ir_type_if,
//		   ir_type_loop,
//		   ir_type_loop_jump,
//		   ir_type_return,
//		   ir_type_swizzle,
//		   ir_type_texture,
//		   ir_type_max
//
//	// Symbol
//    if (it->visitSymbol)
//        it->visitSymbol(this, it);
//
//    // ConstantUnion
//    if (it->visitConstantUnion)
//        it->visitConstantUnion(this, it);
//
//
////
//// Traverse a binary node.
////
//{
//    bool visit = true;
//
//    //
//    // visit the node before children if pre-visiting.
//    //
//    if ((it->preVisit || it->debugVisit) && it->visitBinary)
//        visit = it->visitBinary(true, this, it);
//
//    //
//    // Visit the children, in the right order.
//    //
//    if (visit) {
//        ++it->depth;
//        if (it->rightToLeft) {
//            if (right)
//                right->traverse(it);
//            if (left)
//                left->traverse(it);
//        } else {
//            if (left)
//                left->traverse(it);
//            if (right)
//                right->traverse(it);
//        }
//        --it->depth;
//    }
//
//    //
//    // Visit the node after the children, if requested and the traversal
//    // hasn't been cancelled yet.
//    //
//    if (visit && it->postVisit && it->visitBinary)
//        it->visitBinary(false, this, it);
//}
//
////
//// Traverse a unary node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->preVisit && it->visitUnary)
//        visit = it->visitUnary(true, this, it);
//
//    if (visit) {
//        ++it->depth;
//        operand->traverse(it);
//        --it->depth;
//    }
//
//    if (visit && (it->postVisit || it->debugVisit) && it->visitUnary)
//        it->visitUnary(false, this, it);
//}
//
////
//// Traverse an aggregate node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->preVisit && it->visitAggregate)
//        visit = it->visitAggregate(true, this, it);
//
//    if (visit) {
//        ++it->depth;
//
//        TIntermSequence::iterator sit;
//        if (it->rightToLeft) {
//            sit = sequence.end();
//            while (sit != sequence.begin()) {
//                --sit;
//                (*sit)->traverse(it);
//            }
//        } else {
//            for (sit = sequence.begin(); sit != sequence.end(); ++sit) {
//                (*sit)->traverse(it);
//            }
//        }
//
//        --it->depth;
//    }
//
//    if (visit && (it->postVisit || it->debugVisit) && it->visitAggregate)
//        it->visitAggregate(false, this, it);
//}
//
////
//// Traverse a selection node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->debugVisit) {
//        /* Visit node for optional check of condition */
//        if (it->visitSelection &&
//            (dbgState == DBG_STATE_SELECTION_UNSET ||
//             dbgState == DBG_STATE_SELECTION_INIT  ||
//             dbgState == DBG_STATE_SELECTION_CONDITION_PASSED)) {
//            visit = it->visitSelection(true, this, it);
//        }
//
//        if (visit &&
//            dbgState == DBG_STATE_SELECTION_CONDITION) {
//            ++it->depth;
//            condition->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for choosing debugged branch */
//        if (it->visitSelection &&
//            dbgState == DBG_STATE_SELECTION_CONDITION) {
//            visit = it->visitSelection(true, this, it);
//        }
//
//        if (visit) {
//            if (trueBlock && dbgState == DBG_STATE_SELECTION_IF)
//                trueBlock->traverse(it);
//            if (falseBlock && dbgState == DBG_STATE_SELECTION_ELSE)
//                falseBlock->traverse(it);
//        }
//
//        /* Visit node again for preparation of pass */
//        if (it->visitSelection &&
//            (dbgState == DBG_STATE_SELECTION_IF ||
//             dbgState == DBG_STATE_SELECTION_ELSE)) {
//            visit = it->visitSelection(true, this, it);
//        }
//
//    } else {
//        if (it->preVisit && it->visitSelection)
//            visit = it->visitSelection(true, this, it);
//
//        if (visit) {
//            ++it->depth;
//            if (it->rightToLeft) {
//                if (falseBlock)
//                    falseBlock->traverse(it);
//                if (trueBlock)
//                    trueBlock->traverse(it);
//                condition->traverse(it);
//            } else {
//                condition->traverse(it);
//                if (trueBlock)
//                    trueBlock->traverse(it);
//                if (falseBlock)
//                    falseBlock->traverse(it);
//            }
//            --it->depth;
//        }
//
//        if (visit && it->postVisit && it->visitSelection)
//            it->visitSelection(false, this, it);
//    }
//}
//
////
//// Traverse a switch node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->debugVisit) {
//        /* Visit node for optional check of condition */
//        if (it->visitSwitch &&
//            (dbgState == DBG_STATE_SELECTION_UNSET ||
//             dbgState == DBG_STATE_SELECTION_INIT  ||
//             dbgState == DBG_STATE_SELECTION_CONDITION_PASSED)) {
//            visit = it->visitSwitch(true, this, it);
//        }
//
//        if (visit &&
//            condition &&
//            dbgState == DBG_STATE_SELECTION_CONDITION) {
//            ++it->depth;
//            condition->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for choosing debugged branch */
//        if (it->visitSwitch &&
//            dbgState == DBG_STATE_SELECTION_CONDITION) {
//            visit = it->visitSwitch(true, this, it);
//        }
//
//#if 0
//        if (visit) {
//            if (trueBlock && dbgState == DBG_STATE_SELECTION_IF)
//                trueBlock->traverse(it);
//            if (falseBlock && dbgState == DBG_STATE_SELECTION_ELSE)
//                falseBlock->traverse(it);
//        }
//
//        /* Visit node again for preparation of pass */
//        if (it->visitSelection &&
//            (dbgState == DBG_STATE_SELECTION_IF ||
//             dbgState == DBG_STATE_SELECTION_ELSE)) {
//            visit = it->visitSelection(true, this, it);
//        }
//#endif
//    } else {
//        if (it->preVisit && it->visitSwitch)
//            visit = it->visitSwitch(true, this, it);
//
//        if (visit && caseList) {
//            ++it->depth;
//            caseList->traverse(it);
//            --it->depth;
//        }
//
//        if (visit && it->postVisit && it->visitSwitch)
//            it->visitSwitch(false, this, it);
//    }
//}
//
////
//// Traverse a case node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->preVisit && it->visitCase)
//        visit = it->visitCase(true, this, it);
//
//    if (visit && expression) {
//        ++it->depth;
//        expression->traverse(it);
//        --it->depth;
//    }
//
//    if (visit && caseBody) {
//        ++it->depth;
//        caseBody->traverse(it);
//        --it->depth;
//    }
//
//    if (visit && (it->postVisit || it->debugVisit) && it->visitCase)
//        it->visitCase(false, this, it);
//}
//
////
//// Traverse a loop node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->debugVisit) {
//        /* Visit node first */
//        if (it->visitLoop &&
//            ( dbgState == DBG_STATE_LOOP_UNSET ||
//              dbgState == DBG_STATE_LOOP_QYR_INIT) ) {
//            visit = it->visitLoop(true, this, it);
//        }
//
//        if (visit &&
//            dbgState == DBG_STATE_LOOP_WRK_INIT) {
//            ++it->depth;
//            if (init) init->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for test */
//        if (it->visitLoop &&
//            (dbgState == DBG_STATE_LOOP_WRK_INIT ||
//             dbgState == DBG_STATE_LOOP_QYR_TEST) ) {
//            visit = it->visitLoop(true, this, it);
//        }
//
//        if (visit &&
//            dbgState == DBG_STATE_LOOP_WRK_TEST) {
//            ++it->depth;
//            if (test) test->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for flow selection */
//        if (it->visitLoop &&
//            (dbgState == DBG_STATE_LOOP_WRK_TEST ||
//             dbgState == DBG_STATE_LOOP_SELECT_FLOW)) {
//            visit = it->visitLoop(true, this, it);
//        }
//
//        if (visit &&
//            dbgState == DBG_STATE_LOOP_WRK_BODY) {
//            ++it->depth;
//            if (body) body->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for terminal */
//        if (it->visitLoop &&
//            (dbgState == DBG_STATE_LOOP_WRK_BODY ||
//             dbgState == DBG_STATE_LOOP_QYR_TERMINAL)) {
//            visit = it->visitLoop(true, this, it);
//        }
//
//        if (visit &&
//            dbgState == DBG_STATE_LOOP_WRK_TERMINAL) {
//            ++it->depth;
//            if (terminal) terminal->traverse(it);
//            --it->depth;
//        }
//
//        /* Visit node again for terminal */
//        if (it->visitLoop &&
//            dbgState == DBG_STATE_LOOP_WRK_TERMINAL) {
//            visit = it->visitLoop(true, this, it);
//        }
//#if 0
//        /* Visit node again for choosing debugged branch */
//        if (it->visitSelection &&
//            dbgState == DBG_STATE_SELECTION_CONDITION) {
//            visit = it->visitSelection(true, this, it);
//        }
//
//        if (visit) {
//            if (trueBlock && dbgState == DBG_STATE_SELECTION_IF)
//                trueBlock->traverse(it);
//            if (falseBlock && dbgState == DBG_STATE_SELECTION_ELSE)
//                falseBlock->traverse(it);
//        }
//
//        /* Visit node again for preparation of pass */
//        if (it->visitSelection &&
//            (dbgState == DBG_STATE_SELECTION_IF ||
//             dbgState == DBG_STATE_SELECTION_ELSE)) {
//            visit = it->visitSelection(true, this, it);
//        }
//
//
//
//                break;
//
//        }
//#endif
//    } else {
//
//        if ((it->preVisit || it->debugVisit) && it->visitLoop)
//            visit = it->visitLoop(true, this, it);
//
//        if (visit) {
//            ++it->depth;
//            if (it->rightToLeft) {
//                if (init)
//                    init->traverse(it);
//                if (terminal)
//                    terminal->traverse(it);
//                if (body)
//                    body->traverse(it);
//                if (test)
//                    test->traverse(it);
//            } else {
//                if (test)
//                    test->traverse(it);
//                if (body)
//                    body->traverse(it);
//                if (terminal)
//                    terminal->traverse(it);
//                if (init)
//                    init->traverse(it);
//            }
//            --it->depth;
//        }
//
//        if (visit && it->postVisit && it->visitLoop)
//            it->visitLoop(false, this, it);
//    }
//}
//
////
//// Traverse a branch node.  Same comments in binary node apply here.
////
//{
//    bool visit = true;
//
//    if (it->preVisit && it->visitBranch)
//        visit = it->visitBranch(true, this, it);
//
//    if (visit && expression) {
//        ++it->depth;
//        expression->traverse(it);
//        --it->depth;
//    }
//
//    if (visit && (it->postVisit || it->debugVisit) && it->visitBranch)
//        it->visitBranch(false, this, it);
//}
//
//
//
//// Specification
//{
//    bool visit = true;
//
//    if (it->preVisit && it->visitSpecification)
//        visit = it->visitSpecification(this, it);
//
//    if (visit) {
//        ++it->depth;
//        if (parameter)
//            parameter->traverse(it);
//        if (instances)
//            instances->traverse(it);
//        --it->depth;
//    }
//
//    if (visit && (it->postVisit || it->debugVisit) && it->visitSpecification)
//        visit = it->visitSpecification(this, it);
//
//}
//
//
//// Dummy
//{
//    if (it->visitDummy) {
//        it->visitDummy(this, it);
//    }
//}

}



