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
// Definition of the in-memory high-level intermediate representation
// of shaders.  This is a tree that parser creates.
//
// Nodes in the tree are defined as a hierarchy of classes derived from
// TIntermNode. Each is a node in a tree.  There is no preset branching factor;
// each node can have it's own type of list of children.
//

#ifndef __INTERMEDIATE_H
#define __INTERMEDIATE_H

#include "../Include/Common.h"
#include "../Include/Types.h"
#include "../Include/ConstantUnion.h"


//
// Operators used by the high-level (parse tree) representation.
//
enum TOperator {
    EOpNull,            // if in a node, should only mean a node is still being built
    EOpSequence,        // denotes a list of statements, or parameters, etc.
    EOpDeclaration,     // denotes a list of statements, or parameters, etc.
    EOpSpecification,   // denotes a stucture definition
    EOpParameter,       // denotes a parameter definition
    EOpFunctionCall,
    EOpFunction,        // for function definition
    EOpParameters,      // an aggregate listing the parameters to a function
    EOpInstances,       // an aggregate listing the directly defined instances
                        //    of a struct

    //
    // Unary operators
    //

    EOpNegative,
    EOpLogicalNot,
    EOpVectorLogicalNot,
    EOpBitwiseNot,

    EOpPostIncrement,
    EOpPostDecrement,
    EOpPreIncrement,
    EOpPreDecrement,

    EOpConvIntToBool,
    EOpConvUIntToBool,
    EOpConvFloatToBool,
    EOpConvBoolToFloat,
    EOpConvIntToFloat,
    EOpConvUIntToFloat,
    EOpConvFloatToInt,
    EOpConvBoolToInt,
    EOpConvUIntToInt,
    EOpConvFloatToUInt,
    EOpConvBoolToUInt,
    EOpConvIntToUInt,

    //
    // binary operations
    //

    EOpAdd,
    EOpSub,
    EOpMul,
    EOpDiv,
    EOpMod,
    EOpRightShift,
    EOpLeftShift,
    EOpAnd,
    EOpInclusiveOr,
    EOpExclusiveOr,
    EOpEqual,
    EOpNotEqual,
    EOpVectorEqual,
    EOpVectorNotEqual,
    EOpLessThan,
    EOpGreaterThan,
    EOpLessThanEqual,
    EOpGreaterThanEqual,
    EOpComma,

    EOpVectorTimesScalar,
    EOpVectorTimesMatrix,
    EOpMatrixTimesVector,
    EOpMatrixTimesScalar,

    EOpLogicalOr,
    EOpLogicalXor,
    EOpLogicalAnd,

    EOpIndexDirect,
    EOpIndexIndirect,
    EOpIndexDirectStruct,

    EOpVectorSwizzle,
    EOpSwizzles,

    //
    // Built-in functions potentially mapped to operators
    //

    EOpRadians,
    EOpDegrees,
    EOpSin,
    EOpCos,
    EOpTan,
    EOpAsin,
    EOpAcos,
    EOpAtan,

    EOpPow,
    EOpExp,
    EOpLog,
    EOpExp2,
    EOpLog2,
    EOpSqrt,
    EOpInverseSqrt,

    EOpAbs,
    EOpSign,
    EOpFloor,
    EOpCeil,
    EOpFract,
    EOpMin,
    EOpMax,
    EOpClamp,
    EOpMix,
    EOpStep,
    EOpSmoothStep,
    EOpTruncate,
    EOpRound,

    EOpLength,
    EOpDistance,
    EOpDot,
    EOpCross,
    EOpNormalize,
    EOpFaceForward,
    EOpReflect,
    EOpRefract,

    EOpDPdx,            // Fragment only
    EOpDPdy,            // Fragment only
    EOpFwidth,          // Fragment only

    EOpMatrixTimesMatrix,
    EOpMatrixOuterProduct,
    EOpMatrixTranspose,

    EOpAny,
    EOpAll,

    EOpItof,         // pack/unpack only
    EOpFtoi,         // pack/unpack only
    EOpSkipPixels,   // pack/unpack only
    EOpReadInput,    // unpack only
    EOpWritePixel,   // unpack only
    EOpBitmapLsb,    // unpack only
    EOpBitmapMsb,    // unpack only
    EOpWriteOutput,  // pack only
    EOpReadPixel,    // pack only

