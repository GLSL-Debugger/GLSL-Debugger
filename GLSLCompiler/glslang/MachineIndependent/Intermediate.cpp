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

//
// Build the intermediate representation.
//

#include "localintermediate.h"
#include "QualifierAlive.h"
#include "RemoveTree.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include "utils/dbgprint.h"

#ifndef SQR
#define SQR(a) ((a) * (a))
#endif
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define FRAC(x) ((x) - (int)(x))
#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define LERP(v0,v1, s) ((1.0f - (s))*(v0) + (s) * (v1))

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif


void TIntermNode::copyCPPExtensionChanges(TExtensionList &extMap)
{
    TExtensionList::iterator iter;

    for (iter = extMap.begin(); iter != extMap.end(); iter++) {
        extensionList.push_back(*iter);
    }
}

void TIntermNode::moveCPPExtensionChanges(TExtensionList &extMap)
{
    copyCPPExtensionChanges(extMap);
    extMap.clear();
}


bool CompareStructure(const TType& leftNodeType, constUnion* rightUnionArray, constUnion* leftUnionArray);

////////////////////////////////////////////////////////////////////////////
//
// First set of functions are to help build the intermediate representation.
// These functions are not member functions of the nodes.
// They are called from parser productions.
//
/////////////////////////////////////////////////////////////////////////////

