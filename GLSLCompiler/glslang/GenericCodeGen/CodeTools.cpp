#include "CodeTools.h"

#include "dbgprint.h"

char* getFunctionName(const TString &in)
{
	size_t namelength;
	char *name;
	const char *manglName = in.c_str();

	if (strchr(manglName, '(')) {
		namelength = strchr(manglName, '(') - manglName;
	} else {
		namelength = strlen(manglName);
	}

	if (!(name = (char*) malloc((namelength + 1) * sizeof(char)))) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - Not enough memory for name in getFunctionName %s\n", in.c_str());
		exit(1);
	}
	strncpy(name, manglName, namelength);
	name[namelength] = '\0';
	return name;
}

TIntermNode* getFunctionBySignature(const char *sig, TIntermNode* root)
// Assumption: 1. roots hold all function definitions.
//                for single file shaders this should hold.
// Todo: Add solution for multiple files compiled in one shader.
{
	TIntermAggregate *aggregate;
	TIntermSequence sequence;
	TIntermSequence::iterator sit;

	// Root must be aggregate
	if (!(aggregate = root->getAsAggregate())) {
		return NULL;
	}
	if (aggregate->getOp() == EOpFunction) {
		// do not stop search at function prototypes
		if (aggregate->getSequence().size() != 0) {
			if (!strcmp(sig, aggregate->getName().c_str())) {
				return aggregate;
			}
		}
	} else {
		sequence = aggregate->getSequence();

		for (sit = sequence.begin(); sit != sequence.end(); sit++) {
			if ((*sit)->getAsAggregate()
					&& (*sit)->getAsAggregate()->getOp() == EOpFunction) {
				// do not stop search at function prototypes
				if ((*sit)->getAsAggregate()->getSequence().size() != 0) {
					if (!strcmp(sig,
							(*sit)->getAsAggregate()->getName().c_str())) {
						return *sit;
					}
				}
			}
		}
	}
	return NULL;
}

bool isChildofMain(TIntermNode *node, TIntermNode *root)
{
	TIntermNode *main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, root);

	if (!main) {
		dbgPrint(DBGLVL_ERROR, "CodeTools - could not find main function\n");
		exit(1);
	}

	TIntermAggregate *aggregate;

	if (!(aggregate = main->getAsAggregate())) {
		dbgPrint(DBGLVL_ERROR, "CodeTools - main is not Aggregate\n");
		exit(1);
	}

	TIntermSequence sequence = aggregate->getSequence();
	TIntermSequence::iterator sit;

	for (sit = sequence.begin(); sit != sequence.end(); sit++) {
		if (*sit == node) {
			return true;
		}
	}

	return false;
}

bool isPartofNode(TIntermNode *target, TIntermNode *node)
{
	if (node == target)
		return true;

	if (node->getAsLoopNode()) {
		if (node->getAsLoopNode()->getTest()
				&& isPartofNode(target, node->getAsLoopNode()->getTest()))
			return true;
		if (node->getAsLoopNode()->getBody()
				&& isPartofNode(target, node->getAsLoopNode()->getBody()))
			return true;
		if (node->getAsLoopNode()->getTerminal()
				&& isPartofNode(target, node->getAsLoopNode()->getTerminal()))
			return true;
		if (node->getAsLoopNode()->getInit()
				&& isPartofNode(target, node->getAsLoopNode()->getInit()))
			return true;
	} else if (node->getAsBinaryNode()) {
		if (node->getAsBinaryNode()->getLeft()
				&& isPartofNode(target, node->getAsBinaryNode()->getLeft()))
			return true;
		if (node->getAsBinaryNode()->getRight()
				&& isPartofNode(target, node->getAsBinaryNode()->getRight()))
			return true;
	} else if (node->getAsUnaryNode()) {
		if (node->getAsUnaryNode()->getOperand()
				&& isPartofNode(target, node->getAsUnaryNode()->getOperand()))
			return true;
	} else if (node->getAsAggregate()) {
		TIntermSequence::iterator sit;
		for (sit = node->getAsAggregate()->getSequence().begin();
				sit != node->getAsAggregate()->getSequence().end(); ++sit) {
			if ((*sit) && isPartofNode(target, *sit)) {
				return true;
			}
		}
	} else if (node->getAsSelectionNode()) {
		if (node->getAsSelectionNode()->getCondition()
				&& isPartofNode(target,
						node->getAsSelectionNode()->getCondition()))
			return true;
		if (node->getAsSelectionNode()->getTrueBlock()
				&& isPartofNode(target,
						node->getAsSelectionNode()->getTrueBlock()))
			return true;
		if (node->getAsSelectionNode()->getFalseBlock()
				&& isPartofNode(target,
						node->getAsSelectionNode()->getFalseBlock()))
			return true;
	} else if (node->getAsDeclarationNode()) {
		if (node->getAsDeclarationNode()->getInitialization()
				&& isPartofNode(target,
						node->getAsDeclarationNode()->getInitialization()))
			return false;
	} else if (node->getAsSpecificationNode()) {
		if (node->getAsSpecificationNode()->getParameter()
				&& isPartofNode(target,
						node->getAsSpecificationNode()->getParameter()))
			return false;
		if (node->getAsSpecificationNode()->getInstances()
				&& isPartofNode(target,
						node->getAsSpecificationNode()->getInstances()))
			return false;
	} else {
		return false;
	}
	return false;
}

