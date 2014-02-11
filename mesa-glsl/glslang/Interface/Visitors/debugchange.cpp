/*
 * debugchange.cpp
 *
 *  Created on: 20.10.2013
 */

#include "debugchange.h"
#include "ShaderLang.h"
#include "glsl/list.h"
#include "glslang/Interface/IRScope.h"
#include "glslang/Interface/CodeTools.h"
#include "glsldb/utils/dbgprint.h"


static void get_block_cgbls(ShChangeableList* node_cgbl, exec_list* list,
							ir_debugchange_traverser_visitor* it)
{
	if ( !list->is_empty() ) {
    	ShChangeableList* list_cgbl = get_changeable_list( list );

    	int skip_pair = -1;
    	foreach_list( node, list ) {
			ir_instruction* const inst = (ir_instruction *)node;
			if (!list_iter_check(inst, skip_pair))
				continue;
			inst->accept( it );
			// copy changeables
			copyShChangeableList( list_cgbl, get_changeable_list( inst ) );
		}

    	copyShChangeableList(node_cgbl, list_cgbl);
    }
}


static void get_tokenized_block_cgbls(ShChangeableList* node_cgbl, ir_dummy* ir,
							ir_debugchange_traverser_visitor* it)
{
	if (!ir || !ir->next)
		return;

	int end_token = ir_dummy::pair_type(ir->dummy_type);
	if (end_token >= 0) {
		ShChangeableList* ir_cgbl = get_changeable_list(ir);
		foreach_node_safe(node, ir->next) {
			ir_instruction * const inst = (ir_instruction *)node;
			ir_dummy * const dm = inst->as_dummy();
			if (dm && end_token == dm->dummy_type)
				return;
			inst->accept(it);
			copyShChangeableList(ir_cgbl, get_changeable_list(inst));
		}
		copyShChangeableList(node_cgbl, ir_cgbl);
	}
}


bool ir_debugchange_traverser_visitor::visitIr(ir_variable* ir)
{
	ShChangeableList* node_cgbl = get_changeable_list(ir);
	//dumpShChangeableList(node_cgbl);

	if( ir->constant_value ){
		VPRINT(2, "(%s) changeDeclaration -> begin\n", FormatSourceRange(ir->yy_location).c_str());
		ir->constant_value->accept(this);
		copyShChangeableList(node_cgbl, get_changeable_list(ir->constant_value));
		//dumpShChangeableList(node_cgbl);
		VPRINT(2, "(%s) changeDeclaration -> end\n", FormatSourceRange(ir->yy_location).c_str());
	}else if(ir->constant_initializer) {
		VPRINT(2, "(%s) changeDeclaration -> begin\n", FormatSourceRange(ir->yy_location).c_str());
		ir->constant_initializer->accept(this);
        // copy the changeables of initialization
        copyShChangeableList(node_cgbl, get_changeable_list(ir->constant_initializer));
        //dumpShChangeableList(node_cgbl);
        VPRINT(2, "(%s) changeDeclaration -> end\n", FormatSourceRange(ir->yy_location).c_str());
    }else
    	return true;

    return false;
}

static void traverse_func_param( ir_variable* ir )
{
	if( !ir || (ir->data.mode != ir_var_function_in &&
			ir->data.mode != ir_var_const_in &&	/* check back with Magnus */
			ir->data.mode != ir_var_function_inout ) )
		return;

	ShChangeableList* node_cgbl = get_changeable_list(ir);
	ShVariable* var = NULL; // irToShVariable(ir);
	if( node_cgbl->numChangeables == 0 ){
		VPRINT( 2, "(%s) change FuncParam -> created %i %s\n",
				FormatSourceRange(ir->yy_location).c_str(), var->uniqueId, ir->name );
		addShChangeable( node_cgbl, createShChangeable( var->uniqueId ) );
	}else{
		VPRINT( 2, "(%s) changeFuncParam -> kept %i %s\n",
				FormatSourceRange(ir->yy_location).c_str(), var->uniqueId, ir->name );
	}
}