//
// Add a terminal node for an identifier in an expression.
//
// Returns the added node.
//
TIntermSymbol* TIntermediate::addSymbol(int id, const TString& name, const TType& type, TSourceRange range, TExtensionList &extMap)
{
    TIntermSymbol* node = new TIntermSymbol(id, name, type);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// Add a terminal node for an function parameter
//
// Returns the added node.
//
TIntermFuncParam* TIntermediate::addFuncParam(int id, const TString& name, const TType& type, TSourceRange range, TExtensionList &extMap)
{
    TIntermFuncParam* node = new TIntermFuncParam(id, name, type);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// Connect two nodes with a new parent that does a binary operation on the nodes.
//
// Returns the added node.
//
TIntermTyped* TIntermediate::addBinaryMath(TOperator op, TIntermTyped* left, TIntermTyped* right, TSourceRange range, TSymbolTable& symbolTable, TExtensionList &extMap)
{
    switch (op) {
    case EOpLessThan:
    case EOpGreaterThan:
    case EOpLessThanEqual:
    case EOpGreaterThanEqual:
        if (left->getType().isMatrix() || left->getType().isArray() || left->getType().isVector() || left->getType().getBasicType() == EbtStruct) {
            return 0;
        }
        break;
    case EOpLogicalOr:
    case EOpLogicalXor:
    case EOpLogicalAnd:
        if (left->getType().getBasicType() != EbtBool || left->getType().isMatrix() || left->getType().isArray() || left->getType().isVector()) {
            return 0;
        }
        break;
    case EOpAdd:
    case EOpSub:
    case EOpDiv:
    case EOpMul:
        if (left->getType().getBasicType() == EbtStruct || left->getType().getBasicType() == EbtBool)
            return 0;
    default: break; 
    }

    // 
    // First try converting the children to compatible types.
    //
    if (!(left->getType().getStruct() && right->getType().getStruct())) {
        TIntermTyped* child = addConversion(op, left->getType(), right, extMap);
        if (child)
            right = child;
        else {
            child = addConversion(op, right->getType(), left, extMap);
            if (child)
                left = child;
            else
                return 0;
        }
    } else {
        if (left->getType() != right->getType())
            return 0;
    }

    //
    // Need a new node holding things together then.  Make
    // one and promote it to the right type.
    //
    TIntermBinary* node = new TIntermBinary(op);
    node->setNonAtomic();
    node->setRange(addRange(left->getRange(), right->getRange()));

    node->setLeft(left);
    node->setRight(right);

    if (left->hasSideEffects() || right->hasSideEffects()) {
        node->setHasSideEffects();
    }

    if (! node->promote(infoSink)) {
        return 0;
    }

    TIntermConstantUnion *leftTempConstant = left->getAsConstantUnion();
    TIntermConstantUnion *rightTempConstant = right->getAsConstantUnion();

    if (leftTempConstant)
        leftTempConstant = left->getAsConstantUnion();

    if (rightTempConstant)
        rightTempConstant = right->getAsConstantUnion();

    //
    // See if we can fold constants.
    //

    TIntermTyped* typedReturnNode = 0;
    if ( leftTempConstant && rightTempConstant) {
    
        typedReturnNode = leftTempConstant->fold(node->getOp(), rightTempConstant, infoSink);

        if (typedReturnNode) {
            typedReturnNode->moveCPPExtensionChanges(extMap);
            return typedReturnNode;
        }
    }

    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// Connect two nodes through an assignment.
//
// Returns the added node.
//
TIntermTyped* TIntermediate::addAssign(TOperator op, TIntermTyped* left, TIntermTyped* right, TSourceRange range, TExtensionList &extMap)
{
    //
    // Like adding binary math, except the conversion can only go
    // from right to left.
    //
    TIntermBinary* node = new TIntermBinary(op);
    node->setNonAtomic();
    node->setRange(addRange(left->getRange(), right->getRange()));
    node->setHasSideEffects();

    TIntermTyped* child = addConversion(op, left->getType(), right, extMap);
    if (child == 0) {
        dbgPrint(DBGLVL_INFO, "((((TIntermediate::addAssign addConversion failed))))\n");
        dbgPrint(DBGLVL_INFO, "((((<%s> <%s>))))\n", left->getType().getCompleteString().c_str(),
                                               right->getType().getCompleteString().c_str());
        return 0;
    }

    node->setLeft(left);
    node->setRight(child);
    
    if (! node->promote(infoSink)) {
        dbgPrint(DBGLVL_INFO, "((((TIntermediate::addAssign promote failed))))\n");
        return 0;
    }

    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// Connect two nodes through an index operator, where the left node is the base
// of an array or struct, and the right node is a direct or indirect offset.
//
// Returns the added node.
// The caller should set the type of the returned node.
//
TIntermTyped* TIntermediate::addIndex(TOperator op, TIntermTyped* base, TIntermTyped* index, TSourceRange range, TExtensionList &extMap)
{
    TIntermBinary* node = new TIntermBinary(op);
    if (range == TSourceRangeInit)
        range = index->getRange();
    node->setRange(range);
    node->setLeft(base);
    node->setRight(index);
    if (base->hasSideEffects() || index->hasSideEffects()) {
        node->setHasSideEffects();
    }

    // caller should set the type

    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
//
//
TIntermDeclaration::TIntermDeclaration(TVariable* v, TIntermNode* init)
{
    TStructureMap map;
    variable = new TVariable(*v, map);
    initialization = init;
    first = false;
}

//
// adds our own variable node to the parse tree
//

TIntermNode* TIntermediate::addDeclaration(TSourceRange range, 
                                           TVariable* variable,
                                           TIntermNode* init,
                                           TExtensionList &extMap)
{
    TIntermDeclaration* node;

    node = new TIntermDeclaration(variable, init);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    
    if (init && init->hasSideEffects()) {
        node->setHasSideEffects();
    }
    
    return node;
}


//
// adds our own function declaration node to the parse tree
//

TIntermNode* TIntermediate::addFuncDeclaration(TSourceRange range, TFunction* func, TExtensionList &extMap)
{
    TIntermFuncDeclaration* node;

    // make a copy of the function, as otherwise this would be dependent on the 
    // symbol table
    TStructureMap map;
    TFunction *newFunc = func->clone(map);

    node = new TIntermFuncDeclaration(newFunc);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    node->setHasSideEffects();
    return node;
}


//
//
//
TIntermSpecification::TIntermSpecification(TType* t)
{
    TStructureMap map;
    type = new TType();
    type->copyType(*t, map);
    parameter = NULL;
    instances = NULL;
}

//
// adds our own variable node to the parse tree
//
TIntermNode* TIntermediate::addSpecification(TSourceRange range, TType* type, TExtensionList &extMap)
{
    TIntermSpecification* node;

    node = new TIntermSpecification(type);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
//
//
TIntermParameter::TIntermParameter(TType* t)
{
    TStructureMap map;
    type = new TType();
    type->copyType(*t, map);
}

//
// adds our own variable node to the parse tree
//
TIntermNode* TIntermediate::addParameter(TSourceRange range, TType* type, TExtensionList &extMap)
{
    TIntermParameter* node;

    node = new TIntermParameter(type);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    return node;
}
//
// Add one node as the parent of another that it operates on.
//
// Returns the added node.
//
TIntermTyped* TIntermediate::addUnaryMath(TOperator op, TIntermNode* childNode, TSourceRange range, TSymbolTable& symbolTable, TExtensionList &extMap)
{
    TIntermUnary* node;
    TIntermTyped* child = childNode->getAsTyped();

    if (child == 0) {
        infoSink.info.message(EPrefixInternalError, "Bad type in AddUnaryMath", range);
        return 0;
    }

    switch (op) {
    case EOpLogicalNot:
        if (child->getType().getBasicType() != EbtBool || child->getType().isMatrix() || child->getType().isArray() || child->getType().isVector()) {
            return 0;
        }
        break;

    case EOpPostIncrement:
    case EOpPreIncrement:
    case EOpPostDecrement:
    case EOpPreDecrement:
    case EOpNegative:
        if (child->getType().getBasicType() == EbtStruct || child->getType().isArray())
            return 0;
    default: break;
    }
    
    //
    // Do we need to promote the operand?
    //
    // Note: Implicit promotions were removed from the language.
    //
    TBasicType newType = EbtVoid;
    switch (op) {
        case EOpConstructInt:   newType = EbtInt;   break;
        case EOpConstructUInt:  newType = EbtUInt;  break;
        case EOpConstructBool:  newType = EbtBool;  break;
        case EOpConstructFloat: newType = EbtFloat; break;
        default: break;
    }

    if (newType != EbtVoid) {
        child = addConversion(op, TType(newType, EvqTemporary, EvmNone,
                                                               child->getNominalSize(), 
                                                               child->getMatrixSize(0),
                                                               child->getMatrixSize(1),
                                                               child->isMatrix(), 
                                                               child->isArray()),
                              child, extMap);
        if (child == 0)
            return 0;
    }

    //
    // For constructors, we are now done, it's all in the conversion.
    //
    switch (op) {
        case EOpConstructInt:
        case EOpConstructUInt:
        case EOpConstructBool:
        case EOpConstructFloat:
            return child;
        default: break;
    }
    
    TIntermConstantUnion *childTempConstant = 0;
    if (child->getAsConstantUnion()) {
        childTempConstant = child->getAsConstantUnion();
    }

    //
    // Make a new node for the operator.
    //
    node = new TIntermUnary(op);
    if (range == TSourceRangeInit)
        range = child->getRange();
    node->setRange(range);
    node->setOperand(child);
    
    switch(op) {
        case EOpPostIncrement:
        case EOpPreIncrement:
        case EOpPostDecrement:
        case EOpPreDecrement:
            node->setNonAtomic();
            node->setHasSideEffects();
            break;
        default:
            break;
    }
    
    if (! node->promote(infoSink)) {
        return 0;
    }

    if (childTempConstant)  {
        TIntermTyped* newChild = childTempConstant->fold(op, 0, infoSink);
        
        if (newChild) {
            newChild->moveCPPExtensionChanges(extMap);
            return newChild;
        }
    } 

    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// This is the safe way to change the operator on an aggregate, as it
// does lots of error checking and fixing.  Especially for establishing
// a function call's operation on it's set of parameters.  Sequences
// of instructions are also aggregates, but they just direnctly set
// their operator to EOpSequence.
//
// Returns an aggregate node, which could be the one passed in if
// it was already an aggregate.
//
TIntermAggregate* TIntermediate::setAggregateOperator(TIntermNode* node, TOperator op, TSourceRange range, TExtensionList &extMap)
{
    TIntermAggregate* aggNode;

    //
    // Make sure we have an aggregate.  If not turn it into one.
    //
    
    if (node) {
        aggNode = node->getAsAggregate();
        if (aggNode == 0 || aggNode->getOp() != EOpNull) {
            //
            // Make an aggregate containing this node.
            //
            aggNode = new TIntermAggregate();
            aggNode->getSequence().push_back(node);
            if (range == TSourceRangeInit)
                range = node->getRange();
        }
    } else
        aggNode = new TIntermAggregate();

    //
    // Set the operator.
    //
    aggNode->setOperator(op);
    if (range != TSourceRangeInit) {
        aggNode->setRange(range);
    }

    switch (op) {
        case EOpConstructFloat:
        case EOpConstructVec2:
        case EOpConstructVec3:
        case EOpConstructVec4:
        case EOpConstructBool:
        case EOpConstructBVec2:
        case EOpConstructBVec3:
        case EOpConstructBVec4:
        case EOpConstructInt:
        case EOpConstructUInt:
        case EOpConstructIVec2:
        case EOpConstructIVec3:
        case EOpConstructIVec4:
        case EOpConstructUVec2:
        case EOpConstructUVec3:
        case EOpConstructUVec4:
        case EOpConstructMat2:
        case EOpConstructMat2x3:
        case EOpConstructMat2x4:
        case EOpConstructMat3x2:
        case EOpConstructMat3:
        case EOpConstructMat3x4:
        case EOpConstructMat4x2:
        case EOpConstructMat4x3:
        case EOpConstructMat4:
        case EOpConstructStruct:
            {
                TIntermSequence sequence = aggNode->getSequence();
                TIntermSequence::iterator iter;
                for (iter = sequence.begin(); iter != sequence.end(); iter++) {
                    if ((*iter)->hasSideEffects()) {
                        aggNode->setHasSideEffects();
                    }
                }
            }
            break;
        default:
            aggNode->setHasSideEffects();
            break;
    }

    aggNode->moveCPPExtensionChanges(extMap);
    return aggNode;
}

TIntermConstantUnion* TIntermediate::foldAggregate(TIntermAggregate* node, TOperator op, const TType &type, TSourceRange range, TExtensionList &extMap)
{
    int i;
    constUnion* foldConstant;

    if (!node) {
        return NULL;
    }
    
    // Check if all children are constant and have constant union attached
    TIntermSequence sequence = node->getSequence();
    TIntermSequence::iterator sit;
    TIntermTyped** childs = new TIntermTyped*[sequence.size()];
    for (sit = sequence.begin(), i=0; sit != sequence.end(); sit++, i++) {
        if ((*sit)->getAsTyped() &&
            (*sit)->getAsTyped()->getQualifier() != EvqConst &&
            !((*sit)->getAsConstantUnion())) {
            return NULL;
        } else {
            childs[i] = (*sit)->getAsTyped();
        }
    }

    //
    // First try converting the children to compatible types
    //
    for (i=0; i<(int)sequence.size(); i++) {
        if (childs[i]->getType().getStruct()) {
            // no built-in for structs
            return NULL;
        }
    }
    switch (op) {
        case EOpMin:
        case EOpMax:
            // Default binary case (basetypes should match)
            {
                TIntermTyped* child = addConversion(op, childs[0]->getType(), childs[1], extMap);
                if (child) {
                    childs[1] = child;
                } else {
                    child = addConversion(op, childs[1]->getType(), childs[0], extMap);
                    if (child) {
                        childs[0] = child;
                    } else {
                        return NULL;
                    }
                }
            }
            break;
        case EOpClamp:
            {
                TIntermTyped* child1 = addConversion(op, childs[0]->getType(), childs[1], extMap);
                TIntermTyped* child2 = addConversion(op, childs[0]->getType(), childs[2], extMap);
                if (child1 && child2) {
                    childs[1] = child1;
                    childs[2] = child2;
                } else {
                    child1 = addConversion(op, childs[1]->getType(), childs[0], extMap);
                    child2 = addConversion(op, childs[1]->getType(), childs[2], extMap);
                    if (child1 && child2) {
                        childs[0] = child1;
                        childs[2] = child2;
                    } else {
                        child1 = addConversion(op, childs[2]->getType(), childs[0], extMap);
                        child2 = addConversion(op, childs[2]->getType(), childs[1], extMap);
                        if (child1 && child2) {
                            childs[0] = child1;
                            childs[1] = child2;
                        } else {
                            return NULL;
                        }
                    }
                }
            }
            break;
        default:
            return NULL;
    }

    //
    // Promote
    //
    switch (op) {
        case EOpMin:
        case EOpMax:
            // Default genType, genIType, genUType case
            if (childs[0]->isArray() || childs[0]->isMatrix()) {
                return NULL;
            } else if (childs[0]->isVector()) {
                if (childs[1]->isArray() || childs[1]->isMatrix()) {
                    return NULL;
                } else if (childs[1]->isVector()) {
                    if (childs[0]->getType() != childs[1]->getType()) {
                        return NULL;
                    }
                } else {
                    if (childs[0]->getBasicType() != childs[1]->getBasicType()) {
                        return NULL;
                    }
                }
            } else {
                if (childs[1]->isArray() || childs[1]->isMatrix()) {
                    return NULL;
                } else if (childs[1]->isVector()) {
                    if (childs[0]->getBasicType() != childs[1]->getBasicType()) {
                        return false;
                    }
                } else {
                    if (childs[0]->getBasicType() != childs[1]->getBasicType()) {
                        return false;
                    }
                }
            }
            break;
        case EOpClamp:
            if (childs[0]->isArray() || childs[0]->isMatrix()) {
                return NULL;
            } else if (childs[0]->isVector()) {
                if (childs[1]->isArray() || childs[1]->isMatrix()) {
                    return NULL;
                } else if (childs[1]->isVector()) {
                    if (childs[2]->isArray() || childs[2]->isMatrix()) {
                        return NULL;
                    } else if (childs[2]->isVector()) {
                        // all are vectors (type must match)
                        if (childs[0]->getType() != childs[1]->getType() ||
                            childs[1]->getType() != childs[2]->getType()) {
                            return NULL;
                        }
                    } else {
                        return NULL;
                    }
                } else {
                    if (childs[2]->isArray() || childs[2]->isMatrix()) {
                        return NULL;
                    } else if (childs[2]->isVector()) {
                        return NULL;
                    } else {
                        // first vector, second & third scalar (basetypes must match)
                        if (childs[0]->getBasicType() != childs[1]->getBasicType() ||
                            childs[1]->getBasicType() != childs[2]->getBasicType()) {
                            return NULL;
                        }
                    }
                }
            } else {
                if (childs[1]->isArray() || childs[1]->isMatrix()) {
                    return NULL;
                } else if (childs[1]->isVector()) {
                    if (childs[2]->isArray() || childs[2]->isMatrix()) {
                        return NULL;
                    } else if (childs[2]->isVector()) {
                        return NULL;
                    } else {
                        return NULL;
                    }
                } else {
                    if (childs[2]->isArray() || childs[2]->isMatrix()) {
                        return NULL;
                    } else if (childs[2]->isVector()) {
                        return NULL;
                    } else {
                        // all scalar (basetypes must match)
                        if (childs[0]->getBasicType() != childs[1]->getBasicType() ||
                            childs[1]->getBasicType() != childs[2]->getBasicType()) {
                            return NULL;
                        }
                    }
                }
            }
            break;
        default:
            return NULL;
    }

    //
    // Perform constant folding
    //
    foldConstant = new constUnion[type.getObjectSize()];
    switch (op) {
        case EOpMin:
            {
                int ca = 0, cb = 0;
                constUnion *ua = childs[0]->getAsConstantUnion()->getUnionArrayPointer();
                constUnion *ub = childs[1]->getAsConstantUnion()->getUnionArrayPointer();
                for (i=0; i<type.getObjectSize(); i++) {
                    switch (type.getBasicType()) {
                        case EbtInt:
                            foldConstant[i].setIConst(MIN(ua[ca].getIConst(), ub[cb].getIConst()));
                            break;
                        case EbtUInt:
                            foldConstant[i].setUIConst(MIN(ua[ca].getUIConst(), ub[cb].getUIConst()));
                            break;
                        case EbtFloat:
                            foldConstant[i].setFConst(MIN(ua[ca].getFConst(), ub[cb].getFConst()));
                            break;
                        default:
                            return NULL;
                    }

                    if (childs[0]->isVector()) {
                        ca++;
                    }
                    if (childs[1]->isVector()) {
                        cb++;
                    }
                }
            }
            break;
        case EOpMax:
            {
                int ca = 0, cb = 0;
                constUnion *ua = childs[0]->getAsConstantUnion()->getUnionArrayPointer();
                constUnion *ub = childs[1]->getAsConstantUnion()->getUnionArrayPointer();
                for (i=0; i<type.getObjectSize(); i++) {
                    switch (type.getBasicType()) {
                        case EbtInt:
                            foldConstant[i].setIConst(MAX(ua[ca].getIConst(), ub[cb].getIConst()));
                            break;
                        case EbtUInt:
                            foldConstant[i].setUIConst(MAX(ua[ca].getUIConst(), ub[cb].getUIConst()));
                            break;
                        case EbtFloat:
                            foldConstant[i].setFConst(MAX(ua[ca].getFConst(), ub[cb].getFConst()));
                            break;
                        default:
                            return NULL;
                    }

                    if (childs[0]->isVector()) {
                        ca++;
                    }
                    if (childs[1]->isVector()) {
                        cb++;
                    }
                }
            }
            break;
        case EOpClamp:
            {
                int ca = 0, cb = 0, cc = 0;
                constUnion *ua = childs[0]->getAsConstantUnion()->getUnionArrayPointer();
                constUnion *ub = childs[1]->getAsConstantUnion()->getUnionArrayPointer();
                constUnion *uc = childs[2]->getAsConstantUnion()->getUnionArrayPointer();
                for (i=0; i<type.getObjectSize(); i++) {
                    switch (type.getBasicType()) {
                        case EbtInt:
                            foldConstant[i].setIConst(CLAMP(ua[ca].getIConst(), 
                                                            ub[cb].getIConst(), uc[cc].getIConst()));
                            break;
                        case EbtUInt:
                            foldConstant[i].setUIConst(CLAMP(ua[ca].getUIConst(), 
                                                             ub[cb].getUIConst(), uc[cc].getUIConst()));
                            break;
                        case EbtFloat:
                            foldConstant[i].setFConst(CLAMP(ua[ca].getFConst(), 
                                                            ub[cb].getFConst(), uc[cc].getFConst()));
                            break;
                        default:
                            return NULL;
                    }

                    if (childs[0]->isVector()) {
                        ca++;
                    }
                    if (childs[1]->isVector()) {
                        cb++;
                    }
                    if (childs[2]->isVector()) {
                        cc++;
                    }
                }
            }
            break;
        default:
            return NULL;
    }
    
    return addConstantUnion(foldConstant, type, range, extMap);
}


//
// Convert one type to another.
//
// Returns the node representing the conversion, which could be the same
// node passed in if no conversion was needed.
//
// Return 0 if a conversion can't be done.
//
TIntermTyped* TIntermediate::addConversion(TOperator op, const TType& type, TIntermTyped* node, TExtensionList &extMap)
{
    //
    // Does the base type allow operation?
    //
    switch (node->getBasicType()) {
    case EbtVoid:
    case EbtSampler1D:
    case EbtISampler1D:         // EXT_gpu_shader4
    case EbtUSampler1D:         // EXT_gpu_shader4
    case EbtSampler2D:
    case EbtISampler2D:         // EXT_gpu_shader4
    case EbtUSampler2D:         // EXT_gpu_shader4
    case EbtSampler3D:
    case EbtISampler3D:         // EXT_gpu_shader4
    case EbtUSampler3D:         // EXT_gpu_shader4
    case EbtSamplerCube:
    case EbtISamplerCube:       // EXT_gpu_shader4
    case EbtUSamplerCube:       // EXT_gpu_shader4
    case EbtSampler1DShadow:
    case EbtSampler2DShadow:
    case EbtSampler2DRect:        // ARB_texture_rectangle
    case EbtISampler2DRect:       // EXT_gpu_shader4
    case EbtUSampler2DRect:       // EXT_gpu_shader4
    case EbtSampler2DRectShadow:  // ARB_texture_rectangle
    case EbtSampler1DArray:       // EXT_gpu_shader4
    case EbtISampler1DArray:      // EXT_gpu_shader4
    case EbtUSampler1DArray:      // EXT_gpu_shader4
    case EbtSampler2DArray:       // EXT_gpu_shader4
    case EbtISampler2DArray:      // EXT_gpu_shader4
    case EbtUSampler2DArray:      // EXT_gpu_shader4
    case EbtSamplerBuffer:        // EXT_gpu_shader4
    case EbtISamplerBuffer:       // EXT_gpu_shader4
    case EbtUSamplerBuffer:       // EXT_gpu_shader4
    case EbtSampler1DArrayShadow: // EXT_gpu_shader4
    case EbtSampler2DArrayShadow: // EXT_gpu_shader4
    case EbtSamplerCubeShadow:    // EXT_gpu_shader4
        return 0;
    default: break;
    }

    //
    // Otherwise, if types are identical, no problem
    //
    if (type == node->getType())
        return node;

    //
    // If one's a structure, then no conversions.
    //
    if (type.getStruct() || node->getType().getStruct())
        return 0;

    //
    // If one's an array, then no conversions.
    //
    if (type.isArray() || node->getType().isArray())
        return 0;

    TBasicType promoteTo;
    
    switch (op) {
    //
    // Explicit conversions
    //
    case EOpConstructBool:
        promoteTo = EbtBool;
        break;
    case EOpConstructFloat:
        promoteTo = EbtFloat;
        break;
    case EOpConstructInt:
        promoteTo = EbtInt;
        break;
    case EOpConstructUInt:
        promoteTo = EbtUInt;
        break;
    default:
        // implicit conversions were removed from the language.
        // GLSL 1.20 does allow for int -> float conversion

        if (type.getBasicType() == EbtFloat &&
            node->getType().getBasicType() == EbtInt) {
            promoteTo = EbtFloat;
            break;
        }

        // G80 possible implicit conversion
        if (type.getBasicType() == EbtInt &&
                node->getType().getBasicType() == EbtUInt) {
            promoteTo = EbtInt;
            break;
        }
        
        if (type.getBasicType() != node->getType().getBasicType())
            return 0;
        //
        // Size and structure could still differ, but that's
        // handled by operator promotion.
        //
        return node;
    }
    
    if (node->getAsConstantUnion()) {
        return (promoteConstantUnion(promoteTo, node->getAsConstantUnion(), extMap));
    } else {
        //
        // Add a new newNode for the conversion.
        //
        TIntermUnary* newNode = 0;

        TOperator newOp = EOpNull;
        switch (promoteTo) {
        case EbtFloat:
            switch (node->getBasicType()) {
            case EbtInt:   newOp = EOpConvIntToFloat;  break;
            case EbtUInt:  newOp = EOpConvUIntToFloat; break;
            case EbtBool:  newOp = EOpConvBoolToFloat; break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Bad promotion node", node->getRange());
                return 0;
            }
            break;
        case EbtBool:
            switch (node->getBasicType()) {
            case EbtInt:   newOp = EOpConvIntToBool;   break;
            case EbtUInt:  newOp = EOpConvUIntToBool;  break;
            case EbtFloat: newOp = EOpConvFloatToBool; break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Bad promotion node", node->getRange());
                return 0;
            }
            break;
        case EbtInt:
            switch (node->getBasicType()) {
            case EbtBool:   newOp = EOpConvBoolToInt;  break;
            case EbtUInt:   newOp = EOpConvUIntToInt;  break;
            case EbtFloat:  newOp = EOpConvFloatToInt; break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Bad promotion node", node->getRange());
                return 0;
            }
            break;
        case EbtUInt:
            switch (node->getBasicType()) {
                case EbtInt:   newOp = EOpConvIntToUInt;   break;
                case EbtBool:   newOp = EOpConvBoolToUInt;  break;
                case EbtFloat:  newOp = EOpConvFloatToUInt; break;
                default: 
                    infoSink.info.message(EPrefixInternalError, "Bad promotion node", node->getRange());
                    return 0;
            }
            break;
        default: 
            infoSink.info.message(EPrefixInternalError, "Bad promotion type", node->getRange());
            return 0;
        }

        TType type(promoteTo, EvqTemporary, EvmNone, node->getNominalSize(), node->getMatrixSize(0),
                   node->getMatrixSize(1), node->isMatrix(), node->isArray());
        newNode = new TIntermUnary(newOp, type);
        newNode->setRange(node->getRange());
        newNode->setOperand(node);
        newNode->moveCPPExtensionChanges(extMap);
        return newNode;
    }
}

//
// Safe way to combine two nodes into an aggregate.  Works with null pointers, 
// a node that's not a aggregate yet, etc.
//
// Returns the resulting aggregate, unless 0 was passed in for 
// both existing nodes.
//
TIntermAggregate* TIntermediate::growAggregate(TIntermNode* left, TIntermNode* right, TExtensionList &extMap)
{
    if (left == 0 && right == 0)
        return 0;

    TIntermAggregate* aggNode = 0;
    if (left)
        aggNode = left->getAsAggregate();
    if (!aggNode || aggNode->getOp() != EOpNull) {
        aggNode = new TIntermAggregate;
        aggNode->moveCPPExtensionChanges(extMap);
        if (left)
            aggNode->getSequence().push_back(left);
    }

    if (right) {
        aggNode->getSequence().push_back(right);
    }

    
    if (left && right) {
        aggNode->setRange(addRange(left->getRange(), right->getRange()));
    } else if (left) {
        aggNode->setRange(left->getRange());
    } else if (right) {
        aggNode->setRange(right->getRange());
    }

    if ((left && left->hasSideEffects()) ||
        (right && right->hasSideEffects())) {
        aggNode->setHasSideEffects();
    }

    return aggNode;
}

//
// Turn an existing node into an aggregate.
//
// Returns an aggregate, unless 0 was passed in for the existing node.
//
TIntermAggregate* TIntermediate::makeAggregate(TIntermNode* node, TExtensionList &extMap)
{
    if (node == 0)
        return 0;

    TIntermAggregate* aggNode = new TIntermAggregate;
    aggNode->getSequence().push_back(node);
    aggNode->setRange(node->getRange());
    aggNode->moveCPPExtensionChanges(extMap);
    if (node->hasSideEffects()) {
        aggNode->setHasSideEffects();
    }
    return aggNode;
}

//
// For "if" test nodes.  There are three children; a condition,
// a true path, and a false path.  The two paths are in the
// nodePair.
//
// Returns the selection node created.
//
TIntermNode* TIntermediate::addSelection(TIntermTyped* cond, TIntermNodePair nodePair, TSourceRange range, TExtensionList &extMap)
{
    //
    // For compile time constant selections, prune the code and 
    // test now.
    //
    
    if (cond->getAsTyped() && cond->getAsTyped()->getAsConstantUnion()) {
        if (cond->getAsTyped()->getAsConstantUnion()->getUnionArrayPointer()->getBConst())
            return nodePair.node1;
        else
            return nodePair.node2;
    }

    TIntermSelection* node = new TIntermSelection(cond, nodePair.node1, nodePair.node2);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    if ((cond && cond->hasSideEffects()) ||
        (nodePair.node1 && nodePair.node1->hasSideEffects()) ||
        (nodePair.node2 && nodePair.node2->hasSideEffects())) {
        node->setHasSideEffects();
    }
    return node;
}


TIntermTyped* TIntermediate::addComma(TIntermTyped* left, TIntermTyped* right, TSourceRange range, TExtensionList &extMap)
{
    if ((left->getType().getQualifier() == EvqConst || left->getType().getQualifier() == EvqConstNoValue) && 
        (right->getType().getQualifier() == EvqConst || right->getType().getQualifier() == EvqConstNoValue)) {
        return right;
    } else {
        TIntermTyped *commaAggregate = growAggregate(left, right, extMap);
        commaAggregate->getAsAggregate()->setOperator(EOpComma);    
        commaAggregate->setType(right->getType());
        commaAggregate->getTypePointer()->changeQualifier(EvqTemporary);
        commaAggregate->setNonAtomic();
        if ((left && left->hasSideEffects()) ||
            (right && right->hasSideEffects())) {
            commaAggregate->setHasSideEffects();
        }
        return commaAggregate;
    }
}

//
// For "?:" test nodes.  There are three children; a condition,
// a true path, and a false path.  The two paths are specified
// as separate parameters.
//
// Returns the selection node created, or 0 if one could not be.
//
TIntermTyped* TIntermediate::addSelection(TIntermTyped* cond, TIntermTyped* trueBlock, TIntermTyped* falseBlock, TSourceRange range, TExtensionList &extMap)
{
    //
    // Get compatible types.
    //
    TIntermTyped* child = addConversion(EOpSequence, trueBlock->getType(), falseBlock, extMap);
    if (child)
        falseBlock = child;
    else {
        child = addConversion(EOpSequence, falseBlock->getType(), trueBlock, extMap);
        if (child)
            trueBlock = child;
        else
            return 0;
    }

    //
    // See if all the operands are constant, then fold it otherwise not.
    //

    if (cond->getAsConstantUnion() && trueBlock->getAsConstantUnion() && falseBlock->getAsConstantUnion()) {
        if (cond->getAsConstantUnion()->getUnionArrayPointer()->getBConst())
            return trueBlock;
        else
            return falseBlock;
    }

    //
    // Make a selection node.
    //
    TIntermSelection* node = new TIntermSelection(cond, trueBlock, falseBlock, trueBlock->getType());
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    if ((cond && cond->hasSideEffects()) ||
        (trueBlock && trueBlock->hasSideEffects()) ||
        (falseBlock && falseBlock->hasSideEffects())) {
        node->setHasSideEffects();
    }
    return node;
}


// For "switch" test nodes. There are multiple children, but at least there is
// the condition; additionally, more case_label nodes or a single default node
// can be attached. All those nodes are in the nodeList
//
// Returns the switch node created.
//
TIntermNode* TIntermediate::addSwitch(TIntermTyped* cond, TIntermAggregate* nodeList, TSourceRange range, TExtensionList &extMap)
{
    int i;

    // TODO: only if GLSL 1.30 or higher

    // Reorganize nodeList
    // Input:
    //     Case A
    //     Statement 1
    //     Statement 2
    //     Case B
    //     Statement 3
    //     Statement 4
    // Output:
    //     Case A
    //       Statement 1
    //       Statement 2
    //     Case B
    //       Statement 1
    //       Statement 2

    TIntermAggregate* caseList = NULL;

    // Iterate nodeList (input switch body)
    TIntermCase* lastCaseNode = NULL;
    for (i=0; i<nodeList->getSequence().size(); i++) {
        TIntermNode* node = nodeList->getSequence()[i];
        if (!node) continue;

        if (node->getAsCaseNode() != 0) {
            // Create new case node
            caseList = growAggregate(caseList, node, extMap);
            lastCaseNode = node->getAsCaseNode();
        } else {
            if (!node) continue;
            TIntermAggregate* caseBody = lastCaseNode->getCaseBody()->getAsAggregate();
            caseBody = growAggregate(caseBody, node, extMap);
        }
    }

    for (i=0; i<caseList->getSequence().size(); i++) {
        caseList->getSequence()[i]->getAsCaseNode()->getCaseBody()->getAsAggregate()->setOperator(EOpSequence);
    }

    caseList->setOperator(EOpSequence);

    TIntermSwitch* node = new TIntermSwitch(cond, caseList);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);

    if ((cond && cond->hasSideEffects()) ||
        (caseList && caseList->hasSideEffects())) {
        node->setHasSideEffects();
    }

    return node;
}

TIntermNode* TIntermediate::addCase(TIntermTyped* expr, TSourceRange range, TExtensionList &extMap)
{
    // TODO: only if GLSL 1.30 or higher
    TIntermCase* node = new TIntermCase(expr);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);

    if (expr && expr->hasSideEffects()) {
        node->setHasSideEffects();
    }

    return node;
}


//
// Constant terminal nodes.  Has a union that contains bool, float or int constants
//
// Returns the constant union node created.
//

TIntermConstantUnion* TIntermediate::addConstantUnion(constUnion* unionArrayPointer, const TType& t, TSourceRange range, TExtensionList &extMap)
{
    TIntermConstantUnion* node = new TIntermConstantUnion(unionArrayPointer, t);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    return node;
}

TIntermTyped* TIntermediate::addSwizzle(TVectorFields& fields, TSourceRange range, TExtensionList &extMap)
{
    
    TIntermAggregate* node = new TIntermAggregate(EOpSwizzles);

    node->setRange(range);
    TIntermConstantUnion* constIntNode;
    TIntermSequence &sequenceVector = node->getSequence();
    constUnion* unionArray;

    for (int i = 0; i < fields.num; i++) {
        unionArray = new constUnion[1];
        unionArray->setSConst(fields.offsets[i]);
        constIntNode = addConstantUnion(unionArray, TType(EbtInt, EvqConst), range, extMap);
        sequenceVector.push_back(constIntNode);
    }

    node->moveCPPExtensionChanges(extMap);
    return node;
}

//
// Create loop nodes.
//
TIntermNode* TIntermediate::addLoop(TIntermNode* body, TIntermNode *init, TIntermTyped* test, TIntermTyped* terminal, LoopT loopType, TSourceRange range, TExtensionList &extMap)
{
    TIntermNode* node = new TIntermLoop(body, init, test, terminal, loopType);
    node->setRange(range);
    
    node->moveCPPExtensionChanges(extMap);
    if ((body && body->hasSideEffects()) ||
        (init && init->hasSideEffects()) ||
        (test && test->hasSideEffects()) ||
        (terminal && terminal->hasSideEffects())) {
        node->setHasSideEffects();
    }
    return node;
}

//
//
//
bool TIntermLoop::needDbgLoopIter(void)
{
    return (dbgState != DBG_STATE_LOOP_UNSET &&
            dbgState != DBG_STATE_LOOP_QYR_INIT &&
            dbgState != DBG_STATE_LOOP_WRK_INIT);
}

//
// Add branches.
//
TIntermBranch* TIntermediate::addBranch(TOperator branchOp, TSourceRange range, TExtensionList &extMap)
{
    return addBranch(branchOp, 0, range, extMap);
}

TIntermBranch* TIntermediate::addBranch(TOperator branchOp, TIntermTyped* expression, TSourceRange range, TExtensionList &extMap)
{
    TIntermBranch* node = new TIntermBranch(branchOp, expression);
    node->setRange(range);
    node->moveCPPExtensionChanges(extMap);
    node->setHasSideEffects();
    if (branchOp == EOpKill) {
        node->setContainsDiscard();
    }
    return node;
}

//
// This is to be executed once the final root is put on top by the parsing
// process.
//
bool TIntermediate::postProcess(TIntermNode* root, EShLanguage language)
{
    if (root == 0)
        return true;

    //
    // First, finish off the top level sequence, if any
    //
    TIntermAggregate* aggRoot = root->getAsAggregate();
    if (aggRoot && aggRoot->getOp() == EOpNull)
        aggRoot->setOperator(EOpSequence);

    return true;
}

//
// This deletes the tree.
//
void TIntermediate::remove(TIntermNode* root)
{
    if (root)
        RemoveAllTreeNodes(root);
}

////////////////////////////////////////////////////////////////
//
// Member functions of the nodes used for building the tree.
//
////////////////////////////////////////////////////////////////

//
// Say whether or not an operation node changes the value of a variable.
//
// Returns true if state is modified.
//
bool TIntermOperator::modifiesState() const
{
    switch (op) {    
    case EOpPostIncrement: 
    case EOpPostDecrement: 
    case EOpPreIncrement:  
    case EOpPreDecrement:  
    case EOpAssign:    
    case EOpAddAssign: 
    case EOpSubAssign: 
    case EOpMulAssign: 
    case EOpVectorTimesMatrixAssign:
    case EOpVectorTimesScalarAssign:
    case EOpMatrixTimesScalarAssign:
    case EOpMatrixTimesMatrixAssign:
    case EOpDivAssign: 
    case EOpModAssign: 
    case EOpAndAssign: 
    case EOpInclusiveOrAssign: 
    case EOpExclusiveOrAssign: 
    case EOpLeftShiftAssign:   
    case EOpRightShiftAssign:  
        return true;
    default:
        return false;
    }
}

//
// returns true if the operator is for one of the constructors
//
bool TIntermOperator::isConstructor() const
{
    switch (op) {
    case EOpConstructVec2:
    case EOpConstructVec3:
    case EOpConstructVec4:
    case EOpConstructMat2:
    case EOpConstructMat3:
    case EOpConstructMat4:
    case EOpConstructFloat:
    case EOpConstructIVec2:
    case EOpConstructIVec3:
    case EOpConstructIVec4:
    case EOpConstructInt:
    case EOpConstructUInt:
    case EOpConstructUVec2:
    case EOpConstructUVec3:
    case EOpConstructUVec4:
    case EOpConstructBVec2:
    case EOpConstructBVec3:
    case EOpConstructBVec4:
    case EOpConstructBool:
    case EOpConstructStruct:
        return true;
    default:
        return false;
    }
}
//
// Make sure the type of a unary operator is appropriate for its 
// combination of operation and operand type.
//
// Returns false in nothing makes sense.
//
bool TIntermUnary::promote(TInfoSink&)
{
    switch (op) {
    case EOpLogicalNot:
        if (operand->getBasicType() != EbtBool)
            return false;
        break;
    case EOpBitwiseNot:
        if (!(operand->getBasicType() == EbtInt || operand->getBasicType() == EbtUInt))
            return false;
        break;
    case EOpNegative:
    case EOpPostIncrement:
    case EOpPostDecrement:
    case EOpPreIncrement:
    case EOpPreDecrement:
        if (operand->getBasicType() == EbtBool)
            return false;
        break;

    // operators for built-ins are already type checked against their prototype
    case EOpAny:
    case EOpAll:
    case EOpVectorLogicalNot:
        return true;

    // Builtin functions for types genType, genIType
    case EOpAbs:
    case EOpSign:
        if (!(operand->getBasicType() == EbtFloat || operand->getBasicType() == EbtInt)) {
            return false;
        }
        break;
    default:
        if (operand->getBasicType() != EbtFloat)
            return false;
    }
    
    setType(operand->getType());

    return true;
}

//
// Establishes the type of the resultant operation, as well as
// makes the operator the correct one for the operands.
//
// Returns false if operator can't work on operands.
//
bool TIntermBinary::promote(TInfoSink& infoSink)
{
    if (left->isArray()) {
        if (right->isArray()) {
            // Arrays have to be exact matches.
            if (left->getType() != right->getType()) {
                return false;
            }
            // Allowed array operations.
            switch (op) {
                // Promote to conditional
                case EOpEqual:
                case EOpNotEqual:
                    setType(TType(EbtBool));
                    break;
                // Set array information.
                case EOpAssign:
                    setType(TType(left->getBasicType(), EvqTemporary, EvmNone, left->getNominalSize(), 
                                  left->getMatrixSize(0), left->getMatrixSize(1), left->isMatrix()));
                    getType().setArraySize(left->getType().getArraySize());
                    getType().setArrayInformationType(left->getType().getArrayInformationType());
                    break;
                default:
                    return false;
            }
        } else {
            return false;
        }
    } else if (left->isMatrix()) {
        if (right->isArray()) {
            return false;
        } else if (right->isMatrix()) {
            // Matrix types have to be exact matches.
            if (left->getType().getBasicType() != right->getType().getBasicType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    if (left->getMatrixSize(0) != right->getMatrixSize(1)) {
                        return false;
                    }
                    op = EOpMatrixTimesMatrix;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, right->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                case EOpMulAssign:
                    // Matrix sizes have to be exact matches.
                    if (left->getType() != right->getType()) {
                        return false;
                    }
                    op = EOpMatrixTimesMatrixAssign;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, left->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                case EOpAssign:
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                case EOpAddAssign:
                case EOpSubAssign:
                case EOpDivAssign:
                case EOpModAssign:
                    // Matrix sizes have to be exact matches.
                    if (left->getType() != right->getType()) {
                        return false;
                    }
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, left->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                case EOpEqual:
                case EOpNotEqual:
                    setType(TType(EbtBool));
                    break;
                default:
                    return false;
            }
        } else if (right->isVector()) {
            if (left->getBasicType() != right->getBasicType() ||
                left->getMatrixSize(0) != right->getNominalSize()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpMatrixTimesVector;
                    setType(TType(right->getBasicType(), EvqTemporary, EvmNone, left->getMatrixSize(1)));
                    break;
                default:
                    return false;
            }
        } else {
            if (left->getBasicType() != right->getBasicType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpMatrixTimesScalar;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, left->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                case EOpMulAssign:
                    op = EOpMatrixTimesScalarAssign;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, left->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                case EOpAssign:
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                case EOpAddAssign:
                case EOpSubAssign:
                case EOpDivAssign:
                case EOpModAssign:
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, 1, left->getMatrixSize(0), left->getMatrixSize(1), true));
                    break;
                default:
                    return false;
            }
        }
    } else if (left->isVector()) {
        if (right->isArray()) {
            return false;
        } else if (right->isMatrix()) {
            if (left->getBasicType() != right->getBasicType() ||
                left->getNominalSize() != right->getMatrixSize(1)) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpVectorTimesMatrix;
                    setType(TType(left->getBasicType(), EvqTemporary, EvmNone, right->getMatrixSize(0)));
                    break;
                case EOpMulAssign:
                    op = EOpVectorTimesMatrixAssign;
                    setType(TType(left->getBasicType(), EvqTemporary, EvmNone, right->getMatrixSize(0)));
                    break;
                default:
                    return false;
            }
        } else if (right->isVector()) {
            if (left->getType() != right->getType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                case EOpMulAssign:
                case EOpAssign:
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                case EOpAddAssign:
                case EOpSubAssign:
                case EOpDivAssign:
                case EOpModAssign:
                case EOpLeftShift:
                case EOpLeftShiftAssign:
                case EOpRightShift:
                case EOpRightShiftAssign:
                case EOpAnd:
                case EOpAndAssign:
                case EOpInclusiveOr:
                case EOpInclusiveOrAssign:
                case EOpExclusiveOr:
                case EOpExclusiveOrAssign:
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, left->getNominalSize()));
                    break;
                case EOpEqual:
                case EOpNotEqual:
                    setType(TType(EbtBool));
                    break;
                case EOpLessThan:
                case EOpGreaterThan:
                case EOpLessThanEqual:
                case EOpGreaterThanEqual:
                    /* Hint: NVIDIA seems to allow these */
                default:
                    return false;
            }
        } else {
            if (left->getBasicType() != right->getBasicType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpVectorTimesScalar;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, left->getNominalSize()));
                    break;
                case EOpMulAssign:
                    op = EOpVectorTimesScalarAssign;
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, left->getNominalSize()));
                    break;
                case EOpAnd:
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                case EOpAssign:
                case EOpAddAssign:
                case EOpSubAssign:
                case EOpDivAssign:
                case EOpModAssign:
                case EOpLeftShift:
                case EOpLeftShiftAssign:
                case EOpRightShift:
                case EOpRightShiftAssign:
                case EOpAndAssign:
                case EOpInclusiveOr:
                case EOpInclusiveOrAssign:
                case EOpExclusiveOr:
                case EOpExclusiveOrAssign:
                    setType(TType(left->getType().getBasicType(), EvqTemporary, EvmNone, left->getNominalSize()));
                    break;
                default:
                    return false;
            }
        }
    } else {
        if (right->isArray()) {
            return false;
        } else if (right->isMatrix()) {
            if (left->getBasicType() != right->getBasicType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpMatrixTimesScalar;
                    setType(TType(right->getType().getBasicType(), EvqTemporary, EvmNone, 1, right->getMatrixSize(0), right->getMatrixSize(1), true));
                    break;
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                    setType(TType(right->getType().getBasicType(), EvqTemporary, EvmNone, 1, right->getMatrixSize(0), right->getMatrixSize(1), true));
                    break;
                default:
                    return false;
            }
        } else if (right->isVector()) {
            if (left->getBasicType() != right->getBasicType()) {
                return false;
            }
            switch (op) {
                case EOpMul:
                    op = EOpVectorTimesScalar;
                    setType(TType(right->getType().getBasicType(), EvqTemporary, EvmNone, right->getNominalSize()));
                    break;
                case EOpAdd:
                case EOpSub:
                case EOpDiv:
                case EOpMod:
                case EOpAnd:
                case EOpInclusiveOr:
                case EOpExclusiveOr:
                    setType(TType(right->getType().getBasicType(), EvqTemporary, EvmNone, right->getNominalSize()));
                    break;
                default:
                    return false;
            }
        } else {
            if (left->getBasicType() != right->getBasicType()) {
                return false;
            }
            switch (op) {
                // Promote to conditional
                case EOpEqual:
                case EOpNotEqual:
                case EOpLessThan:
                case EOpGreaterThan:
                case EOpLessThanEqual:
                case EOpGreaterThanEqual:
                    setType(TType(EbtBool));
                    break;
                // And and Or operate on conditionals
                case EOpLogicalAnd:
                case EOpLogicalOr:
                case EOpLogicalXor:
                    if (left->getBasicType() != EbtBool)
                        return false;
                    setType(TType(left->getType().getBasicType()));
                    break;
                // Check for integer only operands.
                case EOpMod:
                case EOpRightShift:
                case EOpLeftShift:
                case EOpAnd:
                case EOpInclusiveOr:
                case EOpExclusiveOr:
                case EOpModAssign:
                case EOpAndAssign:
                case EOpInclusiveOrAssign:
                case EOpExclusiveOrAssign:
                case EOpLeftShiftAssign:
                case EOpRightShiftAssign:
                    if (!(left->getBasicType() == EbtInt || left->getBasicType() == EbtUInt))
                        return false;
                    setType(TType(left->getType().getBasicType()));
                    break;
                default:
                    setType(TType(left->getType().getBasicType()));
            }
        }
    }
    /*
    dbgPrint(DBGLVL_INFO, "promote [%s] [%s] --> [%s]\n", left->getType().getCompleteString().c_str(), right->getType().getCompleteString().c_str(), getCompleteString().c_str());
    */
    return true;
}