    //
    // Branch
    //

    EOpKill,            // Fragment only
    EOpReturn,
    EOpBreak,
    EOpContinue,

    //
    // Constructors
    //

    EOpConstructInt,
    EOpConstructUInt,
    EOpConstructBool,
    EOpConstructFloat,
    EOpConstructVec2,
    EOpConstructVec3,
    EOpConstructVec4,
    EOpConstructBVec2,
    EOpConstructBVec3,
    EOpConstructBVec4,
    EOpConstructIVec2,
    EOpConstructIVec3,
    EOpConstructIVec4,
    EOpConstructUVec2,
    EOpConstructUVec3,
    EOpConstructUVec4,
    EOpConstructMat2,
    EOpConstructMat2x3,
    EOpConstructMat2x4,
    EOpConstructMat3x2,
    EOpConstructMat3,
    EOpConstructMat3x4,
    EOpConstructMat4x2,
    EOpConstructMat4x3,
    EOpConstructMat4,
    EOpConstructStruct,

    //
    // moves
    //

    EOpAssign,
    EOpAddAssign,
    EOpSubAssign,
    EOpMulAssign,
    EOpVectorTimesMatrixAssign,
    EOpVectorTimesScalarAssign,
    EOpMatrixTimesScalarAssign,
    EOpMatrixTimesMatrixAssign,
    EOpDivAssign,
    EOpModAssign,
    EOpAndAssign,
    EOpInclusiveOrAssign,
    EOpExclusiveOrAssign,
    EOpLeftShiftAssign,
    EOpRightShiftAssign,

    //
    // Array operators
    //

    EOpArrayLength,
};


enum TDebugState {
    DbgStNone,     // not part of the debug trace
    DbgStPath,     // part of trace
    DbgStTarget,   // leaf of trace
    DbgStFinished  // mark for finished debugging; only allpicable to root
};

enum TDebugOverwrite {
    DbgOwNone,
    DbgOwOriginalCode,
    DbgOwDebugCode
};

class TIntermTraverser;
class TIntermAggregate;
class TIntermBinary;
class TIntermUnary;
class TIntermBranch;
class TIntermConstantUnion;
class TIntermSelection;
class TIntermSwitch;
class TIntermCase;
class TIntermLoop;
class TIntermTyped;
class TIntermSymbol;
class TIntermFuncParam;
class TInfoSink;
class TIntermParameter;
class TIntermDeclaration;
class TIntermFuncDeclaration;
class TIntermSpecification;
class TIntermDummy;

//
//
//
typedef std::list<int> scopeList;

//
// Base class for the tree nodes
//
class TIntermNode {
public:
    POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)
    TIntermNode() :  dbgState(DbgStNone), scope(NULL), atomic(true), sideEffects(false), target(false), emitVertex(false), discard(false), dbgOverwrite(DbgOwNone) {
        range.left.line = 0;
        range.left.colum = 0;
        range.right.line = 0;
        range.right.colum = 0;
        cgbl.numChangeables = 0;
        cgbl.changeables = NULL;
    }

    virtual TSourceRange getRange() const { return range; }
    virtual void setRange(TSourceRange r) { range = r; }
    virtual void traverse(TIntermTraverser*) = 0;
    virtual void setDebugState(TDebugState s) { dbgState = s; }
    virtual TDebugState getDebugState(void) { return dbgState; }
    virtual TIntermTyped*     getAsTyped()         { return 0; }
    virtual TIntermConstantUnion*     getAsConstantUnion()         { return 0; }
    virtual TIntermAggregate* getAsAggregate()     { return 0; }
    virtual TIntermBinary*    getAsBinaryNode()    { return 0; }
    virtual TIntermUnary*     getAsUnaryNode()    { return 0; }
    virtual TIntermBranch*    getAsBranchNode()    { return 0; }
    virtual TIntermSelection* getAsSelectionNode() { return 0; }
    virtual TIntermSwitch*    getAsSwitchNode() { return 0; }
    virtual TIntermCase*    getAsCaseNode() { return 0; }
    virtual TIntermLoop*      getAsLoopNode() { return 0; }
    virtual TIntermSymbol*    getAsSymbolNode()    { return 0; }
    virtual TIntermFuncParam*    getAsFuncParamNode()    { return 0; }
    virtual TIntermParameter*    getAsParameterNode()    { return 0; }
    virtual TIntermDeclaration*    getAsDeclarationNode()    { return 0; }
    virtual TIntermFuncDeclaration* getAsFuncDeclarationNode() { return 0; }
    virtual TIntermSpecification*    getAsSpecificationNode()    { return 0; }
    virtual TIntermDummy*     getAsDummy() { return 0; }
    scopeList* getScope(void) { return scope; }
    void setScope(scopeList *s) { scope = s; }
    ShChangeableList* getCgbList(void) { return &cgbl; }

    bool isAtomic(void)     { return atomic == true; }
    void setNonAtomic(void) { atomic = false; }

    bool hasNoSideEffects(void)  { return sideEffects == false; }
    bool hasSideEffects(void)    { return sideEffects == true; }
    void setHasSideEffects(void) { sideEffects = true; }

    bool containsEmitVertex(void) { return emitVertex == true; }
    void setEmitVertex(void)      { emitVertex = true; }

    bool containsDiscard(void)    { return discard == true; }
    void setContainsDiscard(void) { discard = true; }

    void setTarget(void)      { target = true; }
    void unsetTarget(void)    { target = false; }
    bool isTarget(void)       { return target; }

    void setDbgOverwrite(TDebugOverwrite o) { dbgOverwrite = o; }
    TDebugOverwrite getDbgOverwrite(void)   { return dbgOverwrite; }

    void copyCPPExtensionChanges(TExtensionList&);
    void moveCPPExtensionChanges(TExtensionList&);
    TExtensionList& getCPPExtensionList(void) { return extensionList; }

    virtual ~TIntermNode() { }
