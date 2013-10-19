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
#include <stdio.h>
#include <stdlib.h>
#include "glsl/ir.h"
#include "glsl/list.h"
#include "mesa-glsl/glslang/Include/ShHandle.h"
#include "mesa-glsl/glslang/Public/ShaderLang.h"
#include "mesa-glsl/glslang/Include/intermediate.h"

#include "MShader.h"
#include "CodeInsertion.h"
#include "CodeTools.h"
#include "IRScope.h"
#include "Visitors/output.h"
#include "Visitors/stacktraverser.h"

#include "glsldb/utils/dbgprint.h"

#define DBG_TEXT_BEGIN "\x1B[1;31mgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0)\x1B[0;31m"
#define DBG_TEXT_END "\x1B[0m"

#define EMIT_VERTEX_SIG   "EmitVertex("
#define END_PRIMITIVE_SIG "EndPrimitive("

//
// Here is where real machine specific high-level data would be defined.
//

//class TGenericCompiler : public TCompiler {
//public:
//    TGenericCompiler(EShLanguage l, int dOptions) : TCompiler(l, infoSink), debugOptions(dOptions) { }
//    // generate original source code
//    virtual bool compile(TIntermNode* root);
//    // generate debugged source code
//    virtual bool compileDbg(TIntermNode* root, ShChangeableList *cgbl,
//            ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code);
//    TInfoSink infoSink;
//    int debugOptions;
//protected:
//    TString m_debugProgram;
//
//};


//static TString getBehaviorCode(TBehavior b)
//{
//    switch (b) {
//        case EBhRequire:
//            return TString("require");
//        case EBhEnable:
//            return TString("enable");
//        case EBhWarn:
//            return TString("warn");
//        case EBhDisable:
//            return TString("disable");
//        default:
//            return TString("");
//    }
//}

//
// This function must be provided to create the actual
// compile object used by higher level code.  It returns
// a subclass of TCompiler.
//
//TCompiler* ConstructCompiler(EShLanguage language, int debugOptions)
//{
//    return new TGenericCompiler(language, debugOptions);
//}

//
// Delete the compiler made by ConstructCompiler
//
//void DeleteCompiler(TCompiler* compiler)
//{
//    /* Clean the tree */
//    if (compiler->getParseContext()) {
//        TIntermediate intermediate(compiler->infoSink);
//        if (compiler->getParseContext()->treeRoot) {
//            intermediate.remove(compiler->getParseContext()->treeRoot);
//            compiler->getParseContext()->treeRoot = NULL;
//        }
//    }
//    delete compiler;
//}

