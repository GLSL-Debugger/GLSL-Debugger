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

#include "ParseHelper.h"
#include "Include/InitializeParseContext.h"
#include "osinclude.h"
#include <stdarg.h>
#include "../glsldb/utils/dbgprint.h"

///////////////////////////////////////////////////////////////////////
//
// Sub- vector and matrix fields
//
////////////////////////////////////////////////////////////////////////

//
// Look at a '.' field selector string and change it into offsets
// for a vector.
//
bool TParseContext::parseVectorFields(const TString& compString, int vecSize, TVectorFields& fields, TSourceRange range)
{
    fields.num = (int) compString.size();
    if (fields.num > 4) {
        error(range, "illegal vector field selection", compString.c_str(), "");
        return false;
    }

    enum {
        exyzw,
        ergba,
        estpq,
    } fieldSet[4];

    for (int i = 0; i < fields.num; ++i) {
        switch (compString[i])  {
        case 'x':
            fields.offsets[i] = 0;
            fieldSet[i] = exyzw;
            break;
        case 'r':
            fields.offsets[i] = 0;
            fieldSet[i] = ergba;
            break;
        case 's':
            fields.offsets[i] = 0;
            fieldSet[i] = estpq;
            break;
        case 'y':
            fields.offsets[i] = 1;
            fieldSet[i] = exyzw;
            break;
        case 'g':
            fields.offsets[i] = 1;
            fieldSet[i] = ergba;
            break;
        case 't':
            fields.offsets[i] = 1;
            fieldSet[i] = estpq;
            break;
        case 'z':
            fields.offsets[i] = 2;
            fieldSet[i] = exyzw;
            break;
        case 'b':
            fields.offsets[i] = 2;
            fieldSet[i] = ergba;
            break;
        case 'p':
            fields.offsets[i] = 2;
            fieldSet[i] = estpq;
            break;

        case 'w':
            fields.offsets[i] = 3;
            fieldSet[i] = exyzw;
            break;
        case 'a':
            fields.offsets[i] = 3;
            fieldSet[i] = ergba;
            break;
        case 'q':
            fields.offsets[i] = 3;
            fieldSet[i] = estpq;
            break;
        default:
            error(range, "illegal vector field selection", compString.c_str(), "");
            return false;
        }
    }

    for (int i = 0; i < fields.num; ++i) {
        if (fields.offsets[i] >= vecSize) {
            error(range, "vector field selection out of range",  compString.c_str(), "");
            return false;
        }

        if (i > 0) {
            if (fieldSet[i] != fieldSet[i-1]) {
                error(range, "illegal - vector component fields not from the same set", compString.c_str(), "");
                return false;
            }
        }
    }

    return true;
}