protected:
    TSourceRange range;
    TDebugState dbgState;
    scopeList *scope;
    ShChangeableList cgbl;
    bool atomic;
    bool sideEffects;
    bool target;
    bool emitVertex;
    bool discard;
    TDebugOverwrite dbgOverwrite;

    TExtensionList extensionList;

};

//
// This is just to help yacc.
//
struct TIntermNodePair {
    TIntermNode* node1;
    TIntermNode* node2;
};

class TIntermSymbol;
class TIntermBinary;

//
// Intermediate class for nodes that have a type.
//
class TIntermTyped : public TIntermNode {
public:
	TIntermTyped(const TType& t) : type(t)  { }
    virtual TIntermTyped* getAsTyped()         { return this; }
    virtual void setType(const TType& t) { type = t; }
    virtual TType getType() const { return type; }
    virtual TType* getTypePointer() { return &type; }

    virtual TBasicType getBasicType() const { return type.getBasicType(); }
    virtual TQualifier getQualifier() const { return type.getQualifier(); }
    virtual int getNominalSize() const { return type.getNominalSize(); }
    virtual int getMatrixSize(int i) const { return type.getMatrixSize(i); }
    virtual int getSize() const { return type.getInstanceSize(); }
    virtual bool isMatrix() const { return type.isMatrix(); }
    virtual bool isArray()  const { return type.isArray(); }
    virtual bool isVector() const { return type.isVector(); }
    const char* getBasicString()      const { return type.getBasicString(); }
    const char* getQualifierString(EShLanguage l)  const { return type.getQualifierString(l); }
    TString getCompleteString() const { return type.getCompleteString(); }

protected:
    TType type;
};


//
// Internal states for loops
//
typedef enum {
    DBG_STATE_LOOP_UNSET,
    DBG_STATE_LOOP_QYR_INIT,
    DBG_STATE_LOOP_WRK_INIT,
    DBG_STATE_LOOP_QYR_TEST,
    DBG_STATE_LOOP_WRK_TEST,
    DBG_STATE_LOOP_SELECT_FLOW,
    DBG_STATE_LOOP_WRK_BODY,
    DBG_STATE_LOOP_QYR_TERMINAL,
    DBG_STATE_LOOP_WRK_TERMINAL,
    DBG_STATE_LOOP_PASSED
} DbgStateLoop;

enum LoopT {
    LOOP_WHILE,
    LOOP_DO,
    LOOP_FOR
};