bool ir_debugchange_traverser_visitor::visitIr(ir_function_signature* ir)
{
	VPRINT( 2, "(%s) change function signature array\n", FormatSourceRange(ir->yy_location).c_str() );

    // do not parse an function if this was already done before
    // again this is due to the (*^@*$ function prototypes
	std::set<ir_function_signature*>::iterator iter = parsed_signatures.find(ir);
	if( iter != parsed_signatures.end() )
		return false;
	else
		parsed_signatures.insert(ir);

	ShChangeableList* node_cgbl = get_changeable_list( ir );
	ShChangeableList* node_param_cgbl = get_changeable_list( &ir->parameters );

	// TODO: Not sure, parameters must be processed
	foreach_list( node, &ir->parameters ){
		ir_instruction* inst = (ir_instruction *)node;
		traverse_func_param(inst->as_variable());
		// copy changeables
		ShChangeableList* inst_list = get_changeable_list( inst );
		copyShChangeableList( node_cgbl, inst_list );
		copyShChangeableList( node_param_cgbl, inst_list );
	}

	get_block_cgbls(node_cgbl, &ir->body, this);

    // copy parameters to local parameter list
    // Assumption: first child is responsible for parameters,
    //             check with similar comment above
    copyShChangeableList(get_changeable_paramerers_list(ir), node_param_cgbl);

	return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_expression* ir)
{
    VPRINT(2, "(%s) change Expression\n", FormatSourceRange(ir->yy_location).c_str());
    //dumpShChangeableList(node->getCgbList());
    this->deactivate();
    ShChangeableList* nlist = get_changeable_list(ir);
    int opc = ir->get_num_operands();
    this->depth++;
    for( int i = 0; i < opc; ++i ){
    	ir_instruction* op = ir->operands[i];
    	op->accept(this);
    	// copy the changeables of this operand
    	copyShChangeableList(nlist, get_changeable_list(op));
    	//dumpShChangeableList(nlist);
    }
    this->depth--;

    // It makes no sense to traverse operators again
    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_texture* ir)
{
	// TODO: texture
	return ir;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_swizzle* ir)
{
	VPRINT( 2, "(%s) vector swizzle\n", FormatSourceRange(ir->yy_location).c_str() );
	ShChangeableList* node_cgbl = get_changeable_list(ir);

	// process left branch first
	ir->val->accept(this);

	// then copy changeables
	// should be one or zero, otherwise we have a problem
	copyShChangeableList( node_cgbl, get_changeable_list(ir->val) );

	// remove all changeables not in scope (markus bug)
	{
		//ShChangeableList *cl = node->getCgbList();
		scopeList *sl = get_scope(ir);
		scopeList::iterator sit;

		for( int i = 0; i < node_cgbl->numChangeables; ){
			bool inScope = false;
			for( sit = sl->begin(); sit != sl->end(); sit++ ){
				if( ( *sit ) == node_cgbl->changeables[i]->id ){
					inScope = true;
					break;
				}
			}

			if( !inScope ){
				for( int j = i + 1; j < node_cgbl->numChangeables; j++ ){
					node_cgbl->changeables[j - 1] = node_cgbl->changeables[j];
				}
				node_cgbl->numChangeables--;
			}else{
				i++;
			}
		}
	}

	if( node_cgbl->numChangeables == 0 ){
		return false;
	}else if( node_cgbl->numChangeables == 1 ){
		ShChangeableIndex *cgbIdx;
		int index = 0;

		const unsigned swiz_idx[4] = {
				ir->mask.x, ir->mask.y, ir->mask.z, ir->mask.w
		};

		// Dunno, how original works, like this?
		for (unsigned i = 0; i < ir->mask.num_components; i++) {
			index += 1 << swiz_idx[i];
		}

		ShChangeableType type = SH_CGB_SWIZZLE;
		cgbIdx = createShChangeableIndex( type, index );
		addShIndexToChangeableList( node_cgbl, 0, cgbIdx );
	}else{
		dbgPrint( DBGLVL_ERROR, "DebugVar - swizzle to more than one changeable?\n" );
		exit( 1 );
	}
	return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_dereference_variable* ir)
{
    if (this->isActive()) {
    	ir_variable* node = ir->variable_referenced();
    	ShChangeableList* node_cgbl = get_changeable_list(node);
    	ShVariable* var = NULL; // findShVariableFromSource(node);
    	if (node_cgbl->numChangeables == 0) {
            VPRINT(2, "(%s) changeSymbol -> created %i %s\n",
                    FormatSourceRange(ir->yy_location).c_str(), var->uniqueId, node->name);
            addShChangeable(node_cgbl, createShChangeable(var->uniqueId));
        } else {
            VPRINT(2, "(%s) changeSymbol -> kept %i %s\n",
                    FormatSourceRange(node->yy_location).c_str(), var->uniqueId, node->name);
        }
    }

    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_dereference_array* ir)
{
	VPRINT( 2, "(%s) change Array ref\n", FormatSourceRange(ir->yy_location).c_str() );
	ShChangeableList* node_cgbls = get_changeable_list(ir);

	// process array branch first
	ir->array->accept(this);

	// then copy changeables
	// should be one or zero, otherwise we have a problem
	copyShChangeableList( node_cgbls, get_changeable_list(ir->array) );

	if( node_cgbls->numChangeables == 1 ){
		ShChangeableIndex *cgbIdx;
		ShChangeableType type = SH_CGB_ARRAY_DIRECT;
		ir_constant* c = ir->array_index->constant_expression_value();

		int index = -1;
		if( c ){
			if ( c->type->base_type == GLSL_TYPE_INT )
				index = c->get_int_component(0);
			else if ( c->type->base_type == GLSL_TYPE_UINT )
				index = c->get_uint_component(0);
		}

		cgbIdx = createShChangeableIndex( type, index );
		addShIndexToChangeableList( node_cgbls, 0, cgbIdx );
	}else if(node_cgbls->numChangeables){
		dbgPrint( DBGLVL_WARNING, "DebugVar - index to more than one changeable?\n" );
		/*exit(1);*/
	}

	return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_dereference_record* ir)
{
	VPRINT( 2, "(%s) change Record ref\n", FormatSourceRange(ir->yy_location).c_str() );
	ShChangeableList* node_cgbls = get_changeable_list(ir);

	// process array branch first
	ir->record->accept(this);

	// then copy changeables
	// should be one or zero, otherwise we have a problem
	copyShChangeableList( node_cgbls, get_changeable_list(ir->record) );

	if( node_cgbls->numChangeables == 1 ){
		ShChangeableType type = SH_CGB_STRUCT;
		ShChangeableIndex *cgbIdx = createShChangeableIndex( type, -1 );
		addShIndexToChangeableList( node_cgbls, 0, cgbIdx );
	}else if(node_cgbls->numChangeables){
		dbgPrint( DBGLVL_WARNING, "DebugVar - index to more than one changeable?\n" );
		/*exit(1);*/
	}

	return false;
}


