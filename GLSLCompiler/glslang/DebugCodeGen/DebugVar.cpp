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

#include <string.h>
#include <Include/Common.h>
#include <Include/ShHandle.h>
#include <Include/intermediate.h>
#include <MachineIndependent/SymbolTable.h>
#include <MachineIndependent/localintermediate.h>
#include <MachineIndependent/ParseHelper.h>

#include <algorithm>

#include "dbgprint.h"

#define bold  "[1m"
#define red   "[0;31m"
#define bred  "[1;31m"

#define reset "[0m"
static char esc = 27;

#define VERBOSE 4
#define VPRINT(level, ...) { if (level < VERBOSE) \
                                dbgPrint(DBGLVL_COMPILERINFO, __VA_ARGS__); }

class TOutputDebugVarCompiler : public TCompiler {
    public:
        TOutputDebugVarCompiler(EShLanguage l,
                                int dOptions,
                                ShVariableList *variableList) :
            TCompiler(l, infoSink),
            debugOptions(dOptions),
            vl(variableList) { }
        virtual bool compile(TIntermNode* root);
        virtual bool compileDbg(TIntermNode* root, ShChangeableList *cgbl, ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code) {
            UNUSED_ARG(root)
            UNUSED_ARG(cgbl)
            UNUSED_ARG(vl)
            UNUSED_ARG(dbgCgOptions)
            UNUSED_ARG(code)
            return 0;
        }
        TInfoSink infoSink;
        int debugOptions;
    protected:
        ShVariableList *vl;
};

//
// This function must be provided to create the actual
// compile object used by higher level code.  It returns
// a subclass of TCompiler.
//
TCompiler* ConstructCompilerDebugVar(EShLanguage language, int debugOptions, ShVariableList *vl)
{
    return new TOutputDebugVarCompiler(language, debugOptions, vl);
}

//
// Delete the compiler made by ConstructCompiler
//
void DeleteTraverseDebugVar(TCompiler* compiler)
{
    delete compiler;
}

class TOutputDebugVarTraverser : public TIntermTraverser {
    public:
        TOutputDebugVarTraverser(TInfoSink &i, ShVariableList *variableList)
            : infoSink(i), vl(variableList)
            { }
        TInfoSink &infoSink;
        ShVariableList *getVariableList() { return vl; }

        void addToScope(int id);
        void dumpScope(void);
        scopeList& getScope(void) { return scope; }
        scopeList* getCopyOfScope(void);
    private:
        bool nameIsAlreadyInList(scopeList *l, const char *name);
        ShVariableList *vl;
        scopeList scope;
};

void TOutputDebugVarTraverser::addToScope(int id)
{
    // Double ids can only occur with arrays of undeclared size.
    // Scope is of outer bound, as those variables can be used right after
    // their first definition.

    // search for doubles
    scopeList::iterator e = find(scope.begin(), scope.end(), id);

    // only insert if not already in there
    if (e == scope.end()) {
        scope.push_back(id);
    }
}

bool TOutputDebugVarTraverser::nameIsAlreadyInList(scopeList *l, const char *name)
{
    scopeList::iterator it = l->begin();

    while (it != l->end()) {
        ShVariable *v = findShVariableFromId(vl, *it);
        if (v) {
            if (!strcmp(name, v->name)) {
                return true;
            }
        } else {
            dbgPrint(DBGLVL_ERROR, "DebugVar - could not find id %i in scopeList\n", *it);
            exit(1);
        }
        it++;
    }
    return false;
}

scopeList* TOutputDebugVarTraverser::getCopyOfScope(void)
{
    // Hiding of variables in outer scope by local definitions is
    // implemented here. Out of all variables named the same, only the last
    // one is copied to the list!
    scopeList *copiedList = new scopeList();
    scopeList::reverse_iterator rit = scope.rbegin();

    while (rit != scope.rend()) {
        // Check if variable with same name is already in copiedList
        ShVariable *v = findShVariableFromId(vl, *rit);
        if (!nameIsAlreadyInList(copiedList, v->name)) {
            copiedList->push_front(*rit);
        }
        rit++;
    }

    return copiedList;
}

void TOutputDebugVarTraverser::dumpScope(void)
{
    scopeList::iterator li = scope.begin();

    while (li != scope.end()) {
        ShVariable* v;
        v = findShVariableFromId(vl, *li);

        if (v) {
            VPRINT(4, "<%i,%s> ", *li, v->name);
        } else {
            dbgPrint(DBGLVL_ERROR, "DebugVar - <%i,?> ", *li);
            exit(1);
        }
        li++;
    }
    VPRINT(4, "\n");
}