//
// Handle for, do-while, and while loops.
//
class TIntermLoop : public TIntermNode {
public:
    TIntermLoop(TIntermNode* aBody, TIntermNode *aInit, TIntermTyped* aTest,
                TIntermTyped* aTerminal, LoopT aLoopType) :
        init(aInit),
        body(aBody),
        test(aTest),
        terminal(aTerminal),
        loopType(aLoopType),
        dbgState(DBG_STATE_LOOP_UNSET),
        dbgIteration(0),
        dbgLoopIter(NULL) { }
    virtual TIntermLoop* getAsLoopNode() { return this; }
    virtual void traverse(TIntermTraverser*);
    TIntermNode*  getInit() { return init; }
    TIntermNode*  getBody() { return body; }
    TIntermTyped* getTest() { return test; }
    TIntermTyped* getTerminal() { return terminal; }
    LoopT         getLoopType(void) { return loopType; }
    virtual DbgStateLoop getDbgInternalState() { return dbgState; }
    virtual void setDbgInternalState(DbgStateLoop ds) { dbgState = ds; }

    virtual int  getDbgIter(void) { return dbgIteration; }
    virtual void setDbgIter(int i) { dbgIteration = i; }
    virtual void addDbgIter(int i) { dbgIteration += i; }

    virtual char** getDbgIterNamePointer(void) { return &dbgLoopIter; }
    virtual char*  getDbgIterName(void) { return dbgLoopIter; }
    virtual void clearDbgIterName(void) { free(dbgLoopIter);
                                          dbgLoopIter = NULL; }

    virtual bool needDbgLoopIter(void);
protected:
    TIntermNode* init;
    TIntermNode* body;       // code to loop over
    TIntermTyped* test;      // exit condition associated with loop,
                             // could be 0 for 'for' loops
    TIntermTyped* terminal;  // exists for for-loops
    LoopT loopType;

    DbgStateLoop dbgState;   // internal state used for debugging
    int dbgIteration;        // loop iteration that is currently debugged
    char *dbgLoopIter;       // name of the temporary variable used
};

//
// Handle break, continue, return, and kill.
//
class TIntermBranch : public TIntermNode {
public:
    TIntermBranch(TOperator op, TIntermTyped* e) :
        flowOp(op),
        expression(e) { }
    virtual TIntermBranch* getAsBranchNode()         { return this; }
    virtual void traverse(TIntermTraverser*);
    TOperator getFlowOp() { return flowOp; }
    TIntermTyped* getExpression() { return expression; }
protected:
    TOperator flowOp;
    TIntermTyped* expression;  // non-zero except for "return exp;" statements
};

//
// Nodes that correspond to symbols or constants in the source code.
//
class TIntermSymbol : public TIntermTyped {
public:
	// if symbol is initialized as symbol(sym), the memory comes from the poolallocator of sym. If sym comes from
	// per process globalpoolallocator, then it causes increased memory usage per compile
	// it is essential to use "symbol = sym" to assign to symbol
    TIntermSymbol(int i, const TString& sym, const TType& t) :
        TIntermTyped(t), id(i)  { symbol = sym;}
    virtual int getId() const { return id; }
    virtual const TString& getSymbol() const { return symbol;  }
    virtual void traverse(TIntermTraverser*);
    virtual TIntermSymbol* getAsSymbolNode() { return this; }
protected:
    int id;
    TString symbol;
};

//
// Nodes that correspond to symbols or constants in the source code.
//
class TIntermFuncParam : public TIntermTyped {
public:
	// if symbol is initialized as symbol(sym), the memory comes from the poolallocator of sym. If sym comes from
	// per process globalpoolallocator, then it causes increased memory usage per compile
	// it is essential to use "symbol = sym" to assign to symbol
    TIntermFuncParam(int i, const TString& sym, const TType& t) :
        TIntermTyped(t), id(i)  { symbol = sym;}
    virtual int getId() const { return id; }
    virtual const TString& getSymbol() const { return symbol;  }
    virtual void traverse(TIntermTraverser*);
    virtual TIntermFuncParam* getAsFuncParamNode() { return this; }
protected:
    int id;
    TString symbol;
};

class TIntermConstantUnion : public TIntermTyped {
public:
    TIntermConstantUnion(constUnion *unionPointer, const TType& t) : TIntermTyped(t), unionArrayPointer(unionPointer) { }
    constUnion* getUnionArrayPointer() const { return unionArrayPointer; }
    void setUnionArrayPointer(constUnion *c) { unionArrayPointer = c; }
    virtual TIntermConstantUnion* getAsConstantUnion()  { return this; }
    virtual void traverse(TIntermTraverser* );
    virtual TIntermTyped* fold(TOperator, TIntermTyped*, TInfoSink&);
protected:
    constUnion *unionArrayPointer;
};