//class TOutputTraverser : public TIntermTraverser {
//public:
//    TOutputTraverser(struct gl_shader* sh, ir_instruction *r, TString &dbgProgram,  EShLanguage l,
//    		ShVariableList *list, ShChangeableList *t, IRGenStack *s)
//        : shader(sh), root(r), debugProgram(dbgProgram), language(l), vl(list),
//          cgbl(t), dbgStack(s), dbgTargetProcessed(false), sequenceNoOperation(false),
//          ignoreNextIdentation(false), sequenceUseComma(false), sequencePrintClosure(true)
//        { }
////    TParseContext *parseContext;
//    TInfoSink &infoSink;
//    TString &debugProgram;
//    DbgCgOptions cgOptions;
//    struct gl_shader* shader;
//    ir_instruction *root;
//    EShLanguage language;
//    ShVariableList *vl;
//    ShChangeableList *cgbl;
//    IRGenStack *dbgStack;
//    bool dbgTargetProcessed;
//    bool sequenceNoOperation;
//    bool ignoreNextIdentation;
//    bool sequenceUseComma;
//    bool sequencePrintClosure;
//};
//
//static void outputIndentation(TOutputTraverser* oit, const int depth)
//{
//    int i;
//
//    if (oit->ignoreNextIdentation) {
//        oit->ignoreNextIdentation = false;
//    } else {
//        for (i = 0; i < depth; ++i)
//            oit->debugProgram += "   ";
//    }
//}
//
//static void outputExtensions(ir_instruction *node, TOutputTraverser* oit)
//{
//	// TODO: extensions
//    TExtensionList &list = node->getCPPExtensionList();
//    TExtensionList::iterator iter;
//
//    if (!list.empty()) {
//        oit->debugProgram += "\n";
//        for (iter = list.begin(); iter != list.end(); iter++) {
//            oit->debugProgram += "#extension ";
//            oit->debugProgram += iter->first;
//            oit->debugProgram += " : ";
//            oit->debugProgram += getBehaviorCode(iter->second);
//            oit->debugProgram += "\n";
//        }
//    }
//}
//
//static void processAggregateChildren(TIntermAggregate* node, TIntermTraverser* it, const char* initation, const char* seperator, const char* completion)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    TIntermSequence sequence = node->getSequence();
//    TIntermSequence::iterator sit;
//
//
//    oit->debugProgram += initation;
//
//    if (it->rightToLeft) {
//        sit = sequence.end();
//        while (sit != sequence.begin()) {
//            --sit;
//            (*sit)->traverse(it);
//            if (sit != sequence.begin()) {
//                oit->debugProgram += seperator;
//            }
//        }
//    } else {
//        for (sit = sequence.begin();
//                sit != sequence.end(); ++sit) {
//            (*sit)->traverse(it);
//            if (sit+1 != sequence.end()) {
//                oit->debugProgram += seperator;
//            }
//        }
//    }
//
//    oit->debugProgram += completion;
//}
//
//static bool OutputDeclarationDebugged(TIntermDeclaration* node, TIntermTraverser* it);
//static bool OutputDeclaration(TIntermDeclaration* node, TIntermTraverser* it);
//
//static void processAggregateSequence(TIntermAggregate* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    TIntermSequence sequence = node->getSequence();
//    TIntermSequence::iterator sit;
//
//    if (it->rightToLeft) {
//        sit = sequence.end();
//        while (sit != sequence.begin()) {
//            --sit;
//            if ( (*sit)->getAsDummy() == 0) {
//                outputIndentation(oit, oit->depth);
//            }
//            (*sit)->traverse(it);
//            if ( (*sit)->getAsDummy() != 0) {
//
//            } else if ( (*sit)->getAsAggregate() != 0 &&
//                      ( ((*sit)->getAsAggregate()->getOp() == EOpSequence) ||
//                        ((*sit)->getAsAggregate()->getOp() == EOpFunction)))
//            {
//                oit->debugProgram += "\n";
//            } else if ( ((*sit)->getAsLoopNode() == 0) &&
//                        ((*sit)->getAsSwitchNode() == 0) &&
//                        ((*sit)->getAsCaseNode() == 0) &&
//                        (!(((*sit)->getAsSelectionNode() != 0) &&
//                           !((*sit)->getAsSelectionNode()->isShort())) ) ) {
//                if (oit->sequenceNoOperation) {
//                    oit->sequenceNoOperation  = false;
//                    oit->ignoreNextIdentation = true;
//                } else {
//                    if (oit->sequencePrintClosure ||
//                        sit != sequence.begin()) {
//                        if (oit->sequenceUseComma) {
//                            oit->debugProgram += ", ";
//                        } else {
//                            oit->debugProgram += ";\n";
//                        }
//                    }
//                }
//            }
//        }
//    } else {
//        for (sit = sequence.begin();
//                sit != sequence.end(); ++sit) {
//            if ( (*sit)->getAsDummy() == 0) {
//                outputIndentation(oit, oit->depth);
//            }
//            (*sit)->traverse(it);
//            if ( (*sit)->getAsDummy() != 0) {
//
//            } else if ( (*sit)->getAsAggregate() != 0 &&
//                        ( ((*sit)->getAsAggregate()->getOp() == EOpSequence) ||
//                          ((*sit)->getAsAggregate()->getOp() == EOpFunction)))
//            {
//                // Nested aggregate blocks do not need a semicolon
//                oit->debugProgram += "\n";
//            } else if ( ((*sit)->getAsLoopNode() == 0) &&
//                        ((*sit)->getAsSwitchNode() == 0) &&
//                        ((*sit)->getAsCaseNode() == 0) &&
//                           (!(((*sit)->getAsSelectionNode() != 0) &&
//                            !((*sit)->getAsSelectionNode()->isShort())) ) ) {
//                //
//                if (oit->sequenceNoOperation) {
//                    oit->sequenceNoOperation  = false;
//                    oit->ignoreNextIdentation = true;
//                } else {
//                    if (oit->sequencePrintClosure ||
//                        distance(sit, sequence.end()) > 1) {
//
//                        if (oit->sequenceUseComma) {
//                            oit->debugProgram += ", ";
//                        } else {
//                            oit->debugProgram += ";\n";
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
//
//static bool OutputAggregate(bool, TIntermAggregate* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    TInfoSink& out = oit->infoSink;
//
//    TIntermSequence sequence = node->getSequence();
//    TIntermSequence::iterator sit;
//
//    if (node->getOp() == EOpNull) {
//        out.debug.message(EPrefixError, "node is still EOpNull!");
//        return true;
//    }
//
//    outputExtensions(node, oit);
//
//    switch (node->getOp()) {
//        case EOpSequence:
//            if (node != oit->root) {
//                /* Only add brackets if it's not root node */
//                oit->debugProgram += "{\n";
//                oit->depth++;
//
//                /* If in debug mode add initialization to main function */
//                if (oit->cgOptions != DBG_CG_ORIGINAL_SRC &&
//                    isChildofMain(node, oit->root)) {
//                    switch(oit->cgOptions) {
//                        case DBG_CG_CHANGEABLE:
//                        case DBG_CG_SELECTION_CONDITIONAL:
//                        case DBG_CG_LOOP_CONDITIONAL:
//                        case DBG_CG_COVERAGE:
//                            outputIndentation(oit, oit->depth);
//                            cgAddInitialization(CG_TYPE_RESULT, CG_INIT_BLACK,
//                                                oit->debugProgram, oit->language);
//                            oit->debugProgram += ";\n";
//                            break;
//                        case DBG_CG_VERTEX_COUNT:
//                        case DBG_CG_GEOMETRY_MAP:
//                            outputIndentation(oit, oit->depth);
//                            cgAddInitialization(CG_TYPE_RESULT, CG_INIT_GEOMAP,
//                                                oit->debugProgram, oit->language);
//                            oit->debugProgram += ";\n";
//                            break;
//                        case DBG_CG_GEOMETRY_CHANGEABLE:
//                            outputIndentation(oit, oit->depth);
//                            cgAddInitialization(CG_TYPE_RESULT, CG_INIT_WHITE,
//                                                oit->debugProgram, oit->language);
//                            oit->debugProgram += ";\n";
//                            break;
//                        default:
//                            break;
//                    }
//                }
//            }
//
//            processAggregateSequence(node, it);
//
//            /* Special case for root node */
//            if (node != oit->root) {
//                /* And also add debug output at the end */
//                if (oit->cgOptions != DBG_CG_ORIGINAL_SRC &&
//                    isChildofMain(node, oit->root)) {
//
//                    switch (oit->language) {
//                        case EShLangVertex:
//                            /* No output needed */
//                            break;
//                        case EShLangGeometry:
//                            if (oit->cgOptions == DBG_CG_CHANGEABLE) {
//                                outputIndentation(oit, oit->depth);
//                                cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                            } else if (oit->cgOptions == DBG_CG_COVERAGE) {
//                                outputIndentation(oit, oit->depth);
//                                cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                            } else if (oit->cgOptions == DBG_CG_SELECTION_CONDITIONAL || oit->cgOptions == DBG_CG_LOOP_CONDITIONAL) {
//                                outputIndentation(oit, oit->depth);
//                                cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                            } else if (oit->cgOptions == DBG_CG_VERTEX_COUNT) {
//                                outputIndentation(oit, oit->depth);
//                                cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                            }
//                            break;
//                        case EShLangFragment:
//                            outputIndentation(oit, oit->depth);
//                            cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language,
//                                        oit->parseContext->fragmentShaderOutput);
//                            break;
//                        default:
//                            break;
//                    }
//                }
//
//                oit->depth--;
//                outputIndentation(oit, oit->depth);
//                oit->debugProgram += "}\n";
//            }
//            return false;
//            break;
//        case EOpDeclaration:
//            if ((oit->cgOptions != DBG_CG_ORIGINAL_SRC) &&
//                (node->getDebugState() == DbgStPath)) {
//                it->visitDeclaration = OutputDeclarationDebugged;
//            }
//
//            if (it->rightToLeft) {
//                // TODO: not the same than in other case???
//                exit(1);
//                sit = sequence.end();
//                while (sit != sequence.begin()) {
//                    --sit;
//                    (*sit)->traverse(it);
//                    if (sit != sequence.begin()) {
//                        oit->debugProgram += ", ";
//                    }
//                }
//            } else {
//                for (sit = sequence.begin();
//                        sit != sequence.end(); ++sit) {
//                    (*sit)->traverse(it);
//                    if ( ((*sit)->getAsAggregate() != 0) &&
//                         ((*sit)->getAsAggregate()->getOp() == EOpSpecification) ) {
//                        dbgPrint(DBGLVL_ERROR, "CodeGen - unexpected specification aggregate found!\n");
//                        exit(1);
//                        oit->debugProgram += " ";
//                    } else if ( sit+1 != sequence.end()) {
//                        if (node->getDebugState() == DbgStNone ||
//                            oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                            oit->debugProgram += ", ";
//                        } else {
//                            oit->debugProgram += ";\n";
//                            outputIndentation(oit, oit->depth);
//                        }
//                    }
//                }
//            }
//            if ((oit->cgOptions != DBG_CG_ORIGINAL_SRC) &&
//                (node->getDebugState() == DbgStPath)) {
//                it->visitDeclaration = OutputDeclaration;
//            }
//            return false;
//            break;
//        case EOpSpecification:
//            processAggregateChildren(node, it, "", "", "");
//            return false;
//            break;
//        case EOpParameter:
//            if (it->rightToLeft) {
//                sit = sequence.end();
//                while (sit != sequence.begin()) {
//                    --sit;
//                    outputIndentation(oit, oit->depth);
//                    (*sit)->traverse(it);
//                    if (sit+1 != sequence.end() &&
//                        (*sit)->getAsSpecificationNode() == 0) {
//                        oit->debugProgram += ";\n";
//                    }
//                    if ((*sit)->getAsSpecificationNode() != 0) {
//                        outputIndentation(oit, oit->depth);
//                        oit->debugProgram += "}";
//                    } else if (sit+1 != sequence.end()) {
//                    } else {
//                        oit->debugProgram += ";\n";
//                    }
//                }
//            } else {
//                for (sit = sequence.begin();
//                        sit != sequence.end(); ++sit) {
//                    outputIndentation(oit, oit->depth);
//                    (*sit)->traverse(it);
//                    if (sit+1 != sequence.end() &&
//                        (*sit)->getAsSpecificationNode() == 0) {
//                        oit->debugProgram += ";\n";
//                    }
//                    if ((*sit)->getAsSpecificationNode() != 0) {
//                        outputIndentation(oit, oit->depth);
//                        oit->debugProgram += "}";
//                    } else if (sit+1 != sequence.end()) {
//                    } else {
//                        oit->debugProgram += ";\n";
//                    }
//                }
//            }
//            return false;
//            break;
//        case EOpComma:
//            if (it->rightToLeft) {
//                sit = sequence.end();
//                while (sit != sequence.begin()) {
//                    --sit;
//                    (*sit)->traverse(it);
//                    if (sit+1 != sequence.end()) {
//                        oit->debugProgram += ", ";
//                    }
//                }
//            } else {
//                for (sit = sequence.begin(); sit != sequence.end(); ++sit) {
//                    (*sit)->traverse(it);
//                    if (sit+1 != sequence.end()) {
//                        oit->debugProgram += ", ";
//                    }
//                }
//            }
//            return false;
//            break;
//        case EOpFunction:
//            oit->debugProgram +=
//                node->getTypePointer()->getCodeString(true, oit->language);
//            if (node->getType().isArray()) {
//                char buf[300];
//                int i;
//                for (i=0; i<node->getType().getNumArrays(); i++) {
//                    oit->debugProgram += "[";
//                    if (node->getType().getArraySize(i) != 0) {
//                        sprintf(buf, "%i", node->getType().getArraySize(i));
//                        oit->debugProgram += buf;
//                    }
//                    oit->debugProgram += "]";
//                }
//            }
//            oit->debugProgram += " ";
//
//            /* Check if 'main' is processed */
//            if (!strcmp(node->getName().c_str(), MAIN_FUNC_SIGNATURE)) {
//                oit->debugProgram += getFunctionName(node->getName());
//                processAggregateChildren(node, it, "(", "", "");
//            } else {
//                /* Double function if this is on path.
//                 * This is done to make sure that only the debugged path is calling
//                 * a function with inserted debug code */
//                if ((oit->cgOptions != DBG_CG_ORIGINAL_SRC) &&
//                    (node->getDebugState() == DbgStPath) &&
//                    (node->getDbgOverwrite() != DbgOwOriginalCode)) {
//                    /* Add debugged function */
//                    oit->debugProgram += cgGetDebugName(node->getName().c_str(),
//                                                        oit->root);
//                    processAggregateChildren(node, it, "(", "", "");
//
//                    /* Add normal function */
//                    DbgCgOptions option = oit->cgOptions;
//                    oit->cgOptions = DBG_CG_ORIGINAL_SRC;
//                    oit->debugProgram +=
//		                node->getTypePointer()->getCodeString(true, oit->language);
//                    if (node->getType().isArray()) {
//                        char buf[300];
//                        int i;
//                        for (i=0; i<node->getType().getNumArrays(); i++) {
//                            oit->debugProgram += "[";
//                            if (node->getType().getArraySize(i) != 0) {
//                                sprintf(buf, "%i", node->getType().getArraySize(i));
//                                oit->debugProgram += buf;
//                            }
//                            oit->debugProgram += "]";
//                        }
//                    }
//                    oit->debugProgram += " ";
//                    oit->debugProgram += getFunctionName(node->getName());
//                    processAggregateChildren(node, it, "(", "", "");
//                    oit->cgOptions = option;
//                } else {
//                    oit->debugProgram += getFunctionName(node->getName());
//                    processAggregateChildren(node, it, "(", "", "");
//                }
//            }
//            return false;
//            break;
//        case EOpFunctionCall:
//            if (oit->cgOptions != DBG_CG_ORIGINAL_SRC &&
//                node->isTarget()) {
//
//                oit->dbgTargetProcessed = true;
//
//                /* This function call acts as target */
//
//                /* Check if we can use a parameter for debugging */
//                TIntermNode *funcDec = getFunctionBySignature(node->getName().c_str(),
//                                                              oit->root);
//                if (!funcDec) {
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - could not find function declaration\n");
//                    exit(1);
//                }
//
//                int lastInParameter = getFunctionDebugParameter(
//                        funcDec->getAsAggregate());
//
//                if (lastInParameter >= 0) {
//                    /* we found a usable parameter */
//                    oit->debugProgram += getFunctionName(node->getName());
//
//                    TIntermSequence sequence = node->getSequence();
//                    TIntermSequence::iterator sit;
//
//                    oit->debugProgram += "(";
//
//                    int i;
//
//                    for (sit = sequence.begin(),i=0; sit != sequence.end(); ++sit,++i)
//                    {
//                        if (i == lastInParameter) {
//
//                            if (!getHasSideEffectsDebugParameter(
//                                        node->getAsAggregate(), lastInParameter)) {
//                                /* No special care necessary, just add it before */
//                                oit->debugProgram += "(";
//                                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                                             oit->cgOptions, oit->cgbl, oit->vl,
//                                             oit->dbgStack, 0);
//                                oit->debugProgram += ", ";
//                                (*sit)->traverse(it);
//                                oit->debugProgram += ")";
//                            } else {
//                                /* Copy to temporary, debug, and copy back */
//                                oit->debugProgram += "(";
//                                cgAddDbgCode(CG_TYPE_PARAMETER, oit->debugProgram,
//                                             oit->cgOptions, oit->cgbl, oit->vl,
//                                             oit->dbgStack, 0);
//                                oit->debugProgram += " = ";
//                                if (!((*sit)->isAtomic())) {
//                                    oit->debugProgram += "(";
//                                }
//                                (*sit)->traverse(it);
//                                if (!((*sit)->isAtomic())) {
//                                    oit->debugProgram += ")";
//                                }
//                                oit->debugProgram += ", ";
//                                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                                             oit->cgOptions, oit->cgbl, oit->vl,
//                                             oit->dbgStack, 0);
//                                oit->debugProgram += ", ";
//                                cgAddDbgCode(CG_TYPE_PARAMETER, oit->debugProgram,
//                                             oit->cgOptions, oit->cgbl, oit->vl,
//                                             oit->dbgStack, 0);
//                                oit->debugProgram += ")";
//                            }
//                        } else {
//                            (*sit)->traverse(it);
//                        }
//
//                        if (sit+1 != sequence.end()) {
//                            oit->debugProgram += ", ";
//                        }
//                    }
//                    oit->debugProgram += ")";
//                } else {
//                    /* no usable parameter, so debug before function call */
//                    oit->debugProgram += "(";
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                 oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    oit->debugProgram += getFunctionName(node->getName());
//                    processAggregateChildren(node, it, "(", ", ", ")");
//                    oit->debugProgram += ")";
//                }
//            } else if (oit->cgOptions != DBG_CG_ORIGINAL_SRC &&
//                       node->getDebugState() == DbgStTarget &&
//                       node->getDbgOverwrite() != DbgOwOriginalCode) {
//                /* This call leads to the actual prosition of debugging */
//                oit->debugProgram += cgGetDebugName(node->getName().c_str(),
//                                                    oit->root);
//                processAggregateChildren(node, it, "(", ", ", ")");
//            } else if (oit->language == EShLangGeometry &&
//                       strcmp(node->getName().c_str(), EMIT_VERTEX_SIG) == 0 &&
//                       !(node->isUserDefined())) {
//                /* Special case for geometry shaders "EmitVertex()" */
//
//                switch (oit->cgOptions) {
//                    case DBG_CG_GEOMETRY_MAP:
//                        oit->debugProgram += getFunctionName(node->getName());
//                        processAggregateChildren(node, it, "(", ", ", ")");
//                        oit->debugProgram += ";\n";
//                        outputIndentation(oit, oit->depth);
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//                        break;
//                    case DBG_CG_GEOMETRY_CHANGEABLE:
//                        /* Check if changeable in scope here */
//                        if (oit->cgbl && oit->vl) {
//                            scopeList *sl = node->getScope();
//                            scopeList::iterator sit;
//
//                            int id;
//                            bool allInScope = true;
//
//                            for (id=0; id<oit->cgbl->numChangeables; id++) {
//                                bool inScope = false;
//
//                                /* builtins are always valid */
//                                ShVariable *var = findShVariableFromId(oit->vl, oit->cgbl->changeables[id]->id);
//                                if (!var) {
//                                    dbgPrint(DBGLVL_WARNING, "CodeGen - unkown changeable, stop debugging\n");
//                                    return false;
//                                }
//
//                                if (var->builtin) {
//                                    inScope = true;
//                                } else {
//                                    /* parse the actual scope */
//                                    for (sit = sl->begin(); sit != sl->end(); sit++) {
//                                        if ((*sit) == oit->cgbl->changeables[id]->id) {
//                                            inScope = true;
//                                        }
//                                    }
//                                }
//
//                                if (!inScope) {
//                                    allInScope = false;
//                                    break;
//                                }
//                            }
//
//                            if (allInScope) {
//                                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                             oit->cgbl, oit->vl, oit->dbgStack, CG_GEOM_CHANGEABLE_IN_SCOPE);
//                            } else {
//                                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                             oit->cgbl, oit->vl, oit->dbgStack, CG_GEOM_CHANGEABLE_NO_SCOPE);
//                            }
//                            oit->debugProgram += ";\n";
//                            outputIndentation(oit, oit->depth);
//                        }
//
//                        /* Add original function call */
//                        oit->debugProgram += getFunctionName(node->getName());
//                        processAggregateChildren(node, it, "(", ", ", ")");
//                        break;
//                    case DBG_CG_VERTEX_COUNT:
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                     oit->cgbl, oit->vl, oit->dbgStack, 0,
//                                     oit->parseContext->resources->geoOutputType);
//                        break;
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_CHANGEABLE:
//                    case DBG_CG_SELECTION_CONDITIONAL:
//                    case DBG_CG_LOOP_CONDITIONAL:
//                        oit->sequenceNoOperation = true;
//                        break;
//                    default:
//                        oit->debugProgram += getFunctionName(node->getName());
//                        processAggregateChildren(node, it, "(", ", ", ")");
//                        break;
//                }
//            } else if (oit->language == EShLangGeometry &&
//                       strcmp(node->getName().c_str(), END_PRIMITIVE_SIG) == 0 &&
//                       !(node->isUserDefined())) {
//                /* Special case for geometry shaders "EndPrimitive()" */
//                switch (oit->cgOptions) {
//                    case DBG_CG_GEOMETRY_MAP:
//                        oit->debugProgram += getFunctionName(node->getName());
//                        processAggregateChildren(node, it, "(", ", ", ")");
//                        oit->debugProgram += ";\n";
//                        outputIndentation(oit, oit->depth);
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                     oit->cgbl, oit->vl, oit->dbgStack, 1);
//                        break;
//                    case DBG_CG_VERTEX_COUNT:
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                     oit->cgbl, oit->vl, oit->dbgStack, 1,
//                                     oit->parseContext->resources->geoOutputType);
//                        break;
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_CHANGEABLE:
//                    case DBG_CG_SELECTION_CONDITIONAL:
//                    case DBG_CG_LOOP_CONDITIONAL:
//                        oit->sequenceNoOperation = true;
//                        break;
//                    default:
//                        oit->debugProgram += getFunctionName(node->getName());
//                        processAggregateChildren(node, it, "(", ", ", ")");
//                        break;
//                }
//            } else {
//                oit->debugProgram += getFunctionName(node->getName());
//                processAggregateChildren(node, it, "(", ", ", ")");
//            }
//            return false;
//            break;
//        case EOpParameters:
//            processAggregateChildren(node, it, "", ", ", ")\n");
//            outputIndentation(oit, oit->depth);
//            return false;
//            break;
//        case EOpInstances:
//            processAggregateChildren(node, it, "", ", ", "");
//            return false;
//            break;
//
//
//        case EOpConstructFloat:
//        case EOpConstructVec2:
//        case EOpConstructVec3:
//        case EOpConstructVec4:
//        case EOpConstructBool:
//        case EOpConstructBVec2:
//        case EOpConstructBVec3:
//        case EOpConstructBVec4:
//        case EOpConstructInt:
//        case EOpConstructUInt:
//        case EOpConstructIVec2:
//        case EOpConstructIVec3:
//        case EOpConstructIVec4:
//        case EOpConstructUVec2:
//        case EOpConstructUVec3:
//        case EOpConstructUVec4:
//        case EOpConstructMat2:
//        case EOpConstructMat2x3:
//        case EOpConstructMat2x4:
//        case EOpConstructMat3x2:
//        case EOpConstructMat3:
//        case EOpConstructMat3x4:
//        case EOpConstructMat4x2:
//        case EOpConstructMat4x3:
//        case EOpConstructMat4:
//            oit->debugProgram += node->getType().getCodeString(false, oit->language);
//            if (node->getType().isArray()) {
//                char buf[300];
//                int i;
//                for (i=0; i<node->getType().getNumArrays(); i++) {
//                    oit->debugProgram += "[";
//                    if (node->getType().getArraySize(i) != 0) {
//                        sprintf(buf, "%i", node->getType().getArraySize(i));
//                        oit->debugProgram += buf;
//                    }
//                    oit->debugProgram += "]";
//                }
//            }
//            processAggregateChildren(node, it, "(", ", ", ")");
//            return false;
//        case EOpConstructStruct:
//            {
//                if (node->getTypePointer()) {
//                    oit->debugProgram += node->getTypePointer()->getTypeName().c_str();
//                    if (node->getType().isArray()) {
//                        char buf[300];
//                        int i;
//                        for (i=0; i<node->getType().getNumArrays(); i++) {
//                            oit->debugProgram += "[";
//                            if (node->getType().getArraySize(i) != 0) {
//                                sprintf(buf, "%i", node->getType().getArraySize(i));
//                                oit->debugProgram += buf;
//                            }
//                            oit->debugProgram += "]";
//                        }
//                    }
//                    processAggregateChildren(node, it, "(", ", ", ")");
//                    return false;
//                } else {
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - processing EOpConstructStruct\n");
//                    return true;
//                }
//                break;
//            }
//        case EOpLessThan:
//            processAggregateChildren(node, it, "lessThan(", ", ", ")");
//            return false;
//            break;
//        case EOpGreaterThan:
//            processAggregateChildren(node, it, "greaterThan(", ", ", ")");
//            return false;
//            break;
//        case EOpLessThanEqual:
//            processAggregateChildren(node, it, "lessThanEqual(", ", ", ")");
//            return false;
//            break;
//        case EOpGreaterThanEqual:
//            processAggregateChildren(node, it, "greaterThanEqual(", ", ", ")");
//            return false;
//            break;
//        case EOpVectorEqual:
//            processAggregateChildren(node, it, "equal(", ", ", ")");
//            return false;
//            break;
//        case EOpVectorNotEqual:
//            processAggregateChildren(node, it, "notEqual(", ", ", ")");
//            return false;
//            break;
//
//        case EOpMod:
//            processAggregateChildren(node, it, "mod(", ", ", ")");
//            return false;
//            break;
//        case EOpPow:
//            processAggregateChildren(node, it, "pow(", ", ", ")");
//            return false;
//            break;
//
//        case EOpAtan:
//            processAggregateChildren(node, it, "atan(", ", ", ")");
//            return false;
//            break;
//
//        case EOpMin:
//            processAggregateChildren(node, it, "min(", ", ", ")");
//            return false;
//            break;
//        case EOpMax:
//            processAggregateChildren(node, it, "max(", ", ", ")");
//            return false;
//            break;
//        case EOpClamp:
//            processAggregateChildren(node, it, "clamp(", ", ", ")");
//            return false;
//            break;
//        case EOpMix:
//            processAggregateChildren(node, it, "mix(", ", ", ")");
//            return false;
//            break;
//        case EOpStep:
//            processAggregateChildren(node, it, "step(", ", ", ")");
//            return false;
//            break;
//        case EOpSmoothStep:
//            processAggregateChildren(node, it, "smoothstep(", ", ", ")");
//            return false;
//            break;
//        case EOpTruncate:
//            processAggregateChildren(node, it, "truncate(", ", ", ")");
//            return false;
//            break;
//        case EOpRound:
//            processAggregateChildren(node, it, "round(", ", ", ")");
//            return false;
//            break;
//
//        case EOpDistance:
//            processAggregateChildren(node, it, "distance(", ", ", ")");
//            return false;
//            break;
//        case EOpDot:
//            processAggregateChildren(node, it, "dot(", ", ", ")");
//            return false;
//            break;
//        case EOpCross:
//            processAggregateChildren(node, it, "cross(", ", ", ")");
//            return false;
//            break;
//        case EOpFaceForward:
//            processAggregateChildren(node, it, "faceforward(", ", ", ")");
//            return false;
//            break;
//        case EOpReflect:
//            processAggregateChildren(node, it, "reflect(", ", ", ")");
//            return false;
//            break;
//        case EOpRefract:
//            processAggregateChildren(node, it, "refract(", ", ", ")");
//            return false;
//            break;
//
//        case EOpMul:
//            processAggregateChildren(node, it, "matrixCompMult(", ", ", ")");
//            return false;
//            break;
//        case EOpMatrixOuterProduct:
//            processAggregateChildren(node, it, "outerProduct(", ", ", ")");
//            return false;
//            break;
//
//        case EOpSwizzles:
//            processAggregateChildren(node, it, "", "", "");
//            return false;
//            break;
//
//        default:
//            break;
//    }
//
//    return true;
//}
//
//
//static bool OutputBinary(bool /* preVisit */, TIntermBinary* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    if (node->isTarget() &&
//        oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//        oit->dbgTargetProcessed = true;
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += ", ";
//    }
//
//    if (!(node->getLeft()->isAtomic())) {
//        oit->debugProgram += "(";
//        node->getLeft()->traverse(it);
//        oit->debugProgram += ")";
//    } else {
//        node->getLeft()->traverse(it);
//    }
//
//    switch (node->getOp()) {
//        case EOpIndexDirect:
//            oit->debugProgram +=".";
//            break;
//        case EOpIndexIndirect:
//            oit->debugProgram += "[";
//            node->getRight()->traverse(it);
//            oit->debugProgram += "]";
//            break;
//        case EOpIndexDirectStruct:
//            oit->debugProgram +=".";
//
//            TTypeList* tl;
//            if (!(tl = node->getLeft()->getTypePointer()->getStruct())) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index to a non struct\n");
//                exit(1);
//            }
//            TIntermConstantUnion *cnode;
//            if (!(cnode = node->getRight()->getAsConstantUnion())) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index to is not constant union\n");
//                exit(1);
//            }
//            if (cnode->getType().getObjectSize() != 1) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index union not scalar\n");
//                exit(1);
//            }
//            if (!(cnode->getUnionArrayPointer()[0].getType() == EbtInt || cnode->getUnionArrayPointer()[0].getType() == EbtUInt)) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index union not of type int\n");
//                exit(1);
//            }
//            int idx;
//            idx = cnode->getUnionArrayPointer()[0].getIConst();
//
//            if ((int) tl->size() <= idx) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index out of range\n");
//                exit(1);
//            }
//            TType *t;
//            if (!(t = (*tl)[idx].type)) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - structure index type undefined\n");
//                exit(1);
//            }
//            oit->debugProgram += t->getFieldName().c_str();
//
//            break;
//        case EOpVectorSwizzle:
//            oit->debugProgram += ".";
//            break;
//        case EOpAssign:                   oit->debugProgram += " = ";  break;
//        case EOpAddAssign:                oit->debugProgram += " += "; break;
//        case EOpSubAssign:                oit->debugProgram += " -= "; break;
//        case EOpMulAssign:                oit->debugProgram += " *= "; break;
//        case EOpVectorTimesMatrixAssign:  oit->debugProgram += " *= "; break;
//        case EOpVectorTimesScalarAssign:  oit->debugProgram += " *= "; break;
//        case EOpMatrixTimesScalarAssign:  oit->debugProgram += " *= "; break;
//        case EOpMatrixTimesMatrixAssign:  oit->debugProgram += " *= "; break;
//        case EOpDivAssign:                oit->debugProgram += " /= "; break;
//        case EOpModAssign:                oit->debugProgram += " %= "; break;
//        case EOpAndAssign:                oit->debugProgram += " &= "; break;
//        case EOpInclusiveOrAssign:        oit->debugProgram += " |= "; break;
//        case EOpExclusiveOrAssign:        oit->debugProgram += " ^= "; break;
//        case EOpLeftShiftAssign:          oit->debugProgram += " <<= "; break;
//        case EOpRightShiftAssign:         oit->debugProgram += " >>= "; break;
//
//        case EOpAdd:                      oit->debugProgram += " + "; break;
//        case EOpSub:                      oit->debugProgram += " - "; break;
//        case EOpMul:                      oit->debugProgram += "*"; break;
//        case EOpDiv:                      oit->debugProgram += "/"; break;
//        case EOpMod:                      oit->debugProgram += "%"; break;
//        case EOpRightShift:               oit->debugProgram += " >> "; break;
//        case EOpLeftShift:                oit->debugProgram += " << "; break;
//        case EOpAnd:                      oit->debugProgram += " & "; break;
//        case EOpInclusiveOr:              oit->debugProgram += " | "; break;
//        case EOpExclusiveOr:              oit->debugProgram += " ^ "; break;
//        case EOpEqual:                    oit->debugProgram += " == "; break;
//        case EOpNotEqual:                 oit->debugProgram += " != "; break;
//        case EOpLessThan:                 oit->debugProgram += " < "; break;
//        case EOpGreaterThan:              oit->debugProgram += " > "; break;
//        case EOpLessThanEqual:            oit->debugProgram += " <= "; break;
//        case EOpGreaterThanEqual:         oit->debugProgram += " >= "; break;
//
//        case EOpVectorTimesScalar:        oit->debugProgram += " * "; break;
//        case EOpVectorTimesMatrix:        oit->debugProgram += " * "; break;
//        case EOpMatrixTimesVector:        oit->debugProgram += " * "; break;
//        case EOpMatrixTimesScalar:        oit->debugProgram += " * "; break;
//        case EOpMatrixTimesMatrix:        oit->debugProgram += " * "; break;
//
//        case EOpLogicalOr:                oit->debugProgram += " || "; break;
//        case EOpLogicalXor:               oit->debugProgram += " ^^ "; break;
//        case EOpLogicalAnd:               oit->debugProgram += " && "; break;
//        default:
//                                          dbgPrint(DBGLVL_ERROR, "CodeGen - OutputBinary: unknown operation %d\n", node->getOp());
//    }
//    if (node->getOp() != EOpIndexIndirect &&
//        node->getOp() != EOpIndexDirectStruct) {
//
//        if (!(node->getRight()->isAtomic())) {
//            oit->debugProgram += "(";
//            node->getRight()->traverse(it);
//            oit->debugProgram += ")";
//        } else {
//            node->getRight()->traverse(it);
//        }
//    }
//
//    /* We already processed all childs by our own */
//    return false;
//}
//
//static bool OutputUnary(bool /* preVisit */, TIntermUnary* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    TInfoSink& out = oit->infoSink;
//
//    outputExtensions(node, oit);
//
//    if (node->isTarget() &&
//        oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//        oit->dbgTargetProcessed = true;
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += ", ";
//    }
//
//    switch (node->getOp()) {
//    case EOpNegative:
//        oit->debugProgram += "-(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpVectorLogicalNot:
//    case EOpLogicalNot:
//    case EOpBitwiseNot:
//        oit->debugProgram += "!(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpPostIncrement:
//        oit->debugProgram += "";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += "++";
//        break;
//    case EOpPostDecrement:
//        oit->debugProgram += "";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += "--";
//        break;
//    case EOpPreIncrement:
//        oit->debugProgram += "++";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += "";
//        break;
//    case EOpPreDecrement:
//        oit->debugProgram += "--";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += "";
//        break;
//
//    case EOpConvIntToBool:
//    case EOpConvUIntToBool:
//    case EOpConvFloatToBool:
//    case EOpConvBoolToFloat:
//    case EOpConvIntToFloat:
//    case EOpConvUIntToFloat:
//    case EOpConvFloatToInt:
//    case EOpConvBoolToInt:
//    case EOpConvUIntToInt:
//    case EOpConvFloatToUInt:
//    case EOpConvBoolToUInt:
//    case EOpConvIntToUInt:
//        oit->debugProgram += node->getType().getCodeString(false, oit->language);
//        oit->debugProgram += "(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//
//    case EOpRadians:
//        oit->debugProgram += "radians(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpDegrees:
//        oit->debugProgram += "degrees(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpSin:
//        oit->debugProgram += "sin(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpCos:
//        oit->debugProgram += "cos(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpTan:
//        oit->debugProgram += "tan(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpAsin:
//        oit->debugProgram += "asin(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpAcos:
//        oit->debugProgram += "acos(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpAtan:
//        oit->debugProgram += "atan(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//    case EOpExp:
//        oit->debugProgram += "exp(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpLog:
//        oit->debugProgram += "log(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpExp2:
//        oit->debugProgram += "exp2(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpLog2:
//        oit->debugProgram += "log2(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpSqrt:
//        oit->debugProgram += "sqrt(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpInverseSqrt:
//        oit->debugProgram += "inversesqrt(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//    case EOpAbs:
//        oit->debugProgram += "abs(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpSign:
//        oit->debugProgram += "sign(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpFloor:
//        oit->debugProgram += "floor(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpCeil:
//        oit->debugProgram += "ceil(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpFract:
//        oit->debugProgram += "fract(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//    case EOpLength:
//        oit->debugProgram += "length(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpNormalize:
//        oit->debugProgram += "normalize(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpDPdx:
//        oit->debugProgram += "dFdx(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpDPdy:
//        oit->debugProgram += "dFdy(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpFwidth:
//        oit->debugProgram += "fwidth(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//    case EOpAny:
//        oit->debugProgram += "any(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpAll:
//        oit->debugProgram += "all(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//    case EOpMatrixTranspose:
//        oit->debugProgram += "transpose(";
//        node->getOperand()->traverse(it);
//        oit->debugProgram += ")";
//        break;
//
//    default:
//        out.debug.message(EPrefixError, "Bad unary op");
//        break;
//    }
//
//    return false;
//}