static bool TraverseVarAggregate(bool, TIntermAggregate* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    /* Set actual scope as sequence */
    node->setScope(oit->getCopyOfScope());

    switch (node->getOp()) {
        case EOpSequence:
        case EOpFunction:
            {
                VPRINT(3, "%c%sbegin %s %s at %s %c%s\n",
                        esc, bold,
                        node->getOp()==EOpSequence?"sequence":"function",
                        node->getOp()==EOpSequence?"":node->getName().c_str(),
                        FormatSourceRange(node->getRange()).c_str(),
                        esc, reset);

                TIntermSequence sequence = node->getSequence();
                TIntermSequence::iterator sit = sequence.begin();
                scopeList::iterator end;
                int restoreScope = (int)oit->getScope().size();


                if (restoreScope) {
                    end = --(oit->getScope().end());
                }


                while (sit != sequence.end()) {
                    (*sit)->traverse(it);
                    sit++;
                }

                VPRINT(3, "%c%send %s %s at %s %c%s\n",
                        esc, bold,
                        node->getOp()==EOpSequence?"sequence":"function",
                        node->getOp()==EOpSequence?"":node->getName().c_str(),
                        FormatSourceRange(node->getRange()).c_str(),
                        esc, reset);

                if (restoreScope) {
                    oit->getScope().erase(++end, oit->getScope().end());
                } else {
                    oit->getScope().erase(oit->getScope().begin(),
                                        oit->getScope().end());
                }
            }
            return false;
        default:
            return true;
    }
}

static bool TraverseVarBinary(bool, TIntermBinary* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);
    node->setScope(oit->getCopyOfScope());
    return true;
}

static bool TraverseVarUnary(bool, TIntermUnary* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    switch (node->getOp()) {
        case EOpPostIncrement:
        case EOpPostDecrement:
        case EOpPreIncrement:
        case EOpPreDecrement:
            // set scope of node
            node->setScope(oit->getCopyOfScope());
            return true;
        default:
            break;
    }
    return true;
}

static bool TraverseVarDeclaration(TIntermDeclaration* node, TIntermTraverser* it)
{
    VPRINT(3, "%c%sdeclaration of %s <%i>%c%s\n",
            esc, bold,
            node->getVariable()->getName().c_str(),
            node->getVariable()->getUniqueId(),
            esc, reset);

    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    // Should we care?
    if (node->getVariable()->isShVariable()) {
        // First process the variable
        ShVariableList *vl = oit->getVariableList();
        TVariable *v = node->getVariable();

        // Add variable to the global list of all seen variables
        addShVariable(vl, v->getShVariable(), 0);

        if (node->getInitialization()) {
            // Process childs first without the new variable
            node->getInitialization()->traverse(oit);
            // Handle special case of assignment
            scopeList *sl = node->getInitialization()->getScope();
            if (sl) {
                /*
                * Actually do not add declared variable to the list here, because
                * Code Generation would access the data before it is declared. This should
                * not be needed anyway, since the data would be uninitialized
                *
                // Dont forget to check for double ids
                scopeList::iterator e = find(sl->begin(), sl->end(), v->getUniqueId());

                // Finally at it to the list
                if (e == sl->end()) {
                    sl->push_back(v->getUniqueId());
                }
                */
            } else {
                dbgPrint(DBGLVL_ERROR, "DebugVar - declaration with initialization failed\n");
                exit(1);
            }
        }

        // Now add the list to the actual scope and proceed
        oit->addToScope(v->getUniqueId());
        oit->dumpScope();
        return false;
    } else {
        return true;
    }
}

static bool TraverseVarSelection(bool, TIntermSelection* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    // nothing can be declared here in first place
    node->setScope(oit->getCopyOfScope());

    node->getCondition()->traverse(it);


    // traverse true block
    if (node->getTrueBlock()) {
        // remember end of actual scope
        scopeList::iterator end;
        int restoreScope = (int)oit->getScope().size();
        if (restoreScope) {
            end = --(oit->getScope().end());
        }

        node->getTrueBlock()->traverse(it);

        // restore global scope list
        if (restoreScope) {
            oit->getScope().erase(++end, oit->getScope().end());
        } else {
            oit->getScope().erase(oit->getScope().begin(), oit->getScope().end());
        }
    }

    // traverse false block
    if (node->getFalseBlock()) {
        // remember end of actual scope
        scopeList::iterator end;
        int restoreScope = (int)oit->getScope().size();
        if (restoreScope) {
            end = --(oit->getScope().end());
        }

        node->getFalseBlock()->traverse(it);

        // again restore global scope list
        if (restoreScope) {
            oit->getScope().erase(++end, oit->getScope().end());
        } else {
            oit->getScope().erase(oit->getScope().begin(), oit->getScope().end());
        }

    }

    return false;
}