//
// Intermediate class for node types that hold operators.
//
class TIntermOperator : public TIntermTyped {
public:
    TOperator getOp() { return op; }
    bool modifiesState() const;
    bool isConstructor() const;
    virtual bool promote(TInfoSink&) { return true; }
protected:
    TIntermOperator(TOperator o) : TIntermTyped(TType(EbtFloat)), op(o) {}
    TIntermOperator(TOperator o, TType& t) : TIntermTyped(t), op(o) {}
    TOperator op;
};

//
// Nodes for all the basic binary math operators.
//
class TIntermBinary : public TIntermOperator {
public:
    TIntermBinary(TOperator o) : TIntermOperator(o) {}
    virtual void traverse(TIntermTraverser*);
    virtual void setLeft(TIntermTyped* n) { left = n; }
    virtual void setRight(TIntermTyped* n) { right = n; }
    virtual TIntermTyped* getLeft() const { return left; }
    virtual TIntermTyped* getRight() const { return right; }
    virtual TIntermBinary* getAsBinaryNode() { return this; }
    virtual bool promote(TInfoSink&);
protected:
    TIntermTyped* left;
    TIntermTyped* right;
};

//
// Nodes for unary math operators.
//
class TIntermUnary : public TIntermOperator {
public:
    TIntermUnary(TOperator o, TType& t) : TIntermOperator(o, t), operand(0) {}
    TIntermUnary(TOperator o) : TIntermOperator(o), operand(0) {}
    virtual void traverse(TIntermTraverser*);
    virtual void setOperand(TIntermTyped* o) { operand = o; }
    virtual TIntermTyped* getOperand() { return operand; }
    virtual TIntermUnary* getAsUnaryNode() { return this; }
    virtual bool promote(TInfoSink&);
protected:
    TIntermTyped* operand;
};

typedef TVector<TIntermNode*> TIntermSequence;
typedef TVector<int> TQualifierList;
//
// Nodes that operate on an arbitrary sized set of children.
//
class TIntermAggregate : public TIntermOperator {
public:
    TIntermAggregate() : TIntermOperator(EOpNull), userDefined(false), pragmaTable(0), alreadyParsed(0) {
        cgbParameterList.numChangeables = 0;
        cgbParameterList.changeables = NULL;
    }
    TIntermAggregate(TOperator o) : TIntermOperator(o), userDefined(false), pragmaTable(0), alreadyParsed(0)
    {
        cgbParameterList.numChangeables = 0;
        cgbParameterList.changeables = NULL;
    }
	~TIntermAggregate() { delete pragmaTable; }
    virtual TIntermAggregate* getAsAggregate() { return this; }
    virtual void setOperator(TOperator o) { op = o; }
    virtual TIntermSequence& getSequence() { return sequence; }
	virtual void setName(const TString& n) { name = n; }
    virtual const TString& getName() const { return name; }
    virtual void traverse(TIntermTraverser*);
    virtual void setUserDefined() { userDefined = true; }
    virtual bool isUserDefined() { return userDefined; }
    virtual TQualifierList& getQualifier() { return qualifier; }
	void setOptimize(bool o) { optimize = o; }
	void setDebug(bool d) { debug = d; }
	bool getOptimize() { return optimize; }
	bool getDebug() { return debug; }
	void addToPragmaTable(const TPragmaTable& pTable);
	const TPragmaTable& getPragmaTable() const { return *pragmaTable; }
    ShChangeableList* getCgbParameterList(void) { return &cgbParameterList; }
    bool isAlreadyParsed(void) { return alreadyParsed; }
    void setAlreadyParsed(void) { alreadyParsed = true; }
protected:
	TIntermAggregate(const TIntermAggregate&); // disallow copy constructor
	TIntermAggregate& operator=(const TIntermAggregate&); // disallow assignment operator
    TIntermSequence sequence;
    TQualifierList qualifier;
	TString name;
    bool userDefined; // used for user defined function names
	bool optimize;
	bool debug;
	TPragmaTable *pragmaTable;
    ShChangeableList cgbParameterList;
    bool alreadyParsed;
};

