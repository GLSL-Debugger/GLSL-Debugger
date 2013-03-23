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
#ifndef _PARSER_HELPER_INCLUDED_
#define _PARSER_HELPER_INCLUDED_

#include "../Include/Common.h"
#include "../Include/ShHandle.h"
#include "SymbolTable.h"
#include "localintermediate.h"

struct TMatrixFields {
    bool wholeRow;
    bool wholeCol;
    int row;
    int col;    
};

struct TPragma {
	TPragma(bool o, bool d) : optimize(o), debug(d) { }
	bool optimize;
	bool debug;
	TPragmaTable pragmaTable;
};

//
// The following are extra variables needed during parsing, grouped together so
// they can be passed to the parser without needing a global.
//
struct TParseContext {
    TParseContext(TSymbolTable& symt, TIntermediate& interm, EShLanguage L, TInfoSink& is, TBuiltInResource* rs = NULL) : 
            intermediate(interm), symbolTable(symt), infoSink(is), language(L),
            treeRoot(0), recoveredFromError(false), numErrors(0), lexAfterType(false), loopNestingLevel(0), switchNestingLevel(0),
            inTypeParen(false), versionNumber(0),
            fragmentShaderOutput(EvqLast), contextPragma(true, false)
    {
        resources = rs;
    }
    TIntermediate& intermediate; // to hold and build a parse tree
    TSymbolTable& symbolTable;   // symbol table that goes with the language currently being parsed
    TInfoSink& infoSink;
    EShLanguage language;        // vertex or fragment language (future: pack or unpack)
    TIntermNode* treeRoot;       // root of parse tree being created
    bool recoveredFromError;     // true if a parse error has occurred, but we continue to parse
    int numErrors;
    bool lexAfterType;           // true if we've recognized a type, so can only be looking for an identifier
    int loopNestingLevel;        // 0 if outside all loops
    int switchNestingLevel;        // 0 if outside all switches
    bool inTypeParen;            // true if in parentheses, looking only for an identifier
    const TType* currentFunctionType;  // the return type of the function that's currently being parsed
    bool functionReturnsValue;   // true if a non-void function has a return
    
    // Preprocessor data for usage during code generation
    TMap<TString, TBehavior> extensionBehavior;
    void initializeExtensionBehavior();
    TExtensionList extensionChanged;
    int versionNumber;

    // Resources
    TBuiltInResource* resources;

    // Fragment shader output builtin
    TQualifier fragmentShaderOutput;

    void C_DECL error(TSourceRange, const char *szReason, const char *szToken, 
                      const char *szExtraInfoFormat, ...);
    bool reservedErrorCheck(TSourceRange, const TString& identifier);
    void recover(const char* f, int n);
    
    bool parseVectorFields(const TString&, int vecSize, TVectorFields&, TSourceRange);
    bool parseMatrixFields(const TString&, int mS1, int mS2, TMatrixFields&, TSourceRange);
    void assignError(TSourceRange, const char* op, TString left, TString right);
    void unaryOpError(TSourceRange, const char* op, TString operand);
    void binaryOpError(TSourceRange, const char* op, TString left, TString right);
    bool lValueErrorCheck(TSourceRange, const char* op, TIntermTyped*);
    bool constErrorCheck(TIntermTyped* node);
    bool integerErrorCheck(TIntermTyped* node, const char* token);
    bool globalErrorCheck(TSourceRange, bool global, const char* token);
    bool constructorErrorCheck(TSourceRange, TIntermNode*, TFunction&, TOperator, TType*);
    bool varyingModifyerErrorCheck(TSourceRange range, TPublicType type, TQualifier qual);
    bool arraySizeErrorCheck(TSourceRange, TIntermTyped* expr, int& size);
    bool arraySizeGeometryVaryingInErrorCheck(TSourceRange range, int inputPrimSize, int size);
    bool arrayQualifierErrorCheck(TSourceRange, TPublicType type);
    bool arrayTypeErrorCheck(TSourceRange, TPublicType type);
    bool arrayErrorCheck(TSourceRange, TString& identifier, TPublicType type, TVariable*& variable);
    bool arraySizeUnspecifiedErrorCheck(TSourceRange, TPublicType type);
    bool nonArrayGeometryVaryingInErrorCheck(TSourceRange range, TPublicType type, TString& identifier);
    bool arrayFullDefinedGeometryVaryingInErrorCheck(TSourceRange range, TString& identifier, TPublicType type);
    bool connectBuitInArrayWithBuiltInSymbol(const char* iArrayName, const char* iSymbolAName, const char* iSymbolBName = NULL);
    bool insertBuiltInArrayAtGlobalLevel();
    bool voidErrorCheck(TSourceRange, const TString&, const TPublicType&);
    bool boolErrorCheck(TSourceRange, const TIntermTyped*);
    bool boolErrorCheck(TSourceRange, const TPublicType&);
    bool samplerErrorCheck(TSourceRange, const TPublicType& pType, const char* reason);
    bool structQualifierErrorCheck(TSourceRange, const TPublicType& pType);
    bool parameterSamplerErrorCheck(TSourceRange, TQualifier qualifier, const TType& type);
    bool containsSampler(TType& type);
    bool nonInitConstErrorCheck(TSourceRange, TString& identifier, TPublicType& type);
    bool nonInitErrorCheck(TSourceRange, TString& identifier, TPublicType& type);
    bool paramErrorCheck(TSourceRange, TQualifier qualifier, TQualifier paramQualifier, TType* type);
    bool extensionErrorCheck(TSourceRange, const char*);
    bool extensionActiveCheck(const char* extension);
    const TFunction* findFunction(TSourceRange, TFunction* pfnCall, bool *builtIn = 0);
    bool executeInitializer(TSourceRange, TString& identifier, TPublicType& pType, 
                            TIntermTyped* initializer, TIntermNode*& intermNode, TVariable* variable = 0);
    bool areAllChildConst(TIntermAggregate* aggrNode);
    TIntermTyped* addConstructor(TIntermNode*, const TType*, TOperator, TFunction*, TSourceRange);
    TIntermTyped* foldConstConstructor(TIntermAggregate* aggrNode, const TType& type);
    TIntermTyped* constructStruct(TIntermNode*, TType*, int, TSourceRange, bool subset);
    TIntermTyped* constructBuiltIn(const TType*, TOperator, TIntermNode*, TSourceRange, bool subset);
    TIntermTyped* addConstVectorNode(TVectorFields&, TIntermTyped*, TSourceRange);
    TIntermTyped* addConstMatrixNode(int , TIntermTyped*, TSourceRange);
    TIntermTyped* addConstArrayNode(int index, TIntermTyped* node, TSourceRange);
    TIntermTyped* addConstStruct(TString& , TIntermTyped*, TSourceRange);
    bool arraySetMaxSize(TIntermSymbol*, TType*, int, bool, TSourceRange);
	struct TPragma contextPragma;
	TString HashErrMsg; 
    bool AfterEOF;
};

int PaParseStrings(char* argv[], int strLen[], int argc, TParseContext&);
void PaReservedWord();
int PaIdentOrType(TString& id, TParseContext&, TSymbol*&);
int PaParseComment(int &lineno, TParseContext&);
void setInitialState();

typedef TParseContext* TParseContextPointer;
extern TParseContextPointer& GetGlobalParseContext();
#define GlobalParseContext GetGlobalParseContext()

typedef struct TThreadParseContextRec
{
	TParseContext *lpGlobalParseContext;
} TThreadParseContext;

#endif // _PARSER_HELPER_INCLUDED_