//static void OutputSymbol(TIntermSymbol* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    outputExtensions(node, oit);
//    oit->debugProgram += node->getSymbol();
//
//    /* Add postfix to all non-builtin symboles due to !@$#^$% scope hiding */
//    int postfix = node->getId();
//
//    ShVariable* var = findShVariableFromId(oit->vl, postfix);
//
//    if (var) {
//        if (!var->builtin &&
//             var->qualifier != SH_VARYING_IN &&
//             var->qualifier != SH_VARYING_OUT &&
//             var->qualifier != SH_UNIFORM &&
//             var->qualifier != SH_ATTRIBUTE) {
//            char buf[300];
//            sprintf(buf, "_%i", postfix);
//            oit->debugProgram += buf;
//        }
//    }
//}

//static void OutputFuncParam(ir_variable* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    outputExtensions(node, oit);
//    oit->debugProgram += getCodeString(node, true, oit->language) + " ";
//    oit->debugProgram += std::string(node->name);
//
//    /* Add postfix to all non-builtin symbols due to !@$#^$% scope hiding */
//    ShVariable* var = findShVariableFromSource( node );
//    if (var && !var->builtin && var->qualifier != SH_VARYING_IN &&
//    		    var->qualifier != SH_VARYING_OUT && var->qualifier != SH_UNIFORM &&
//    		    var->qualifier != SH_ATTRIBUTE ) {
//    	char buf[300];
//    	sprintf(buf, "_%i", var->uniqueId);
//    	oit->debugProgram += buf;
//    }
//
//    oit->debugProgram += getCodeArraySize(node->type);
//}

