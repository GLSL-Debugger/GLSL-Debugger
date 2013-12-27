#include "CodeTools.h"

#include "glsl/list.h"
#include "glsl/glsl_symbol_table.h"
#include "Visitors/sideeffects.h"
#include "glsldb/utils/dbgprint.h"


static void makeMangledName( const struct glsl_type* type, std::string& mangledName )
{
	if( !type )
		return;

    if ( type->is_matrix() ) {
        mangledName += 'm';
    } else if ( type->is_vector() ) {
        mangledName += 'v';
    }

    switch (type->base_type) {
    	case GLSL_TYPE_UINT:    mangledName += 'u';      break;
    	case GLSL_TYPE_INT:		mangledName += 'i';      break;
    	case GLSL_TYPE_FLOAT:	mangledName += 'f';      break;
    	case GLSL_TYPE_BOOL:	mangledName += 'b';      break;
    	case GLSL_TYPE_SAMPLER:
    	{
    		mangledName += 's';
    		if( type->sampler_type == GLSL_TYPE_INT )
    			mangledName += 'I';
    		else if( type->sampler_type == GLSL_TYPE_UINT )
    			mangledName += 'U';

    		if( type->sampler_shadow )
    			mangledName += 'S';

    		switch( type->sampler_dimensionality ){
    			case GLSL_SAMPLER_DIM_1D:	mangledName += '1';	break;
    			case GLSL_SAMPLER_DIM_2D: 	mangledName += '2';	break;
    			case GLSL_SAMPLER_DIM_3D:	mangledName += '3';	break;
    			case GLSL_SAMPLER_DIM_CUBE: mangledName += 'C';	break;
    			case GLSL_SAMPLER_DIM_RECT: mangledName += 'R';	break;
    			case GLSL_SAMPLER_DIM_BUF:	mangledName += 'B';	break;
    			default:					mangledName += "Unknown";	break;


    		}
//    		   GLSL_SAMPLER_DIM_EXTERNAL,
//    		   GLSL_SAMPLER_DIM_MS
//    	    case EbtSampler1DArray:     mangledName += "sA1";    break;  // EXT_gpu_shader4
//    	    case EbtISampler1DArray:    mangledName += "sIA1";   break;  // EXT_gpu_shader4
//    	    case EbtUSampler1DArray:    mangledName += "sUA1";   break;  // EXT_gpu_shader4
//    	    case EbtSampler2DArray:     mangledName += "sA2";    break;  // EXT_gpu_shader4
//    	    case EbtISampler2DArray:    mangledName += "sIA2";   break;  // EXT_gpu_shader4
//    	    case EbtUSampler2DArray:    mangledName += "sUA2";   break;  // EXT_gpu_shader4
//    	    case EbtSampler1DArrayShadow:     mangledName += "sAS1";    break;  // EXT_gpu_shader4
//    	    case EbtSampler2DArrayShadow:     mangledName += "sAS2";    break;  // EXT_gpu_shader4
    		break;
    	}
    	case GLSL_TYPE_STRUCT:
    	{
    		mangledName += "struct-" + std::string(type->name);
    		for (unsigned int i = 0; i < type->length; ++i) {
    			mangledName += '-';
    			makeMangledName( type->fields.structure[i].type, mangledName );
    		}
    		break;
    	}
    	case GLSL_TYPE_VOID:	mangledName += "void"; break;
    	/* GLSL_TYPE_INTERFACE, GLSL_TYPE_ARRAY, GLSL_TYPE_ERROR */
    	default:	break;
    }

    if ( type->is_matrix() ) {
        mangledName += static_cast<char>('0' + type->vector_elements);
        mangledName += 'x';
        mangledName += static_cast<char>('0' + type->matrix_columns);
    } else {
        mangledName += static_cast<char>('0' + type->length);
    }

    if ( type->is_array() ) {
        char buf[100];
        sprintf(buf, "%d", type->length);
        mangledName += '[';
        mangledName += buf;
        mangledName += ']';
    }
}