bool CompareStruct(const TType& leftNodeType, constUnion* rightUnionArray, constUnion* leftUnionArray)
{
    TTypeList* fields = leftNodeType.getStruct();

    size_t structSize = fields->size();
    int index = 0;

    for (size_t j = 0; j < structSize; j++) {
        int size = (*fields)[j].type->getObjectSize();
        for (int i = 0; i < size; i++) {
            if ((*fields)[j].type->getBasicType() == EbtStruct) {
                if (!CompareStructure(*(*fields)[j].type, &rightUnionArray[index], &leftUnionArray[index]))
                    return false;
            } else {
                if (leftUnionArray[index] != rightUnionArray[index])
                    return false;
                index++;
            }    
            
        }
    }
    return true;
} 

bool CompareStructure(const TType& leftNodeType, constUnion* rightUnionArray, constUnion* leftUnionArray)
{
    if (leftNodeType.isArray()) {
        TType typeWithoutArrayness = leftNodeType;
        typeWithoutArrayness.clearArrayness();

        int arraySize = leftNodeType.getArraySize();

        for (int i = 0; i < arraySize; ++i) {
            int offset = typeWithoutArrayness.getObjectSize() * i;
            if (!CompareStruct(typeWithoutArrayness, &rightUnionArray[offset], &leftUnionArray[offset]))
                return false;
        }
    } else
        return CompareStruct(leftNodeType, rightUnionArray, leftUnionArray);    
    
    return true;
} 