//static void OutputConstantUnion(TIntermConstantUnion* node,
//                                TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    TInfoSink& out = oit->infoSink;
//
//    outputExtensions(node, oit);
//    int size = node->getType().getObjectSize();
//
//    // Preceeding type cast for non scalar constant unions
//    if (size != 1) {
//        oit->debugProgram += node->getType().getCodeString(false, oit->language);
//        oit->debugProgram += "(";
//    }
//
//    for (int i = 0; i < size; i++) {
//        switch (node->getUnionArrayPointer()[i].getType()) {
//            case EbtBool:
//                if (node->getUnionArrayPointer()[i].getBConst()) {
//                    oit->debugProgram += "true";
//                } else {
//                    oit->debugProgram += "false";
//                }
//                break;
//            case EbtFloat:
//                {
//                    char buf[300];
//                    sprintf(buf, "%f",
//                            node->getUnionArrayPointer()[i].getFConst());
//                    oit->debugProgram += buf;
//                }
//                break;
//            case EbtInt:
//                {
//                    char buf[300];
//                    sprintf(buf, "%i",
//                            node->getUnionArrayPointer()[i].getIConst());
//                    oit->debugProgram += buf;
//                }
//                break;
//            case EbtUInt:
//                {
//                    char buf[300];
//                    sprintf(buf, "%iu",
//                            node->getUnionArrayPointer()[i].getUIConst());
//                    oit->debugProgram += buf;
//                }
//                break;
//            case EbtSwizzle:
//                {
//                    char buf[300];
//                    sprintf(buf, "%s",
//                            itoSwizzle(
//                                node->getUnionArrayPointer()[i].getIConst()) );
//                    oit->debugProgram += buf;
//                }
//                break;
//            default:
//                out.info.message(EPrefixInternalError,
//                        "Unknown constant", node->getRange());
//            break;
//        }
//        if (i + 1 < size) {
//            oit->debugProgram += ", ";
//        }
//    }
//    // Closing bracket for non scalar constant unions
//    if (size != 1) {
//        oit->debugProgram += ")";
//    }
//}