std::string getMangledName(ir_function_signature* fs)
{
	std::string mname = "";

	if (!fs)
		return mname;

	mname += std::string(fs->function_name()) + "(";

	// Assume function has only one signature
	foreach_iter( exec_list_iterator, param, fs->parameters ) {
		ir_variable* v = ((ir_instruction*) param.get())->as_variable();
		if (v)
			makeMangledName(v->type, mname);
	}

	return mname;
}


char* getFunctionName(const char* manglName)
{
    size_t namelength;
    char *name;

    if (strchr(manglName, '(')) {
        namelength = strchr(manglName, '(') - manglName;
    } else {
        namelength = strlen(manglName);
    }

    if (!(name = (char*) malloc((namelength+1)*sizeof(char)))) {
        dbgPrint(DBGLVL_ERROR, "CodeTools - Not enough memory for name in getFunctionName %s\n",
        		manglName);
        exit(1);
    }
    strncpy(name, manglName, namelength);
    name[namelength] = '\0';
    return name;
}

ir_function* getFunctionBySignature(const char *sig, struct gl_shader* shader)
// Assumption: 1. roots hold all function definitions.
//                for single file shaders this should hold.
// Todo: Add solution for multiple files compiled in one shader.
{
	VPRINT(3, "Search for function [%s]\n", sig);
	return shader->symbols->get_function(sig);
}

//bool isChildofMain(TIntermNode *node, TIntermNode *root)
//{
//    TIntermNode *main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, root);
//
//    if (!main) {
//        dbgPrint(DBGLVL_ERROR, "CodeTools - could not find main function\n");
//        exit(1);
//    }
//
//    TIntermAggregate *aggregate;
//
//    if (!(aggregate = main->getAsAggregate())) {
//        dbgPrint(DBGLVL_ERROR, "CodeTools - main is not Aggregate\n");
//        exit(1);
//    }
//
//    TIntermSequence sequence = aggregate->getSequence();
//    TIntermSequence::iterator sit;
//
//    for(sit = sequence.begin(); sit != sequence.end(); sit++) {
//        if (*sit == node) {
//            return true;
//        }
//    }
//
//    return false;
//}


//bool isPartofNode(TIntermNode *target, TIntermNode *node)
//{
//    if (node == target) return true;
//
//    if (node->getAsLoopNode()) {
//        if (node->getAsLoopNode()->getTest() && isPartofNode(target, node->getAsLoopNode()->getTest()))
//            return true;
//        if (node->getAsLoopNode()->getBody() && isPartofNode(target, node->getAsLoopNode()->getBody()))
//            return true;
//        if (node->getAsLoopNode()->getTerminal() && isPartofNode(target, node->getAsLoopNode()->getTerminal()))
//            return true;
//        if (node->getAsLoopNode()->getInit() && isPartofNode(target, node->getAsLoopNode()->getInit()))
//            return true;
//    } else if (node->getAsBinaryNode()) {
//        if (node->getAsBinaryNode()->getLeft() && isPartofNode(target, node->getAsBinaryNode()->getLeft()))
//            return true;
//        if (node->getAsBinaryNode()->getRight() && isPartofNode(target, node->getAsBinaryNode()->getRight()))
//            return true;
//    } else if (node->getAsUnaryNode()) {
//        if (node->getAsUnaryNode()->getOperand() && isPartofNode(target, node->getAsUnaryNode()->getOperand()))
//            return true;
//    } else if (node->getAsAggregate()) {
//        TIntermSequence::iterator sit;
//        for (sit = node->getAsAggregate()->getSequence().begin(); sit != node->getAsAggregate()->getSequence().end(); ++sit) {
//            if ((*sit) && isPartofNode(target, *sit)) {
//                return true;
//            }
//        }
//    } else if (node->getAsSelectionNode()) {
//        if (node->getAsSelectionNode()->getCondition() && isPartofNode(target, node->getAsSelectionNode()->getCondition()))
//            return true;
//        if (node->getAsSelectionNode()->getTrueBlock() && isPartofNode(target, node->getAsSelectionNode()->getTrueBlock()))
//            return true;
//        if (node->getAsSelectionNode()->getFalseBlock() && isPartofNode(target, node->getAsSelectionNode()->getFalseBlock()))
//            return true;
//    } else if (node->getAsDeclarationNode()) {
//        if (node->getAsDeclarationNode()->getInitialization() && isPartofNode(target, node->getAsDeclarationNode()->getInitialization()))
//            return false;
//    } else if (node->getAsSpecificationNode()) {
//        if (node->getAsSpecificationNode()->getParameter() && isPartofNode(target, node->getAsSpecificationNode()->getParameter()))
//            return false;
//        if (node->getAsSpecificationNode()->getInstances() && isPartofNode(target, node->getAsSpecificationNode()->getInstances()))
//            return false;
//    } else {
//        return false;
//    }
//    return false;
//}
//
//bool isPartofMain(TIntermNode *target, TIntermNode *root)
//{
//    TIntermNode *main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, root);
//
//    if (!main) {
//        dbgPrint(DBGLVL_ERROR, "CodeTools - could not find main function\n");
//        exit(1);
//    }
//
//    return isPartofNode(target, main);
//}

