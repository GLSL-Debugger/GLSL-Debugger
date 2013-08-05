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

#include "../Include/intermediate.h"
#include "RemoveTree.h"
//
// Code to recursively delete the intermediate tree.
//

bool RemoveAggregate(bool, TIntermAggregate* node, TIntermTraverser*)
{
    delete node;
    node = NULL;

    return true;
}

bool RemoveBinary(bool, TIntermBinary* node, TIntermTraverser*)
{
    delete node;

    return true;
}

void RemoveConstantUnion(TIntermConstantUnion* node, TIntermTraverser*)
{
    delete node;
}

bool RemoveSelection(bool, TIntermSelection* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveSwitch(bool, TIntermSwitch* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveCase(bool, TIntermCase* node, TIntermTraverser*)
{
    delete node;

    return true;
}

void RemoveSymbol(TIntermSymbol* node, TIntermTraverser*)
{
    delete node;
}

void RemoveFuncParam(TIntermFuncParam* node, TIntermTraverser*)
{
    delete node;
}

bool RemoveUnary(bool, TIntermUnary* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveLoop(bool, TIntermLoop* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveBranch(bool, TIntermBranch* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveDeclaration(TIntermDeclaration* node, TIntermTraverser*)
{
    delete node;

    return true;
}

bool RemoveSpecification(TIntermSpecification* node, TIntermTraverser*)
{
    delete node;

    return true;
}

void RemoveParameter(TIntermParameter* node, TIntermTraverser*)
{
    delete node;
}

//
// Entry point.
//
void RemoveAllTreeNodes(TIntermNode* root)
{
    TIntermTraverser it;

    it.visitAggregate     = RemoveAggregate;
    it.visitBinary        = RemoveBinary;
    it.visitConstantUnion = RemoveConstantUnion;
    it.visitSelection     = RemoveSelection;
    it.visitSwitch        = RemoveSwitch;
    it.visitCase          = RemoveCase;
    it.visitSymbol        = RemoveSymbol;
    it.visitFuncParam     = RemoveFuncParam;
    it.visitUnary         = RemoveUnary;
    it.visitLoop          = RemoveLoop;
    it.visitBranch        = RemoveBranch;
    it.visitDeclaration   = RemoveDeclaration;
    it.visitSpecification = RemoveSpecification;
    it.visitParameter     = RemoveParameter;

    it.preVisit = false;
    it.postVisit = true;

    if (!root) {
        return;
    }

    root->traverse(&it);
}