static bool TraverseVarSwitch(bool, TIntermSwitch* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    // nothing can be declared here in first place
    node->setScope(oit->getCopyOfScope());

    node->getCondition()->traverse(it);

    // traverse case list
    if (node->getCaseList()) {
        node->getCaseList()->traverse(it);
    }

    return false;
}

static bool TraverseVarCase(bool, TIntermCase* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    // nothing can be declared here in first place
    node->setScope(oit->getCopyOfScope());

    // traverse expression
    if (node->getExpression()) {
        node->getExpression()->traverse(it);
    }

    // traverse case body
    if (node->getCaseBody()) {
        node->getCaseBody()->traverse(it);
    }
    return false;
}

static bool TraverseVarLoop(bool, TIntermLoop* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    // declarations made in the initialization are not in scope of the loop
    node->setScope(oit->getCopyOfScope());

    // remember end of actual scope, initialization only changes scope of body
    scopeList::iterator end;
    int restoreScope = (int)oit->getScope().size();
    if (restoreScope) {
        end = --(oit->getScope().end());
    }

    // visit optional initialization
    if (node->getInit()) {
        node->getInit()->traverse(it);
    }

    // visit test, this cannot change scope anyway, so order is unimportant
    if (node->getTest()) {
        node->getTest()->traverse(it);
    }

    // visit optional terminal, this cannot change the scope either
    if (node->getTerminal()) {
        node->getTerminal()->traverse(it);
    }

    // visit body
    if (node->getBody()) {
        node->getBody()->traverse(it);
    }

    // restore global scope list
    if (restoreScope) {
        oit->getScope().erase(++end, oit->getScope().end());
    } else {
        oit->getScope().erase(oit->getScope().begin(), oit->getScope().end());
    }

    return false;
}

static bool TraverseVarBranch(bool, TIntermBranch* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    if (node->getExpression()) {
        node->setScope(oit->getCopyOfScope());
    }

    return true;
}

static void TraverseVarFuncParam(TIntermFuncParam* node, TIntermTraverser* it)
{
    VPRINT(3, "%c%sparameter %s <%i>%c%s\n",
            esc, bold,
            node->getSymbol().c_str(),
            node->getId(),
            esc, reset);

    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    ShVariableList *vl = oit->getVariableList();

    if (!(IsSampler(node->getTypePointer()->getBasicType()))) {

        ShVariable *v = node->getTypePointer()->getShVariable();

        v->uniqueId = node->getId();
        v->name = (char*) malloc(strlen(node->getSymbol().c_str())+1);
        strcpy(v->name, node->getSymbol().c_str());

        addShVariable(vl, v, 0);

        oit->addToScope(node->getId());
        oit->dumpScope();
    }
}

static void TraverseVarDummy(TIntermDummy* node, TIntermTraverser* it)
{
    TOutputDebugVarTraverser *oit = static_cast<TOutputDebugVarTraverser*>(it);

    node->setScope(oit->getCopyOfScope());
}

//
//
// Changeables
//
//
//
//

static TIntermNode* getFunctionBySignature(const char *sig, TIntermNode* root)
// Assumption: 1. roots hold all function definitions.
//                for single file shaders this should hold.
// Todo: Add solution for multiple files compiled in one shader.
{
    TIntermAggregate *aggregate;
    TIntermSequence sequence;
    TIntermSequence::iterator sit;

    // Root must be aggregate
    if (!(aggregate = root->getAsAggregate())) {
        dbgPrint(DBGLVL_ERROR, "DebugVar - root is not aggregate!!!\n");
        exit(1);
        return NULL;
    }
    if (aggregate->getOp() == EOpFunction) {
        // do not stop search at function prototypes
        if (aggregate->getSequence().size() != 0) {
            if (!strcmp( sig, aggregate->getName().c_str() )) {
                VPRINT(3, "getFunctionBySignature: found %s at %p\n",
                        sig, aggregate);
                return aggregate;
            }
        }
    } else {
        sequence = aggregate->getSequence();

        for(sit = sequence.begin(); sit != sequence.end(); sit++) {
            if ( (*sit)->getAsAggregate() &&
                    (*sit)->getAsAggregate()->getOp() == EOpFunction ) {
                VPRINT(2, "compare %s %s\n", sig, (*sit)->getAsAggregate()->getName().c_str());
                // do not stop search at function prototypes
                if ((*sit)->getAsAggregate()->getSequence().size() != 0) {


                    if (!strcmp( sig, (*sit)->getAsAggregate()->getName().c_str() )) {
                        VPRINT(3, "getFunctionBySignature: found %s at %p\n",
                                sig, *sit);
                        return *sit;
                    }
                }
            }
        }
    }
    return NULL;
}