//static bool OutputSelection(bool, TIntermSelection* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    bool copyCondition = false;
//
//    outputExtensions(node, oit);
//
//    /* Trigraph should be in brackets as if could be child of another statement */
//    if (node->isShort()) {
//        oit->debugProgram += "(";
//    }
//
//    /* Add debug code */
//    if (node->isTarget()) {
//        oit->dbgTargetProcessed = true;
//        switch (oit->cgOptions) {
//            case DBG_CG_COVERAGE:
//            case DBG_CG_CHANGEABLE:
//            case DBG_CG_GEOMETRY_CHANGEABLE:
//                switch (node->getDbgInternalState()) {
//                    case DBG_STATE_SELECTION_UNSET:
//                        dbgPrint(DBGLVL_ERROR, "CodeGen - selection status is unset\n");
//                        exit(1);
//                        break;
//                    case DBG_STATE_SELECTION_INIT:
//                    case DBG_STATE_SELECTION_CONDITION:
//                        /* Add debug code prior to selection */
//                        if (node->isShort()) {
//                            oit->debugProgram += "(";
//                        }
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                                     oit->cgOptions, oit->cgbl, oit->vl,
//                                     oit->dbgStack, 0);
//                        if (node->isShort()) {
//                            oit->debugProgram += "), ";
//                        } else {
//                            oit->debugProgram += "; ";
//                        }
//                        break;
//                    case DBG_STATE_SELECTION_CONDITION_PASSED:
//                    case DBG_STATE_SELECTION_IF:
//                    case DBG_STATE_SELECTION_ELSE:
//                        /* Add temporary register for condition */
//
//                        /* Fix: trigraph initialized condition register
//                         *      even if trigraph is part of another
//                         *      statement
//                         *
//                        cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
//                        cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
//                        outputIndentation(oit, oit->depth);
//                        */
//                        copyCondition = true;
//                        break;
//                    default:
//                        break;
//                }
//                break;
//            default:
//                break;
//        }
//    }
//
//    /* Add begin of condition */
//    if (!node->isShort()) {
//        oit->debugProgram += "if (";
//    }
//
//    /* Add condition */
//    if (copyCondition) {
//        if (node->isShort()) {
//            oit->debugProgram += "(";
//        }
//
//        cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += " = (";
//        node->getCondition()->traverse(it);
//        oit->debugProgram += "), ";
//
//        /* Add debug code */
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                             oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += ", ";
//        cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram, oit->cgOptions,
//                oit->cgbl, oit->vl, oit->dbgStack, 0);
//        if (node->isShort()) {
//            oit->debugProgram += ")";
//        }
//    } else {
//        node->getCondition()->traverse(it);
//    }
//
//    if (!node->isShort()) {
//        oit->debugProgram += ") {\n";
//        oit->depth++;
//    } else {
//        oit->debugProgram += " ? (";
//    }
//
//    if (node->getTrueBlock()) {
//        /* Traverse true block */
//        TIntermAggregate *trueCase = node->getTrueBlock()->getAsAggregate();
//
//        if (node->isTarget() &&
//            oit->cgOptions == DBG_CG_SELECTION_CONDITIONAL &&
//            node->getDbgInternalState() == DBG_STATE_SELECTION_CONDITION_PASSED) {
//            /* Add code to colorize condition */
//            if (!node->isShort()) {
//                outputIndentation(oit, oit->depth);
//                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                             oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, true);
//                oit->debugProgram += ";\n";
//            } else {
//                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                             oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, true);
//                oit->debugProgram += ", ";
//            }
//        }
//
//        if (trueCase) {
//            if (node->isShort()) {
//                oit->sequenceUseComma = true;
//                oit->sequencePrintClosure = false;
//            }
//            processAggregateSequence(trueCase, it);
//            if (node->isShort()) {
//                oit->sequenceUseComma = false;
//                oit->sequencePrintClosure = true;
//            }
//        } else {
//            if (!node->isShort()) {
//                outputIndentation(oit, oit->depth);
//            }
//            node->getTrueBlock()->traverse(it);
//            if (!node->isShort()) {
//                oit->debugProgram += ";\n";
//            }
//        }
//    }
//    if (!node->isShort()) {
//        oit->depth--;
//        outputIndentation(oit, oit->depth);
//        oit->debugProgram += "}";
//    } else {
//        oit->debugProgram += ")";
//    }
//
//    if (node->getFalseBlock()) {
//        if (!node->isShort()) {
//            oit->debugProgram += " else {\n";
//            oit->depth++;
//        } else {
//            oit->debugProgram += " : (";
//        }
//        /* Traverse false block */
//        TIntermAggregate *falseCase = node->getFalseBlock()->getAsAggregate();
//
//        if (node->isTarget() &&
//            oit->cgOptions == DBG_CG_SELECTION_CONDITIONAL &&
//            node->getDbgInternalState() == DBG_STATE_SELECTION_CONDITION_PASSED) {
//            /* Add code to colorize condition */
//            if (!node->isShort()) {
//                outputIndentation(oit, oit->depth);
//                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                             oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, false);
//                oit->debugProgram += ";\n";
//            } else {
//                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                             oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, false);
//                oit->debugProgram += ", ";
//            }
//        }
//
//        if (falseCase) {
//            if (node->isShort()) {
//                oit->sequenceUseComma = true;
//                oit->sequencePrintClosure = false;
//            }
//            processAggregateSequence(falseCase, it);
//            if (node->isShort()) {
//                oit->sequenceUseComma = false;
//                oit->sequencePrintClosure = true;
//            }
//        } else {
//            oit->depth++;
//            if (!node->isShort()) {
//                outputIndentation(oit, oit->depth);
//            }
//            node->getFalseBlock()->traverse(it);
//            if (!node->isShort()) {
//                oit->debugProgram += ";\n";
//            }
//            oit->depth--;
//        }
//        if (!node->isShort()) {
//            oit->depth--;
//            outputIndentation(oit, oit->depth);
//            oit->debugProgram += "}";
//        } else {
//            oit->debugProgram += ")";
//        }
//    }
//
//    /* Trigraph should be in brackets as if could be child of another statement */
//    if (node->isShort()) {
//        oit->debugProgram += ")";
//    }
//
//    if (!node->isShort()) {
//        oit->debugProgram += "\n";
//    }
//
//    return false;
//}

//static bool OutputSwitch(bool, TIntermSwitch* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    /* Add begin of switch */
//    oit->debugProgram += "switch (";
//
//    /* Add condition */
//    node->getCondition()->traverse(it);
//
//    /* End of condition */
//    oit->debugProgram += ") ";
//
//    /* Body */
//    if (node->getCaseList()) {
//        node->getCaseList()->traverse(it);
//    }
//
//    return false;
//}

//static bool OutputCase(bool, TIntermCase* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    if (node->getExpression()) {
//        /* Add begin of case */
//        oit->debugProgram += "case ";
//        /* Add expression */
//        node->getExpression()->traverse(it);
//        /* End of case */
//        oit->debugProgram += ": ";
//    } else {
//        oit->debugProgram += "default: ";
//    }
//
//    /* Body */
//    if (node->getCaseBody()) {
//        node->getCaseBody()->traverse(it);
//    }
//
//
//    return false;
//
//}