//
// For if tests.
//
typedef enum {
    DBG_STATE_SELECTION_UNSET,             // not debugged so far
    DBG_STATE_SELECTION_INIT,              // already visited once
    DBG_STATE_SELECTION_CONDITION,         // condition in process
    DBG_STATE_SELECTION_CONDITION_PASSED,  // condition processed
    DBG_STATE_SELECTION_IF,                // descided to debug true branch
    DBG_STATE_SELECTION_ELSE,              // descided to debug false branch
    DBG_STATE_SELECTION_PASSED             // debugging is past selection
} DbgStateSelection;

class TIntermSelection : public TIntermTyped {
public:
    // 3 parameter equals 'if' operator
    TIntermSelection(TIntermTyped* cond, TIntermNode* trueB, TIntermNode* falseB) : TIntermTyped(TType(EbtVoid)), condition(cond), trueBlock(trueB), falseBlock(falseB), shortOperation(false), dbgState(DBG_STATE_SELECTION_UNSET) {}
    // 4 parameter equals '?' operator
    TIntermSelection(TIntermTyped* cond, TIntermNode* trueB, TIntermNode* falseB, const TType& type) : TIntermTyped(type), condition(cond), trueBlock(trueB), falseBlock(falseB), shortOperation(true), dbgState(DBG_STATE_SELECTION_UNSET) {}
    virtual void traverse(TIntermTraverser*);
    virtual TIntermNode* getCondition() const { return condition; }
    virtual TIntermNode* getTrueBlock() const { return trueBlock; }
    virtual TIntermNode* getFalseBlock() const { return falseBlock; }
    virtual TIntermSelection* getAsSelectionNode() { return this; }
    virtual bool isShort(void) { return shortOperation; }
    virtual DbgStateSelection getDbgInternalState() { return dbgState; }
    virtual void setDbgInternalState(DbgStateSelection ds) { dbgState = ds; }
protected:
    TIntermTyped* condition;
    TIntermNode* trueBlock;
    TIntermNode* falseBlock;
    bool shortOperation;
    DbgStateSelection dbgState;
};

//
// For switch tests.
//
class TIntermSwitch : public TIntermTyped {
public:
    TIntermSwitch(TIntermTyped* cond, TIntermAggregate* caseList) : TIntermTyped(TType(EbtVoid)), condition(cond), caseList(caseList), dbgState(DBG_STATE_SELECTION_UNSET) {}

    virtual void traverse(TIntermTraverser*);
    virtual TIntermNode* getCondition() const { return condition; }
    virtual TIntermNode* getCaseList() const { return caseList; }
    virtual TIntermSwitch* getAsSwitchNode() { return this; }
    virtual DbgStateSelection getDbgInternalState() { return dbgState; }
    virtual void setDbgInternalState(DbgStateSelection ds) { dbgState = ds; }
protected:
    TIntermTyped* condition;
    TIntermAggregate* caseList;
    DbgStateSelection dbgState;
};

//
// For switch cases.
//
class TIntermCase : public TIntermTyped {
public:
    TIntermCase(TIntermTyped* expr) : TIntermTyped( expr ? expr->getType() : TType(EbtVoid)), expression(expr) { caseBody = new TIntermAggregate(); }

    virtual void traverse(TIntermTraverser*);
    virtual TIntermTyped* getExpression() const { return expression; }
    virtual TIntermNode* getCaseBody() const { return caseBody; }
    virtual TIntermCase* getAsCaseNode() { return this; }

protected:
    TIntermTyped* expression;
    TIntermAggregate* caseBody;
};


//
// For variable declarations.
//
class TVariable;
class TIntermDeclaration : public TIntermNode {
public:
    TIntermDeclaration(TVariable* v, TIntermNode* init);
    virtual void traverse(TIntermTraverser*);
    virtual TIntermDeclaration* getAsDeclarationNode() { return this; }
    virtual TVariable* getVariable() const { return variable; }
    virtual TIntermNode* getInitialization() const { return initialization; }
    void setFirst(bool f) { first = f; }
    bool isFirst() { return first; }
protected:
    TVariable* variable;
    TIntermNode* initialization;
    bool first;
};