//
// Look at a '.' field selector string and change it into offsets
// for a matrix.
//
bool TParseContext::parseMatrixFields(const TString& compString, int mS1, int mS2, TMatrixFields& fields, TSourceRange range)
{
    fields.wholeRow = false;
    fields.wholeCol = false;
    fields.row = -1;
    fields.col = -1;

    if (compString.size() != 2) {
        error(range, "illegal length of matrix field selection", compString.c_str(), "");
        return false;
    }

    if (compString[0] == '_') {
        if (compString[1] < '0' || compString[1] > '3') {
            error(range, "illegal matrix field selection", compString.c_str(), "");
            return false;
        }
        fields.wholeCol = true;
        fields.col = compString[1] - '0';
    } else if (compString[1] == '_') {
        if (compString[0] < '0' || compString[0] > '3') {
            error(range, "illegal matrix field selection", compString.c_str(), "");
            return false;
        }
        fields.wholeRow = true;
        fields.row = compString[0] - '0';
    } else {
        if (compString[0] < '0' || compString[0] > '3' ||
            compString[1] < '0' || compString[1] > '3') {
            error(range, "illegal matrix field selection", compString.c_str(), "");
            return false;
        }
        fields.row = compString[0] - '0';
        fields.col = compString[1] - '0';
    }

    if (fields.row >= mS1 || fields.col >= mS2) {
        error(range, "matrix field selection out of range", compString.c_str(), "");
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////
//
// Errors
//
////////////////////////////////////////////////////////////////////////

//
// Track whether errors have occurred.
//
void TParseContext::recover(const char* f, int n)
{
    infoSink.info.prefix(EPrefixError);
    infoSink.info << "Recover" << n << "\n";
    recoveredFromError = true;
    dbgPrint(DBGLVL_INFO, "recover exit at %s:%i\n", f, n);
}

//
// Used by flex/bison to output all syntax and parsing errors.
//
void C_DECL TParseContext::error(TSourceRange nRange, const char *szReason, const char *szToken,
                                const char *szExtraInfoFormat, ...)
{
    char szExtraInfo[400];
    va_list marker;

    va_start(marker, szExtraInfoFormat);

    _vsnprintf(szExtraInfo, sizeof(szExtraInfo), szExtraInfoFormat, marker);

    /* VC++ format: file(linenum) : error #: 'token' : extrainfo */
    infoSink.info.prefix(EPrefixError);
    infoSink.info.location(nRange);
    infoSink.info << "'" << szToken <<  "' : " << szReason << " " << szExtraInfo << "\n";

    va_end(marker);

    ++numErrors;
}

//
// Same error message for all places assignments don't work.
//
void TParseContext::assignError(TSourceRange range, const char* op, TString left, TString right)
{
    error(range, "", op, "cannot convert from '%s' to '%s'",
        right.c_str(), left.c_str());
}

//
// Same error message for all places unary operations don't work.
//
void TParseContext::unaryOpError(TSourceRange range, const char* op, TString operand)
{
error(range, " wrong operand type", op,
        "no operation '%s' exists that takes an operand of type %s (or there is no acceptable conversion)",
        op, operand.c_str());
}

//
// Same error message for all binary operations don't work.
//
void TParseContext::binaryOpError(TSourceRange range, const char* op, TString left, TString right)
{
    error(range, " wrong operand types ", op,
            "no operation '%s' exists that takes a left-hand operand of type '%s' and "
            "a right operand of type '%s' (or there is no acceptable conversion)",
            op, left.c_str(), right.c_str());
}

//
// Both test and if necessary, spit out an error, to see if the node is really
// an l-value that can be operated on this way.
//
// Returns true if the was an error.
//
bool TParseContext::lValueErrorCheck(TSourceRange range, const char* op, TIntermTyped* node)
{
    TIntermSymbol* symNode = node->getAsSymbolNode();
    TIntermBinary* binaryNode = node->getAsBinaryNode();

    if (binaryNode) {
        bool errorReturn;

        switch(binaryNode->getOp()) {
        case EOpIndexDirect:
        case EOpIndexIndirect:
        case EOpIndexDirectStruct:
            return lValueErrorCheck(range, op, binaryNode->getLeft());
        case EOpVectorSwizzle:
            errorReturn = lValueErrorCheck(range, op, binaryNode->getLeft());
            if (!errorReturn) {
                int offset[4] = {0,0,0,0};

                TIntermTyped* rightNode = binaryNode->getRight();
                TIntermAggregate *aggrNode = rightNode->getAsAggregate();

                for (TIntermSequence::iterator p = aggrNode->getSequence().begin();
                                            p != aggrNode->getSequence().end(); p++) {
                    int value = (*p)->getAsTyped()->getAsConstantUnion()->getUnionArrayPointer()->getIConst();
                    offset[value]++;
                    if (offset[value] > 1) {
                        error(range, " l-value of swizzle cannot have duplicate components", op, "", "");

                        return true;
                    }
                }
            }

            return errorReturn;
        default:
            break;
        }
        error(range, " l-value required", op, "", "");

        return true;
    }


    const char* symbol = 0;
    if (symNode != 0)
        symbol = symNode->getSymbol().c_str();

    const char* message = 0;
    switch (node->getQualifier()) {
        case EvqConst:          message = "can't modify a const";          break;
        case EvqConstNoValue:   message = "can't modify a const";          break;
        case EvqConstReadOnly:  message = "can't modify a const";          break;
        case EvqAttribute:      message = "can't modify an attribute";     break;
        case EvqUniform:        message = "can't modify a uniform";        break;
        case EvqVaryingIn:      message = "can't modify a varying";        break;
        case EvqInput:          message = "can't modify an input";         break;
        case EvqVertexID:       message = "can't modify gl_VertexID";      break;
        case EvqInstanceID:     message = "can't modify gl_InstanceID";    break;
        case EvqFace:           message = "can't modify gl_FrontFace";     break;
        case EvqFragCoord:      message = "can't modify gl_FragCoord";     break;
        case EvqPrimitiveID:    {
                                    if (language != EShLangGeometry) {
                                        message = "can't modify gl_PrimitiveID";
                                    }
                                    break;
                                }
        case EvqPrimitiveIDIn:  message = "can't modify gl_PrimitiveIDIn"; break;
        case EvqLayer:          {
                                    if (language != EShLangGeometry) {
                                        message = "can't modify gl_Layer";
                                    }
                                    break;
                                }
        default:

            //
            // Type that can't be written to?
            //
            switch (node->getBasicType()) {
                case EbtSampler1D:
                case EbtISampler1D:        // EXT_gpu_shader4
                case EbtUSampler1D:        // EXT_gpu_shader4
                case EbtSampler2D:
                case EbtISampler2D:        // EXT_gpu_shader4
                case EbtUSampler2D:        // EXT_gpu_shader4
                case EbtSampler3D:
                case EbtISampler3D:        // EXT_gpu_shader4
                case EbtUSampler3D:        // EXT_gpu_shader4
                case EbtSamplerCube:
                case EbtISamplerCube:      // EXT_gpu_shader4
                case EbtUSamplerCube:      // EXT_gpu_shader4
                case EbtSampler1DShadow:
                case EbtSampler2DShadow:
                case EbtSampler2DRect:     // ARB_texture_rectangle
                case EbtISampler2DRect:    // EXT_gpu_shader4
                case EbtUSampler2DRect:    // EXT_gpu_shader4
                case EbtSampler2DRectShadow: // ARB_texture_rectangle
                case EbtSampler1DArray:      // EXT_gpu_shader4
                case EbtISampler1DArray:     // EXT_gpu_shader4
                case EbtUSampler1DArray:     // EXT_gpu_shader4
                case EbtSampler2DArray:      // EXT_gpu_shader4
                case EbtISampler2DArray:     // EXT_gpu_shader4
                case EbtUSampler2DArray:     // EXT_gpu_shader4
                case EbtSamplerBuffer:       // EXT_gpu_shader4
                case EbtISamplerBuffer:      // EXT_gpu_shader4
                case EbtUSamplerBuffer:      // EXT_gpu_shader4
                case EbtSampler1DArrayShadow:      // EXT_gpu_shader4
                case EbtSampler2DArrayShadow:      // EXT_gpu_shader4
                case EbtSamplerCubeShadow:         // EXT_gpu_shader4
                    message = "can't modify a sampler";
                    break;
                case EbtVoid:
                    message = "can't modify void";
                    break;
                default:
                    break;
            }
    }

    // Additional test for fragment shader output convention (GL_ARB_draw_buffers)
    switch (node->getQualifier()) {
        case EvqFragColor:
            switch (fragmentShaderOutput) {
                case EvqLast:
                    fragmentShaderOutput = EvqFragColor;
                    break;
                case EvqFragData:
                    message = "can't modify gl_FragColor after modifying gl_FragData";   break;
                    break;
                default:
                    break;
            }
            break;
        case EvqFragData:
            switch (fragmentShaderOutput) {
                case EvqLast:
                    fragmentShaderOutput = EvqFragData;
                    break;
                case EvqFragColor:
                    message = "can't modify gl_FragData after modifying gl_FragColor";   break;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    if (message == 0 && binaryNode == 0 && symNode == 0) {
        error(range, " l-value required", op, "", "");

        return true;
    }


    //
    // Everything else is okay, no error.
    //
    if (message == 0) {
        return false;
    }

    //
    // If we get here, we have an error and a message.
    //
    if (symNode)
        error(range, " l-value required", op, "\"%s\" (%s)", symbol, message);
    else
        error(range, " l-value required", op, "(%s)", message);

    return true;
}

//
// Both test, and if necessary spit out an error, to see if the node is really
// a constant.
//
// Returns true if the was an error.
//
bool TParseContext::constErrorCheck(TIntermTyped* node)
{
    if (node->getQualifier() == EvqConst || node->getQualifier() == EvqConstNoValue)
        return false;

    error(node->getRange(), "constant expression required", "", "");

    return true;
}

//
// Both test, and if necessary spit out an error, to see if the node is really
// an integer.
//
// Returns true if the was an error.
//
bool TParseContext::integerErrorCheck(TIntermTyped* node, const char* token)
{
    if ((node->getBasicType() == EbtInt || node->getBasicType() == EbtUInt) &&
        node->getNominalSize() == 1 &&
        !(node->isMatrix()))
        return false;

    error(node->getRange(), "integer expression required", token, "");

    return true;
}

//
// Both test, and if necessary spit out an error, to see if we are currently
// globally scoped.
//
// Returns true if the was an error.
//
bool TParseContext::globalErrorCheck(TSourceRange range, bool global, const char* token)
{
    if (global)
        return false;

    error(range, "only allowed at global scope", token, "");

    return true;
}

//
// For now, keep it simple:  if it starts "gl_", it's reserved, independent
// of scope.  Except, if the symbol table is at the built-in push-level,
// which is when we are parsing built-ins.
//
// Returns true if there was an error.
//
bool TParseContext::reservedErrorCheck(TSourceRange range, const TString& identifier)
{
    if (!symbolTable.atBuiltInLevel()) {
        if (identifier.substr(0, 3) == TString("gl_")) {
            error(range, "reserved built-in name", "gl_", "");
            return true;
        }
        if (identifier.find("__") != TString::npos) {
            //error(range, "Two consecutive underscores are reserved for future use.", identifier.c_str(), "", "");
            //return true;
            infoSink.info.message(EPrefixWarning, "Two consecutive underscores are reserved for future use.", range);
            return false;
        }
    }

    return false;
}

//
// Make sure there is enough data provided to the constructor to build
// something of the type of the constructor.  Also returns the type of
// the constructor.
//
// Returns true if there was an error in construction.
//
bool TParseContext::constructorErrorCheck(TSourceRange range, TIntermNode* node, TFunction& function, TOperator op, TType* type)
{
    *type = function.getReturnType();

    bool constructingMatrix = false;
    switch(op) {
    case EOpConstructMat2:
    case EOpConstructMat2x3:
    case EOpConstructMat2x4:
    case EOpConstructMat3x2:
    case EOpConstructMat3:
    case EOpConstructMat3x4:
    case EOpConstructMat4x2:
    case EOpConstructMat4x3:
    case EOpConstructMat4:
        constructingMatrix = true;
        break;
    default:
        break;
    }

    //
    // Note: It's okay to have too many components available, but not okay to have unused
    // arguments.  'full' will go to true when enough args have been seen.  If we loop
    // again, there is an extra argument, so 'overfull' will become true.
    //

    int size = 0;
    bool constType = true;
    bool constNoValue = false;
    bool full = false;
    bool overFull = false;
    bool matrixInMatrix = false;
    bool arrayArg = false;
    for (int i = 0; i < function.getParamCount(); ++i) {
        size += function[i].type->getObjectSize();

        if (constructingMatrix && function[i].type->isMatrix())
            matrixInMatrix = true;
        if (full)
            overFull = true;
        if (op != EOpConstructStruct && !type->isArray() && size >= type->getObjectSize())
            full = true;
        if (!(function[i].type->getQualifier() == EvqConst || function[i].type->getQualifier() == EvqConstNoValue))
        {
            constType = false;
        }
        if (function[i].type->getQualifier() == EvqConstNoValue) {
            constNoValue = true;
        }
        if (function[i].type->isArray())
            arrayArg = true;
    }

    if (constType) {
        if (constNoValue) {
            type->changeQualifier(EvqConstNoValue);
        } else {
            type->changeQualifier(EvqConst);
        }
    }

    if (type->isArray() && type->getArraySize() != function.getParamCount()) {
        error(range, "array constructor needs one argument per array element", "constructor", "");
        return true;
    }

    if (arrayArg && op != EOpConstructStruct) {
        error(range, "constructing from a non-dereferenced array", "constructor", "");
        return true;
    }

    if (matrixInMatrix && !type->isArray()) {
        // If a matrix argument is given to a matrix constructor,
        // it is an error to have any other arguments.
        if (function.getParamCount() != 1) {
            error(range, "constructing matrix from matrix requires a single argument", "constructor", "");
            return true;
        }
    }

    if (overFull) {
        error(range, "too many arguments", "constructor", "");
        return true;
    }

    if (op == EOpConstructStruct && !type->isArray() && (int) type->getStruct()->size() != function.getParamCount()) {
        error(range, "Number of constructor parameters does not match the number of structure fields", "constructor", "");
        return true;
    }

    if ((op != EOpConstructStruct && size != 1 && size < type->getObjectSize()) ||
        (op == EOpConstructStruct && size < type->getObjectSize())) {
        error(range, "not enough data provided for construction", "constructor", "");
        return true;
    }

    TIntermTyped* typed = node->getAsTyped();
    if (typed == 0) {
        error(range, "constructor argument does not have a type", "constructor", "");
        return true;
    }
    if (op != EOpConstructStruct && IsSampler(typed->getBasicType())) {
        error(range, "cannot convert a sampler", "constructor", "");
        return true;
    }
    if (typed->getBasicType() == EbtVoid) {
        error(range, "cannot convert a void", "constructor", "");
        return true;
    }

    return false;
}

// This function checks to see if a void variable has been declared and raise an error message for such a case
//
// returns true in case of an error
//
bool TParseContext::voidErrorCheck(TSourceRange range, const TString& identifier, const TPublicType& pubType)
{
    if (pubType.type == EbtVoid) {
        error(range, "illegal use of type 'void'", identifier.c_str(), "");
        return true;
    }

    return false;
}

// This function checks to see if the node (for the expression) contains a scalar boolean expression or not
//
// returns true in case of an error
//
bool TParseContext::boolErrorCheck(TSourceRange range, const TIntermTyped* type)
{
    if (type->getBasicType() != EbtBool || type->isArray() || type->isMatrix() || type->isVector()) {
        error(range, "boolean expression expected", "", "");
        return true;
    }

    return false;
}

// This function checks to see if the node (for the expression) contains a scalar boolean expression or not
//
// returns true in case of an error
//
bool TParseContext::boolErrorCheck(TSourceRange range, const TPublicType& pType)
{
    if (pType.type != EbtBool || pType.array || pType.matrix || (pType.size > 1)) {
        error(range, "boolean expression expected", "", "");
        return true;
    }

    return false;
}

bool TParseContext::samplerErrorCheck(TSourceRange range, const TPublicType& pType, const char* reason)
{
    if (pType.type == EbtStruct) {
        if (containsSampler(*pType.userDef)) {
            error(range, reason, TType::getBasicString(pType.type), "(structure contains a sampler)");

            return true;
        }

        return false;
    } else if (IsSampler(pType.type)) {
        error(range, reason, TType::getBasicString(pType.type), "");

        return true;
    }

    return false;
}

bool TParseContext::structQualifierErrorCheck(TSourceRange range, const TPublicType& pType)
{
    if ((pType.qualifier == EvqVaryingIn || pType.qualifier == EvqVaryingOut || pType.qualifier == EvqAttribute) &&
        pType.type == EbtStruct) {
        error(range, "cannot be used with a structure", getQualifierString(pType.qualifier, language), "");

        return true;
    }

    if (pType.qualifier != EvqUniform && samplerErrorCheck(range, pType, "samplers must be uniform"))
        return true;

    return false;
}

bool TParseContext::parameterSamplerErrorCheck(TSourceRange range, TQualifier qualifier, const TType& type)
{
    if ((qualifier == EvqOut || qualifier == EvqInOut) &&
            type.getBasicType() != EbtStruct && IsSampler(type.getBasicType())) {
        error(range, "samplers cannot be output parameters", type.getBasicString(), "");
        return true;
    }

    return false;
}

bool TParseContext::containsSampler(TType& type)
{
    if (IsSampler(type.getBasicType()))
        return true;

    if (type.getBasicType() == EbtStruct) {
        TTypeList& structure = *type.getStruct();
        for (unsigned int i = 0; i < structure.size(); ++i) {
            if (containsSampler(*structure[i].type))
                return true;
        }
    }

    return false;
}

bool TParseContext::connectBuitInArrayWithBuiltInSymbol(const char* iArrayName,
                                                        const char* iSymbolAName,
                                                        const char* iSymbolBName)
{
    // find the target variable
    TString *name = NewPoolTString(iArrayName);
    TSymbol* symbol = symbolTable.find(*name);
    if (!symbol) {
        error(TSourceRangeInit, "INTERNAL ERROR finding symbol", name->c_str(), "");
        return true;
    }

    TVariable* variable = static_cast<TVariable*>(symbol);
    TVariable* newVariable = new TVariable(name, variable->getType());


    // fix missing resource limit
    bool builtIn = false;
    TSymbol* texCoord = symbolTable.find(iSymbolAName, &builtIn);
    if (texCoord == 0) {
        dbgPrint(DBGLVL_ERROR, "COULD NOT FIND %s\n", iSymbolAName);
        exit(1);
    }
    int texCoordValue = static_cast<TVariable*>(texCoord)->getConstPointer()[0].getIConst();
    variable->getType().setArraySize(texCoordValue);
    variable->getType().setMaxArraySize(texCoordValue);
    newVariable->getType().setArraySize(texCoordValue);
    newVariable->getType().setMaxArraySize(texCoordValue);

    if (iSymbolBName) {
        TSymbol* texCoord = symbolTable.find(iSymbolBName, &builtIn);
        if (texCoord == 0) {
            dbgPrint(DBGLVL_ERROR, "COULD NOT FIND %s\n", iSymbolBName);
            exit(1);
        }
        int texCoordValue = static_cast<TVariable*>(texCoord)->getConstPointer()[0].getIConst();
        variable->getType().setArraySize(texCoordValue, 1);
        variable->getType().setMaxArraySize(texCoordValue, 1);
        newVariable->getType().setArraySize(texCoordValue, 1);
        newVariable->getType().setMaxArraySize(texCoordValue, 1);
    }

#if 0
    // FIX level-2 variable issue?
    // TODO: Check if this is ok, i.e. again check original 3Dlabs code

    // reinsert new Variable, overwriting the old one
    if (! symbolTable.insert(*newVariable)) {
        delete newVariable;
        error(TSourceRangeInit, "INTERNAL ERROR inserting new symbol", name->c_str(), "");
        return true;
    }
#endif

    return false;

}

bool TParseContext::insertBuiltInArrayAtGlobalLevel()
{
    // builtin arrays
    switch (language) {
        case EShLangVertex:
        case EShLangFragment:
            if (connectBuitInArrayWithBuiltInSymbol("gl_TexCoord", "gl_MaxTextureCoords")) {
                return false;
            }
            break;
        case EShLangGeometry:
            if (connectBuitInArrayWithBuiltInSymbol("gl_FrontColorIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_BackColorIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_FrontSecondaryColorIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_BackSecondaryColorIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_TexCoordIn", "gl_VerticesIn", "gl_MaxTextureCoords")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_FogFragCoordIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_PositionIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_PointSizeIn", "gl_VerticesIn")) {
                return false;
            }
            if (connectBuitInArrayWithBuiltInSymbol("gl_ClipVertexIn", "gl_VerticesIn")) {
                return false;
            }
            break;
        default:
            break;
    }


    return false;
}

//
// Check that varying modifyers are only used with varying
//
//
//
bool TParseContext::varyingModifyerErrorCheck(TSourceRange range, TPublicType type, TQualifier qual)
{
    if (type.qualifier != EvqVaryingIn && type.qualifier != EvqVaryingOut) {
        if (qual != EvqInOut) {
            error(TSourceRangeInit, "varying in/out only allowed for varyings", "", "");
            return true;
        } else {
            return false;
        }
    }

    switch (language) {
        case EShLangVertex:
        case EShLangFragment:
            switch (qual) {
                case EvqInOut:
                    return false;
                case EvqOut:
                    if (extensionActiveCheck("GL_EXT_gpu_shader4")) {
                        return false;
                    }
                    else
                    {
                        error(range, "varying out only allowed for geometry shaders", "", "");
                        return true;
                    }
                case EvqIn:
                    error(range, "varying in only allowed for geometry shaders", "", "");
                    return true;
                default:
                    break;
            }
            break;
        case EShLangGeometry:
            switch (qual) {
                case EvqIn:
                case EvqConstIn:
                case EvqOut:
                case EvqInOut:
                    return false;
                default:
                    error(range, "only in/out only allowed for varying", "", "");
                    return true;
            }
            break;
        default:
            return true;
    }
    return false;
}


//
// Do size checking for an array type's size.
//
// Returns true if there was an error.
//
bool TParseContext::arraySizeErrorCheck(TSourceRange range, TIntermTyped* expr, int& size)
{
    TIntermConstantUnion* constant = expr->getAsConstantUnion();
    if (constant == 0 ||
        (!(constant->getBasicType() == EbtInt || constant->getBasicType() == EbtUInt))) {
        error(range, "array size must be a constant integer expression", "", "");
        return true;
    }

    size = constant->getUnionArrayPointer()->getIConst();

    if (size <= 0) {
        error(range, "array size must be a positive integer", "", "");
        size = 1;
        return true;
    }

    return false;
}

bool TParseContext::arraySizeGeometryVaryingInErrorCheck(TSourceRange range, int inputPrimSize, int size)
{
    if (size == inputPrimSize) {
        return false;
    } else {
        error(range, "array size does not match input primitive size", "", "");
        return true;
    }
}

//
// See if this qualifier can be an array.
//
// Returns true if there is an error.
//
bool TParseContext::arrayQualifierErrorCheck(TSourceRange range, TPublicType type)
{
    if (type.qualifier == EvqAttribute) {
        error(range, "cannot declare arrays of this qualifier", TType(type).getCompleteString().c_str(), "");
        return true;
    }

    return false;
}

//
// See if this type can be an array.
//
// Returns true if there is an error.
//
bool TParseContext::arrayTypeErrorCheck(TSourceRange range, TPublicType type)
{
    //
    // Can the type be an array?
    //
    if (type.array) {
        error(range, "cannot declare arrays of arrays", TType(type).getCompleteString().c_str(), "");
        return true;
    }

    return false;
}

//
//
//
bool TParseContext::arraySizeUnspecifiedErrorCheck(TSourceRange range, TPublicType type)
{
    UNUSED_ARG(range)
    if (type.array && type.arraySize == 0) {
        return true;
    }
    return false;
}

//
// Do all the semantic checking for declaring an array, with and
// without a size, and make the right changes to the symbol table.
//
// size == 0 means no specified size.
//
// Returns true if there was an error.
//
bool TParseContext::arrayErrorCheck(TSourceRange range, TString& identifier, TPublicType type, TVariable*& variable)
{
    //
    // Don't check for reserved word use until after we know it's not in the symbol table,
    // because reserved arrays can be redeclared.
    //

    bool builtIn = false;
    bool sameScope = false;
    int i;
    TSymbol* symbol = symbolTable.find(identifier, &builtIn, &sameScope);
    if (symbol == 0 || !sameScope) {
        if (reservedErrorCheck(range, identifier))
            return true;

        variable = new TVariable(&identifier, TType(type));

        for (i=0; i<MAX_ARRAYS; i++) {
            variable->getType().setArraySize(type.arraySize[i], i);
        }

        if (! symbolTable.insert(*variable)) {
            delete variable;
            error(range, "INTERNAL ERROR inserting new symbol", identifier.c_str(), "");
            return true;
        }
    } else {
        if (! symbol->isVariable()) {
            error(range, "variable expected", identifier.c_str(), "");
            return true;
        }

        variable = static_cast<TVariable*>(symbol);
        if (! variable->getType().isArray()) {
            error(range, "redeclaring non-array as array", identifier.c_str(), "");
            return true;
        }
        if (variable->getType().getArraySize() > 0) {
            error(range, "redeclaration of array with size", identifier.c_str(), "");
            return true;
        }

        if (! variable->getType().sameElementType(TType(type))) {
            error(range, "redeclaration of array with a different type", identifier.c_str(), "");
            return true;
        }

        TType* t = variable->getArrayInformationType();
        while (t != 0) {
            for (i=0; i<MAX_ARRAYS; i++) {
                if (t->getMaxArraySize(i) > type.arraySize[i]) {
                    error(range, "higher index value already used for the array", identifier.c_str(), "");
                    return true;
                }
                t->setArraySize(type.arraySize[i], i);
            }
            t = t->getArrayInformationType();
        }

        for (i=0; i<MAX_ARRAYS; i++) {
            variable->getType().setArraySize(type.arraySize[i], i);
        }

    }

    if (voidErrorCheck(range, identifier, type))
        return true;

    return false;
}

bool TParseContext::arraySetMaxSize(TIntermSymbol *node, TType* type, int size, bool updateFlag, TSourceRange range)
{
    if (!node) {
        error(range, " resize of more-dimensional array", "", "");
        return true;
    }

    bool builtIn = false;
    TSymbol* symbol = symbolTable.find(node->getSymbol(), &builtIn);
    if (symbol == 0) {
        error(range, " undeclared identifier", node->getSymbol().c_str(), "");
        return true;
    }
    TVariable* variable = static_cast<TVariable*>(symbol);

    type->setArrayInformationType(variable->getArrayInformationType());
    variable->updateArrayInformationType(type);

    // special casing to test index value of gl_TexCoord. If the accessed index is >= gl_MaxTextureCoords
    // its an error
    if (node->getSymbol() == "gl_TexCoord") {
        TSymbol* texCoord = symbolTable.find("gl_MaxTextureCoords", &builtIn);
        if (texCoord == 0) {
            infoSink.info.message(EPrefixInternalError, "gl_MaxTextureCoords not defined", range);
            return true;
        }

        int texCoordValue = static_cast<TVariable*>(texCoord)->getConstPointer()[0].getIConst();
        if (texCoordValue <= size) {
            error(range, "", "[", "gl_TexCoord can only have a max array size of up to gl_MaxTextureCoords", "");
            return true;
        }
    }

    // we dont want to update the maxArraySize when this flag is not set, we just want to include this
    // node type in the chain of node types so that its updated when a higher maxArraySize comes in.
    if (!updateFlag)
        return false;

    size++;
    variable->getType().setMaxArraySize(size);
    type->setMaxArraySize(size);
    TType* tt = type;

    while(tt->getArrayInformationType() != 0) {
        tt = tt->getArrayInformationType();
        tt->setMaxArraySize(size);
    }

    return false;
}

bool TParseContext::arrayFullDefinedGeometryVaryingInErrorCheck(TSourceRange range, TString& identifier, TPublicType type)
{
    int i;
    bool builtIn = false;
    symbolTable.find(identifier, &builtIn);

    if (!builtIn) {
        for (i=0; i<MAX_ARRAYS; i++) {
            if (type.arraySize[i] == 0) {
                error(range, " undefined array suffix for varying in", identifier.c_str(), "");
                return true;
            }
        }
    }
    return false;
}

bool TParseContext::nonArrayGeometryVaryingInErrorCheck(TSourceRange range, TPublicType type, TString& identifier)
{
    if (!type.matrix) {
        error(range, "varyings in geometry shader must be declared array", identifier.c_str(), "");
        return true;
    } else {
        return false;
    }
}

//
// Enforce non-initializer type/qualifier rules.
//
// Returns true if there was an error.
//
bool TParseContext::nonInitConstErrorCheck(TSourceRange range, TString& identifier, TPublicType& type)
{
    //
    // Make the qualifier make sense.
    //
    if (type.qualifier == EvqConst || type.qualifier == EvqConstNoValue) {
        type.qualifier = EvqTemporary;
        error(range, "variables with qualifier 'const' must be initialized", identifier.c_str(), "");
        return true;
    }

    return false;
}

//
// Do semantic checking for a variable declaration that has no initializer,
// and update the symbol table.
//
// Returns true if there was an error.
//
bool TParseContext::nonInitErrorCheck(TSourceRange range, TString& identifier, TPublicType& type)
{
    if (reservedErrorCheck(range, identifier))
        recover(__FILE__, __LINE__);

    TVariable* variable = new TVariable(&identifier, TType(type));

    if (! symbolTable.insert(*variable)) {
        error(range, "redefinition", variable->getName().c_str(), "");
        delete variable;
        return true;
    }

    if (voidErrorCheck(range, identifier, type))
        return true;

    return false;
}

bool TParseContext::paramErrorCheck(TSourceRange range, TQualifier qualifier, TQualifier paramQualifier, TType* type)
{
    if (qualifier != EvqConst && qualifier != EvqConstNoValue && qualifier != EvqTemporary) {
        error(range, "qualifier not allowed on function parameter", getQualifierString(qualifier, language), "");
        return true;
    }
    if ((qualifier == EvqConst || qualifier == EvqConstNoValue) && paramQualifier != EvqIn) {
        error(range, "qualifier not allowed with ", getQualifierString(qualifier, language), getQualifierString(paramQualifier, language));
        return true;
    }

    if (qualifier == EvqConst || qualifier == EvqConstNoValue) {
        if (paramQualifier == EvqIn) {
            type->changeQualifier(EvqConstIn); /* check back with Magnus */
        } else {
            type->changeQualifier(EvqConstReadOnly);
        }
    } else {
        type->changeQualifier(paramQualifier);
    }

    return false;
}

bool TParseContext::extensionErrorCheck(TSourceRange range, const char* extension)
{
    if (extensionBehavior[extension] == EBhWarn) {
        infoSink.info.message(EPrefixWarning, ("extension " + TString(extension) + " is being used").c_str(), range);
        return false;
    }
    if (extensionBehavior[extension] == EBhDisable) {
        error(range, "extension", extension, "is disabled");
        return true;
    }

    return false;
}

bool TParseContext::extensionActiveCheck(const char* extension)
{
    if (extensionBehavior[extension] == EBhEnable ||
        extensionBehavior[extension] == EBhRequire) {
        return true;
    } else {
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////
//
// Non-Errors.
//
/////////////////////////////////////////////////////////////////////////////////

//
// Look up a function name in the symbol table, and make sure it is a function.
//
// Return the function symbol if found, otherwise 0.
//
const TFunction* TParseContext::findFunction(TSourceRange range, TFunction* call, bool *builtIn)
{
    TSymbol* symbol = symbolTable.find(call->getMangledName(), builtIn);
    TSymbol* newSymbol;
    bool     newBuiltIn;

    if (symbol == 0) {
        /* no direct match, now search for matches with implicit cast */
        TString origName = call->getMangledName();
        std::list<TString::size_type> intParamPositions;
        TString::size_type            pos = origName.find('(');
        /*
        dbgPrint(DBGLVL_COMPILERINFO, "---> %s\n", origName.c_str());
        */
        /* search for all possible implicit casts */
        while ((pos = origName.find('i', pos)) != TString::npos) {
            /*
            dbgPrint(DBGLVL_COMPILERINFO, "%i\n", pos);
            */
            intParamPositions.push_back(pos);
            pos++;
        }

        if (intParamPositions.size() == 0) {
            error(range, "no matching overloaded function found", call->getName().c_str(), "");
            return 0;
        }

        /* try all combinations */
        for(int i = 1; i < 1<<(intParamPositions.size()); i++) {
            TString newMangledName = origName;
            std::list<TString::size_type>::iterator iPos = intParamPositions.begin();
            for (int j = 0; j < (int) intParamPositions.size(); j++) {
                if ((1<<j)&i) {
                    newMangledName[*iPos] = 'f';
                }
                iPos++;
            }

            /*
            dbgPrint(DBGLVL_COMPILERINFO, "newName(%i) %s\n", i, newMangledName.c_str());
            */

            if ((newSymbol = symbolTable.find(newMangledName, &newBuiltIn))) {
                if (symbol) {
                    error(range, "unresolvable ambiguity found for function call", call->getName().c_str(), "");
                    return 0;
                } else {
                    /*
                    dbgPrint(DBGLVL_COMPILERINFO, "\t match\n");
                    */
                    symbol = newSymbol;
                    *builtIn = newBuiltIn;
                }
            }
        }
    }

    if (!symbol) {
        error(range, "no matching overloaded function found", call->getName().c_str(), "");
        return 0;
    }

    if (! symbol->isFunction()) {
        error(range, "function name expected", call->getName().c_str(), "");
        return 0;
    }

    const TFunction* function = static_cast<const TFunction*>(symbol);

    return function;
}

//
// Initializers show up in several places in the grammar.  Have one set of
// code to handle them here.
//
bool TParseContext::executeInitializer(TSourceRange range, TString& identifier, TPublicType& pType,
                                    TIntermTyped* initializer, TIntermNode*& intermNode,
                                    TVariable* variable)
{
    TType type = TType(pType);

    if (variable == 0) {
        if (reservedErrorCheck(range, identifier))
            return true;

        if (voidErrorCheck(range, identifier, pType))
            return true;

        //
        // add variable to symbol table
        //
        variable = new TVariable(&identifier, type);
        if (! symbolTable.insert(*variable)) {
            error(range, "redefinition", variable->getName().c_str(), "");
            return true;
            // don't delete variable, it's used by error recovery, and the pool
            // pop will take care of the memory
        }
    }

    //
    // identifier must be of type uniform, constant, a global, or a temporary
    //
    TQualifier qualifier = variable->getType().getQualifier();
    if ((qualifier != EvqUniform) && (qualifier != EvqTemporary) && (qualifier != EvqGlobal) && (qualifier != EvqConst) && (qualifier != EvqConstNoValue)) {
        error(range, " cannot initialize this type of qualifier ", variable->getType().getQualifierString(language), "");
        return true;
    }
    //
    // test for and propagate constant
    //
    constUnion* unionArray = NULL;
    if (qualifier == EvqConst || qualifier == EvqConstNoValue) {
        if (type != initializer->getType()) {
            error(range, " non-matching types for const initializer ",
                variable->getType().getQualifierString(language), "");
            variable->getType().changeQualifier(EvqTemporary);
            return true;
        }
        if (qualifier != initializer->getType().getQualifier()) {
            /*
            error(range, " assigning non-constant to", "=", "'%s'",
                variable->getType().getCompleteString().c_str());
            variable->getType().changeQualifier(EvqTemporary);
            return true;
            */
            variable->getType().changeQualifier(EvqConstNoValue);
        } else if (initializer->getAsConstantUnion()) {
            unionArray = variable->getConstPointer();

            if (type.getObjectSize() == 1 && type.getBasicType() != EbtStruct) {
                *unionArray = (initializer->getAsConstantUnion()->getUnionArrayPointer())[0];
            } else {
                unionArray = initializer->getAsConstantUnion()->getUnionArrayPointer();
                variable->shareConstPointer(unionArray);
            }
        } else if (initializer->getAsSymbolNode()) {
            const TSymbol* symbol = symbolTable.find(initializer->getAsSymbolNode()->getSymbol());
            const TVariable* tVar = static_cast<const TVariable*>(symbol);

            unionArray = tVar->getConstPointer();
            variable->shareConstPointer(unionArray);
        } else {
            error(range, " cannot assign to", "=", "'%s'", variable->getType().getCompleteString().c_str());
            variable->getType().changeQualifier(EvqTemporary);
            return true;
        }
    }

    TIntermSymbol* intermSymbol = intermediate.addSymbol(variable->getUniqueId(), variable->getName(), variable->getType(), range, extensionChanged);
    intermNode = intermediate.addAssign(EOpAssign, intermSymbol, initializer, range, extensionChanged);
    if (intermNode == 0) {
        assignError(range, "=", intermSymbol->getCompleteString(), initializer->getCompleteString());
        return true;
    }

    return false;
}

bool TParseContext::areAllChildConst(TIntermAggregate* aggrNode)
{
    if (!aggrNode->isConstructor())
        return false;

    bool allConstant = true;

    // check if all the child nodes are constants so that they can be inserted into
    // the parent node
    if (aggrNode) {
        TIntermSequence &childSequenceVector = aggrNode->getSequence() ;
        for (TIntermSequence::iterator p = childSequenceVector.begin();
                                    p != childSequenceVector.end(); p++) {
            if (!(*p)->getAsTyped()->getAsConstantUnion())
                return false;
        }
    }

    return allConstant;
}

// This function is used to test for the correctness of the parameters passed to various constructor functions
// and also convert them to the right datatype if it is allowed and required.
//
// Returns 0 for an error or the constructed node (aggregate or typed) for no error.
//
TIntermTyped* TParseContext::addConstructor(TIntermNode* node, const TType* type, TOperator op, TFunction* fnCall, TSourceRange range)
{
    UNUSED_ARG(fnCall)

    if (node == 0)
        return 0;

    TIntermAggregate* aggrNode = node->getAsAggregate();

    TTypeList::iterator memberTypes;
    if (op == EOpConstructStruct)
        memberTypes = type->getStruct()->begin();

    TType elementType = *type;
    if (type->isArray())
        elementType.clearArrayness();

    bool singleArg;
    if (aggrNode) {
        if (aggrNode->getOp() != EOpNull || aggrNode->getSequence().size() == 1)
            singleArg = true;
        else
            singleArg = false;
    } else
        singleArg = true;

    TIntermTyped *newNode;
    if (singleArg) {
        // If structure constructor or array constructor is being called
        // for only one parameter inside the structure, we need to call constructStruct function once.
        if (type->isArray())
            newNode = constructStruct(node, &elementType, 1, node->getRange(), false);
        else if (op == EOpConstructStruct)
            newNode = constructStruct(node, (*memberTypes).type, 1, node->getRange(), false);
        else
            newNode = constructBuiltIn(type, op, node, node->getRange(), false);

        if (newNode && newNode->getAsAggregate()) {
            /* return newNode;
            * TODO: this change was inserted to fix a problem with vec2(1) being not properly
            *      restored. Check if this is desireable.
            * HINT: folded constructors are marked as todo by original code (see few lines down)
            */
            TIntermTyped* constConstructor = foldConstConstructor(newNode->getAsAggregate(), *type);
            if (constConstructor)
                return constConstructor;
        }

        return newNode;
    }

    //
    // Handle list of arguments.
    //
    TIntermSequence &sequenceVector = aggrNode->getSequence() ;
    // Stores the information about the parameter to the constructor
    // if the structure constructor contains more than one parameter, then construct
    // each parameter

    int paramCount = 0;  // keeps a track of the constructor parameter number being checked

    // for each parameter to the constructor call, check to see if the right type is passed or convert them
    // to the right type if possible (and allowed).
    // for structure constructors, just check if the right type is passed, no conversion is allowed.

    TIntermSequence sv2;
    for (TIntermSequence::iterator p = sequenceVector.begin();
                                p != sequenceVector.end(); p++, paramCount++) {
        if (type->isArray())
            newNode = constructStruct(*p, &elementType, paramCount+1, node->getRange(), true);
        else if (op == EOpConstructStruct)
            newNode = constructStruct(*p, (memberTypes[paramCount]).type, paramCount+1, node->getRange(), true);
        else
            newNode = constructBuiltIn(type, op, *p, node->getRange(), true);

        if (newNode) {
            //sequenceVector.erase(p);
            //sequenceVector.insert(p, newNode);
            sv2.push_back(newNode);
        }
    }
    sequenceVector.swap(sv2);

    TIntermTyped* constructor = intermediate.setAggregateOperator(aggrNode, op, range, extensionChanged);

    TIntermTyped* constConstructor = foldConstConstructor(constructor->getAsAggregate(), *type);
    if (constConstructor)
        return constConstructor;

    return constructor;
}

TIntermTyped* TParseContext::foldConstConstructor(TIntermAggregate* aggrNode, const TType& type)
{
    bool canBeFolded = areAllChildConst(aggrNode);
    aggrNode->setType(type);
    if (canBeFolded) {
        bool returnVal = false;
        constUnion* unionArray = new constUnion[type.getObjectSize()];
        if (aggrNode->getSequence().size() == 1)  {
            returnVal = intermediate.parseConstTree(aggrNode->getRange(), aggrNode, unionArray, aggrNode->getOp(), symbolTable,  type, true);
        }
        else {
            returnVal = intermediate.parseConstTree(aggrNode->getRange(), aggrNode, unionArray, aggrNode->getOp(), symbolTable,  type);
        }
        if (returnVal)
            return 0;

        return intermediate.addConstantUnion(unionArray, type, aggrNode->getRange(), extensionChanged);
    }

    return 0;
}

// Function for constructor implementation. Calls addUnaryMath with appropriate EOp value
// for the parameter to the constructor (passed to this function). Essentially, it converts
// the parameter types correctly. If a constructor expects an int (like ivec2) and is passed a
// float, then float is converted to int.
//
// Returns 0 for an error or the constructed node.
//
TIntermTyped* TParseContext::constructBuiltIn(const TType* type, TOperator op, TIntermNode* node, TSourceRange range, bool subset)
{
    TIntermTyped* newNode;
    TOperator basicOp;

    //
    // First, convert types as needed.
    //
    switch (op) {
        case EOpConstructVec2:
        case EOpConstructVec3:
        case EOpConstructVec4:
        case EOpConstructMat2:
        case EOpConstructMat2x3:
        case EOpConstructMat2x4:
        case EOpConstructMat3x2:
        case EOpConstructMat3:
        case EOpConstructMat3x4:
        case EOpConstructMat4x2:
        case EOpConstructMat4x3:
        case EOpConstructMat4:
        case EOpConstructFloat:
            basicOp = EOpConstructFloat;
            break;
        case EOpConstructIVec2:
        case EOpConstructIVec3:
        case EOpConstructIVec4:
        case EOpConstructInt:
            basicOp = EOpConstructInt;
            break;
        case EOpConstructUVec2:
        case EOpConstructUVec3:
        case EOpConstructUVec4:
        case EOpConstructUInt:
            basicOp = EOpConstructUInt;
            break;
        case EOpConstructBVec2:
        case EOpConstructBVec3:
        case EOpConstructBVec4:
        case EOpConstructBool:
            basicOp = EOpConstructBool;
            break;

        default:
            error(range, "unsupported construction", "", "");
            recover(__FILE__, __LINE__);

            return 0;
    }

    newNode = intermediate.addUnaryMath(basicOp, node, node->getRange(), symbolTable, extensionChanged);
    if (newNode == 0) {
        error(range, "can't convert", "constructor", "");
        return 0;
    }

    //
    // Now, if there still isn't an operation to do the construction, and we need one, add one.
    //

    // Otherwise, skip out early.
    if (subset || newNode != node && newNode->getType() == *type)
        return newNode;

    // setAggregateOperator will insert a new node for the constructor, as needed.
    return intermediate.setAggregateOperator(newNode, op, range, extensionChanged);
}

// This function tests for the type of the parameters to the structures constructors. Raises
// an error message if the expected type does not match the parameter passed to the constructor.
//
// Returns 0 for an error or the input node itself if the expected and the given parameter types match.
//
TIntermTyped* TParseContext::constructStruct(TIntermNode* node, TType* type, int paramCount, TSourceRange range, bool subset)
{
    if (*type == node->getAsTyped()->getType()) {
        if (subset)
            return node->getAsTyped();
        else
            return intermediate.setAggregateOperator(node->getAsTyped(), EOpConstructStruct, range, extensionChanged);
    } else {
        error(range, "", "constructor", "cannot convert parameter %d from '%s' to '%s'", paramCount,
                node->getAsTyped()->getType().getBasicString(), type->getBasicString());
        recover(__FILE__, __LINE__);
    }

    return 0;
}

//
// This function returns the tree representation for the vector field(s) being accessed from contant vector.
// If only one component of vector is accessed (v.x or v[0] where v is a contant vector), then a contant node is
// returned, else an aggregate node is returned (for v.xy). The input to this function could either be the symbol
// node or it could be the intermediate tree representation of accessing fields in a constant structure or column of
// a constant matrix.
//
TIntermTyped* TParseContext::addConstVectorNode(TVectorFields& fields, TIntermTyped* node, TSourceRange range)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();

    constUnion *unionArray;
    if (tempConstantNode) {
        unionArray = tempConstantNode->getUnionArrayPointer();

        if (!unionArray) {  // this error message should never be raised
            infoSink.info.message(EPrefixInternalError, "constUnion not initialized in addConstVectorNode function", range);
            recover(__FILE__, __LINE__);

            return node;
        }
    } else { // The node has to be either a symbol node or an aggregate node or a tempConstant node, else, its an error
        error(range, "Cannot offset into the vector", "Error", "");
        recover(__FILE__, __LINE__);

        return 0;
    }

    constUnion* constArray = new constUnion[fields.num];

    for (int i = 0; i < fields.num; i++) {
        if (fields.offsets[i] >= node->getType().getObjectSize()) {
            error(range, "", "[", "vector field selection out of range '%d'", fields.offsets[i]);
            recover(__FILE__, __LINE__);
            fields.offsets[i] = 0;
        }

        constArray[i] = unionArray[fields.offsets[i]];

    }
    typedNode = intermediate.addConstantUnion(constArray, node->getType(), range, extensionChanged);
    return typedNode;
}

//
// This function returns the column being accessed from a constant matrix. The values are retrieved from
// the symbol table and parse-tree is built for a vector (each column of a matrix is a vector). The input
// to the function could either be a symbol node (m[0] where m is a constant matrix)that represents a
// constant matrix or it could be the tree representation of the constant matrix (s.m1[0] where s is a constant structure)
//
TIntermTyped* TParseContext::addConstMatrixNode(int index, TIntermTyped* node, TSourceRange range)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();

    if (index >= node->getType().getMatrixSize(0)) {
        error(range, "", "[", "matrix field selection out of range '%d'", index);
        recover(__FILE__, __LINE__);
        index = 0;
    }

    if (tempConstantNode) {
        constUnion* unionArray = tempConstantNode->getUnionArrayPointer();
        int size = tempConstantNode->getType().getMatrixSize(1);
        typedNode = intermediate.addConstantUnion(&unionArray[size*index], tempConstantNode->getType(), range, extensionChanged);
    } else {
        error(range, "Cannot offset into the matrix", "Error", "");
        recover(__FILE__, __LINE__);

        return 0;
    }

    return typedNode;
}


//
// This function returns an element of an array accessed from a constant array. The values are retrieved from
// the symbol table and parse-tree is built for the type of the element. The input
// to the function could either be a symbol node (a[0] where a is a constant array)that represents a
// constant array or it could be the tree representation of the constant array (s.a1[0] where s is a constant structure)
//
TIntermTyped* TParseContext::addConstArrayNode(int index, TIntermTyped* node, TSourceRange range)
{
    TIntermTyped* typedNode;
    TIntermConstantUnion* tempConstantNode = node->getAsConstantUnion();
    TType arrayElementType = node->getType();
    arrayElementType.clearArrayness();

    if (index >= node->getType().getArraySize()) {
        error(range, "", "[", "array field selection out of range '%d'", index);
        recover(__FILE__, __LINE__);
        index = 0;
    }

    int arrayElementSize = arrayElementType.getObjectSize();

    if (tempConstantNode) {
        constUnion* unionArray = tempConstantNode->getUnionArrayPointer();
        typedNode = intermediate.addConstantUnion(&unionArray[arrayElementSize * index], tempConstantNode->getType(), range, extensionChanged);
    } else {
        error(range, "Cannot offset into the array", "Error", "");
        recover(__FILE__, __LINE__);

        return 0;
    }

    return typedNode;
}


//
// This function returns the value of a particular field inside a constant structure from the symbol table.
// If there is an embedded/nested struct, it appropriately calls addConstStructNested or addConstStructFromAggr
// function and returns the parse-tree with the values of the embedded/nested struct.
//
TIntermTyped* TParseContext::addConstStruct(TString& identifier, TIntermTyped* node, TSourceRange range)
{
    TTypeList* fields = node->getType().getStruct();
    TIntermTyped *typedNode;
    int instanceSize = 0;
    unsigned int index = 0;
    TIntermConstantUnion *tempConstantNode = node->getAsConstantUnion();

    for ( index = 0; index < fields->size(); ++index) {
        if ((*fields)[index].type->getFieldName() == identifier) {
            break;
        } else {
            instanceSize += (*fields)[index].type->getObjectSize();
        }
    }

    if (tempConstantNode) {
        constUnion* constArray = tempConstantNode->getUnionArrayPointer();

        typedNode = intermediate.addConstantUnion(constArray+instanceSize, tempConstantNode->getType(), range, extensionChanged); // type will be changed in the calling function
    } else {
        error(range, "Cannot offset into the structure", "Error", "");
        recover(__FILE__, __LINE__);

        return 0;
    }

    return typedNode;
}

//
// Initialize all supported extensions to disable
//
void TParseContext::initializeExtensionBehavior()
{
    //
    // example code: extensionBehavior["test"] = EBhDisable; // where "test" is the name of
    // supported extension
    //
    extensionBehavior["GL_ARB_texture_rectangle"] = EBhDisable;
    extensionBehavior["GL_ARB_draw_buffers"] = EBhDisable;
    extensionBehavior["GL_EXT_bindable_uniform"] = EBhDisable;
    extensionBehavior["GL_EXT_geometry_shader4"] = EBhDisable;
    extensionBehavior["GL_EXT_gpu_shader4"] = EBhDisable;
    extensionBehavior["GL_EXT_texture_array"] = EBhDisable;
}

OS_TLSIndex GlobalParseContextIndex = OS_INVALID_TLS_INDEX;

bool InitializeParseContextIndex()
{
    if (GlobalParseContextIndex != OS_INVALID_TLS_INDEX) {
        assert(0 && "InitializeParseContextIndex(): Parse Context already initalised");
        return false;
    }

    //
    // Allocate a TLS index.
    //
    GlobalParseContextIndex = OS_AllocTLSIndex();

    if (GlobalParseContextIndex == OS_INVALID_TLS_INDEX) {
        assert(0 && "InitializeParseContextIndex(): Parse Context already initalised");
        return false;
    }

    return true;
}

bool InitializeGlobalParseContext()
{
    if (GlobalParseContextIndex == OS_INVALID_TLS_INDEX) {
        assert(0 && "InitializeGlobalParseContext(): Parse Context index not initalised");
        return false;
    }

    TThreadParseContext *lpParseContext = static_cast<TThreadParseContext *>(OS_GetTLSValue(GlobalParseContextIndex));
    if (lpParseContext != 0) {
        assert(0 && "InitializeParseContextIndex(): Parse Context already initalised");
        return false;
    }

    TThreadParseContext *lpThreadData = new TThreadParseContext();
    if (lpThreadData == 0) {
        assert(0 && "InitializeGlobalParseContext(): Unable to create thread parse context");
        return false;
    }

    lpThreadData->lpGlobalParseContext = 0;
    OS_SetTLSValue(GlobalParseContextIndex, lpThreadData);

    return true;
}

TParseContextPointer& GetGlobalParseContext()
{
    //
    // Minimal error checking for speed
    //

    TThreadParseContext *lpParseContext = static_cast<TThreadParseContext *>(OS_GetTLSValue(GlobalParseContextIndex));

    return lpParseContext->lpGlobalParseContext;
}

bool FreeParseContext()
{
    if (GlobalParseContextIndex == OS_INVALID_TLS_INDEX) {
        assert(0 && "FreeParseContext(): Parse Context index not initalised");
        return false;
    }

    TThreadParseContext *lpParseContext = static_cast<TThreadParseContext *>(OS_GetTLSValue(GlobalParseContextIndex));
    if (lpParseContext)
        delete lpParseContext;

    return true;
}

bool FreeParseContextIndex()
{
    OS_TLSIndex tlsiIndex = GlobalParseContextIndex;

    if (GlobalParseContextIndex == OS_INVALID_TLS_INDEX) {
        assert(0 && "FreeParseContextIndex(): Parse Context index not initalised");
        return false;
    }

    GlobalParseContextIndex = OS_INVALID_TLS_INDEX;

    return OS_FreeTLSIndex(tlsiIndex);
}