//static bool OutputLoop(bool, TIntermLoop* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    switch (node->getLoopType()) {
//        case LOOP_WHILE:
//            if (oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                oit->debugProgram += "while(";
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//                oit->debugProgram += ") {\n";
//                break;
//            }
//
//            /* Add debug temoprary register to copy condition */
//            if (node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_SELECT_FLOW) {
//                switch (oit->cgOptions) {
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_LOOP_CONDITIONAL:
//                    case DBG_CG_CHANGEABLE:
//					case DBG_CG_GEOMETRY_CHANGEABLE:
//                        cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
//                        cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
//                        outputIndentation(oit, oit->depth);
//                        break;
//                    default:
//                        break;
//                }
//            }
//
//            /* Add loop counter */
//            if (node->needDbgLoopIter()) {
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += " = 0;\n";
//                outputIndentation(oit, oit->depth);
//            }
//
//            oit->debugProgram += "while(";
//
//            /* Add condition */
//            if (node->isTarget()) {
//                oit->dbgTargetProcessed = true;
//                if (node->getDbgInternalState() == DBG_STATE_LOOP_QYR_TEST) {
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                } else if (node->getDbgInternalState() == DBG_STATE_LOOP_SELECT_FLOW) {
//                    /* Copy test */
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += " = (";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                    oit->debugProgram += "), ";
//                    /* Add debug code */
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                            oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                }
//            } else {
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//            }
//            oit->debugProgram += ") {\n";
//            break;
//        case LOOP_DO:
//            if (oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                oit->debugProgram += "do {\n";
//                break;
//            }
//
//            /* Add debug temoprary register to copy condition */
//            if (node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_SELECT_FLOW) {
//                switch (oit->cgOptions) {
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_LOOP_CONDITIONAL:
//                    case DBG_CG_CHANGEABLE:
//					case DBG_CG_GEOMETRY_CHANGEABLE:
//                        cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
//                        cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
//                        outputIndentation(oit, oit->depth);
//                        break;
//                    default:
//                        break;
//                }
//            }
//
//            /* Add loop counter */
//            if (node->needDbgLoopIter()) {
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += " = 0;\n";
//                outputIndentation(oit, oit->depth);
//            }
//
//            oit->debugProgram += "do {\n";
//            break;
//        case LOOP_FOR:
//            if (oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                oit->debugProgram += "for(";
//                if (node->getInit()) {
//                    node->getInit()->traverse(it);
//                }
//                oit->debugProgram += "; ";
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//                oit->debugProgram += "; ";
//                if (node->getTerminal()) {
//                    node->getTerminal()->traverse(it);
//                }
//                oit->debugProgram += ") {\n";
//
//                break;
//            }
//
//            /* Add loop counter */
//            if (node->needDbgLoopIter()) {
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += " = 0;\n";
//                outputIndentation(oit, oit->depth);
//            }
//
//            /* Add debug code prior to loop */
//            if (node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_QYR_INIT) {
//                switch (oit->cgOptions) {
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_CHANGEABLE:
//                    case DBG_CG_GEOMETRY_CHANGEABLE:
//                        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                               oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                        oit->debugProgram += ";\n";
//                        outputIndentation(oit, oit->depth);
//                        break;
//                    default:
//                        break;
//                }
//            } else if (!node->isTarget() &&
//                       node->getDbgInternalState() == DBG_STATE_LOOP_WRK_INIT) {
//                oit->debugProgram += "{\n";
//                oit->depth++;
//                outputIndentation(oit, oit->depth);
//                if (node->getInit()) {
//                    node->getInit()->traverse(it);
//                }
//                oit->debugProgram += ";\n";
//                outputIndentation(oit, oit->depth);
//            }
//
//            /* Add optional debug temoprary register to copy condition */
//            if (node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_SELECT_FLOW) {
//                switch (oit->cgOptions) {
//                    case DBG_CG_COVERAGE:
//                    case DBG_CG_LOOP_CONDITIONAL:
//                    case DBG_CG_CHANGEABLE:
//					case DBG_CG_GEOMETRY_CHANGEABLE:
//                        cgInit(CG_TYPE_CONDITION, NULL, oit->vl, oit->language);
//                        cgAddDeclaration(CG_TYPE_CONDITION, oit->debugProgram, oit->language);
//                        outputIndentation(oit, oit->depth);
//                        break;
//                    default:
//                        break;
//                }
//            }
//
//            /* Add initialization code */
//            oit->debugProgram += "for(";
//            if (node->getDbgInternalState() != DBG_STATE_LOOP_WRK_INIT) {
//                if (node->getInit()) {
//                    node->getInit()->traverse(it);
//                }
//            }
//
//            oit->debugProgram += "; ";
//
//            /* Add condition */
//            if (node->isTarget()) {
//                oit->dbgTargetProcessed = true;
//                if (node->getDbgInternalState() == DBG_STATE_LOOP_QYR_TEST) {
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                } else if (node->getDbgInternalState() ==
//                                                DBG_STATE_LOOP_SELECT_FLOW) {
//                    /* Copy test */
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += " = (";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                    oit->debugProgram += "), ";
//                    /* Add debug code */
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                            oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                } else {
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                }
//            } else {
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//            }
//            oit->debugProgram += "; ";
//
//            /* Add terminal */
//            if (node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_QYR_TERMINAL) {
//                cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                        oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                if (node->getTerminal()) {
//                    oit->debugProgram += ", ";
//                }
//            }
//            if (node->getTerminal()) {
//                node->getTerminal()->traverse(it);
//            }
//
//            /* Add increment of debug loop iterator */
//            if (node->needDbgLoopIter()) {
//                if (node->getTerminal()) {
//                    oit->debugProgram += ", ";
//                }
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += "++";
//            }
//
//            oit->debugProgram += ") {\n";
//            break;
//    }
//
//    if (node->getBody()) {
//        if (node->getBody()->getAsAggregate()) {
//            oit->depth++;
//            processAggregateSequence(node->getBody()->getAsAggregate(), it);
//            oit->depth--;
//        } else {
//            oit->depth++;
//            outputIndentation(oit, oit->depth);
//            node->getBody()->traverse(it);
//            if (!(node->getBody()->getAsLoopNode() ||
//                  node->getBody()->getAsSelectionNode())) {
//                oit->debugProgram += ";\n";
//            }
//            oit->depth--;
//        }
//    }
//
//    outputIndentation(oit, oit->depth);
//    switch (node->getLoopType()) {
//        case LOOP_WHILE:
//            /* Add increment of debug loop iterator */
//            if (node->needDbgLoopIter() &&
//                oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//                outputIndentation(oit, 1);
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += "++;\n";
//                outputIndentation(oit, oit->depth);
//            }
//            oit->debugProgram += "}\n";
//            break;
//        case LOOP_FOR:
//            oit->debugProgram += "}\n";
//
//            if (!node->isTarget() &&
//                node->getDbgInternalState() == DBG_STATE_LOOP_WRK_INIT &&
//                oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//                oit->depth--;
//                outputIndentation(oit, oit->depth);
//                oit->debugProgram += "}\n";
//            }
//            break;
//        case LOOP_DO:
//            if (oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                oit->debugProgram += "} while(";
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//                oit->debugProgram += ");\n";
//                break;
//            }
//
//            /* Add increment of debug loop iterator */
//            if (node->needDbgLoopIter()) {
//                outputIndentation(oit, 1);
//                oit->debugProgram += node->getDbgIterName();
//                oit->debugProgram += "++;\n";
//                outputIndentation(oit, oit->depth);
//            }
//            oit->debugProgram += "} while(";
//            /* Add condition */
//            if (node->isTarget()) {
//                oit->dbgTargetProcessed = true;
//                if (node->getDbgInternalState() == DBG_STATE_LOOP_QYR_TEST) {
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                } else if (node->getDbgInternalState() == DBG_STATE_LOOP_SELECT_FLOW) {
//                    /* Copy test */
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += " = (";
//                    if (node->getTest()) {
//                        node->getTest()->traverse(it);
//                    } else {
//                        oit->debugProgram += "true";
//                    }
//                    oit->debugProgram += "), ";
//                    /* Add debug code */
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                            oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += ", ";
//                    cgAddDbgCode(CG_TYPE_CONDITION, oit->debugProgram,
//                            oit->cgOptions, oit->cgbl, oit->vl, oit->dbgStack, 0);
//                }
//            } else {
//                if (node->getTest()) {
//                    node->getTest()->traverse(it);
//                } else {
//                    oit->debugProgram += "true";
//                }
//            }
//            oit->debugProgram += ");\n";
//            break;
//    }
//    return false;
//}

//static bool OutputBranch(bool /* previsit*/, TIntermBranch* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    char *tmpRegister = NULL;
//
//    outputExtensions(node, oit);
//
//    /* Add debug code before statement if not return */
//    if (node->getFlowOp() != EOpReturn &&
//        node->isTarget() &&
//        oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//        oit->dbgTargetProcessed = true;
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += "; ";
//    }
//
//    switch (node->getFlowOp()) {
//        case EOpKill:
//            if (!(oit->dbgTargetProcessed) || oit->cgOptions == DBG_CG_ORIGINAL_SRC) {
//                oit->debugProgram += "discard";
//            }
//            break;
//        case EOpBreak:
//            oit->debugProgram += "break";
//            break;
//        case EOpContinue:
//            oit->debugProgram += "continue";
//            break;
//        case EOpReturn:
//            if (node->isTarget() &&
//                oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//                oit->dbgTargetProcessed = true;
//
//                if (node->getExpression()) {
//                    /* Declaraion: type name = expression; */
//                    /* type */
//                    oit->debugProgram += node->getExpression()->getType().getCodeString(true, oit->language);
//                    oit->debugProgram += " ";
//                    /* name */
//                    cgGetNewName(&tmpRegister, oit->vl, "dbgBranch");
//                    oit->debugProgram += tmpRegister;
//
//                    /* assignment */
//                    oit->debugProgram += " = ";
//
//                    /* expression */
//                    node->getExpression()->traverse(it);
//                    oit->debugProgram += ";\n";
//
//                    /* Debug code */
//                    outputIndentation(oit, oit->depth);
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                 oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += "; ";
//                } else {
//                    cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                                 oit->cgbl, oit->vl, oit->dbgStack, 0);
//                    oit->debugProgram += "; ";
//                }
//            }
//
//            /* Geometry: If control flow ends program execution append code
//             *           for emitting vertex and primitive.
//             */
//
//            if (oit->language == EShLangGeometry &&
//                (oit->cgOptions == DBG_CG_CHANGEABLE ||
//                 oit->cgOptions == DBG_CG_VERTEX_COUNT ||
//                 oit->cgOptions == DBG_CG_COVERAGE ||
//                 oit->cgOptions == DBG_CG_SELECTION_CONDITIONAL ||
//                 oit->cgOptions == DBG_CG_LOOP_CONDITIONAL )) {
//
//                /* Node must be direct/indirect child of main function call */
//                if (isPartofMain(node, oit->root)) {
//                    cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                    outputIndentation(oit, oit->depth);
//                }
//            } else if (oit->cgOptions != DBG_CG_ORIGINAL_SRC &&
//                       (oit->language == EShLangVertex || oit->language == EShLangFragment)) {
//                if (isPartofMain(node, oit->root)) {
//                    cgAddOutput(CG_TYPE_RESULT, oit->debugProgram, oit->language);
//                    outputIndentation(oit, oit->depth);
//                }
//            }
//
//            oit->debugProgram += "return";
//
//            if (node->getExpression()) {
//                if (node->isTarget() &&
//                    oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//                    /* Name */
//                    oit->debugProgram += " ";
//                    oit->debugProgram += tmpRegister;
//                } else {
//                    oit->debugProgram += " ";
//                    node->getExpression()->traverse(it);
//                }
//            }
//
//            break;
//        default:
//            break;
//    }
//
//    return false;
//}


//static bool OutputDeclaration(ir_variable* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
////	TODO: dunno
////    if (node->isFirst()) {
////        /* First element of possible declaration sequence
////         * -> add type specifier */
////        oit->debugProgram += v->getType().getCodeString(true, oit->language);
////        oit->debugProgram += " ";
////    }
//    // FIXME: if previous is resolved: this line is not needed
//    oit->debugProgram += getCodeString(node, true, oit->language) + " ";
//
//    oit->debugProgram += std::string(node->name);
//
//    /* Add postfix to all non-builtin symbols due to !@$#^$% scope hiding */
//    ShVariable* var = findShVariableFromSource( node );
//    if (var && !var->builtin && var->qualifier != SH_VARYING_IN &&
//    		    var->qualifier != SH_VARYING_OUT && var->qualifier != SH_UNIFORM &&
//    		    var->qualifier != SH_ATTRIBUTE ) {
//    	char buf[300];
//    	sprintf(buf, "_%i", var->uniqueId);
//    	oit->debugProgram += buf;
//    }
//
//    oit->debugProgram += getCodeArraySize(node->type);
//
//    // TODO: initialization
////    if (node->getInitialization()) {
////        /* initialization */
////        if (node->getInitialization() &&
////            node->getInitialization()->getAsBinaryNode() &&
////            node->getInitialization()->getAsBinaryNode()->getOp() == EOpAssign) {
////            oit->debugProgram += " = ";
////            node->getInitialization()->getAsBinaryNode()->getRight()->traverse(it);
////        }
////    }
//
//    return false;
//}

//static bool OutputDeclarationDebugged(ir_variable* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//// TODO: initialization
//    if (node->has_initializer &&
//        node->getInitialization()->isTarget() &&
//        oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//        oit->dbgTargetProcessed = true;
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += ";\n";
//        outputIndentation(oit, oit->depth);
//    }
//
//    oit->debugProgram += getCodeString(node, true, oit->language);
//    oit->debugProgram += " " + std::string(node->name);
//
//    /* Add postfix to all non-builtin symbols due to !@$#^$% scope hiding */
//    ShVariable* var = findShVariableFromSource( node );
//    if (var && !var->builtin && var->qualifier != SH_VARYING_IN &&
//    		    var->qualifier != SH_VARYING_OUT && var->qualifier != SH_UNIFORM &&
//    		    var->qualifier != SH_ATTRIBUTE ) {
//    	char buf[300];
//    	sprintf(buf, "_%i", var->uniqueId);
//    	oit->debugProgram += buf;
//    }
//
//    oit->debugProgram += getCodeArraySize(node->type);
//
//// TODO: initialization
////    if (node->getInitialization()) {
////        if (node->getInitialization()->getAsBinaryNode() &&
////            node->getInitialization()->getAsBinaryNode()->getOp() == EOpAssign) {
////
////            oit->debugProgram += " = ";
////
////            node->getInitialization()->getAsBinaryNode()->getRight()->traverse(it);
////        } else {
////            dbgPrint(DBGLVL_ERROR, "CodeGen - initialization is not an assign node???\n");
////        }
////    }
//
//    return false;
//}

//static void outputFuncPrototype(ir_function *f, const char* fname, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//    ir_function_signature* fs = ((ir_instruction*)f->signatures.head)->as_function_signature();
//    struct glsl_type* return_type = fs->return_type;
//
//    oit->debugProgram += getCodeString( return_type );
//    oit->debugProgram += getCodeArraySize( return_type );
//    oit->debugProgram += " " + std::string(fname) + "(";
//
//    // add parameter
//    foreach_iter( exec_list_iterator, iter, fs->parameters ){
//    	ir_instruction* ir = iter.get();
//    	if( ir->ir_type != ir_variable ){
//    		printf("FIXME: type other than variable does not supported in func params");
//    		continue;
//    	}
//    	ir_variable* var = ir->as_variable();
//    	oit->debugProgram += getCodeString(var, true, oit->language) + " ";
//    	oit->debugProgram += std::string(var->name) + getCodeArraySize( var->type );
//        if ( iter.has_next() )
//            oit->debugProgram += ", ";
//    }
//    oit->debugProgram += ")";
//}
//
//
//static void OutputFuncDeclaration(ir_function* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    std::string mangled = getMangledName(node);
//    const char* mname = mangled.c_str();
//
//
//    /* Check if function is on debug path */
//    foreach_iter( exec_list_iterator, iter, node->signatures ){
//    	ir_function_signature* funcDef = ((ir_instruction*)iter.get())->as_function_signature();
//    	if( funcDef ){
//    		/* Add debugged prototype */
//    		outputFuncPrototype(node, cgGetDebugName(mname, oit->shader), it);
//    	}else{
//    		dbgPrint(DBGLVL_WARNING, "CodeGen - could not find function definition of prototype %s\n", mname);
//    	}
//    }
//
//    /* Add unchanged prototype */
//    outputFuncPrototype(node, node->name, it);
//}


//static bool OutputSpecification(TIntermSpecification* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    if (node->getInstances()) {
//		if (!node->getInstances()->getSequence().empty()) {
//			TIntermNode* itermNode = node->getInstances()->getSequence()[0];
//			if (itermNode) {
//				TIntermDeclaration* declNode = itermNode->getAsDeclarationNode();
//				if (declNode) {
//					TVariable* variable = declNode->getVariable();
//					if (variable) {
//						oit->debugProgram +=
//							variable->getType().getQualifierString(oit->language);
//						oit->debugProgram += " ";
//					}
//				}
//			}
//		}
//    }
//
//    TType* t = node->getType();
//
//    oit->debugProgram += "struct ";
//    oit->debugProgram += t->getTypeName().c_str();
//    oit->debugProgram += " {\n";
//
//    oit->depth++;
//    if (node->getParameter()) {
//        node->getParameter()->traverse(it);
//    }
//    oit->depth--;
//
//    outputIndentation(oit, oit->depth);
//    oit->debugProgram += "}";
//
//    if (node->getInstances()) {
//        oit->debugProgram += " ";
//        node->getInstances()->traverse(it);
//    }
//
//    return false;
//}

//static void OutputParameter(TIntermParameter* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    TType* t = node->getType();
//
//    oit->debugProgram += t->getCodeString(true, oit->language);
//    oit->debugProgram += " ";
//    oit->debugProgram += t->getFieldName();
//    if (t->isArray()) {
//        char buffer[100];
//        int i;
//        for (i=0; i<t->getNumArrays(); i++) {
//            oit->debugProgram += "[";
//            sprintf(buffer, "%i", t->getArraySize(i));
//            oit->debugProgram += buffer;
//            oit->debugProgram += "]";
//        }
//    }
//}

//static void OutputDummy(TIntermDummy* node, TIntermTraverser* it)
//{
//    TOutputTraverser* oit = static_cast<TOutputTraverser*>(it);
//
//    outputExtensions(node, oit);
//
//    if (node->isTarget() &&
//        oit->cgOptions != DBG_CG_ORIGINAL_SRC) {
//        oit->dbgTargetProcessed = true;
//        outputIndentation(oit, oit->depth);
//        cgAddDbgCode(CG_TYPE_RESULT, oit->debugProgram, oit->cgOptions,
//                     oit->cgbl, oit->vl, oit->dbgStack, 0);
//        oit->debugProgram += ";\n";
//    }
//}

bool appendVersion(struct gl_shader* shader, std::string& program)
{
    if( !shader || shader->Version == 0 )
    	return false;

    char buf[10];
    program += "#version ";
    sprintf(buf, "%i", shader->Version);
    program += buf;
    program += "\n";
    return true;
}


//
//  Generate code from the given parse tree
//
bool compileShaderCode(struct gl_shader* shader)
{
	exec_list* list = shader->ir;
//    bool haveValidObjectCode = true;

    std::string m_debugProgram;
    EShLanguage language = EShLangVertex;
    if( shader->Type == GL_FRAGMENT_SHADER )
    	language = EShLangFragment;
    else if( shader->Type == GL_GEOMETRY_SHADER )
		language = EShLangGeometry;

    char* debug_program = NULL;
    ir_output_traverser_visitor it(debug_program, shader, language,
    									DBG_CG_ORIGINAL_SRC, NULL, NULL, NULL);
    it.append_version();
//    TOutputTraverser it(root, m_debugProgram, language, NULL, NULL, NULL);
//    it.preVisit = true;
//    it.postVisit = false;
//    m_debugProgram = "";

//    if( !appendVersion(shader, m_debugProgram) )
//    	return false;
//
//    /* Check for empty parse tree */
//    if ( list->head->next == NULL || list->tail_pred == NULL )
//        return haveValidObjectCode;
//
//    it.visitAggregate = OutputAggregate;
//    it.visitBinary = OutputBinary;
//    it.visitConstantUnion = OutputConstantUnion;
//    it.visitSelection = OutputSelection;
//    it.visitSwitch = OutputSwitch;
//    it.visitCase = OutputCase;
//    it.visitSymbol = OutputSymbol;
//    it.visitFuncParam = OutputFuncParam;
//    it.visitUnary = OutputUnary;
//    it.visitLoop = OutputLoop;
//    it.visitBranch = OutputBranch;
//    it.visitDeclaration = OutputDeclaration;
//    it.visitFuncDeclaration = OutputFuncDeclaration;
//    it.visitSpecification = OutputSpecification;
//    it.visitParameter = OutputParameter;
//    it.visitDummy = OutputDummy;
//
//    it.cgOptions = DBG_CG_ORIGINAL_SRC;
//
//    TraverseList(list, &it);
    it.run(list);
//    haveValidObjectCode = true;

    dbgPrint(DBGLVL_COMPILERINFO, "\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"
                   "%s\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",
                   m_debugProgram.c_str());

//    return haveValidObjectCode;
    return true;
}

/*
 * DEBUG STACK CREATION
 */


//class TStackTraverser : public TIntermTraverser {
//public:
//    TStackTraverser(ir_instruction* r, ShVariableList *list)
//        : root(r), vl(list)
//        { dbgStack.clear(); }
//    ir_instruction* root;
//    IRGenStack dbgStack;
//    ShVariableList *vl;
//};

//static bool stackAggregate(ir_instruction* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    switch(node->ir_type) {
//        case ir_type_call:
//            if (node->debug_state == ir_dbg_state_target) {
//                /* add to list and parse function */
//                oit->dbgStack.push_back(node);
//                Traverse(node->as_call()->callee, oit);
//                return false;
//            } else if (node->debug_state == ir_dbg_state_path) {
//                /* add to list and parse parameter */
//                oit->dbgStack.push_back(node);
//                return true;
//            } else {
//                return false;
//            }
//        default:
//            if (node->debug_state == ir_dbg_state_target) {
//                dbgPrint(DBGLVL_ERROR, "CodeGen - found aggregate as target while building stack\n");
//                exit(1);
//            } else if (node->debug_state == ir_dbg_state_path) {
//                return true;
//            } else {
//                return false;
//            }
//    }
//}

//static bool stackExpression(ir_expression* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    if (node->debug_state == ir_dbg_state_target) {
//        /* add node to stack and finish */
//        oit->dbgStack.push_back(node);
//        return false;
//    } else if (node->debug_state == ir_dbg_state_path) {
//        /* add node to stack and process children */
//        /* unaries don't have an own scope, as the never change it
//         * thus we don't put it on the stack for simplicity
//         * even if it would work
//         * but we continue regularily with its child
//         */
//    	if( node->operation > ir_last_unop )
//    		oit->dbgStack.push_back(node);
//        return true;
//    }
//
//    return false;
//}

//static bool stackSelection(bool, TIntermSelection* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    switch (node->getDebugState()) {
//        case DbgStTarget:
//            switch (node->getDbgInternalState()) {
//                case DBG_STATE_SELECTION_INIT:
//                case DBG_STATE_SELECTION_CONDITION_PASSED:
//                    oit->dbgStack.push_back(node);
//                    return false;
//                default:
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - selection as target has invalid internal state\n");
//                    exit(1);
//            }
//            return false;
//        case DbgStPath:
//            switch (node->getDbgInternalState()) {
//                case DBG_STATE_SELECTION_CONDITION:
//                    oit->dbgStack.push_back(node);
//                    if (node->getCondition())
//                        node->getCondition()->traverse(it);
//                    return false;
//                case DBG_STATE_SELECTION_IF:
//                    oit->dbgStack.push_back(node);
//                    if (node->getTrueBlock())
//                        node->getTrueBlock()->traverse(it);
//                    return false;
//                case DBG_STATE_SELECTION_ELSE:
//                    oit->dbgStack.push_back(node);
//                    if (node->getFalseBlock())
//                        node->getFalseBlock()->traverse(it);
//                    return false;
//                default:
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - selection as path has invalid internal state\n");
//                    exit(1);
//            }
//            return false;
//        default:
//            return false;
//    }
//}

//static bool stackLoop(bool, TIntermLoop* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    /* Clear old name for dbgLoopIter */
//    node->clearDbgIterName();
//
//    switch (node->getDebugState()) {
//        case DbgStTarget:
//            switch (node->getDbgInternalState()) {
//                case DBG_STATE_LOOP_QYR_INIT:
//                    oit->dbgStack.push_back(node);
//                    return false;
//                case DBG_STATE_LOOP_QYR_TEST:
//                case DBG_STATE_LOOP_SELECT_FLOW:
//                case DBG_STATE_LOOP_QYR_TERMINAL:
//                    oit->dbgStack.push_back(node);
//                    cgSetLoopIterName(node->getDbgIterNamePointer(), oit->vl);
//                    return false;
//                default:
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - loop target has invalid internal state\n");
//                    exit(1);
//            }
//            return false;
//        case DbgStPath:
//            switch (node->getDbgInternalState()) {
//                case DBG_STATE_LOOP_WRK_INIT:
//                    oit->dbgStack.push_back(node);
//                    cgSetLoopIterName(node->getDbgIterNamePointer(), oit->vl);
//                    if (node->getInit())
//                        node->getInit()->traverse(it);
//                    return false;
//                case DBG_STATE_LOOP_WRK_TEST:
//                    oit->dbgStack.push_back(node);
//                    cgSetLoopIterName(node->getDbgIterNamePointer(), oit->vl);
//                    if (node->getTest())
//                        node->getTest()->traverse(it);
//                    return false;
//                case DBG_STATE_LOOP_WRK_BODY:
//                    oit->dbgStack.push_back(node);
//                    cgSetLoopIterName(node->getDbgIterNamePointer(), oit->vl);
//                    if (node->getBody())
//                        node->getBody()->traverse(it);
//                    return false;
//                case DBG_STATE_LOOP_WRK_TERMINAL:
//                    oit->dbgStack.push_back(node);
//                    cgSetLoopIterName(node->getDbgIterNamePointer(), oit->vl);
//                    if (node->getTerminal())
//                        node->getTerminal()->traverse(it);
//                    return false;
//                default:
//                    dbgPrint(DBGLVL_ERROR, "CodeGen - loop path has invalid internal state\n");
//                    exit(1);
//            }
//            return false;
//        default:
//            return false;
//    }
//}

//static bool stackBranch(bool, TIntermBranch* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    if (node->getDebugState() == DbgStTarget) {
//        /* add node to stack and finish */
//        oit->dbgStack.push_back(node);
//        return false;
//    } else if (node->getDebugState() == DbgStPath) {
//        /* add node to stack and process children */
//        oit->dbgStack.push_back(node);
//        return true;
//    } else {
//        return false;
//    }
//}

//static void stackDummy(TIntermDummy* node, TIntermTraverser* it)
//{
//    TStackTraverser* oit = static_cast<TStackTraverser*>(it);
//
//    if (node->getDebugState() == DbgStTarget) {
//        /* add node to stack and finish */
//        oit->dbgStack.push_back(node);
//    }
//}


static void dumpNodeInfo(ir_instruction* node)
{
    dbgPrint(DBGLVL_COMPILERINFO, "(%s) ", FormatSourceRange(node->yy_location).c_str());
    switch( node->ir_type ){
    	case ir_type_call:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION CALL %s",
    		                node->as_call()->callee_name());
    		break;
    	case ir_type_function:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION %s",
    		    		                node->as_function()->name);
    		break;
    	case ir_type_function_signature:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "FUNCTION SIGNATURE %s",
    				node->as_function_signature()->function_name());
    		break;
    	case ir_type_expression:
    	{
    		ir_expression* eir = node->as_expression();
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "EXPRESSION: %s",
    				eir->operation < ir_last_unop ? "UNARY" :
    				eir->operation < ir_last_binop ? "BINARY" :
    				eir->operation < ir_last_triop ? "TERNARY" : "QUAD"	);
    		break;
    	}
    	case ir_type_if:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "if");
    		break;