//
// The fold functions see if an operation on a constant can be done in place,
// without generating run-time code.
//
// Returns the node to keep using, which may or may not be the node passed in.
//

TIntermTyped* TIntermConstantUnion::fold(TOperator op, TIntermTyped* constantNode, TInfoSink& infoSink)
{   
    constUnion *unionArray = getUnionArrayPointer(); 
    int objectSize = getType().getObjectSize();
        
    if (constantNode) {  // binary operations
    
        TIntermConstantUnion *node = constantNode->getAsConstantUnion();
        constUnion *rightUnionArray = node->getUnionArrayPointer();
        TType returnType = getType();

        // for a case like float f = 1.2 + vec4(2,3,4,5);
        if (constantNode->getType().getObjectSize() == 1 && objectSize > 1) {
            rightUnionArray = new constUnion[objectSize];
            for (int i = 0; i < objectSize; ++i)
                rightUnionArray[i] = *node->getUnionArrayPointer(); 
            returnType = getType();
        } else if (constantNode->getType().getObjectSize() > 1 && objectSize == 1) {
            // for a case like float f = vec4(2,3,4,5) + 1.2;
            unionArray = new constUnion[constantNode->getType().getObjectSize()];
            for (int i = 0; i < constantNode->getType().getObjectSize(); ++i)
                unionArray[i] = *getUnionArrayPointer(); 
            returnType = node->getType();
            objectSize = constantNode->getType().getObjectSize();
        }
        
        constUnion* tempConstArray = 0;
        TIntermConstantUnion *tempNode;
        bool boolNodeFlag = false;
        switch(op) {
        case EOpAdd: 
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] + rightUnionArray[i];
            }
            break;
        case EOpSub: 
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] - rightUnionArray[i];
            }
            break;

        case EOpMul:
        case EOpVectorTimesScalar:
        case EOpMatrixTimesScalar: 
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] * rightUnionArray[i];
            }
            break;
        case EOpMatrixTimesMatrix:                
            if (getType().getBasicType() != EbtFloat || node->getBasicType() != EbtFloat) {
                infoSink.info.message(EPrefixInternalError, "Constant Folding cannot be done for matrix multiply", getRange());
                return 0;
            }
            {// support MSVC++6.0
                tempConstArray = new constUnion[constantNode->getType().getMatrixSize(0) * getMatrixSize(1)];
                for (int row = 0; row < getMatrixSize(1); row++) {
                    for (int column = 0; column < constantNode->getType().getMatrixSize(0); column++) {
                        tempConstArray[getMatrixSize(1) * column + row].setFConst(0.0f);
                        for (int i = 0; i < getMatrixSize(0); i++) {
                            tempConstArray[getMatrixSize(1) * column + row].setFConst(
                                    tempConstArray[getMatrixSize(1) * column + row].getFConst() +
                                    unionArray[i * getMatrixSize(1) + row].getFConst() * 
                                    (rightUnionArray[column * constantNode->getType().getMatrixSize(0) + i].getFConst())); 
                        }
                    }
                }
            }
            break;
        case EOpDiv: 
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++) {
                    switch (getType().getBasicType()) {
                    case EbtFloat: 
                        if (rightUnionArray[i] == 0.0f) {
                            infoSink.info.message(EPrefixWarning, "Divide by zero error during constant folding", getRange());
                            tempConstArray[i].setFConst(FLT_MAX);
                        } else
                            tempConstArray[i].setFConst(unionArray[i].getFConst() / rightUnionArray[i].getFConst());
                    break;

                    case EbtInt:   
                    case EbtUInt:
                        if (rightUnionArray[i] == 0) {
                            infoSink.info.message(EPrefixWarning, "Divide by zero error during constant folding", getRange());
                            tempConstArray[i].setIConst(INT_MAX);
                        } else
                            tempConstArray[i].setIConst(unionArray[i].getIConst() / rightUnionArray[i].getIConst());
                        break;            
                    default: 
                        infoSink.info.message(EPrefixInternalError, "Constant folding cannot be done for \"/\"", getRange());
                        return 0;
                    }
                }
            }
            break;

        case EOpMatrixTimesVector: 
            if (node->getBasicType() != EbtFloat) {
                infoSink.info.message(EPrefixInternalError, "Constant Folding cannot be done for matrix times vector", getRange());
                return 0;
            }
            tempConstArray = new constUnion[getMatrixSize(1)];
            
            {// support MSVC++6.0                    
                for (int i = 0; i < getMatrixSize(1); i++) {
                    tempConstArray[i].setFConst(0.0f);
                    for (int j = 0; j < getMatrixSize(0); j++) {
                        tempConstArray[i].setFConst(tempConstArray[i].getFConst() + ((unionArray[j*getMatrixSize(1) + i].getFConst()) * rightUnionArray[j].getFConst()));
                    }
                }
            }
            
            tempNode = new TIntermConstantUnion(tempConstArray, node->getType());
            tempNode->setRange(getRange());

            return tempNode;                

        case EOpVectorTimesMatrix:
            if (getType().getBasicType() != EbtFloat) {
                infoSink.info.message(EPrefixInternalError, "Constant Folding cannot be done for vector times matrix", getRange());
                return 0;
            }  

            tempConstArray = new constUnion[constantNode->getType().getMatrixSize(0)];
            {// support MSVC++6.0
                for (int i = 0; i < constantNode->getType().getMatrixSize(0); i++) {
                    tempConstArray[i].setFConst(0.0f);
                    for (int j = 0; j < getNominalSize(); j++) {
                        tempConstArray[i].setFConst(tempConstArray[i].getFConst() + ((unionArray[j].getFConst()) * rightUnionArray[i*constantNode->getType().getMatrixSize(1) + j].getFConst()));
                    }
                }
            }
            break;
        case EOpMod:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] % rightUnionArray[i];
            }
            break;
    
        case EOpRightShift:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] >> rightUnionArray[i];
            }
            break;

        case EOpLeftShift:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] << rightUnionArray[i];
            }
            break;
    
        case EOpAnd:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] & rightUnionArray[i];
            }
            break;
        case EOpInclusiveOr:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] | rightUnionArray[i];
            }
            break;
        case EOpExclusiveOr:
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] ^ rightUnionArray[i];
            }
            break;

        case EOpLogicalAnd: // this code is written for possible future use, will not get executed currently
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] && rightUnionArray[i];
            }
            break;

        case EOpLogicalOr: // this code is written for possible future use, will not get executed currently
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    tempConstArray[i] = unionArray[i] || rightUnionArray[i];
            }
            break;

        case EOpLogicalXor:  
            tempConstArray = new constUnion[objectSize];
            {// support MSVC++6.0
                for (int i = 0; i < objectSize; i++)
                    switch (getType().getBasicType()) {
                    case EbtBool: tempConstArray[i].setBConst((unionArray[i] == rightUnionArray[i]) ? false : true); break;
                    default: assert(false && "Default missing");
                    }
            }
            break;

        case EOpLessThan:         
            assert(objectSize == 1);
            tempConstArray = new constUnion[1];
            tempConstArray->setBConst(*unionArray < *rightUnionArray);
            returnType = TType(EbtBool, EvqConst);
            break;
        case EOpGreaterThan:      
            assert(objectSize == 1);
            tempConstArray = new constUnion[1];
            tempConstArray->setBConst(*unionArray > *rightUnionArray);
            returnType = TType(EbtBool, EvqConst);
            break;
        case EOpLessThanEqual:
        {
            assert(objectSize == 1);
            constUnion constant;
            constant.setBConst(*unionArray > *rightUnionArray);
            tempConstArray = new constUnion[1];
            tempConstArray->setBConst(!constant.getBConst());
            returnType = TType(EbtBool, EvqConst);
            break;
        }
        case EOpGreaterThanEqual: 
        {
            assert(objectSize == 1);
            constUnion constant;
            constant.setBConst(*unionArray < *rightUnionArray);
            tempConstArray = new constUnion[1];
            tempConstArray->setBConst(!constant.getBConst());
            returnType = TType(EbtBool, EvqConst);
            break;
        }

        case EOpEqual: 
            if (getType().getBasicType() == EbtStruct) {
                if (!CompareStructure(node->getType(), node->getUnionArrayPointer(), unionArray))
                    boolNodeFlag = true;
            } else {
                for (int i = 0; i < objectSize; i++) {    
                    if (unionArray[i] != rightUnionArray[i]) {
                        boolNodeFlag = true;
                        break;  // break out of for loop
                    }
                }
            }

            tempConstArray = new constUnion[1];
            if (!boolNodeFlag) {
                tempConstArray->setBConst(true);
            }
            else {
                tempConstArray->setBConst(false);
            }
            
            tempNode = new TIntermConstantUnion(tempConstArray, TType(EbtBool, EvqConst));
            tempNode->setRange(getRange());

            return tempNode;         

        case EOpNotEqual: 
            if (getType().getBasicType() == EbtStruct) {
                if (CompareStructure(node->getType(), node->getUnionArrayPointer(), unionArray))
                    boolNodeFlag = true;
            } else {
                for (int i = 0; i < objectSize; i++) {    
                    if (unionArray[i] == rightUnionArray[i]) {
                        boolNodeFlag = true;
                        break;  // break out of for loop
                    }
                }
            }

            tempConstArray = new constUnion[1];
            if (!boolNodeFlag) {
                tempConstArray->setBConst(true);
            }
            else {
                tempConstArray->setBConst(false);
            }
            
            tempNode = new TIntermConstantUnion(tempConstArray, TType(EbtBool, EvqConst));
            tempNode->setRange(getRange());

            return tempNode;         
        
        default: 
            infoSink.info.message(EPrefixInternalError, "Invalid operator for constant folding", getRange());
            return 0;
        }
        tempNode = new TIntermConstantUnion(tempConstArray, returnType);
        tempNode->setRange(getRange());

        return tempNode;                
    } else { 
        //
        // Do unary operations
        //
        TIntermConstantUnion *newNode = 0;
        constUnion* tempConstArray = new constUnion[objectSize];
        for (int i = 0; i < objectSize; i++) {
            switch(op) {
                case EOpRadians:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst((float)(M_PI/180.0*unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpDegrees:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst((float)(180.0/M_PI*unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpSin:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(sin(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpCos:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(cos(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpTan:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(tan(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpAsin:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(asin(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpAcos:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(acos(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpAtan:
                    switch (getType().getBasicType()) {
                        case EbtFloat:
                            tempConstArray[i].setFConst(atan(unionArray[i].getFConst()));
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpNegative:
                    switch (getType().getBasicType()) {
                        case EbtFloat: 
                            tempConstArray[i].setFConst(-unionArray[i].getFConst()); 
                            break;
                        case EbtUInt:  
                            tempConstArray[i].setIConst(-unionArray[i].getIConst()); 
                            break;
                        case EbtInt:   
                            tempConstArray[i].setIConst(-unionArray[i].getIConst()); 
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                case EOpLogicalNot:
                    switch (getType().getBasicType()) {
                        case EbtBool:  
                            tempConstArray[i].setBConst(!unionArray[i].getBConst()); 
                            break;
                        default:
                            infoSink.info.message(EPrefixInternalError, 
                                                  "Unary operation not folded into constant", getRange());
                            return 0;
                    }
                    break;
                default: 
                    return 0;
            }
        }
        newNode = new TIntermConstantUnion(tempConstArray, getType());
        newNode->setRange(getRange());
        return newNode;     
    }

    return this;
}

TIntermTyped* TIntermediate::promoteConstantUnion(TBasicType promoteTo, TIntermConstantUnion* node, TExtensionList &extMap) 
{
    constUnion *rightUnionArray = node->getUnionArrayPointer();
    int size = node->getType().getObjectSize();

    constUnion *leftUnionArray = new constUnion[size];

    for (int i=0; i < size; i++) {
        
        switch (promoteTo) {
        case EbtFloat:
            switch (node->getType().getBasicType()) {
            case EbtInt:
                leftUnionArray[i].setFConst(static_cast<float>(rightUnionArray[i].getIConst()));
                break;
            case EbtUInt:
                leftUnionArray[i].setFConst(static_cast<float>(rightUnionArray[i].getIConst()));
                break;
            case EbtBool:
                leftUnionArray[i].setFConst(static_cast<float>(rightUnionArray[i].getBConst()));
                break;
            case EbtFloat:
                leftUnionArray[i] = rightUnionArray[i];
                break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Cannot promote", node->getRange());
                return 0;
            }                
            break;
        case EbtUInt:
            switch (node->getType().getBasicType()) {
            case EbtInt:
                leftUnionArray[i].setUIConst(static_cast<int>(rightUnionArray[i].getIConst()));
                break;
            case EbtUInt:
                leftUnionArray[i] = rightUnionArray[i];
                break;
            case EbtBool:
                leftUnionArray[i].setUIConst(static_cast<int>(rightUnionArray[i].getBConst()));
                break;
            case EbtFloat:
                leftUnionArray[i].setUIConst(static_cast<int>(rightUnionArray[i].getFConst()));
                break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Cannot promote", node->getRange());
                return 0;
            }                
            break;
        case EbtInt:
            switch (node->getType().getBasicType()) {
            case EbtInt:
                leftUnionArray[i] = rightUnionArray[i];
                break;
            case EbtUInt:
                leftUnionArray[i].setIConst(static_cast<int>(rightUnionArray[i].getUIConst()));
                break;
            case EbtBool:
                leftUnionArray[i].setIConst(static_cast<int>(rightUnionArray[i].getBConst()));
                break;
            case EbtFloat:
                leftUnionArray[i].setIConst(static_cast<int>(rightUnionArray[i].getFConst()));
                break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Cannot promote", node->getRange());
                return 0;
            }                
            break;
        case EbtBool:
            switch (node->getType().getBasicType()) {
            case EbtInt:
                leftUnionArray[i].setBConst(rightUnionArray[i].getIConst() != 0);
                break;
            case EbtUInt:
                leftUnionArray[i].setBConst(rightUnionArray[i].getIConst() != 0);
                break;
            case EbtBool:
                leftUnionArray[i] = rightUnionArray[i];
                break;
            case EbtFloat:
                leftUnionArray[i].setBConst(rightUnionArray[i].getFConst() != 0.0f);
                break;
            default: 
                infoSink.info.message(EPrefixInternalError, "Cannot promote", node->getRange());
                return 0;
            }                
            
            break;
        default:
            infoSink.info.message(EPrefixInternalError, "Incorrect data type found", node->getRange());
            return 0;
        }
    
    }
    
    const TType& t = node->getType();
    
    return addConstantUnion(leftUnionArray, TType(promoteTo, t.getQualifier(), t.getVaryingModifier(), t.getNominalSize(), t.getMatrixSize(0), t.getMatrixSize(1), t.isMatrix(), t.isArray()), node->getRange(), extMap);
}

void TIntermAggregate::addToPragmaTable(const TPragmaTable& pTable)
{
    assert(!pragmaTable);
    pragmaTable = new TPragmaTable();
    *pragmaTable = pTable;
}

TIntermDummy* TIntermediate::addDummy(TSourceRange range, TExtensionList &extMap)
{
    TSourceRange dummyRange;
    dummyRange.left.line = range.right.line;
    dummyRange.left.colum = range.right.colum;
    dummyRange.right.line = range.right.line;
    dummyRange.right.colum = range.right.colum;

    TIntermDummy* node = new TIntermDummy();
    node->setRange(dummyRange);
    node->moveCPPExtensionChanges(extMap);
    return node;
}