bool ir_debugchange_traverser_visitor::visitIr(ir_assignment* ir)
{
	VPRINT(2, "(%s) change Assigment\n", FormatSourceRange(ir->yy_location).c_str());
	ShChangeableList* nlist = get_changeable_list(ir);

    // process left branch actively
    if (ir->lhs) {
        VPRINT(2, "===== left ============================\n");

        this->activate();
        ir->lhs->accept(this);
        this->deactivate();

        // copy the changeables of left branch
        copyShChangeableList(nlist, get_changeable_list(ir->lhs));
        //dumpShChangeableList(nlist);
        VPRINT(2, "=======================================\n");
    }

    // process right branch passively
    if (ir->rhs) {
        VPRINT(2, "===== right ===========================\n");

        this->deactivate();
        ir->rhs->accept(this);

        // copy the changeables of right branch
        copyShChangeableList(nlist, get_changeable_list(ir->rhs));
        //dumpShChangeableList(nlist);
        VPRINT(2, "=======================================\n");
    }

    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_constant* ir)
{
	// TODO: constant
	return ir;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_call* ir)
{
	VPRINT( 2, "(%s) change Call\n", FormatSourceRange(ir->yy_location).c_str() );

	// only user defined functions can have out/inout parameters
	if( ir->callee->is_builtin() )
		return false;

	// changed variables are all out/inout parameters and
	// all changes made in the body of this function

	ir_function_signature* funcDec = ir->callee;

	// if function is not already parsed, do it now
	// this is neccessary due to $*&T# function prototypes
	funcDec->accept(this);

	// now the function should be aware of it's changeables
	copyShChangeableList( get_changeable_list( ir ), get_changeable_list( funcDec ) );

	// for parameters we need more care
	// iterate over all parameters simultaneously in the function call
	// and the function declaration
	// Assumption: 1. We assume that there is a parameter sequence in the
	//                function declaration
	//             2. Further more it's assumed that both sequences have
	//                the same amount of parameters.
	//             Both should hold pretty easy, as we searched for a
	//             function with the same parameters. So we trust ourselves,
	//             even if we know we should not (:

	ShChangeableList* node_cgbl = get_changeable_list( ir );
	ShChangeableList* node_cgbl_param = get_changeable_paramerers_list( ir );

	foreach_two_lists(pC, &ir->actual_parameters, pD, &funcDec->parameters){
		// check if parameter is of interest
		ir_variable* dir = ((ir_instruction*)pD)->as_variable();
		if( !dir )
			continue;

		ir_instruction* cir = (ir_instruction*)pC;
		ShChangeableList* cir_cgbl = get_changeable_list( cir );
		if( dir->data.mode == ir_var_function_out || dir->data.mode == ir_var_function_inout
				|| dir->data.mode == ir_var_shader_out ){ // FIXME: Not sure about shader_out
			this->activate();
			cir->accept(this);
			this->deactivate();

			// add these parameters to parameter-list
			copyShChangeableList( node_cgbl_param, cir_cgbl );
		}else{
			this->deactivate();
			cir->accept(this);

			// these can be added direcly to the changeables
			copyShChangeableList( node_cgbl, cir_cgbl );
		}
	}

	// Add parameter to changeables, since they need to be passed to parents
	copyShChangeableList( node_cgbl, node_cgbl_param );
	return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_return* ir)
{
	if( ir->value ) {
        // simply traverse and copy result
        this->deactivate();
        ir->value->accept(this);
        copyShChangeableList(get_changeable_list( ir ),
        					 get_changeable_list( ir->value ));
    }
    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_discard* ir)
{
	if( ir->condition ) {
        // simply traverse and copy result
        this->deactivate();
        ir->condition->accept(this);
        copyShChangeableList(get_changeable_list( ir ),
        					 get_changeable_list( ir->condition ));
    }
    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_if* ir)
{
	ShChangeableList* node_cgbl = get_changeable_list(ir);

	// just traverse and copy changeables all together
    this->deactivate();
    ir->condition->accept(this);
    copyShChangeableList(node_cgbl, get_changeable_list(ir->condition));

    get_block_cgbls(node_cgbl, &ir->then_instructions, this);
    get_block_cgbls(node_cgbl, &ir->else_instructions, this);

    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_loop* ir)
{
	ShChangeableList* node_cgbl = get_changeable_list( ir );
	// just traverse and copy changeables all together
	this->deactivate();

	// visit optional initialization
	get_tokenized_block_cgbls(node_cgbl, ir->debug_init, this);

	// visit test, this should not change the changeables, but to be sure
	ir_rvalue* check = ir->condition();
	if (check){
		check->accept(this);
		copyShChangeableList(node_cgbl, get_changeable_list(check));
	}

	// visit optional terminal, this cannot change the changeables either
	get_tokenized_block_cgbls(node_cgbl, ir->debug_terminal, this);

	// visit body
	get_block_cgbls( node_cgbl, &ir->body_instructions, this );

	return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_loop_jump*)
{
    return false;
}

bool ir_debugchange_traverser_visitor::visitIr(ir_dummy *)
{
	return false;
}
