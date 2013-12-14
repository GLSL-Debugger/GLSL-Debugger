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

#ifndef _LOCAL_INTERMEDIATE_INCLUDED_
#define _LOCAL_INTERMEDIATE_INCLUDED_

#include "../Include/intermediate.h"
#include "../Public/ShaderLang.h"
#include "SymbolTable.h"

struct TVectorFields {
	int offsets[4];
	int num;
};

//
// Set of helper functions to help parse and build the tree.
//
class TInfoSink;
class TIntermediate {
public:
	POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)

	TIntermediate(TInfoSink& i) :
			infoSink(i)
	{
	}
	TIntermSymbol* addSymbol(int Id, const TString&, const TType&, TSourceRange,
			TExtensionList&);
	TIntermFuncParam* addFuncParam(int Id, const TString&, const TType&,
			TSourceRange, TExtensionList&);
	TIntermTyped* addConversion(TOperator, const TType&, TIntermTyped*,
			TExtensionList&);
	TIntermTyped* addBinaryMath(TOperator op, TIntermTyped* left,
			TIntermTyped* right, TSourceRange, TSymbolTable&, TExtensionList&);
	TIntermTyped* addAssign(TOperator op, TIntermTyped* left,
			TIntermTyped* right, TSourceRange, TExtensionList&);
	TIntermTyped* addIndex(TOperator op, TIntermTyped* base,
			TIntermTyped* index, TSourceRange, TExtensionList&);
	TIntermTyped* addUnaryMath(TOperator op, TIntermNode* child, TSourceRange,
			TSymbolTable&, TExtensionList&);
	TIntermAggregate* growAggregate(TIntermNode* left, TIntermNode* right,
			TExtensionList&);
	TIntermAggregate* makeAggregate(TIntermNode* node, TExtensionList&);
	TIntermAggregate* setAggregateOperator(TIntermNode*, TOperator,
			TSourceRange, TExtensionList&);
	TIntermConstantUnion* foldAggregate(TIntermAggregate* node, TOperator op,
			const TType &type, TSourceRange range, TExtensionList &extMap);
	TIntermNode* addSelection(TIntermTyped* cond, TIntermNodePair code,
			TSourceRange, TExtensionList&);
	TIntermTyped* addSelection(TIntermTyped* cond, TIntermTyped* trueBlock,
			TIntermTyped* falseBlock, TSourceRange, TExtensionList&);
	TIntermNode* addSwitch(TIntermTyped* cond, TIntermAggregate* nodeList,
			TSourceRange range, TExtensionList &extMap);
	TIntermNode* addCase(TIntermTyped* expr, TSourceRange range,
			TExtensionList &extMap);
	TIntermTyped* addComma(TIntermTyped* left, TIntermTyped* right,
			TSourceRange, TExtensionList&);
	TIntermConstantUnion* addConstantUnion(constUnion*, const TType&,
			TSourceRange, TExtensionList&);
	TIntermTyped* promoteConstantUnion(TBasicType, TIntermConstantUnion*,
			TExtensionList&);
	bool parseConstTree(TSourceRange, TIntermNode*, constUnion*, TOperator,
			TSymbolTable&, TType, bool singleConstantParam = false);
	TIntermNode* addLoop(TIntermNode*, TIntermNode*, TIntermTyped*,
			TIntermTyped*, LoopT, TSourceRange, TExtensionList&);
	TIntermBranch* addBranch(TOperator, TSourceRange, TExtensionList&);
	TIntermBranch* addBranch(TOperator, TIntermTyped*, TSourceRange,
			TExtensionList&);
	TIntermTyped* addSwizzle(TVectorFields&, TSourceRange, TExtensionList&);
	TIntermNode* addDeclaration(TSourceRange, TVariable*, TIntermNode*,
			TExtensionList&);
	TIntermNode* addFuncDeclaration(TSourceRange, TFunction*, TExtensionList&);
	TIntermNode* addSpecification(TSourceRange, TType* type, TExtensionList&);
	TIntermNode* addParameter(TSourceRange, TType* type, TExtensionList&);
	TIntermDummy* addDummy(TSourceRange, TExtensionList&);
	bool postProcess(TIntermNode*, EShLanguage);
	void remove(TIntermNode*);
	void outputTree(TIntermNode*);

protected:
	TInfoSink& infoSink;

private:
	void operator=(TIntermediate&);  // prevent assignments
};

#endif // _LOCAL_INTERMEDIATE_INCLUDED_