int getFunctionDebugParameter(ir_function_signature *node)
{
    int result = -1;

    if (!node)
        return result;

    int i = 0;
    foreach_iter( exec_list_iterator, iter, node->parameters ){
    	ir_variable* ir = ((ir_instruction*)iter.get())->as_variable();
    	if( ir->mode == ir_var_function_in )
    		result = i;
    	++i;
    }

    return result;
}


ir_instruction* getIRDebugParameter(exec_list *list, int pnum)
{
	if (!list)
		return NULL;

	int i = 0;
	ir_instruction* param = NULL;
	foreach_iter( exec_list_iterator, iter, *list ) {
		if (i == pnum)
			param = (ir_instruction*) iter.get();
		++i;
	}

	if (param) {
		dbgPrint(DBGLVL_ERROR, "CodeTools -  function does not have this much parameter\n");
		exit(1);
	}

	return param;
}

ir_instruction* getSideEffectsDebugParameter(ir_call *ir, int pnum)
{
	if (!ir)
		return NULL;

	int i = 0;
	foreach_iter(exec_list_iterator, iter, *ir) {
		ir_instruction* inst = (ir_instruction *) iter.get();
		if (i == pnum)
			return inst->debug_sideeffects & ir_dbg_se_general ? inst : NULL;
		++i;
	}

	dbgPrint(DBGLVL_ERROR, "CodeTools - function does not have this much parameter\n");
	exit(1);
}


bool dbg_state_not_match(exec_list* list, enum ir_dbg_state state)
{
	int skip_pair = -1;
	foreach_iter(exec_list_iterator, iter, *list) {
		ir_instruction * const inst = (ir_instruction *) iter.get();
		if (inst->ir_type == ir_type_dummy) {
			ir_dummy * const dm = inst->as_dummy();
			if (skip_pair < 0) {
				skip_pair = ir_dummy::pair_type(dm->dummy_type);
			} else if (skip_pair == dm->dummy_type) {
				skip_pair = -1;
				continue;
			}
		}
		if (skip_pair >= 0)
			continue;
		if (inst->debug_state != state)
			return true;
	}
	return false;
}

bool dbg_state_not_match(ir_dummy* first, enum ir_dbg_state state)
{
	if (!first || !first->next)
		return false;

	int end_token = ir_dummy::pair_type(first->dummy_type);
	if (end_token >= 0) {
		foreach_node_safe(node, first->next) {
			ir_instruction * const inst = (ir_instruction *) node;
			ir_dummy * const dm = inst->as_dummy();
			if (dm && end_token == dm->dummy_type)
				break;
			if (inst->debug_state != state)
				return true;
		}
	}

	return false;
}
