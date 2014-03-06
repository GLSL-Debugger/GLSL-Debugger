#include "CodeTools.h"

#include "glsl/list.h"
#include "glsl/glsl_symbol_table.h"
#include "Visitors/postprocess.h"
#include "glsldb/utils/dbgprint.h"
#include <map>

#define X 1
#define R 5
#define S 9
#define I 13

long strToSwizzleIdx(const char* str)
{
	/* See ir.cpp ir_swizzle::create */

	/* For each possible swizzle character, this table encodes the value in
	 * \c idx_map that represents the 0th element of the vector.  For invalid
	 * swizzle characters (e.g., 'k'), a special value is used that will allow
	 * detection of errors.
	 */
	static const unsigned char base_idx[26] = {
	/* a  b  c  d  e  f  g  h  i  j  k  l  m */
	   R, R, I, I, I, I, R, I, I, I, I, I, I,
	/* n  o  p  q  r  s  t  u  v  w  x  y  z */
	   I, I, S, S, R, S, S, I, I, X, X, X, X
	};

	/* Each valid swizzle character has an entry in the previous table.  This
	 * table encodes the base index encoded in the previous table plus the actual
	 * index of the swizzle character.  When processing swizzles, the first
	 * character in the string is indexed in the previous table.  Each character
	 * in the string is indexed in this table, and the value found there has the
	 * value form the first table subtracted.  The result must be on the range
	 * [0,3].
	 *
	 * For example, the string "wzyx" will get X from the first table.  Each of
	 * the charcaters will get X+3, X+2, X+1, and X+0 from this table.  After
	 * subtraction, the swizzle values are { 3, 2, 1, 0 }.
	 *
	 * The string "wzrg" will get X from the first table.  Each of the characters
	 * will get X+3, X+2, R+0, and R+1 from this table.  After subtraction, the
	 * swizzle values are { 3, 2, 4, 5 }.  Since 4 and 5 are outside the range
	 * [0,3], the error is detected.
	 */
	static const unsigned char idx_map[26] = {
	/* a    b    c    d    e    f    g    h    i    j    k    l    m */
	  R+3, R+2, 0,   0,   0,   0,   R+1, 0,   0,   0,   0,   0,   0,
	/* n    o    p    q    r    s    t    u    v    w    x    y    z */
	   0,   0,   S+2, S+3, R+0, S+0, S+1, 0,   0,   X+3, X+0, X+1, X+2
	};

	/* Validate the first character in the swizzle string and look up the base
	 * index value as described above.
	 */
	if ((str[0] < 'a') || (str[0] > 'z'))
		return 0;

	long swiz_idx = 0;
	const unsigned base = base_idx[str[0] - 'a'];
	for (unsigned i = 0; (i < 4) && (str[i] != '\0'); i++) {
		if ((str[i] < 'a') || (str[i] > 'z'))
			return 0;
		int val = (idx_map[str[i] - 'a'] - base);
		// Driver must care about it, not debugger.
		if (val < 0)
			return 0;
		// shift it to next power of two for number of table elements
		//swiz_idx += val << (5*i);
		// It something like this in original. I think, order will be lost here.
		swiz_idx += 1 << val;
	}
	return swiz_idx;
}

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
	foreach_list(node, &fs->parameters) {
		ir_variable* v = ((ir_instruction*) node)->as_variable();
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

int getFunctionDebugParameter(ir_function_signature *ir)
{
    int result = -1;

    if (!ir)
        return result;

    int i = 0;
    foreach_list( node, &ir->parameters ){
    	ir_variable* ir = ((ir_instruction*)node)->as_variable();
    	if( ir->data.mode == ir_var_function_in )
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
	foreach_list(node, list) {
		if (i == pnum)
			param = (ir_instruction*) node;
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
	foreach_list(node, &ir->actual_parameters) {
		ir_instruction* inst = (ir_instruction *) node;
		if (i == pnum)
			return inst->debug_sideeffects & ir_dbg_se_general ? inst : NULL;
		++i;
	}

	dbgPrint(DBGLVL_ERROR, "CodeTools - function does not have this much parameter\n");
	exit(1);
}

/**
 * Check node for spectial cases
 * Return iteration flow status.
 * False mean iteration must be skipped
 */
bool list_iter_check(ir_instruction* const inst, int& state)
{
	switch(inst->ir_type){
	case ir_type_variable: {
		ir_variable *var = static_cast<ir_variable*>(inst);
		if ((strstr(var->name, "gl_") == var->name) && !var->data.invariant)
			return false;
		break;
	}
	case ir_type_dummy: {
		ir_dummy * const dm = (ir_dummy* const ) inst;
		if (state < 0) {
			state = ir_dummy::pair_type(dm->dummy_type);
		} else if (state == dm->dummy_type) {
			state = -1;
			return false;
		}
		break;
	}
	default:
		break;
	}

	return (state < 0);
}


bool dbg_state_not_match(ast_node* node, enum ast_dbg_state state)
{
	if (node && node->debug_state != state)
		return true;
	return false;
}