//    } else if (node->getAsBranchNode()) {
//        dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "BRANCH");
//    } else {
    	default:
    		dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "unknown");
    		break;
    }
	dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
}

static void dumpDbgStack(IRGenStack *stack)
{
	IRGenStack::iterator iter;

    dbgPrint(DBGLVL_COMPILERINFO, "## STACK #####################################\n");

    for(iter = stack->begin(); iter != stack->end(); iter++) {
    	ir_instruction* ir = (*iter);
        dumpNodeInfo(ir);
        switch ( ir->debug_overwrite ) {
            case DbgOwDebugCode:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_debug> ");
                break;
            case DbgOwOriginalCode:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_original> ");
                break;
            default:
                dbgPrint(DBGLVL_COMPILERINFO, " <ir_dbg_ow_unset> ");
                break;
        }
        dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
        scopeList *sl = get_scope(ir);

        if (sl) {
            scopeList::iterator sit;
            for (sit = sl->begin(); sit != sl->end(); sit++) {
                dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "%i ", (*sit));
            }
            dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "\n");
        } else {
            dbgPrintNoPrefix(DBGLVL_COMPILERINFO, "no scope\n");
        }
    }

    dbgPrint(DBGLVL_COMPILERINFO, "###############################################\n");
}


/*
 * DEBUG INTERFACE
 */