//
// For function declarations.
//
class TFunction;
class TIntermFuncDeclaration : public TIntermNode {
    public:
        TIntermFuncDeclaration(TFunction* f) : function(f) { }
        virtual void traverse(TIntermTraverser*);
        virtual TIntermFuncDeclaration* getAsFuncDeclarationNode() {
            return this; }
        virtual TFunction* getFunction() const { return function; }
    protected:
        TFunction* function;
};

//
//
//
class TIntermSpecification : public TIntermNode {
public:
    TIntermSpecification(TType* t);
    virtual void traverse(TIntermTraverser*);
    virtual TIntermSpecification* getAsSpecificationNode() { return this; }
    virtual TType* getType() const { return type; }
    void setParameter(TIntermAggregate *pl) { parameter = pl; }
    TIntermAggregate** getParameterPointer(void) { return &parameter; }
    TIntermAggregate* getParameter(void) { return parameter; }
    void setInstances(TIntermAggregate *in) { instances = in; }
    TIntermAggregate** getInstancesPointer(void) { return &instances; }
    TIntermAggregate* getInstances(void) { return instances; }
protected:
    TType* type;
    TIntermAggregate* parameter;
    TIntermAggregate* instances;
};

//
//
//
class TIntermParameter : public TIntermNode {
public:
    TIntermParameter(TType* t);
    virtual void traverse(TIntermTraverser*);
    virtual TIntermParameter* getAsParameterNode() { return this; }
    virtual TType* getType() const { return type; }
protected:
    TType* type;
};

//
// Intermediate class for dummy nodes
//
class TIntermDummy : public TIntermNode {
public:
    TIntermDummy(void) { }
    virtual TIntermDummy* getAsDummy()         { return this; }
    virtual void traverse(TIntermTraverser*);
};


//
// For traversing the tree.  User should derive from this,
// put their traversal specific data in it, and then pass
// it to a Traverse method.
//
// When using this, just fill in the methods for nodes you want visited.
// Return false from a pre-visit to skip visiting that node's subtree.
//
class TIntermTraverser {
public:
    POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)

    TIntermTraverser() :
        visitSymbol(0),
        visitFuncParam(0),
        visitConstantUnion(0),
        visitDeclaration(0),
        visitFuncDeclaration(0),
        visitSpecification(0),
        visitParameter(0),
        visitBinary(0),
        visitUnary(0),
        visitSelection(0),
        visitSwitch(0),
        visitCase(0),
        visitAggregate(0),
        visitLoop(0),
        visitBranch(0),
        visitDummy(0),
        depth(0),
        preVisit(true),
        postVisit(false),
        debugVisit(false),
        rightToLeft(false) {}

    void (*visitSymbol)(TIntermSymbol*, TIntermTraverser*);
    bool (*visitFuncParam)(ir_variable*, TIntermTraverser*);
    void (*visitConstantUnion)(TIntermConstantUnion*, TIntermTraverser*);
    bool (*visitDeclaration)(TIntermDeclaration*, TIntermTraverser*);
    void (*visitFuncDeclaration)(TIntermFuncDeclaration*, TIntermTraverser*);
    bool (*visitSpecification)(TIntermSpecification*, TIntermTraverser*);
    void (*visitParameter)(TIntermParameter*, TIntermTraverser*);
    bool (*visitBinary)(bool preVisit, TIntermBinary*, TIntermTraverser*);
    bool (*visitUnary)(bool preVisit, TIntermUnary*, TIntermTraverser*);
    bool (*visitSelection)(bool preVisit, TIntermSelection*, TIntermTraverser*);
    bool (*visitSwitch)(bool preVisit, TIntermSwitch*, TIntermTraverser*);
    bool (*visitCase)(bool preVisit, TIntermCase*, TIntermTraverser*);
    bool (*visitAggregate)(bool preVisit, TIntermAggregate*, TIntermTraverser*);
    bool (*visitLoop)(bool preVisit, TIntermLoop*, TIntermTraverser*);
    bool (*visitBranch)(bool preVisit, TIntermBranch*,  TIntermTraverser*);
    void (*visitDummy)(TIntermDummy*, TIntermTraverser*);

    int  depth;
    bool preVisit;
    bool postVisit;
    bool debugVisit;
    bool rightToLeft;
};

#endif // __INTERMEDIATE_H