class TOutputDebugCgbTraverser : public TIntermTraverser {
    public:
        TOutputDebugCgbTraverser(TInfoSink &i)
            : infoSink(i), active(0)
            { }
        TInfoSink &infoSink;
        bool isActive(void) { return active; }
        // active:  all coming symboles are being changed
        void activate(void) { active = true; }
        // passive: coming symboles act as input and are not changed
        void deactivate(void) { active = false; }
        TIntermNode *root;
    private:
        bool active;

};

static bool TraverseChangeAggregate(bool, TIntermAggregate* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    VPRINT(2, "(%s) changeAggregate \n", FormatSourceRange(node->getRange()).c_str());

    switch (node->getOp()) {
        case EOpFunctionCall:
            // only user defined functions can have out/inout parameters
            if (node->getAsAggregate()->isUserDefined()) {
                // changed variables are all out/inout parameters and
                // all changes made in the body of this function

                TIntermNode *funcDec;
                funcDec = getFunctionBySignature(node->getName().c_str(), oit->root);
                if (!funcDec) {
                    dbgPrint(DBGLVL_ERROR, "DebugVar - could not find function definition %s\n",
                            node->getName().c_str());
                    exit(1);
                }

                // if function is not already parsed, do it now
                // this is neccessary due to $*&T# function prototypes
                funcDec->traverse(it);

                // now the function should be aware of it's changeables
                copyShChangeableList(node->getCgbList(), funcDec->getCgbList());

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
                TIntermSequence funcCallParamSeq =
                    node->getAsAggregate()->getSequence();
                TIntermSequence funcDeclSeq =
                    funcDec->getAsAggregate()->getSequence();
                TIntermSequence funcDeclParamSeq =
                    funcDeclSeq[0]->getAsAggregate()->getSequence();

                TIntermSequence::iterator pC = funcCallParamSeq.begin();
                TIntermSequence::iterator pD = funcDeclParamSeq.begin();

                for (; pC != funcCallParamSeq.end(); ++pC, ++pD) {
                    // check if parameter is of interest
                    if ((*pD)->getAsFuncParamNode()->getType().getQualifier()==EvqOut ||
                        (*pD)->getAsFuncParamNode()->getType().getQualifier()==EvqInOut)
                    {
                        oit->activate();
                        (*pC)->traverse(it);
                        oit->deactivate();

                        // add these parameters to parameter-list
                        copyShChangeableList(node->getAsAggregate()->
                                                                getCgbParameterList(),
                                            (*pC)->getCgbList());
                    } else {
                        oit->deactivate();
                        (*pC)->traverse(it);

                        // these can be added direcly to the changeables
                        copyShChangeableList(node->getCgbList(), (*pC)->getCgbList());
                    }
                }

                // Add parameter to changeables, since they need to be passed to parents
                copyShChangeableList(node->getCgbList(), node->getAsAggregate()->
                                                                getCgbParameterList());

                // emitVertex push
                if (funcDec->containsEmitVertex()) {
                    node->setEmitVertex();
                }
                // discard push
                if (funcDec->containsDiscard()) {
                    node->setContainsDiscard();
                }
            }
            return false;
        case EOpFunction:
            // do not parse an function if this was already done before
            // again this is due to the (*^@*$ function prototypes
            if (node->getAsAggregate()->isAlreadyParsed()) {
                return false;
            }
            node->getAsAggregate()->setAlreadyParsed();

            {
                // now visit children
                TIntermSequence::iterator sit;
                for (sit=node->getSequence().begin();
                        sit!=node->getSequence().end(); ++sit) {
                    VPRINT(2, "(%s) changeChild \n", FormatSourceRange(node->getRange()).c_str());
                    (*sit)->traverse(it);

                    // copy changeables
                    copyShChangeableList(node->getCgbList(), (*sit)->getCgbList());
                    //(node->getCgbList());

                    // emitVertex push
                    if ((*sit)->containsEmitVertex()) {
                        node->setEmitVertex();
                    }
                    // discard push
                    if ((*sit)->containsDiscard()) {
                        node->setContainsDiscard();
                    }
                }
            }

            // copy parameters to local parameter list
            // Assumption: first child is responsible for parameters,
            //             check with simmilar comment above
            copyShChangeableList(node->getAsAggregate()->getCgbParameterList(),
                                node->getAsAggregate()->getSequence()[0]->
                                            getAsAggregate()->getCgbList());

            return false;
        default:
            oit->deactivate();
    }

    // visit children
    TIntermSequence::iterator sit;
    for (sit=node->getSequence().begin(); sit!=node->getSequence().end(); ++sit) {
        VPRINT(2, "(%s) changeChild \n", FormatSourceRange(node->getRange()).c_str());
        (*sit)->traverse(it);

        // copy changeables
        copyShChangeableList(node->getCgbList(), (*sit)->getCgbList());
        //(node->getCgbList());

        // emitVertex push
        if ((*sit)->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if ((*sit)->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}

static bool TraverseChangeBinary(bool, TIntermBinary* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    switch (node->getOp()) {
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
            VPRINT(2, "(%s) changeAssigment\n", FormatSourceRange(node->getRange()).c_str());
            //(node->getCgbList());

            // process left branch actively
            if (node->getLeft()) {
                VPRINT(2, "===== left ============================\n");

                oit->activate();
                node->getLeft()->traverse(it);
                oit->deactivate();

                // copy the changeables of left branch
                copyShChangeableList(node->getCgbList(),
                                    node->getLeft()->getCgbList());
                //dumpShChangeableList(node->getCgbList());

                // emitVertex push
                if (node->getLeft()->containsEmitVertex()) {
                    node->setEmitVertex();
                }
                // discard push
                if (node->getLeft()->containsDiscard()) {
                    node->setContainsDiscard();
                }
            }

            // process right branch passively
            if (node->getRight()) {
                VPRINT(2, "===== right ===========================\n");

                oit->deactivate();
                node->getRight()->traverse(it);

                // copy the changeables of right branch
                copyShChangeableList(node->getCgbList(),
                                    node->getRight()->getCgbList());
                //dumpShChangeableList(node->getCgbList());

                // emitVertex push
                if (node->getRight()->containsEmitVertex()) {
                    node->setEmitVertex();
                }
                // discard push
                if (node->getRight()->containsDiscard()) {
                    node->setContainsDiscard();
                }
            }
            return false;
        case EOpVectorSwizzle:
            VPRINT(2, "(%s) vectorSwizzle\n", FormatSourceRange(node->getRange()).c_str());

            // process left branch first
            if (node->getLeft()) {
                node->getLeft()->traverse(it);
            }
            // then copy changeables
            // should be one or zero, otherwise we have a problem
            copyShChangeableList(node->getCgbList(),
                                node->getLeft()->getCgbList());

            // remove all changeables not in scope (markus bug)
            {
                ShChangeableList *cl = node->getCgbList();
                scopeList *sl = node->getScope();
                scopeList::iterator sit;

                for (int i=0; i<cl->numChangeables;) {
                    bool inScope = false;
                    for (sit = sl->begin(); sit != sl->end(); sit++) {
                        if ((*sit) == cl->changeables[i]->id) {
                            inScope = true;
                            break;
                        }
                    }

                    if (!inScope) {
                        for (int j=i+1; j<cl->numChangeables; j++) {
                            cl->changeables[j-1] = cl->changeables[j];
                        }
                        cl->numChangeables--;
                    } else {
                        i++;
                    }
                }
            }

            // emitVertex push
            if (node->getLeft()->containsEmitVertex()) {
                node->setEmitVertex();
            }
            // discard push
            if (node->getLeft()->containsDiscard()) {
                node->setContainsDiscard();
            }

            if (node->getCgbList()->numChangeables == 0) {
                return false;
            } else if (node->getCgbList()->numChangeables == 1) {
                ShChangeableIndex *cgbIdx;

                TIntermAggregate* agg = NULL;
                if (node->getRight() &&
                    node->getRight()->getAsAggregate() &&
                    node->getRight()->getAsAggregate()->getOp() == EOpSwizzles) {
                    agg = node->getRight()->getAsAggregate();
                }
                if (!agg) {
                    dbgPrint(DBGLVL_ERROR, "DebugVar - could not get swizzle aggregate\n");
                    exit(1);
                }

                int index = 0;
                int i = 0;

                TIntermSequence sequence;
                TIntermSequence::reverse_iterator rit;
                sequence = agg->getSequence();

                for(rit = sequence.rbegin(); rit != sequence.rend(); rit++) {
                    /*
                    index += ((int)pow(10, i)) *
                            (1+(*rit)->getAsConstantUnion()->getUnionArrayPointer()[0].getIConst());
                    */
                    index += 1 << ((*rit)->getAsConstantUnion()->getUnionArrayPointer()[0].getIConst());
                    i++;
                }

                cgbIdx = createShChangeableIndex(SH_CGB_SWIZZLE, index);
                addShIndexToChangeableList(node->getCgbList(), 0, cgbIdx);
            } else {
                dbgPrint(DBGLVL_ERROR, "DebugVar - swizzle to more than one changeable?\n");
                exit(1);
            }
            return false;
            break;
        case EOpIndexDirect:
        case EOpIndexDirectStruct:
        case EOpIndexIndirect:
            VPRINT(2, "(%s) changeIndex %i\n", FormatSourceRange(node->getRange()).c_str(), node->getOp());
            // process left branch first
            if (node->getLeft()) {
                node->getLeft()->traverse(it);
            }

            // then copy changeables
            // should be one or zero, otherwise we have a problem
            copyShChangeableList(node->getCgbList(),
                                node->getLeft()->getCgbList());

            // emitVertex push
            if (node->getLeft()->containsEmitVertex()) {
                node->setEmitVertex();
            }
            // discard push
            if (node->getLeft()->containsDiscard()) {
                node->setContainsDiscard();
            }

            if (node->getCgbList()->numChangeables == 0) {
                return false;
            } else if (node->getCgbList()->numChangeables == 1) {
                ShChangeableIndex *cgbIdx;
                ShChangeableType type;

                switch (node->getOp()) {
                    case EOpIndexDirect:
                        type = SH_CGB_ARRAY_DIRECT;
                        break;
                    case EOpIndexDirectStruct:
                        type = SH_CGB_STRUCT;
                        break;
                    case EOpIndexIndirect:
                        type = SH_CGB_ARRAY_INDIRECT;
                        break;
                    default:
                        dbgPrint(DBGLVL_ERROR, "DebugVar - invalid node op\n");
                        exit(1);
                }

                int index;
                if (node->getRight() &&
                    node->getRight()->getAsConstantUnion() &&
                    node->getRight()->getAsConstantUnion()->getUnionArrayPointer() &&
                    node->getRight()->getAsConstantUnion()->getType().getObjectSize() == 1 &&
                    (node->getRight()->getAsConstantUnion()->getUnionArrayPointer()[0].getType() == EbtInt ||
                    node->getRight()->getAsConstantUnion()->getUnionArrayPointer()[0].getType() == EbtUInt ||
                    node->getRight()->getAsConstantUnion()->getUnionArrayPointer()[0].getType() == EbtSwizzle)) {

                    index = node->getRight()->getAsConstantUnion()->getUnionArrayPointer()[0].getIConst();
                } else {
                    index = -1;
                }
                cgbIdx = createShChangeableIndex(type, index);
                addShIndexToChangeableList(node->getCgbList(), 0, cgbIdx);
            } else {
                dbgPrint(DBGLVL_WARNING, "DebugVar - index to more than one changeable?\n");
                /*exit(1);*/
            }
            return false;
        default:
            VPRINT(2, "(%s) changeAssigment\n", FormatSourceRange(node->getRange()).c_str());
            //dumpShChangeableList(node->getCgbList());
            oit->deactivate();
            if (node->getLeft()) {
                node->getLeft()->traverse(it);
                // copy the changeables of left branch
                copyShChangeableList(node->getCgbList(),
                                    node->getLeft()->getCgbList());

                // emitVertex push
                if (node->getLeft()->containsEmitVertex()) {
                    node->setEmitVertex();
                }
                // discard push
                if (node->getLeft()->containsDiscard()) {
                    node->setContainsDiscard();
                }
            }
            if (node->getRight()) {
                node->getRight()->traverse(it);
                // copy the changeables of right branch
                copyShChangeableList(node->getCgbList(),
                                    node->getRight()->getCgbList());
                //dumpShChangeableList(node->getCgbList());

                // emitVertex push
                if (node->getRight()->containsEmitVertex()) {
                    node->setEmitVertex();
                }
                // discard push
                if (node->getRight()->containsDiscard()) {
                    node->setContainsDiscard();
                }
            }
            return true;
    }
    return true;
}

static bool TraverseChangeDeclaration(TIntermDeclaration* node,
                                    TIntermTraverser* it)
{
    //TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    if (node->getInitialization()) {
        VPRINT(2, "(%s) changeDeclaration -> begin\n", FormatSourceRange(node->getRange()).c_str());
        //dumpShChangeableList(node->getCgbList());

        node->getInitialization()->traverse(it);

        // copy the changeables of initialization
        copyShChangeableList(node->getCgbList(),
                node->getInitialization()->getCgbList());

        VPRINT(2, "(%s) changeDeclaration -> end\n", FormatSourceRange(node->getRange()).c_str());

        // emitVertex push
        if (node->getInitialization()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getInitialization()->containsDiscard()) {
            node->setContainsDiscard();
        }

        //dumpShChangeableList(node->getCgbList());
        return false;
    }

    return true;
}

static void TraverseChangeSymbol(TIntermSymbol* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    if (oit->isActive()) {
        if (node->getCgbList()->numChangeables == 0) {
            VPRINT(2, "(%s) changeSymbol -> created %i %s\n",
                    FormatSourceRange(node->getRange()).c_str(), node->getId(), node->getSymbol().c_str());
            addShChangeable(node->getCgbList(), createShChangeable(node->getId()));
        } else {
            VPRINT(2, "(%s) changeSymbol -> kept %i %s\n",
                    FormatSourceRange(node->getRange()).c_str(), node->getId(), node->getSymbol().c_str());
        }
    }
}

static void TraverseChangeFuncParam(TIntermFuncParam* node, TIntermTraverser* it)
{
    UNUSED_ARG(it)
    //TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    if (node->getType().getQualifier() == EvqIn ||
        node->getType().getQualifier() == EvqConstIn || /* check back with Magnus */
        node->getType().getQualifier() == EvqInOut) {

        if (node->getCgbList()->numChangeables == 0) {
            VPRINT(2, "(%s) changeFuncParam -> created %i %s\n",
                    FormatSourceRange(node->getRange()).c_str(), node->getId(), node->getSymbol().c_str());
            addShChangeable(node->getCgbList(), createShChangeable(node->getId()));
        } else {
            VPRINT(2, "(%s) changeFuncParam -> kept %i %s\n",
                    FormatSourceRange(node->getRange()).c_str(), node->getId(), node->getSymbol().c_str());
        }
    }
}

static bool TraverseChangeUnary(bool, TIntermUnary* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    switch (node->getOp()) {
        case EOpPostIncrement:
        case EOpPostDecrement:
        case EOpPreIncrement:
        case EOpPreDecrement:
            // activate and traverse and copy result
            oit->activate();
            node->getOperand()->traverse(it);
            oit->deactivate();
            copyShChangeableList(node->getCgbList(),
                                node->getOperand()->getCgbList());
            // emitVertex push
            if (node->getOperand()->containsEmitVertex()) {
                node->setEmitVertex();
            }
            // discard push
            if (node->getOperand()->containsDiscard()) {
                node->setContainsDiscard();
            }

            return false;
        default:
            // simply traverse and copy result
            oit->deactivate();
            node->getOperand()->traverse(it);
            copyShChangeableList(node->getCgbList(),
                                node->getOperand()->getCgbList());
            // emitVertex push
            if (node->getOperand()->containsEmitVertex()) {
                node->setEmitVertex();
            }
            // discard push
            if (node->getOperand()->containsDiscard()) {
                node->setContainsDiscard();
            }

            return true;
    }
}

static bool TraverseChangeSelection(bool, TIntermSelection* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    // just traverse and copy changeables all together
    oit->deactivate();
    node->getCondition()->traverse(it);
    copyShChangeableList(node->getCgbList(),
                        node->getCondition()->getCgbList());
    if (node->getTrueBlock()) {
        node->getTrueBlock()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getTrueBlock()->getCgbList());
        // emitVertex push
        if (node->getTrueBlock()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getTrueBlock()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }
    if (node->getFalseBlock()) {
        node->getFalseBlock()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getFalseBlock()->getCgbList());
        // emitVertex push
        if (node->getFalseBlock()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getFalseBlock()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}

static bool TraverseChangeSwitch(bool, TIntermSwitch* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    // just traverse and copy changeables all together
    oit->deactivate();
    node->getCondition()->traverse(it);
    copyShChangeableList(node->getCgbList(),
                        node->getCondition()->getCgbList());
    if (node->getCaseList()) {
        node->getCaseList()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getCaseList()->getCgbList());
        // emitVertex push
        if (node->getCaseList()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getCaseList()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}

static bool TraverseChangeCase(bool, TIntermCase* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    // just traverse and copy changeables all together
    oit->deactivate();
    if (node->getExpression()) {
        node->getExpression()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getExpression()->getCgbList());
        // emitVertex push
        if (node->getExpression()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getExpression()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    if (node->getCaseBody()) {
        node->getCaseBody()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getCaseBody()->getCgbList());
        // emitVertex push
        if (node->getCaseBody()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getCaseBody()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}

static bool TraverseChangeLoop(bool, TIntermLoop* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    // just traverse and copy changeables all together
    oit->deactivate();

    // visit optional initialization
    if (node->getInit()) {
        node->getInit()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getInit()->getCgbList());
        // emitVertex push
        if (node->getInit()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getInit()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    // visit test, this should not change the changeables, but to be sure
    if (node->getTest()) {
        node->getTest()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getTest()->getCgbList());
        // emitVertex push
        if (node->getTest()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getTest()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    // visit optional terminal, this cannot change the changeables either
    if (node->getTerminal()) {
        node->getTerminal()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getTerminal()->getCgbList());
        // emitVertex push
        if (node->getTerminal()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getTerminal()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    // visit body
    if (node->getBody()) {
        node->getBody()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getBody()->getCgbList());
        // emitVertex push
        if (node->getBody()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getBody()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}

static bool TraverseChangeBranch(bool, TIntermBranch* node, TIntermTraverser* it)
{
    TOutputDebugCgbTraverser *oit = static_cast<TOutputDebugCgbTraverser*>(it);

    if (node->getExpression()) {
        // simply traverse and copy result
        oit->deactivate();
        node->getExpression()->traverse(it);
        copyShChangeableList(node->getCgbList(),
                            node->getExpression()->getCgbList());

        // emitVertex push
        if (node->getExpression()->containsEmitVertex()) {
            node->setEmitVertex();
        }
        // discard push
        if (node->getExpression()->containsDiscard()) {
            node->setContainsDiscard();
        }
    }

    return false;
}


//
//  Generate code from the given parse tree
//
bool TOutputDebugVarCompiler::compile(TIntermNode *root)
{
    /* Check for empty parse tree */
    if (root == 0) {
        return 0;
    }

    //
    // Fill exernal variable list and determine scope
    //
    VPRINT(2, "==Processing=Variables=============================================\n");
    TOutputDebugVarTraverser it(infoSink, vl);
    it.visitAggregate = TraverseVarAggregate;
    it.visitBinary = TraverseVarBinary;
    it.visitUnary = TraverseVarUnary;
    it.visitDeclaration = TraverseVarDeclaration;
    it.visitSelection = TraverseVarSelection;
    it.visitSwitch = TraverseVarSwitch;
    it.visitCase = TraverseVarCase;
    it.visitLoop = TraverseVarLoop;
    it.visitBranch = TraverseVarBranch;
    it.visitFuncParam = TraverseVarFuncParam;
    it.visitDummy = TraverseVarDummy;
    it.preVisit = true;
    it.postVisit = false;
    it.debugVisit = false;
    it.rightToLeft = false;
    root->traverse(&it);
    VPRINT(2, "==Processing=Variables=done========================================\n");


    // Now check which variables get altered at target and also
    // insert this information at end of sequences to be able to keep track
    // all changes made in functions and branches
    VPRINT(2, "==Processing=Changes===============================================\n");
    TOutputDebugCgbTraverser itChange(infoSink);
    itChange.root = root;
    itChange.visitAggregate = TraverseChangeAggregate;
    itChange.visitBinary = TraverseChangeBinary;
    itChange.visitDeclaration = TraverseChangeDeclaration;
    itChange.visitSymbol = TraverseChangeSymbol;
    itChange.visitFuncParam = TraverseChangeFuncParam;
    itChange.visitUnary = TraverseChangeUnary;
    itChange.visitSelection = TraverseChangeSelection;
    itChange.visitSwitch = TraverseChangeSwitch;
    itChange.visitCase = TraverseChangeCase;
    itChange.visitLoop = TraverseChangeLoop;
    itChange.visitBranch = TraverseChangeBranch;
    itChange.preVisit = true;
    itChange.postVisit = false;
    itChange.debugVisit = false;
    itChange.rightToLeft = false;
    root->traverse(&itChange);
    VPRINT(2, "==Processing=Changes=done==========================================\n");
    return 1;
}