void change_DbgOverwrite(ir_call* cir, enum ir_dbg_overwrite status)
{
    dbgPrint(DBGLVL_COMPILERINFO, "---->>> FUNCTION CALL ir_dbg_ow_%s\n",
    		status == ir_dbg_ow_original ? "original" :
    		status == ir_dbg_ow_debug ? "debug" : "unset" );
    dumpNodeInfo(cir);
    dbgPrint(DBGLVL_COMPILERINFO, "---------------------------------------\n");

    cir->debug_overwrite = status;

    ir_function_signature* funcDec = cir->callee;
    funcDec->debug_overwrite = status;
}

bool prepareTarget(ir_instruction** out, IRGenStack& dbgStack, DbgCgOptions dbgCgOptions,
					ShChangeableList *cgbl, ShVariableList *vl)
{
	ir_instruction* target = NULL;
	switch( dbgCgOptions ){
		case DBG_CG_GEOMETRY_MAP:
		case DBG_CG_VERTEX_COUNT:
			break;
		case DBG_CG_COVERAGE:
		case DBG_CG_SELECTION_CONDITIONAL:
		case DBG_CG_LOOP_CONDITIONAL:
		case DBG_CG_GEOMETRY_CHANGEABLE:
			target = dbgStack.back();
			break;
		case DBG_CG_CHANGEABLE:
		{
			IRGenStack::reverse_iterator rit;

			if( !cgbl )
				return false;

			/* iterate backwards thru stack */
			for( rit = dbgStack.rbegin(); rit != dbgStack.rend(); rit++ ){

				ir_instruction* rir = *rit;
				scopeList *sl = get_scope( rir );
				scopeList::iterator sit;
				bool user_call = (rir->ir_type == ir_type_call
						&& !rir->as_call()->callee->is_builtin);

				/* check if all changeables are in scope */bool allInScope = true;

				for( int id = 0; id < cgbl->numChangeables; id++ ){
					bool inScope = false;

					/* builtins are always valid */
					ShVariable *var = findShVariableFromId( vl,
							cgbl->changeables[id]->id );
					if( !var ){
						dbgPrint( DBGLVL_WARNING,
								"CodeGen - unknown changeable, stop debugging\n" );
						return false;
					}

					if( var->builtin ){
						inScope = true;
					}else{
						/* parse the actual scope */
						for( sit = sl->begin(); sit != sl->end(); sit++ ){
							if( ( *sit ) == cgbl->changeables[id]->id ){
								inScope = true;
								break;
							}
						}
					}

					if( !inScope ){
						/* This is not the place to read out the target */
						if( user_call )
							change_DbgOverwrite( rir->as_call(), ir_dbg_ow_original );
						else
							dumpNodeInfo( rir );

						/* pop stack since we cannot process all changeables here */
						allInScope = false;
						break;
					}
				}

				if( allInScope ){
					/* we are finially finished */
					target = rir;

					if( user_call )
						change_DbgOverwrite( rir->as_call(), ir_dbg_ow_original );
					else
						dumpNodeInfo( *rit );
					break;
				}
			}

			if( !target ){
				dbgPrint( DBGLVL_WARNING,
						"CodeGen - target not in stack, no debugging possible\n" );
				dumpDbgStack( &dbgStack );
				return false;
			}else{
				/* iterate trough the rest of the stack */
				for( rit = dbgStack.rbegin(); rit != dbgStack.rend();
						rit++ ){
					ir_instruction* rir = *rit;

					if( rir->ir_type == ir_type_call && !rir->as_call()->callee->is_builtin
							&& rir->debug_overwrite == ir_dbg_ow_unset ){
						change_DbgOverwrite( rir->as_call(), ir_dbg_ow_debug );
					}
				}
				/* DEBUG OUTPUT */
				dumpDbgStack( &dbgStack );
			}
		}
			break;
		default:
			break;
	}

	*out = target;
	return true;
}


bool compileDbgShaderCode(struct gl_shader* shader, ShChangeableList *cgbl,
        ShVariableList *vl, DbgCgOptions dbgCgOptions, char** code)
{
    if (dbgCgOptions == DBG_CG_ORIGINAL_SRC) {
        return compileShaderCode(shader);
    }

    exec_list* list = shader->ir;
//    ir_instruction* root = (ir_instruction*)list->head;

    /* Check for empty parse tree */
    if ( list->head->next == NULL || list->tail_pred == NULL )
        return false;

    EShLanguage language = EShLangVertex;
    if( shader->Type == GL_FRAGMENT_SHADER )
    	language = EShLangFragment;
    else if( shader->Type == GL_GEOMETRY_SHADER )
		language = EShLangGeometry;

    /* Version number handling */
//    if( !appendVersion(shader, m_debugProgram) )
//    	return false;

	cgInitLoopIter();

    /* 1. Pass:
     * - build up debug stack (list of all nodes on the dbgPath)
     * - mark target node where code has to be inserted
     */
	ir_stack_traverser_visitor it1pass(vl);

//    TStackTraverser it1pass(root, vl);
    it1pass.preVisit = true;
    it1pass.postVisit = false;
//
//    it1pass.visitAggregate = stackAggregate;
//    it1pass.visitBinary = stackExpression;
//    it1pass.visitUnary = stackExpression;
//    it1pass.visitSelection = stackSelection;
//    it1pass.visitLoop = stackLoop;
//    it1pass.visitBranch = stackBranch;
//    it1pass.visitDummy = stackDummy;

    ir_function* main = getFunctionBySignature(MAIN_FUNC_SIGNATURE, shader);
    if (!main) {
        dbgPrint(DBGLVL_ERROR, "CodeGen - could not find main function!\n");
        exit(1);
    }
    it1pass.visit(main);

    //Traverse(main, &it1pass);

    /* Find target, mark it and prepare neccessary dbgTemporaries */
    ir_instruction* target = NULL;
	if( !prepareTarget( &target, it1pass.dbgStack, dbgCgOptions, cgbl, vl ) )
		return false;

    if (target) {
        dbgPrint(DBGLVL_COMPILERINFO, "TARGET is %i:%i = %i:%i\n", target->yy_location.first_line,
                                                     target->yy_location.first_column,
                                                     target->yy_location.last_line,
                                                     target->yy_location.last_column);
        target->debug_target = true;
    }

    /* Prepare debug temporary registers */

    /* Always allocate a result register */
    dbgPrint(DBGLVL_COMPILERINFO, "initialize CG_TYPE_RESULT for %i\n", language);
    {
    	int size = ( dbgCgOptions == DBG_CG_GEOMETRY_MAP ||
    			     dbgCgOptions == DBG_CG_VERTEX_COUNT ) ? 3 :
    			     (dbgCgOptions == DBG_CG_GEOMETRY_CHANGEABLE) ? 2 : 0;
    	if( size ) {
    		ShVariable ret;
    		ret.uniqueId   = -1;
    		ret.builtin    = 0;
    		ret.name       = NULL;
    		ret.size       = size;
    		ret.isMatrix   = 0;
    		ret.isArray    = 0;
    		for (int i=0; i < MAX_ARRAYS; i++) {
    			ret.arraySize[0]  = 0;
    		}
    		ret.structName = NULL;
    		ret.structSize = 0;
    		ret.structSpec = NULL;
    		ret.type       = SH_FLOAT;
    		ret.qualifier  = SH_VARYING_OUT;
    		cgInit(CG_TYPE_RESULT, &ret, vl, language);
    	}else{
    		cgInit(CG_TYPE_RESULT, NULL, vl, language);
    	}
    }

    /* Check if an optional parameter register is neccessary */
    if (target && target->as_call() ) {

        /* Now check if a parameter is used for code insertion */
    	ir_call* cir = target->as_call();
    	ir_function_signature* funcDec = cir->callee;

        int lastInParameter = getFunctionDebugParameter(funcDec);
        if (lastInParameter >=0) {
        	ir_instruction* t = getSideEffectsDebugParameter( cir, lastInParameter );
        	ir_variable* var = t ? t->as_variable() : NULL;
        	if( var )
        		cgInit(CG_TYPE_PARAMETER, findShVariableFromSource(var), vl, language);
        	else {
                dbgPrint(DBGLVL_ERROR, "CodeGen - side effects returned type is invalid\n");
                exit(1);
        	}
        }
    }

    /* Check if a selection condition needs to be copied */
    if (target && (dbgCgOptions == DBG_CG_COVERAGE ||
    			   dbgCgOptions == DBG_CG_CHANGEABLE ||
         dbgCgOptions == DBG_CG_GEOMETRY_CHANGEABLE) &&
         target->ir_type == ir_type_if)
    {
        switch ( target->as_if()->debug_state_internal ) {
            case ir_dbg_if_condition_passed:
            case ir_dbg_if_if:
            case ir_dbg_if_else:
                cgInit(CG_TYPE_CONDITION, NULL, vl, language);
                break;
            default:
                break;
        }
    }

    char* debug_program = NULL;
    ir_output_traverser_visitor it(debug_program, shader, language,
    							   dbgCgOptions, vl, cgbl, &(it1pass.dbgStack));
    it.append_version();

    /* Add declaration of all neccessary types */
    cgAddDeclaration(CG_TYPE_ALL, debug_program, language);

    /* Clear the name map holding debugged names */
    cgInitNameMap();

    /* 2. Pass:
     * - do the actual code generation
     */

//    TOutputTraverser it(shader, root, m_debugProgram, language, vl, cgbl, &(it1pass.dbgStack));
//    it.preVisit = true;
//    it.postVisit = false;
//
//    it.visitAggregate = OutputAggregate;
//    it.visitBinary = OutputBinary;
//    it.visitConstantUnion = OutputConstantUnion;
//    it.visitSelection = OutputSelection;
//    it.visitSwitch = OutputSwitch;
//    it.visitCase = OutputCase;
//    it.visitSymbol = OutputSymbol;
//    it.visitFuncParam = OutputFuncParam;
//    it.visitUnary = OutputUnary;
//    it.visitLoop = OutputLoop;
//    it.visitBranch = OutputBranch;
//    it.visitDeclaration = OutputDeclaration;
//    it.visitFuncDeclaration = OutputFuncDeclaration;
//    it.visitSpecification = OutputSpecification;
//    it.visitParameter = OutputParameter;
//    it.visitDummy = OutputDummy;
//
//    it.cgOptions = dbgCgOptions;
//    it.root = root;
    it.run(list);

//    TraverseList(list, &it);

    /* Unset target again */
    if (target)
    	target->debug_target = false;

    cgDestruct(CG_TYPE_ALL);

    /* Cleanup */

    /* Reset overwrite modes */
    for (IRGenStack::iterator stit = it1pass.dbgStack.begin(), end = it1pass.dbgStack.end();
    		 stit != end; stit++) {
    	ir_instruction* stir = *stit;
    	stir->debug_overwrite = ir_dbg_ow_unset;
    	ir_call* cstir = stir->as_call();

        if ( cstir && ! cstir->callee->is_builtin ) {
        	ir_function_signature* funcDec = cstir->callee;
            funcDec->debug_overwrite = ir_dbg_ow_unset;
        }
    }
    dumpDbgStack(&(it1pass.dbgStack));
    it1pass.dbgStack.clear();

    dbgPrint(DBGLVL_COMPILERINFO, "\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n"
                   "%s\n"
                   "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n",
                   debug_program);

#if 0
#ifdef _WIN32
    *code = _strdup(debug_program);
#else
	*code = strdup(debug_program);
#endif
#endif
	*code = debug_program;

    return true;
}