bool isPartofMain(TIntermNode *target, TIntermNode *root)
{
	TIntermNode *main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, root);

	if (!main) {
		dbgPrint(DBGLVL_ERROR, "CodeTools - could not find main function\n");
		exit(1);
	}

	return isPartofNode(target, main);
}

int getFunctionDebugParameter(TIntermAggregate *node)
{
	int result = -1;
	int i;

	if (!node)
		return result;

	if (node->getOp() != EOpFunction) {
		return result;
	}

	TIntermSequence funcDeclSeq = node->getSequence();

	if (!funcDeclSeq[0] || !funcDeclSeq[0]->getAsAggregate()
			|| !(funcDeclSeq[0]->getAsAggregate()->getOp() == EOpParameters)) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - function does not comply with assumptions\n");
		exit(1);
	}
	TIntermSequence funcDeclParamSeq =
			(funcDeclSeq[0]->getAsAggregate())->getSequence();
	TIntermSequence::iterator pD = funcDeclParamSeq.begin();

	for (i = 0; pD != funcDeclParamSeq.end(); ++pD, ++i) {
		if ((*pD)->getAsFuncParamNode()->getType().getQualifier() == EvqIn) {
			result = i;
		}
	}

	return result;
}

TType* getTypeDebugParameter(TIntermAggregate *node, int pnum)
{
	TType *result = NULL;

	if (!node)
		return result;

	if (node->getOp() != EOpFunctionCall)
		return result;

	TIntermSequence funcCallSeq = node->getSequence();

	if ((int) funcCallSeq.size() < pnum) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools -  function does not have this much parameter\n");
		exit(1);
	}

	if (!funcCallSeq[pnum]->getAsTyped()) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools -  in parameter is not of type TIntermTyped\n");
		exit(1);
	}

	return funcCallSeq[pnum]->getAsTyped()->getTypePointer();
}

bool getAtomicDebugParameter(TIntermAggregate *node, int pnum)
{
	if (!node)
		return false;

	if (node->getOp() != EOpFunctionCall)
		return false;

	TIntermSequence funcCallSeq = node->getSequence();

	if ((int) funcCallSeq.size() < pnum) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - function does not have this much parameter\n");
		exit(1);
	}

	if (!funcCallSeq[pnum]->getAsTyped()) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - in parameter is not of type TIntermTyped\n");
		exit(1);
	}

	return funcCallSeq[pnum]->getAsTyped()->isAtomic();
}

bool getHasSideEffectsDebugParameter(TIntermAggregate *node, int pnum)
{
	if (!node)
		return false;

	if (node->getOp() != EOpFunctionCall)
		return false;

	TIntermSequence funcCallSeq = node->getSequence();

	if ((int) funcCallSeq.size() < pnum) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - function does not have this much parameter\n");
		exit(1);
	}

	if (!funcCallSeq[pnum]->getAsTyped()) {
		dbgPrint(DBGLVL_ERROR,
				"CodeTools - in parameter is not of type TIntermTyped\n");
		exit(1);
	}

	return funcCallSeq[pnum]->getAsTyped()->hasSideEffects();
}
