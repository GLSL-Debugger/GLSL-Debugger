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

/**
 * This is bison grammar and production code for parsing the OpenGL 2.0 shading
 * languages.
 */
%{

/* Based on:
ANSI C Yacc grammar

In 1985, Jeff Lee published his Yacc grammar (which is accompanied by a 
matching Lex specification) for the April 30, 1985 draft version of the 
ANSI C standard.  Tom Stockfisch reposted it to net.sources in 1987; that
original, as mentioned in the answer to question 17.25 of the comp.lang.c
FAQ, can be ftp'ed from ftp.uu.net, file usenet/net.sources/ansi.c.grammar.Z.
 
I intend to keep this version as close to the current C Standard grammar as 
possible; please let me know if you discover discrepancies. 

Jutta Degener, 1995 
*/

#include "SymbolTable.h"
#include "ParseHelper.h"
#include "../Public/ShaderLang.h"

#define EMIT_VERTEX_SIG   "EmitVertex("

#ifdef _WIN32
    #define YYPARSE_PARAM parseContext
    #define YYPARSE_PARAM_DECL TParseContext&
    #define YY_DECL int yylex(YYSTYPE* pyylval, TParseContext& parseContext)
    #define YYLEX_PARAM parseContext
#else
    #define YYPARSE_PARAM parseContextLocal
    #define parseContext (*((TParseContext*)(parseContextLocal)))
    #define YY_DECL int yylex(YYSTYPE* pyylval, void* parseContextLocal)
    #define YYLEX_PARAM (void*)(parseContextLocal)
    extern void yyerror(char*);    
#endif

#define FRAG_VERT_GEOM_ONLY(S, L) {                                      \
    if (parseContext.language != EShLangFragment &&                      \
        parseContext.language != EShLangGeometry &&                      \
        parseContext.language != EShLangVertex) {                        \
        parseContext.error(L,                                            \
            " supported in vertex/geom/frag shaders only ", S, "", "");  \
        parseContext.recover(__FILE__, __LINE__);                        \
    }                                                                    \
}

#define FRAG_VERT_ONLY(S, L) {                                           \
    if (parseContext.language != EShLangFragment &&                      \
        parseContext.language != EShLangVertex) {                        \
        parseContext.error(L,                                            \
            " supported in vertex/fragment shaders only ", S, "", "");   \
        parseContext.recover(__FILE__, __LINE__);                        \
    }                                                                    \
}

#define VERT_GEOM_ONLY(S, L) {                                           \
    if (parseContext.language != EShLangGeometry &&                      \
        parseContext.language != EShLangVertex) {                        \
        parseContext.error(L,                                            \
            " supported in vertex/geometry shaders only ", S, "", "");   \
        parseContext.recover(__FILE__, __LINE__);                        \
    }                                                                    \
}

#define FRAG_GEOM_ONLY(S, L) {                                           \
    if (parseContext.language != EShLangFragment &&                      \
        parseContext.language != EShLangGeometry) {                      \
        parseContext.error(L,                                            \
            " supported in geometry/fragment shaders only ", S, "", ""); \
        parseContext.recover(__FILE__, __LINE__);                        \
    }                                                                    \
}

#define VERTEX_ONLY(S, L) {                                                     \
    if (parseContext.language != EShLangVertex) {                               \
        parseContext.error(L, " supported in vertex shaders only ", S, "", "");            \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}

#define FRAG_ONLY(S, L) {                                                       \
    if (parseContext.language != EShLangFragment) {                             \
        parseContext.error(L, " supported in fragment shaders only ", S, "", "");          \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}

#define GEOM_ONLY(S, L) {                                                       \
    if (parseContext.language != EShLangGeometry) {                             \
        parseContext.error(L, " supported in geometry shaders only ", S, "", "");          \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}

#define PACK_ONLY(S, L) {                                                       \
    if (parseContext.language != EShLangPack) {                                 \
        parseContext.error(L, " supported in pack shaders only ", S, "", "");              \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}

#define UNPACK_ONLY(S, L) {                                                     \
    if (parseContext.language != EShLangUnpack) {                               \
        parseContext.error(L, " supported in unpack shaders only ", S, "", "");            \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}

#define PACK_UNPACK_ONLY(S, L) {                                                \
    if (parseContext.language != EShLangUnpack &&                               \
        parseContext.language != EShLangPack) {                                 \
        parseContext.error(L, " supported in pack/unpack shaders only ", S, "", "");       \
        parseContext.recover(__FILE__, __LINE__);                                                            \
    }                                                                           \
}
%}
%union {
    struct {
        TSourceRange range;
        union {
            TString *string;
            float f;
            int i;
			unsigned int ui;
            bool b;
        };
        TSymbol* symbol;
    } lex;
    struct {
        TSourceRange range;
        TOperator op;
        union {
            TIntermNode* intermNode;
            TIntermNodePair nodePair;
            TIntermTyped* intermTypedNode;
            TIntermAggregate* intermAggregate;
        };
        union {
            TPublicType type;
            TQualifier qualifier;
            TVaryingModifier varyingModifier;
            TFunction* function;
            TParameter param;
            TTypeRange typeRange;
            TTypeList* typeList;
        };
    } interm;
}

%{
#ifndef _WIN32
    extern int yylex(YYSTYPE*, void*);
#endif

void addStructInstance(TIntermSpecification* specificationNode, 
                       TIntermNode* intermNode, 
                       TParseContext &pC)
{
    if (!specificationNode || !intermNode) return;
    
    TIntermAggregate* instancePointer = *(specificationNode->getInstancesPointer());

    if (instancePointer && instancePointer->getOp() != EOpNull) {
        instancePointer->setOperator(EOpNull);
    }

    specificationNode->setInstances(
        pC.intermediate.growAggregate(instancePointer, intermNode, pC.extensionChanged)
    );

    instancePointer = *(specificationNode->getInstancesPointer());
    
    if (instancePointer && instancePointer->getOp() != EOpInstances) {
        instancePointer->setOperator(EOpInstances);
    }
}

void processStruct(TTypeList *paramList, TIntermAggregate** p, TParseContext &pC) {
    
    TTypeList::iterator iter = paramList->begin();
    for(; iter != paramList->end(); iter++) {
        if(iter->type->isSpecified() == true &&
           iter->type->getBasicType() == EbtStruct) {

            /* add specification */
            TIntermNode *structNode = 
                pC.intermediate.addSpecification(iter->range, iter->type, pC.extensionChanged);

            /* recursive process this struct */
            processStruct(iter->type->getStruct(), 
                          structNode->getAsSpecificationNode()->getParameterPointer(), 
                          pC);
            
            /* Add instance */
            TIntermNode *intermNode = 
                pC.intermediate.addParameter(iter->range, iter->type, pC.extensionChanged);
                
            addStructInstance(structNode->getAsSpecificationNode(),
                              intermNode, pC);
            
            /* Make aggregate and add to parameters */
            TIntermAggregate *structAggregate = 
                pC.intermediate.makeAggregate(structNode, pC.extensionChanged);
            structAggregate->setOperator(EOpSpecification);

            *p = pC.intermediate.growAggregate(*p, structAggregate, pC.extensionChanged);

        } else {
            TIntermNode *paramNode = 
                pC.intermediate.addParameter(iter->range, iter->type, pC.extensionChanged);
            *p = pC.intermediate.growAggregate(*p, paramNode, pC.extensionChanged);

        }
    }

    (*p)->setOperator(EOpParameter); 
}

%}

%pure_parser /* Just in case is called from multiple threads */
%expect 1 /* One shift reduce conflict because of if | else */
%token <lex> ATTRIBUTE CONST_QUAL BOOL_TYPE FLOAT_TYPE INT_TYPE INVARIANT
%token <lex> BREAK CONTINUE DO ELSE FOR IF DISCARD RETURN SWITCH CASE DEFAULT
%token <lex> BVEC2 BVEC3 BVEC4 IVEC2 IVEC3 IVEC4 VEC2 VEC3 VEC4
%token <lex> MATRIX2 MATRIX3 MATRIX4 
%token <lex> MATRIX2X2 MATRIX3X2 MATRIX4X2
%token <lex> MATRIX2X3 MATRIX3X3 MATRIX4X3
%token <lex> MATRIX2X4 MATRIX3X4 MATRIX4X4
%token <lex> IN_QUAL OUT_QUAL INOUT_QUAL UNIFORM VARYING
%token <lex> STRUCT VOID_TYPE WHILE
%token <lex> SAMPLER1D SAMPLER2D SAMPLER3D SAMPLERCUBE SAMPLER1DSHADOW SAMPLER2DSHADOW
%token <lex> SAMPLER2DRECTARB SAMPLER2DRECTSHADOWARB // ARB_texture_rectangle
%token <lex> IDENTIFIER TYPE_NAME FLOATCONSTANT INTCONSTANT BOOLCONSTANT
%token <lex> FIELD_SELECTION
%token <lex> LEFT_OP RIGHT_OP
%token <lex> INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token <lex> AND_OP OR_OP XOR_OP MUL_ASSIGN DIV_ASSIGN ADD_ASSIGN
%token <lex> MOD_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token <lex> SUB_ASSIGN
%token <lex> LEFT_PAREN RIGHT_PAREN LEFT_BRACKET RIGHT_BRACKET LEFT_BRACE RIGHT_BRACE DOT
%token <lex> COMMA COLON EQUAL SEMICOLON BANG DASH TILDE PLUS STAR SLASH PERCENT
%token <lex> LEFT_ANGLE RIGHT_ANGLE VERTICAL_BAR CARET AMPERSAND QUESTION
/* EXT_gpu_shader4 */
%token <lex> UINTCONSTANT
%token <lex> UNSIGNED
%token <lex> NOPERSPECTIVE FLAT CENTROID
%token <lex> UVEC2 UVEC3 UVEC4

%token <lex> SAMPLER1DARRAY SAMPLER2DARRAY
%token <lex> SAMPLER1DARRAYSHADOW SAMPLER2DARRAYSHADOW SAMPLERCUBESHADOW
%token <lex> ISAMPLER1D ISAMPLER2D ISAMPLER3D ISAMPLERCUBE
%token <lex> ISAMPLER2DRECT ISAMPLER1DARRAY ISAMPLER2DARRAY
%token <lex> USAMPLER1D USAMPLER2D USAMPLER3D USAMPLERCUBE
%token <lex> USAMPLER2DRECT USAMPLER1DARRAY USAMPLER2DARRAY
%token <lex> SAMPLERBUFFER ISAMPLERBUFFER USAMPLERBUFFER


%type <interm> assignment_operator unary_operator
%type <interm.intermTypedNode> variable_identifier primary_expression postfix_expression
%type <interm.intermTypedNode> expression integer_expression assignment_expression
%type <interm.intermTypedNode> unary_expression multiplicative_expression additive_expression
%type <interm.intermTypedNode> relational_expression equality_expression 
%type <interm.intermTypedNode> conditional_expression constant_expression
%type <interm.intermTypedNode> logical_or_expression logical_xor_expression logical_and_expression
%type <interm.intermTypedNode> shift_expression and_expression exclusive_or_expression inclusive_or_expression
%type <interm.intermTypedNode> function_call initializer condition conditionopt

%type <interm.intermNode> translation_unit function_definition
%type <interm.intermNode> statement simple_statement
%type <interm.intermAggregate>  statement_list compound_statement switch_statement_list
%type <interm.intermNode> declaration_statement selection_statement expression_statement
%type <interm.intermNode> declaration external_declaration
%type <interm.intermNode> for_init_statement compound_statement_no_new_scope
%type <interm.nodePair> selection_rest_statement for_rest_statement
%type <interm.intermNode> iteration_statement jump_statement statement_no_new_scope
%type <interm.intermNode> switch_statement case_label
%type <interm> single_declaration init_declarator_list 
%type <interm.type> array_declarator_suffix

%type <interm> parameter_declaration parameter_declarator parameter_type_specifier
%type <interm.qualifier> parameter_qualifier varying_geom_modifyer
%type <interm.varyingModifier> varying_modifier

%type <interm.type> type_qualifier fully_specified_type type_specifier 
%type <interm.type> type_specifier_nonarray
%type <interm.type> struct_specifier 
%type <interm.typeRange> struct_declarator 
%type <interm.typeList> struct_declarator_list struct_declaration struct_declaration_list
%type <interm.function> function_header function_declarator function_identifier
%type <interm.function> function_header_with_parameters function_call_header 
%type <interm> function_call_header_with_parameters function_call_header_no_parameters function_call_generic function_prototype
%type <interm> function_call_or_method

%start translation_unit 
%%

variable_identifier 
    : IDENTIFIER {
        
        // The symbol table search was done in the lexical phase
        const TSymbol* symbol = $1.symbol;
        const TVariable* variable;
        if (symbol == 0) {
            parseContext.error($1.range, "undeclared identifier", $1.string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
            TType type(EbtFloat);
            TVariable* fakeVariable = new TVariable($1.string, type);
            parseContext.symbolTable.insert(*fakeVariable);
            variable = fakeVariable;
        } else {
            // This identifier can only be a variable type symbol 
            if (! symbol->isVariable()) {
                parseContext.error($1.range, "variable expected", $1.string->c_str(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            variable = static_cast<const TVariable*>(symbol);
        }

        // don't delete $1.string, it's used by error recovery, and the pool
        // pop will reclaim the memory

        if (variable->getType().getQualifier() == EvqConst ) {
            constUnion* constArray = variable->getConstPointer();
            TType t(variable->getType());
            $$ = parseContext.intermediate.addConstantUnion(constArray, t, $1.range, parseContext.extensionChanged);
        } else {
            $$ = parseContext.intermediate.addSymbol(variable->getUniqueId(), 
                                                     variable->getName(), 
                                                     variable->getType(), 
                                                     $1.range,
                                                     parseContext.extensionChanged);
        }
        if ($$) $$->setRange($1.range);
    }
    ;

primary_expression
    : variable_identifier {
        $$ = $1;
    }
    | INTCONSTANT {
        //
        // INT_TYPE is only 16-bit plus sign bit for vertex/fragment shaders, 
        // check for overflow for constants
        //
		fprintf(stderr, "I:%d\n", $1.i);
        if (abs($1.i) >= (1 << 16)) {
            if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                parseContext.error($1.range, " integer constant overflow", "", "");
                parseContext.recover(__FILE__, __LINE__);
            } /* else {
                if (abs($1.i) >= (1 << 32)) {
                    parseContext.error($1.range, " integer constant overflow", "", "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } */
        }
        constUnion *unionArray = new constUnion[1];
        unionArray->setIConst($1.i);
        $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange($1.range);
    }
    | UINTCONSTANT {
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }

        /*
        if (abs($1.i) >= (1 << 32)) {
            parseContext.error($1.range, " integer constant overflow", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        */

        constUnion *unionArray = new constUnion[1];
		fprintf(stderr, "UI:%u\n", $1.ui);
        unionArray->setUIConst($1.ui);
        $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtUInt, EvqConst), $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange($1.range);

    }
    | FLOATCONSTANT {
        constUnion *unionArray = new constUnion[1];
        unionArray->setFConst($1.f);
        $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange($1.range);
    }
    | BOOLCONSTANT {
        constUnion *unionArray = new constUnion[1];
        unionArray->setBConst($1.b);
        $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange($1.range);
    }
    | LEFT_PAREN expression RIGHT_PAREN {
        $$ = $2;
        if ($$) $$->setRange(addRange($1.range, $3.range));
    }
    ;

postfix_expression
    : primary_expression { 
        $$ = $1;
    } 
    | postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET {
        if (!$1->isArray() && !$1->isMatrix() && !$1->isVector()) {
            if ($1->getAsSymbolNode())
                parseContext.error($2.range, " left of '[' is not of type array, matrix, or vector ", $1->getAsSymbolNode()->getSymbol().c_str(), "");
            else
                parseContext.error($2.range, " left of '[' is not of type array, matrix, or vector ", "expression", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ($1->getType().getQualifier() == EvqConst && $3->getQualifier() == EvqConst) {
            if ($1->isArray()) { // constant folding for arrays
                $$ = parseContext.addConstArrayNode($3->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), $1, $2.range);
            } else if ($1->isVector()) {  // constant folding for vectors
                TVectorFields fields;                
                fields.num = 1;
                fields.offsets[0] = $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst(); // need to do it this way because v.xy sends fields integer array
                $$ = parseContext.addConstVectorNode(fields, $1, $2.range);
            } else if ($1->isMatrix()) { // constant folding for matrices
                $$ = parseContext.addConstMatrixNode($3->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), $1, $2.range);
            } 
        } else {
            if ($3->getQualifier() == EvqConst) {
                if ($1->isVector() && $1->getType().getNominalSize() <= $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst() && !$1->isArray() ) {
                    parseContext.error($2.range, "", "[", "field selection out of range '%d'", $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                    parseContext.recover(__FILE__, __LINE__);
                } else if ($1->isMatrix() && $1->getType().getMatrixSize(1) <= $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst() && !$1->isArray() ) {
                    parseContext.error($2.range, "", "[", "matrix field selection out of range '%d'", $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                    parseContext.recover(__FILE__, __LINE__);
                } else {
                    if ($1->isArray()) {
                        if ($1->getType().getArraySize() == 0) {
                            if ($1->getType().getMaxArraySize() <= $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst()) {
                                if (parseContext.arraySetMaxSize($1->getAsSymbolNode(), $1->getTypePointer(), $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), true, $2.range))
                                    parseContext.recover(__FILE__, __LINE__); 
                            } else {
                                if (parseContext.arraySetMaxSize($1->getAsSymbolNode(), $1->getTypePointer(), 0, false, $2.range))
                                    parseContext.recover(__FILE__, __LINE__); 
                            }
                        } else if ( $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst() >= $1->getType().getArraySize()) {
                            parseContext.error($2.range, "", "[", "array index out of range '%d'", $3->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                            parseContext.recover(__FILE__, __LINE__);
                        }
                    }
                    /* TODO: changed this to indirect!!! Check again if valid */
                    $$ = parseContext.intermediate.addIndex(EOpIndexIndirect, $1, $3, $2.range, parseContext.extensionChanged);
                }
            } else {
                if ($1->isArray() && $1->getType().getArraySize() == 0) {
                    parseContext.error($2.range, "", "[", "array must be redeclared with a size before being indexed with a variable");
                    parseContext.recover(__FILE__, __LINE__);
                }
                
                $$ = parseContext.intermediate.addIndex(EOpIndexIndirect, $1, $3, $2.range, parseContext.extensionChanged);
            }
        } 
        if ($$ == 0) {
            constUnion *unionArray = new constUnion[1];
            unionArray->setFConst(0.0f);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), $2.range, parseContext.extensionChanged);
        } else if ($1->isArray()) {
            if ($1->getType().getStruct()) {
                $$->setType(TType($1->getType().getStruct(), $1->getType().getTypeName()));
            } else {
                if ($1->isMatrix()) {
                    $$ ->setType(TType($1->getBasicType(), EvqTemporary, EvmNone, 1,
                                       $1->getMatrixSize(0), $1->getMatrixSize(1), $1->isMatrix()));
                } else {
                    $$ ->setType(TType($1->getBasicType(), EvqTemporary, EvmNone, $1->getNominalSize(),
                                       1,1, $1->isMatrix()));
               }
            }
            if ($1->getType().getQualifier() == EvqConst) {
                $$->getTypePointer()->changeQualifier(EvqConst);
            }
            // add possible remaining arrays
            int i;
            for (i=1; i<$1->getType().getNumArrays(); i++) {
                $$->getTypePointer()->setArraySize($1->getType().getArraySize(i), i-1);
            }
        } else if ($1->isMatrix() && $1->getType().getQualifier() == EvqConst) {
            $$->setType(TType($1->getBasicType(), EvqConst, EvmNone, $1->getMatrixSize(1)));
        } else if ($1->isMatrix()) {
            $$->setType(TType($1->getBasicType(), EvqTemporary, EvmNone, $1->getMatrixSize(1)));
        } else if ($1->isVector() && $1->getType().getQualifier() == EvqConst) {
            $$->setType(TType($1->getBasicType(), EvqConst));
        } else if ($1->isVector()) {
            $$->setType(TType($1->getBasicType(), EvqTemporary));
        } else {
            $$->setType($1->getType());
        }
        if ($$ && $1) $$->setRange(addRange($1->getRange(), $4.range));
    }
    | function_call {
        $$ = $1;
    }
    | postfix_expression DOT FIELD_SELECTION {        
        if ($1->isArray()) {
            parseContext.error($3.range, "cannot apply dot operator to an array", ".", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        if ($1->isVector()) {
            TVectorFields fields;
            if (! parseContext.parseVectorFields(*$3.string, $1->getNominalSize(), fields, $3.range)) {
                fields.num = 1;
                fields.offsets[0] = 0;
                parseContext.recover(__FILE__, __LINE__);
            }

            if ($1->getType().getQualifier() == EvqConst) { // constant folding for vector fields
                $$ = parseContext.addConstVectorNode(fields, $1, $3.range);
                if ($$ == 0) {
                    parseContext.recover(__FILE__, __LINE__);
                    $$ = $1;
                }
                else
                    $$->setType(TType($1->getBasicType(), EvqConst, EvmNone, (int) (*$3.string).size()));
            } else {
                if (fields.num == 1) {
                    constUnion *unionArray = new constUnion[1];
                    /* TODO: Changed to setSConst from setIConst! Check if valid */
                    unionArray->setSConst(fields.offsets[0]);
                    TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $3.range, parseContext.extensionChanged);
                    $$ = parseContext.intermediate.addIndex(EOpIndexDirect, $1, index, $2.range, parseContext.extensionChanged);
                    $$->setType(TType($1->getBasicType()));
                } else {
                    TString vectorString = *$3.string;
                    TIntermTyped* index = parseContext.intermediate.addSwizzle(fields, $3.range, parseContext.extensionChanged);                
                    $$ = parseContext.intermediate.addIndex(EOpVectorSwizzle, $1, index, $2.range, parseContext.extensionChanged);
                    $$->setType(TType($1->getBasicType(),EvqTemporary, EvmNone, (int) vectorString.size()));  
                }
            }
        } else if ($1->isMatrix()) {
            TMatrixFields fields;
            if (! parseContext.parseMatrixFields(*$3.string, $1->getMatrixSize(0), $1->getMatrixSize(1), fields, $3.range)) {
                fields.wholeRow = false;
                fields.wholeCol = false;
                fields.row = 0;
                fields.col = 0;
                parseContext.recover(__FILE__, __LINE__);
            }

            if (fields.wholeRow || fields.wholeCol) {
                parseContext.error($2.range, " non-scalar fields not implemented yet", ".", "");
                parseContext.recover(__FILE__, __LINE__);
                constUnion *unionArray = new constUnion[1];
                /* TODO: Changed to setSConst from setIConst! Check if valid */
                unionArray->setSConst(0);
                TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $3.range, parseContext.extensionChanged);
                $$ = parseContext.intermediate.addIndex(EOpIndexDirect, $1, index, $2.range, parseContext.extensionChanged);
                if (fields.wholeRow) {
                    $$->setType(TType($1->getBasicType(), EvqTemporary, EvmNone, $1->getMatrixSize(0)));
                } else {
                    $$->setType(TType($1->getBasicType(), EvqTemporary, EvmNone, $1->getMatrixSize(1)));
                }
            } else {
                constUnion *unionArray = new constUnion[1];
                /* TODO: Changed to setSConst from setIConst! Check if valid */
                unionArray->setSConst(fields.col * $1->getMatrixSize(1) + fields.row);
                TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $3.range, parseContext.extensionChanged);
                $$ = parseContext.intermediate.addIndex(EOpIndexDirect, $1, index, $2.range, parseContext.extensionChanged);
                $$->setType(TType($1->getBasicType()));
            }
        } else if ($1->getBasicType() == EbtStruct) {
            bool fieldFound = false;
            TTypeList* fields = $1->getType().getStruct();
            if (fields == 0) {
                parseContext.error($2.range, "structure has no fields", "Internal Error", "");
                parseContext.recover(__FILE__, __LINE__);
                $$ = $1;
            } else {
                unsigned int i;
                for (i = 0; i < fields->size(); ++i) {
                    if ((*fields)[i].type->getFieldName() == *$3.string) {
                        fieldFound = true;
                        break;
                    }                
                }
                if (fieldFound) {
                    if ($1->getType().getQualifier() == EvqConst) {
                        $$ = parseContext.addConstStruct(*$3.string, $1, $2.range);
                        if ($$ == 0) {
                            parseContext.recover(__FILE__, __LINE__);
                            $$ = $1;
                        }
                        else {
                            $$->setType(*(*fields)[i].type);
                            // change the qualifier of the return type, not of the structure field
                            // as the structure definition is shared between various structures.
                            $$->getTypePointer()->changeQualifier(EvqConst);
                        }
                    } else {
                        constUnion *unionArray = new constUnion[1];
                        unionArray->setIConst(i);
                        TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $3.range, parseContext.extensionChanged);
                        $$ = parseContext.intermediate.addIndex(EOpIndexDirectStruct, $1, index, $2.range, parseContext.extensionChanged);
                        $$->setType(*(*fields)[i].type);
                    }
                } else {
                    parseContext.error($2.range, " no such field in structure", $3.string->c_str(), "");
                    parseContext.recover(__FILE__, __LINE__);
                    $$ = $1;
                }
            }
        } else {
            parseContext.error($2.range, " field selection requires structure, vector, or matrix on left hand side", $3.string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1) $$->setRange(addRange($1->getRange(), $3.range));
        // don't delete $3.string, it's from the pool
    }
    | postfix_expression INC_OP {
        if (parseContext.lValueErrorCheck($2.range, "++", $1))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addUnaryMath(EOpPostIncrement, $1, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.unaryOpError($2.range, "++", $1->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1) $$->setRange(addRange($1->getRange(), $2.range));
    }
    | postfix_expression DEC_OP {
        if (parseContext.lValueErrorCheck($2.range, "--", $1))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addUnaryMath(EOpPostDecrement, $1, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.unaryOpError($2.range, "--", $1->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1) $$->setRange(addRange($1->getRange(), $2.range));
    }
    ;

integer_expression 
    : expression {
        if (parseContext.integerErrorCheck($1, "[]"))
            parseContext.recover(__FILE__, __LINE__);
        $$ = $1;
    }
    ;

function_call 
    : function_call_or_method {
        TFunction* fnCall = $1.function;
        TOperator op = fnCall->getBuiltInOp();
        
        if (op == EOpArrayLength) {
            if ($1.intermNode->getAsTyped() == 0 || $1.intermNode->getAsTyped()->getType().getArraySize() == 0) {
                parseContext.error($1.range, "", fnCall->getName().c_str(), "array must be declared with a size before using this method");
                parseContext.recover(__FILE__, __LINE__);
            }

            constUnion *unionArray = new constUnion[1];
            unionArray->setIConst($1.intermNode->getAsTyped()->getType().getArraySize());
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), $1.range, parseContext.extensionChanged);
        } else if (op != EOpNull) {
            
            //
            // Then this should be a constructor.
            // Don't go through the symbol table for constructors.  
            // Their parameters will be verified algorithmically.
            //
            TType type(EbtVoid);  // use this to get the type back

            if (parseContext.constructorErrorCheck($1.range, $1.intermNode, *fnCall, op, &type)) {
                $$ = 0;
            } else {
                //
                // It's a constructor, of type 'type'.
                //
                $$ = parseContext.addConstructor($1.intermNode, &type, op, fnCall, $1.range);
            }
            if ($$ == 0) {
                parseContext.recover(__FILE__, __LINE__);
                $$ = parseContext.intermediate.setAggregateOperator(0, op, $1.range, parseContext.extensionChanged);
            }
            $$->setType(type);
            
        } else {
            //
            // Not a constructor.  Find it in the symbol table.
            //
            const TFunction* fnCandidate;
            bool builtIn;
            fnCandidate = parseContext.findFunction($1.range, fnCall, &builtIn);
            if (fnCandidate) {
                //
                // A declared function.  But, it might still map to a built-in
                // operation.
                //
                op = fnCandidate->getBuiltInOp();
                if (builtIn && op != EOpNull) {
                    //
                    // A function call mapped to a built-in operation.
                    //
                    if (fnCandidate->getParamCount() == 1) {
                        //
                        // Treat it like a built-in unary operator.
                        //
                        $$ = parseContext.intermediate.addUnaryMath(op, $1.intermNode, 
                            TSourceRangeInit, parseContext.symbolTable, parseContext.extensionChanged);
                        if ($$ == 0)  {
                            parseContext.error($1.intermNode->getRange(), " wrong operand type", "Internal Error", 
                                "built in unary operator function.  Type: %s",
                                static_cast<TIntermTyped*>($1.intermNode)->getCompleteString().c_str());
                            YYERROR;
                        }
                        $$->setType(fnCandidate->getReturnType());
                    } else {
                        TIntermTyped *constantInterm = parseContext.intermediate.foldAggregate($1.intermAggregate, op, fnCandidate->getReturnType(), $1.range, parseContext.extensionChanged);
                        if (!constantInterm) {
                            $$ = parseContext.intermediate.setAggregateOperator($1.intermAggregate, op, $1.range, parseContext.extensionChanged);
                            $$->setType(fnCandidate->getReturnType());
                        } else {
                            constantInterm->getType().changeQualifier(EvqConst);
                            $$ = constantInterm;
                            // bail out for not changeing the type anymore!
                            TType newType = TType(fnCandidate->getReturnType());
                            newType.changeQualifier(EvqConst);
                            $$->setType(newType);
                        }
                    }
                } else {
                    // This is a real function call
                    
                    $$ = parseContext.intermediate.setAggregateOperator($1.intermAggregate, EOpFunctionCall, $1.range, parseContext.extensionChanged);
                    $$->setType(fnCandidate->getReturnType());                   
                    
                    // this is how we know whether the given function is a builtIn function or a user defined function
                    // if builtIn == false, it's a userDefined -> could be an overloaded builtIn function also
                    // if builtIn == true, it's definitely a builtIn function with EOpNull
                    if (!builtIn) {
                        $$->getAsAggregate()->setUserDefined(); 
                    } else {
                        if (strcmp(fnCandidate->getMangledName().c_str(), EMIT_VERTEX_SIG) == 0) {
                            $$->setEmitVertex();
                        }
                    }
                    $$->getAsAggregate()->setName(fnCandidate->getMangledName());

                    TQualifier qual;
                    TQualifierList& qualifierList = $$->getAsAggregate()->getQualifier();
                    for (int i = 0; i < fnCandidate->getParamCount(); ++i) {
                        qual = (*fnCandidate)[i].type->getQualifier();
                        if (qual == EvqOut || qual == EvqInOut) {
                            if (parseContext.lValueErrorCheck($$->getRange(), "assign", $$->getAsAggregate()->getSequence()[i]->getAsTyped())) {
                                parseContext.error($1.intermNode->getRange(), "Constant value cannot be passed for 'out' or 'inout' parameters.", "Error", "");
                                parseContext.recover(__FILE__, __LINE__);
                            }
                        }
                        qualifierList.push_back(qual);
                    }
                    $$->setType(fnCandidate->getReturnType());
                }
            } else {
                // error message was put out by PaFindFunction()
                // Put on a dummy node for error recovery
                constUnion *unionArray = new constUnion[1];
                unionArray->setFConst(0.0f);
                $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), $1.range, parseContext.extensionChanged);
                parseContext.recover(__FILE__, __LINE__);
            }
        }
        delete fnCall;
        if ($$) $$->setRange($1.range);
    }
    ;

function_call_or_method
    : function_call_generic {
        $$ = $1;
    }
    | postfix_expression DOT function_call_generic {
        if ($1->isArray() && $3.function->getName() == "length") {
            //
            // implement array.length()
            //
            $$ = $3;
            $$.intermNode = $1;
            $$.function->relateToOperator(EOpArrayLength);
        } else {
            parseContext.error($3.range, "methods are not supported", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $$ = $3;
        }
        $$.range = addRange($1->getRange(), $3.range);
    }
    ;

function_call_generic
    : function_call_header_with_parameters RIGHT_PAREN {
        if ($1.function->getBuiltInOp() != EOpNull &&
            $1.function->getReturnTypePointer()->isArray() && 
            $1.function->getReturnTypePointer()->getArraySize() == 0) {
                $1.function->getReturnTypePointer()->setArraySize($1.function->getParamCount());
        }
        $$ = $1;
        $$.range = addRange($1.range, $2.range);
    }
    | function_call_header_no_parameters RIGHT_PAREN {
        $$ = $1;
        $$.range = addRange($1.range, $2.range);
    }
    ;
    
function_call_header_no_parameters 
    : function_call_header VOID_TYPE {
        $$.function = $1;
        $$.intermNode = 0;
    }
    | function_call_header {
        $$.function = $1;
        $$.intermNode = 0;
    }
    ;

function_call_header_with_parameters
    : function_call_header assignment_expression {
        TParameter param = { 0, new TType($2->getType()) };
        $1->addParameter(param);
        $$.function = $1;
        $$.intermNode = $2;
    }
    | function_call_header_with_parameters COMMA assignment_expression {
        TParameter param = { 0, new TType($3->getType()) };
        $1.function->addParameter(param);
        $$.function = $1.function;
        $$.intermNode = parseContext.intermediate.growAggregate($1.intermNode, $3, parseContext.extensionChanged);
    }
    ;

function_call_header 
    : function_identifier LEFT_PAREN {
        $$ = $1;
    }
    ;

// Grammar Note:  Constructors look like functions, but are recognized as types.
    
function_identifier
    : type_specifier {
        
        //
        // Constructor
        //
        if ($1.userDef) {
            TString tempString = "";
            TType type($1);
            TFunction *function = new TFunction(&tempString, type, EOpConstructStruct);
            $$ = function;
        } else {
            TOperator op = EOpNull;
            switch ($1.type) {
            case EbtFloat:
                if ($1.matrix) {
                    switch($1.matrixSize[0]) {
                        case 2:
                            switch($1.matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat2;    break;
                                case 3:
                                                                op = EOpConstructMat2x3;  break;
                                case 4:
                                                                op = EOpConstructMat2x4;  break;
                            }
                        case 3:
                            switch($1.matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat3x2;  break;
                                case 3:
                                                                op = EOpConstructMat3;    break;
                                case 4:
                                                                op = EOpConstructMat3x4;  break;
                            }
                        case 4:
                            switch($1.matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat4x2;  break;
                                case 3:
                                                                op = EOpConstructMat4x3;  break;
                                case 4:
                                                                op = EOpConstructMat4;    break;
                            }
                    }         
                } else {      
                    switch($1.size) {
                    case 1:                                     op = EOpConstructFloat; break;
                    case 2:                                     op = EOpConstructVec2;  break;
                    case 3:                                     op = EOpConstructVec3;  break;
                    case 4:                                     op = EOpConstructVec4;  break;
                    }       
                }  
                break;               
            case EbtInt:
                switch($1.size) {
                case 1:                                         op = EOpConstructInt;   break;
                case 2:       FRAG_VERT_GEOM_ONLY("ivec2", $1.range); op = EOpConstructIVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("ivec3", $1.range); op = EOpConstructIVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("ivec4", $1.range); op = EOpConstructIVec4; break;
                }         
                break;    
            case EbtUInt:
                switch($1.size) {
                case 1:                                          op = EOpConstructUInt;  break;
                case 2:       FRAG_VERT_GEOM_ONLY("uvec2", $1.range); op = EOpConstructUVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("uvec3", $1.range); op = EOpConstructUVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("uvec4", $1.range); op = EOpConstructUVec4; break;
                }         
                break;    
            case EbtBool:
                switch($1.size) {
                case 1:                                         op = EOpConstructBool;  break;
                case 2:       FRAG_VERT_GEOM_ONLY("bvec2", $1.range); op = EOpConstructBVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("bvec3", $1.range); op = EOpConstructBVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("bvec4", $1.range); op = EOpConstructBVec4; break;
                }         
                break;
            }
            if (op == EOpNull) {                    
                parseContext.error($1.range, "cannot construct this type", TType::getBasicString($1.type), "");
                parseContext.recover(__FILE__, __LINE__);
                $1.type = EbtFloat;
                op = EOpConstructFloat;
            }            
            TString tempString = "";
            TType type($1);
            TFunction *function = new TFunction(&tempString, type, op);
            $$ = function;
        }
    }
    | IDENTIFIER {
        if (parseContext.reservedErrorCheck($1.range, *$1.string)) 
            parseContext.recover(__FILE__, __LINE__);
        TType type(EbtVoid);
        TFunction *function = new TFunction($1.string, type);
        $$ = function;
    }
    | FIELD_SELECTION {
        if (parseContext.reservedErrorCheck($1.range, *$1.string)) 
            parseContext.recover(__FILE__, __LINE__);
        TType type(EbtVoid);
        TFunction *function = new TFunction($1.string, type);
        $$ = function;
    }
    ;

unary_expression
    : postfix_expression {
        $$ = $1;
    }
    | INC_OP unary_expression {
        if (parseContext.lValueErrorCheck($1.range, "++", $2))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addUnaryMath(EOpPreIncrement, $2, $1.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.unaryOpError($1.range, "++", $2->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $2;
        }
        if ($$ && $2) $$->setRange(addRange($1.range, $2->getRange()));
    }
    | DEC_OP unary_expression {
        if (parseContext.lValueErrorCheck($1.range, "--", $2))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addUnaryMath(EOpPreDecrement, $2, $1.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.unaryOpError($1.range, "--", $2->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $2;
        }
        if ($$ && $2) $$->setRange(addRange($1.range, $2->getRange()));
    }
    | unary_operator unary_expression {
        if ($1.op != EOpNull) {
            $$ = parseContext.intermediate.addUnaryMath($1.op, $2, $1.range, parseContext.symbolTable, parseContext.extensionChanged);
            if ($$ == 0) {
                const char* errorOp = "";
                switch($1.op) {
                case EOpNegative:   errorOp = "-"; break;
                case EOpLogicalNot: errorOp = "!"; break;
                case EOpBitwiseNot: errorOp = "~"; break;
				default: break;
                }
                parseContext.unaryOpError($1.range, errorOp, $2->getCompleteString());
                parseContext.recover(__FILE__, __LINE__);
                $$ = $2;
            }
        } else
            $$ = $2;
        if ($$ && $2) $$->setRange(addRange($1.range, $2->getRange()));
    }
    ;
// Grammar Note:  No traditional style type casts.

unary_operator
    : PLUS  { $$.range = $1.range; $$.op = EOpNull; }
    | DASH  { $$.range = $1.range; $$.op = EOpNegative; }
    | BANG  { $$.range = $1.range; $$.op = EOpLogicalNot; }
    | TILDE { 
              if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                  parseContext.recover(__FILE__, __LINE__);
              }
              $$.range = $1.range; 
              $$.op = EOpBitwiseNot; 
            }
    ;
// Grammar Note:  No '*' or '&' unary ops.  Pointers are not supported.

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression STAR unary_expression {
        FRAG_VERT_GEOM_ONLY("*", $2.range);
        $$ = parseContext.intermediate.addBinaryMath(EOpMul, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "*", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | multiplicative_expression SLASH unary_expression {
        FRAG_VERT_GEOM_ONLY("/", $2.range); 
        $$ = parseContext.intermediate.addBinaryMath(EOpDiv, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "/", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | multiplicative_expression PERCENT unary_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
                parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpMod, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "%", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression {  
        $$ = parseContext.intermediate.addBinaryMath(EOpAdd, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "+", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | additive_expression DASH multiplicative_expression {
        $$ = parseContext.intermediate.addBinaryMath(EOpSub, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "-", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        } 
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression LEFT_OP additive_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpLeftShift, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "<<", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | shift_expression RIGHT_OP additive_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpRightShift, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, ">>", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression LEFT_ANGLE shift_expression { 
        $$ = parseContext.intermediate.addBinaryMath(EOpLessThan, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "<", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | relational_expression RIGHT_ANGLE shift_expression  { 
        $$ = parseContext.intermediate.addBinaryMath(EOpGreaterThan, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, ">", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | relational_expression LE_OP shift_expression  { 
        $$ = parseContext.intermediate.addBinaryMath(EOpLessThanEqual, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "<=", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | relational_expression GE_OP shift_expression  { 
        $$ = parseContext.intermediate.addBinaryMath(EOpGreaterThanEqual, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, ">=", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ_OP relational_expression  {
        $$ = parseContext.intermediate.addBinaryMath(EOpEqual, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "==", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    | equality_expression NE_OP relational_expression { 
        $$ = parseContext.intermediate.addBinaryMath(EOpNotEqual, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "!=", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

and_expression
    : equality_expression { $$ = $1; }
    | and_expression AMPERSAND equality_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpAnd, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "&", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression CARET and_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpExclusiveOr, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "^", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression VERTICAL_BAR exclusive_or_expression {
        if (parseContext.extensionErrorCheck($1->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ = parseContext.intermediate.addBinaryMath(EOpInclusiveOr, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "|", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression AND_OP inclusive_or_expression {
        $$ = parseContext.intermediate.addBinaryMath(EOpLogicalAnd, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "&&", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

logical_xor_expression
    : logical_and_expression { $$ = $1; }
    | logical_xor_expression XOR_OP logical_and_expression  { 
        $$ = parseContext.intermediate.addBinaryMath(EOpLogicalXor, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "^^", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

logical_or_expression
    : logical_xor_expression { $$ = $1; }
    | logical_or_expression OR_OP logical_xor_expression  { 
        $$ = parseContext.intermediate.addBinaryMath(EOpLogicalOr, $1, $3, $2.range, parseContext.symbolTable, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, "||", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            $$ = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), $2.range, parseContext.extensionChanged);
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression QUESTION expression COLON assignment_expression {
       if (parseContext.boolErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);
       
        $$ = parseContext.intermediate.addSelection($1, $3, $5, $2.range, parseContext.extensionChanged);

        /* GLSL 1.20 does not require the expressions to have the same type,
         * as long as there is a conversion to one of the expressions to make
         * their types match. The resulting matching is the type of the
         * entire expression */
        /*
        if ($3->getType() != $5->getType())
            $$ = 0;
        */

        if ($$ == 0) {
            parseContext.binaryOpError($2.range, ":", $3->getCompleteString(), $5->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $5;
        }
        if ($$ && $1 && $5) $$->setRange(addRange($1->getRange(), $5->getRange()));
    }
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | unary_expression assignment_operator assignment_expression {        
        if (parseContext.lValueErrorCheck($2.range, "assign", $1))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addAssign($2.op, $1, $3, $2.range, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.assignError($2.range, "assign", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $1;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

assignment_operator
    : EQUAL        {                                    $$.range = $1.range; $$.op = EOpAssign; }
    | MUL_ASSIGN   { FRAG_VERT_GEOM_ONLY("*=", $1.range);     $$.range = $1.range; $$.op = EOpMulAssign; }
    | DIV_ASSIGN   { FRAG_VERT_GEOM_ONLY("/=", $1.range);     $$.range = $1.range; $$.op = EOpDivAssign; }
    | MOD_ASSIGN   { 
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; $$.op = EOpModAssign; 
                   }
    | ADD_ASSIGN   { $$.range = $1.range; $$.op = EOpAddAssign; }
    | SUB_ASSIGN   { $$.range = $1.range; $$.op = EOpSubAssign; }
    | LEFT_ASSIGN  {
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; 
                     $$.op = EOpLeftShiftAssign; 
                   }
    | RIGHT_ASSIGN {
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; 
                     $$.op = EOpRightShiftAssign; 
                   }
    | AND_ASSIGN   { 
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; 
                     $$.op = EOpAndAssign; 
                   }
    | XOR_ASSIGN   {
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; 
                     $$.op = EOpExclusiveOrAssign; 
                   }
    | OR_ASSIGN    {
                     if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     $$.range = $1.range; 
                     $$.op = EOpInclusiveOrAssign; 
                   }
    ;

expression
    : assignment_expression {
        $$ = $1;
    }
    | expression COMMA assignment_expression {
        $$ = parseContext.intermediate.addComma($1, $3, $2.range, parseContext.extensionChanged);
        if ($$ == 0) {
            parseContext.binaryOpError($2.range, ",", $1->getCompleteString(), $3->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            $$ = $3;
        }
        if ($$ && $1 && $3) $$->setRange(addRange($1->getRange(), $3->getRange()));
    }
    ;

constant_expression
    : conditional_expression {
        if (parseContext.constErrorCheck($1))
            parseContext.recover(__FILE__, __LINE__);
        $$ = $1;
    }
    ;

declaration
    : function_prototype SEMICOLON   {
        TIntermNode *funcDecl;
        funcDecl = parseContext.intermediate.addFuncDeclaration($1.range, $1.function, parseContext.extensionChanged);
        $$ = funcDecl;
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    | init_declarator_list SEMICOLON { 
        if (($1.intermAggregate) && ($1.intermAggregate->getOp() == EOpNull)) {
                $1.intermAggregate->setOperator(EOpDeclaration);
        }
        $$ = $1.intermAggregate; 
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    ;

function_prototype 
    : function_declarator RIGHT_PAREN  {
        //
        // Multiple declarations of the same function are allowed.
        //
        // If this is a definition, the definition production code will check for redefinitions 
        // (we don't know at this point if it's a definition or not).
        //
        // Redeclarations are allowed.  But, return types and parameter qualifiers must match.
        //        
        TFunction* prevDec = static_cast<TFunction*>(parseContext.symbolTable.find($1->getMangledName()));
        if (prevDec) {
            if (prevDec->getReturnType() != $1->getReturnType()) {
                parseContext.error($2.range, "overloaded functions must have the same return type", $1->getReturnType().getBasicString(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            for (int i = 0; i < prevDec->getParamCount(); ++i) {
                if ((*prevDec)[i].type->getQualifier() != (*$1)[i].type->getQualifier()) {
                    parseContext.error($2.range, "overloaded functions must have the same parameter qualifiers", (*$1)[i].type->getQualifierString(), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        
        //
        // If this is a redeclaration, it could also be a definition,
        // in which case, we want to use the variable names from this one, and not the one that's
        // being redeclared.  So, pass back up this declaration, not the one in the symbol table.
        //
        $$.function = $1;

        parseContext.symbolTable.insert(*$$.function);
    }
    ;

function_declarator 
    : function_header {
        $$ = $1;
    }
    | function_header_with_parameters { 
        $$ = $1;  
    }
    ;


function_header_with_parameters
    : function_header parameter_declaration {
        // Add the parameter 
        $$ = $1;
        if ($2.param.type->getBasicType() != EbtVoid)
            $1->addParameter($2.param);
        else
            delete $2.param.type;
    }
    | function_header_with_parameters COMMA parameter_declaration {   
        //
        // Only first parameter of one-parameter functions can be void
        // The check for named parameters not being void is done in parameter_declarator 
        //
        if ($3.param.type->getBasicType() == EbtVoid) {
            //
            // This parameter > first is void
            //
            parseContext.error($2.range, "cannot be an argument type except for '(void)'", "void", "");
            parseContext.recover(__FILE__, __LINE__);
            delete $3.param.type;
        } else {
            // Add the parameter 
            $$ = $1; 
            $1->addParameter($3.param);
        }
    }
    ;

function_header 
    : fully_specified_type IDENTIFIER LEFT_PAREN {
        if ($1.qualifier != EvqGlobal && $1.qualifier != EvqTemporary) {
            parseContext.error($2.range, "no qualifiers allowed for function return", getQualifierString($1.qualifier, parseContext.language), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        // make sure a sampler is not involved as well...
        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);
        
        // Add the function as a prototype after parsing it (we do not support recursion) 
        TFunction *function;
        TType type($1);
        function = new TFunction($2.string, type);
        $$ = function;
    }
    ;

parameter_declarator
    // Type + name 
    : type_specifier IDENTIFIER {
        if (parseContext.arraySizeUnspecifiedErrorCheck($1.range, $1)) {
            parseContext.error($1.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $1.setArray(false);
        }
        if ($1.type == EbtVoid) {
            parseContext.error($2.range, "illegal use of type 'void'", $2.string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if (parseContext.reservedErrorCheck($2.range, *$2.string))
            parseContext.recover(__FILE__, __LINE__);
        TParameter param = {$2.string, new TType($1)};
        $$.range = $2.range;
        $$.param = param;
    }
    | type_specifier IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET {
        if (parseContext.arraySizeUnspecifiedErrorCheck($1.range, $1)) {
            parseContext.error($1.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $1.setArray(false);
        }
        // Check that we can make an array out of this type
        if (parseContext.arrayTypeErrorCheck($3.range, $1))
            parseContext.recover(__FILE__, __LINE__);
            
        if (parseContext.reservedErrorCheck($2.range, *$2.string))
            parseContext.recover(__FILE__, __LINE__);
            
        int size;
        if (parseContext.arraySizeErrorCheck($3.range, $4, size))
            parseContext.recover(__FILE__, __LINE__);
        $1.setArray(true, size);
             
        TType* type = new TType($1);
        TParameter param = { $2.string, type };
        $$.range = addRange($2.range, $5.range);
        $$.param = param;
    }
    ;

parameter_declaration 
    // 
    // The only parameter qualifier a parameter can have are 
    // IN_QUAL, OUT_QUAL, INOUT_QUAL, or CONST.
    //
    
    //
    // Type + name 
    //
    : type_qualifier parameter_qualifier parameter_declarator {
        $$ = $3;
        if (parseContext.paramErrorCheck($3.range, $1.qualifier, $2, $$.param.type))
            parseContext.recover(__FILE__, __LINE__);
        $$.range = addRange($1.range, $3.range);
    }
    | parameter_qualifier parameter_declarator {
        $$ = $2;
        if (parseContext.parameterSamplerErrorCheck($2.range, $1, *$2.param.type))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.paramErrorCheck($2.range, EvqTemporary, $1, $$.param.type))
            parseContext.recover(__FILE__, __LINE__);
        $$.range = $2.range;
    }
    //
    // Only type 
    //
    | type_qualifier parameter_qualifier parameter_type_specifier {
        $$ = $3;
        if (parseContext.paramErrorCheck($3.range, $1.qualifier, $2, $$.param.type))
            parseContext.recover(__FILE__, __LINE__);
        $$.range = addRange($1.range, $3.range);
    }
    | parameter_qualifier parameter_type_specifier {
        $$ = $2;
        if (parseContext.parameterSamplerErrorCheck($2.range, $1, *$2.param.type))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.paramErrorCheck($2.range, EvqTemporary, $1, $$.param.type))
            parseContext.recover(__FILE__, __LINE__);
        $$.range = $2.range;
    }
    ;
    
parameter_qualifier
    : /* empty */ {
        $$ = EvqIn;
    }
    | IN_QUAL {
        $$ = EvqIn;
    }
    | OUT_QUAL {
        $$ = EvqOut;
    }
    | INOUT_QUAL {
        $$ = EvqInOut;
    }
    ;

parameter_type_specifier 
    : type_specifier {
        if (parseContext.arraySizeUnspecifiedErrorCheck($1.range, $1)) {
            parseContext.error($1.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $1.setArray(false);
        }
        TParameter param = { 0, new TType($1) };
        $$.param = param;
        $$.range = $1.range;
    }
    ;

array_declarator_suffix
    : LEFT_BRACKET RIGHT_BRACKET {
        $$.setBasic(EbtVoid, EvqConst, EvmNone, $1.range);
        $$.setArray(true);
        $$.range = addRange($1.range, $2.range);
    }
    | LEFT_BRACKET constant_expression RIGHT_BRACKET {
        $$.setBasic(EbtVoid, EvqConst, EvmNone, $1.range);

        int size;
        if (parseContext.arraySizeErrorCheck($1.range, $2, size))
            parseContext.recover(__FILE__, __LINE__);

        $$.setArray(true, size);

        $$.range = addRange($1.range, $3.range);
    }
    | LEFT_BRACKET RIGHT_BRACKET array_declarator_suffix {
        GEOM_ONLY("multiple array suffixes", $3.range);
        $$ = $3;
        $$.insertArray();
        $$.range = addRange($1.range, $3.range);
    }
    | LEFT_BRACKET constant_expression RIGHT_BRACKET array_declarator_suffix {
        GEOM_ONLY("multiple array suffixes", $3.range);
        $$ = $4;
        int size;
        if (parseContext.arraySizeErrorCheck($1.range, $2, size))
            parseContext.recover(__FILE__, __LINE__);
        $$.insertArray(size);
        $$.range = addRange($1.range, $4.range);
    }
    ;

init_declarator_list
    : single_declaration {
        $$ = $1;
    } 
    | init_declarator_list COMMA IDENTIFIER {
        $$ = $1;

        if ($1.type.type != EbtInvariant) {
        
            if (parseContext.structQualifierErrorCheck($3.range, $$.type))
                parseContext.recover(__FILE__, __LINE__);
        
            if (parseContext.nonInitConstErrorCheck($3.range, *$3.string, $$.type))
                parseContext.recover(__FILE__, __LINE__);

            if (parseContext.nonInitErrorCheck($3.range, *$3.string, $$.type))
                parseContext.recover(__FILE__, __LINE__);
                
            TSymbol *sym = parseContext.symbolTable.find(*$3.string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration($3.range, (TVariable*)sym, NULL, 
                                                         parseContext.extensionChanged);

            /* Special care taken for structs */
            if ( $1.type.type == EbtStruct 
                 && $1.type.userDef != 0 
                 && $1.type.userDef->isSpecified() ) {
                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ($1.intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  intermNode->getAsDeclarationNode(), 
                                  parseContext);
            } else {
                /* Add declaration normally to the tree */
                $$.intermAggregate = 
                    parseContext.intermediate.growAggregate($1.intermNode, intermNode, 
                                                            parseContext.extensionChanged);
            }
        } else {
            TSymbol *sym = parseContext.symbolTable.find(*$3.string);
            if (sym && sym->isVariable()) {
            
                TVariable *var = static_cast<TVariable*>(sym);
                var->getType().addVaryingModifier(EvmInvariant);
            
                TString *newName = new TString(var->getName());
                TType newType(EbtInvariant);
                TPublicType newPType;
                newPType.setBasic(EbtInvariant, EvqTemporary);
                TVariable *newVar = new TVariable(newName, newType);

                TIntermNode *intermNode = 
                    parseContext.intermediate.addDeclaration($3.range, newVar, NULL, 
                                                             parseContext.extensionChanged);
                /* Add declaration normally to the tree */
                $$.intermAggregate = 
                    parseContext.intermediate.growAggregate($1.intermNode, intermNode, 
                                                            parseContext.extensionChanged);
            }
        }
        $$.range = addRange($1.range, $3.range);
    }
    | init_declarator_list COMMA IDENTIFIER array_declarator_suffix {
        if (parseContext.structQualifierErrorCheck($3.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
            
        if (parseContext.nonInitConstErrorCheck($3.range, *$3.string, $1.type))
            parseContext.recover(__FILE__, __LINE__);

        $$ = $1;
        
        if (parseContext.arrayTypeErrorCheck($4.range, $1.type) || parseContext.arrayQualifierErrorCheck($4.range, $1.type)) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int i;
            for (i=0; i<MAX_ARRAYS; i++) {
                $1.type.addArray(true, $4.arraySize[i], i);
            }
            
            TVariable* variable;
            if (parseContext.arrayErrorCheck($4.range, *$3.string, $1.type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }
        
        /* Check for multi-dimensional arrays usage */
        if ($4.getNumArrays() > 1 && $1.type.qualifier != EvqVaryingIn) {
            parseContext.error($4.range, "multi-dimensional array usage error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && $1.qualifier == EvqVaryingIn) {
            
            // First, find 'gl_VerticesIn' in symbol table
            int vertexInSize = 0;
            TVariable* variable = NULL;
            TSymbol* symbol = parseContext.symbolTable.find("gl_VerticesIn");
            if (symbol) {
                variable = static_cast<TVariable*>(symbol);
                constUnion* varUnion      = variable->getConstPointer();
                vertexInSize = variable->getConstPointer()[0].getIConst();
            
                if ($4.arraySize[0] == 0) {
                    // Use gl_VertexIn for initialization of the array
                    $1.type.setArray(true, vertexInSize);
    
                    if (parseContext.arrayErrorCheck($4.range, *$3.string, $1.type, variable))
                        parseContext.recover(__FILE__, __LINE__);
                } else {
                    if (parseContext.arraySizeGeometryVaryingInErrorCheck($4.range, vertexInSize, $1.type.arraySize[0])) {
                        parseContext.recover(__FILE__, __LINE__);
                    }
                }
            }

            if (parseContext.arrayFullDefinedGeometryVaryingInErrorCheck($4.range, *$3.string, $1.type)) {
                parseContext.recover(__FILE__, __LINE__);
            }
        }
 
        TSymbol *sym = parseContext.symbolTable.find(*$3.string);
        TIntermNode *intermNode = 
            parseContext.intermediate.addDeclaration($3.range, (TVariable*)sym, NULL, parseContext.extensionChanged);

        /* Special care taken for structs */
        if ( $1.type.type == EbtStruct 
             && $1.type.userDef != 0 
             && $1.type.userDef->isSpecified() ) {
            /* Add declaration to instances of stuct */
            TIntermSpecification* specificationNode = 
                ($1.intermAggregate->getSequence())[0]->getAsSpecificationNode();

            addStructInstance(specificationNode,
                              intermNode->getAsDeclarationNode(), 
                              parseContext);
        } else {
            /* Add declaration normally to the tree */
            $$.intermAggregate = 
                parseContext.intermediate.growAggregate($1.intermNode, intermNode, parseContext.extensionChanged);
            $$.intermAggregate->setRange(addRange($1.range, $4.range));
        }
        $$.range = addRange($1.range, $4.range);
    }
    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET RIGHT_BRACKET EQUAL initializer {
        if (parseContext.structQualifierErrorCheck($3.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
            
        $$ = $1;
            
        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck($4.range, $1.type) || parseContext.arrayQualifierErrorCheck($4.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
        else {
			$1.type.setArray(true, $7->getType().getArraySize());
            if (parseContext.arrayErrorCheck($4.range, *$3.string, $1.type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        TSymbol *sym = parseContext.symbolTable.find(*$3.string);
        TIntermNode *intermNode = 
            parseContext.intermediate.addDeclaration($3.range, (TVariable*)sym, NULL, parseContext.extensionChanged);
        /* Special care taken for structs */
        if ( $1.type.type == EbtStruct && $1.type.userDef != 0 && $1.type.userDef->isSpecified() ) {

            TIntermNode* initNode;
            if (!parseContext.executeInitializer($3.range, *$3.string, $1.type, $7, initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$3.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ($1.intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode, 
                                  parseContext);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        } else {
            /* Add declaration normally to the tree */
            TIntermNode* initNode;
            if (!parseContext.executeInitializer($3.range, *$3.string, $1.type, $7, initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$3.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                $$.intermAggregate =
                    parseContext.intermediate.growAggregate($$.intermAggregate, decNode, parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $7->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        }
        $$.range = addRange($1.range, $7->getRange());
    }
    | init_declarator_list COMMA IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET EQUAL initializer {
        if (parseContext.structQualifierErrorCheck($3.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
            
        $$ = $1;
            
        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck($4.range, $1.type) || parseContext.arrayQualifierErrorCheck($4.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
        else {
            int size;
            if (parseContext.arraySizeErrorCheck($4.range, $5, size))
                parseContext.recover(__FILE__, __LINE__);
            $1.type.setArray(true, size);
            if (parseContext.arrayErrorCheck($4.range, *$3.string, $1.type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        TSymbol *sym = parseContext.symbolTable.find(*$3.string);
        TIntermNode *intermNode =
            parseContext.intermediate.addDeclaration($3.range, (TVariable*)sym, NULL, parseContext.extensionChanged);
        /* Special care taken for structs */
        if ( $1.type.type == EbtStruct && $1.type.userDef != 0 && $1.type.userDef->isSpecified() ) {
            TIntermNode* initNode;
            if (!parseContext.executeInitializer($3.range, *$3.string, $1.type, $8, initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$3.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ($1.intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode, 
                                  parseContext);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        } else {
            /* Add declaration normally to the tree */
            TIntermNode* initNode;
            if (!parseContext.executeInitializer($3.range, *$3.string, $1.type, $8, initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$3.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                $$.intermAggregate =
                    parseContext.intermediate.growAggregate($$.intermAggregate, decNode, parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $8->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        }
        $$.range = addRange($1.range, $8->getRange());
    }
    | init_declarator_list COMMA IDENTIFIER EQUAL initializer {
        if (parseContext.structQualifierErrorCheck($3.range, $1.type))
            parseContext.recover(__FILE__, __LINE__);
        
        $$ = $1;
        
        TIntermNode* intermNode;
        if (!parseContext.executeInitializer($3.range, *$3.string, $1.type, $5, intermNode)) {
            //
            // build the intermediate representation
            //
            TSymbol *sym = parseContext.symbolTable.find(*$3.string);
            TIntermNode *decNode;
            decNode = 
                parseContext.intermediate.addDeclaration($3.range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
            /* Special care taken for structs */
           if ( $1.type.type == EbtStruct 
                 && $1.type.userDef != 0 
                 && $1.type.userDef->isSpecified() ) {
                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ($1.intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode->getAsDeclarationNode(), 
                                  parseContext);
            } else {
                /* Add declaration normally to the tree */
                $$.intermAggregate = 
                    parseContext.intermediate.growAggregate($1.intermNode, decNode, parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $5->getRange()));
            }
         } else {
            parseContext.recover(__FILE__, __LINE__);
            $$.intermAggregate = 0;
        }
        $$.range = addRange($1.range, $5->getRange());
    }
    ;

single_declaration 
    : fully_specified_type {
        $$.type = $1;
        $$.intermAggregate = 0;

        if ( $1.type == EbtStruct &&
             $1.userDef != 0 && 
             $1.userDef->isSpecified() == true ) {
            TIntermNode *intermNode = 
                parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                
            processStruct($1.userDef->getStruct(), 
                          intermNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);

            $$.intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);

            if ($$.intermAggregate)
                $$.intermAggregate->setOperator(EOpSpecification); 
        }
        $$.range = $1.range;
    }
    | fully_specified_type IDENTIFIER {
        $$.intermAggregate = 0;
        $$.type = $1;
    
        if (parseContext.structQualifierErrorCheck($2.range, $$.type))
            parseContext.recover(__FILE__, __LINE__);
        
        if (parseContext.nonInitConstErrorCheck($2.range, *$2.string, $$.type))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.nonInitErrorCheck($2.range, *$2.string, $$.type))
            parseContext.recover(__FILE__, __LINE__);

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && $1.qualifier == EvqVaryingIn) {
            if (parseContext.nonArrayGeometryVaryingInErrorCheck($2.range, $1, *$2.string))
                parseContext.recover(__FILE__, __LINE__);
        }
        
        /* Special handling for structs */
        if ( $1.type == EbtStruct  && $1.userDef != 0 && $1.userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                
            processStruct($1.userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TSymbol *sym = parseContext.symbolTable.find(*$2.string);
            TIntermNode *declarationNode = 
                parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            $$.intermAggregate = specificationAggregate;

        } else {
            /* None-struct declarations: just add declaration node */
            TSymbol *sym = parseContext.symbolTable.find(*$2.string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
            ((TIntermDeclaration*)intermNode)->setFirst(true);
            $$.intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);
        }
        $$.range = addRange($1.range, $2.range);
    }
    | fully_specified_type IDENTIFIER array_declarator_suffix {
        $$.intermAggregate = 0;
        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.nonInitConstErrorCheck($2.range, *$2.string, $1))
            parseContext.recover(__FILE__, __LINE__);

        $$.type = $1;
        
        if (parseContext.arrayTypeErrorCheck($3.range, $1) || parseContext.arrayQualifierErrorCheck($3.range, $1)) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int i;
            for (i=0; i<MAX_ARRAYS; i++) {
                $1.addArray(true, $3.arraySize[i], i);
            }
            
            TVariable* variable;
            if (parseContext.arrayErrorCheck($3.range, *$2.string, $1, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        /* Check for multi-dimensional arrays usage */
        if ($3.getNumArrays() > 1 && $1.qualifier != EvqVaryingIn) {
            parseContext.error($3.range, "multi-dimensional array usage error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && $1.qualifier == EvqVaryingIn) {
            
            // First, find 'gl_VerticesIn' in symbol table
            int vertexInSize = 0;
            TVariable* variable = NULL;
            TSymbol* symbol = parseContext.symbolTable.find("gl_VerticesIn");
            if (symbol) {
                variable = static_cast<TVariable*>(symbol);
                constUnion* varUnion      = variable->getConstPointer();
                vertexInSize = variable->getConstPointer()[0].getIConst();
            
                if ($3.arraySize[0] == 0) {
                    // Use gl_VertexIn for initialization of the array
                    $1.setArray(true, vertexInSize);
    
                    if (parseContext.arrayErrorCheck($3.range, *$2.string, $1, variable))
                        parseContext.recover(__FILE__, __LINE__);
                } else {
                    if (parseContext.arraySizeGeometryVaryingInErrorCheck($3.range, vertexInSize, $1.arraySize[0])) {
                        parseContext.recover(__FILE__, __LINE__);
                    }
                }
            }

            if (parseContext.arrayFullDefinedGeometryVaryingInErrorCheck($3.range, *$2.string, $1)) {
                parseContext.recover(__FILE__, __LINE__);
            }
        }
 
        /* Special handling for structs */
        if ( $1.type == EbtStruct  && $1.userDef != 0 && $1.userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                
            processStruct($1.userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TSymbol *sym = parseContext.symbolTable.find(*$2.string);
            TIntermNode *declarationNode = 
                parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            $$.intermAggregate = specificationAggregate;

        } else {
            /* None-struct declarations: just add declaration node */
            TSymbol *sym = parseContext.symbolTable.find(*$2.string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
            intermNode->getAsDeclarationNode()->setFirst(true);
            $$.intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, 
                                                        parseContext.extensionChanged);
            $$.intermAggregate->setRange(addRange($1.range, $3.range));
        }
        $$.range = addRange($1.range, $3.range);

    }
    | fully_specified_type IDENTIFIER LEFT_BRACKET RIGHT_BRACKET EQUAL initializer {
        $$.intermAggregate = 0;

        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);

        $$.type = $1;

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck($3.range, $1) || parseContext.arrayQualifierErrorCheck($3.range, $1))
            parseContext.recover(__FILE__, __LINE__);
        else {
            $1.setArray(true, $6->getType().getArraySize());
            if (parseContext.arrayErrorCheck($3.range, *$2.string, $1, variable))
                parseContext.recover(__FILE__, __LINE__);
        }
        
        /* Special handling for structs */
        if ( $1.type == EbtStruct  && $1.userDef != 0 && $1.userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                
            processStruct($1.userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TIntermNode* initNode;
            TIntermNode *declarationNode;
            if (!parseContext.executeInitializer($2.range, *$2.string, $1, $6, initNode, variable)) {
                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                declarationNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                declarationNode = NULL;
            }

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            $$.intermAggregate = specificationAggregate;

        } else {
            TIntermNode* intermNode;
            if (!parseContext.executeInitializer($2.range, *$2.string, $1, $6, intermNode, variable)) {
                //
                // Build intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                }

                $$.intermAggregate = parseContext.intermediate.makeAggregate(decNode, 
                                                                   parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $6->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        }
        $$.range = addRange($1.range, $6->getRange());
    }
    | fully_specified_type IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET EQUAL initializer {
        $$.intermAggregate = 0;

        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);

        $$.type = $1;

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck($3.range, $1) || parseContext.arrayQualifierErrorCheck($3.range, $1))
            parseContext.recover(__FILE__, __LINE__);
        else {
            int size;
            if (parseContext.arraySizeErrorCheck($3.range, $4, size))
                parseContext.recover(__FILE__, __LINE__);
            
            $1.setArray(true, size);
            if (parseContext.arrayErrorCheck($3.range, *$2.string, $1, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        
        /* Special handling for structs */
        if ( $1.type == EbtStruct  && $1.userDef != 0 && $1.userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                
            processStruct($1.userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TIntermNode* initNode;
            TIntermNode *declarationNode;
            if (!parseContext.executeInitializer($2.range, *$2.string, $1, $7, initNode, variable)) {
                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                declarationNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, initNode, parseContext.extensionChanged);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                declarationNode = NULL;
            }

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            $$.intermAggregate = specificationAggregate;

        } else {
            TIntermNode* intermNode;
            if (!parseContext.executeInitializer($2.range, *$2.string, $1, $7, intermNode, variable)) {
                //
                // Build intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                } else {
                    decNode = parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                }

                $$.intermAggregate = parseContext.intermediate.makeAggregate(decNode, parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $7->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                $$.intermAggregate = 0;
            }
        }
        $$.range = addRange($1.range, $7->getRange());
    }
    | fully_specified_type IDENTIFIER EQUAL initializer {
        
        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);

        $$.type = $1;

        TIntermNode* intermNode;
        if (!parseContext.executeInitializer($2.range, *$2.string, $1, $4, intermNode)) {
            //
            // Build intermediate representation
            //
            /* Special handling for structs */
            if ( $1.type == EbtStruct  && $1.userDef != 0 && $1.userDef->isSpecified() ) {
                /* Struct declarations: add a Specification node to the parse tree */
                TIntermNode *specificationNode =
                    parseContext.intermediate.addSpecification($1.range, $1.userDef, parseContext.extensionChanged);
                    
                processStruct($1.userDef->getStruct(),
                              specificationNode->getAsSpecificationNode()->getParameterPointer(),
                              parseContext);
                
                TIntermAggregate *specificationAggregate = 
                    parseContext.intermediate.makeAggregate(specificationNode, parseContext.extensionChanged);
            
                if (specificationAggregate)
                    specificationAggregate->setOperator(EOpSpecification);

                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                TIntermNode *declarationNode = 
                    parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, 
                                                             intermNode, parseContext.extensionChanged);

                addStructInstance(specificationNode->getAsSpecificationNode(),
                                  declarationNode->getAsDeclarationNode(), 
                                  parseContext);
        
                $$.intermAggregate = specificationAggregate;

            } else {
                /* None-struct declarations: just add declaration node */
                TSymbol *sym = parseContext.symbolTable.find(*$2.string);
                TIntermNode *decNode = 
                    parseContext.intermediate.addDeclaration($2.range, (TVariable*) sym, 
                                                             intermNode, parseContext.extensionChanged);
                decNode->getAsDeclarationNode()->setFirst(true);
                $$.intermAggregate = 
                    parseContext.intermediate.makeAggregate(decNode, parseContext.extensionChanged);
                $$.intermAggregate->setRange(addRange($1.range, $4->getRange()));
            }
#if 0
            TSymbol *sym = parseContext.symbolTable.find(*$2.string);
            TIntermNode *decNode;
            
            if (intermNode) {
                decNode = parseContext.intermediate.addDeclaration($2.range,
                                                                   (TVariable*) sym,
                                                                   intermNode);
            } else {
                decNode = parseContext.intermediate.addDeclaration($2.range,
                                                                   (TVariable*) sym, 
                                                                   NULL);
            }
            decNode->getAsDeclarationNode()->setFirst(true);
            
            $$.intermAggregate = parseContext.intermediate.makeAggregate(decNode, $2.range);
#endif
        } else {
            parseContext.recover(__FILE__, __LINE__);
            $$.intermAggregate = 0;
        }
        $$.range = addRange($1.range, $4->getRange());
    }
    | INVARIANT IDENTIFIER {
        TSymbol *sym = parseContext.symbolTable.find(*$2.string);
        if (sym && sym->isVariable()) {
            
            TVariable *var = static_cast<TVariable*>(sym);
            var->getType().addVaryingModifier(EvmInvariant);
            
            TString *newName = new TString(var->getName());
            TType newType(EbtInvariant);
            TPublicType newPType;
            newPType.setBasic(EbtInvariant, EvqTemporary);
            TVariable *newVar = new TVariable(newName, newType);

            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration($2.range, newVar, NULL, 
                                                         parseContext.extensionChanged);
            ((TIntermDeclaration*)intermNode)->setFirst(true);
            $$.intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);
            $$.type = newPType;
        } else {
            parseContext.recover(__FILE__, __LINE__);
            $$.intermAggregate = 0;
        }
        $$.range = $2.range;
    }
    
//
// Place holder for the pack/unpack languages.
//
//    | buffer_specifier {
//        $$.intermAggregate = 0;
//    }
    ;

// Grammar Note:  No 'enum', or 'typedef'.

//
// Place holder for the pack/unpack languages.
//
//%type <interm> buffer_declaration
//%type <interm.type> buffer_specifier input_or_output buffer_declaration_list 
//buffer_specifier
//    : input_or_output LEFT_BRACE buffer_declaration_list RIGHT_BRACE {
//    }
//    ;
//
//input_or_output
//    : INPUT {
//        if (parseContext.globalErrorCheck($1.range, parseContext.symbolTable.atGlobalLevel(), "input"))
//            parseContext.recover(__FILE__, __LINE__);
//        UNPACK_ONLY("input", $1.range);
//        $$.qualifier = EvqInput;        
//    }
//    | OUTPUT {
//        if (parseContext.globalErrorCheck($1.range, parseContext.symbolTable.atGlobalLevel(), "output"))
//            parseContext.recover(__FILE__, __LINE__);
//        PACK_ONLY("output", $1.range);
//        $$.qualifier = EvqOutput;
//    }
//    ;

//
// Place holder for the pack/unpack languages.
//
//buffer_declaration_list
//    : buffer_declaration {
//    }
//    | buffer_declaration_list buffer_declaration {
//    }
//    ;

//
// Input/output semantics:
//   float must be 16 or 32 bits
//   float alignment restrictions?
//   check for only one input and only one output
//   sum of bitfields has to be multiple of 32
//

//
// Place holder for the pack/unpack languages.
//
//buffer_declaration
//    : type_specifier IDENTIFIER COLON constant_expression SEMICOLON {
//        if (parseContext.reservedErrorCheck($2.range, *$2.string, parseContext))
//            parseContext.recover(__FILE__, __LINE__);
//        $$.variable = new TVariable($2.string, $1);
//        if (! parseContext.symbolTable.insert(*$$.variable)) {
//            parseContext.error($2.range, "redefinition", $$.variable->getName().c_str(), "");
//            parseContext.recover(__FILE__, __LINE__);
//            // don't have to delete $$.variable, the pool pop will take care of it
//        }
//    }
//    ;

fully_specified_type
    : type_specifier {
        if (parseContext.arraySizeUnspecifiedErrorCheck($1.range, $1)) {
            parseContext.error($1.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $1.setArray(false);
        }
        $$ = $1;
    }
//
// TODO: this is a fast solution for a shift/reduce conflict!!!
// Check new additions to grammar by EXT_geometry_shader4 with respect to GLSL 1.20
//
    | type_qualifier varying_geom_modifyer type_specifier  {
        if (parseContext.varyingModifyerErrorCheck($3.range, $1, $2)) {
            parseContext.error($3.range, "varying modifier syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        if (parseContext.arraySizeUnspecifiedErrorCheck($3.range, $3)) {
            parseContext.error($3.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $3.setArray(false);
        }
        if ($3.array && parseContext.arrayQualifierErrorCheck($3.range, $1)) {
            parseContext.recover(__FILE__, __LINE__);
            $3.setArray(false);
        }
        
        if ($1.qualifier == EvqAttribute){
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ($3.type == EbtBool || $3.type == EbtInt) {
                    parseContext.error($3.range, "cannot be bool or int", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ($3.type == EbtBool) {
                    parseContext.error($3.range, "cannot be bool", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        
        
#if 0 // OLD     
	 	/* Promote varying modifier to varyings in case of geometry shaders */
        if (parseContext.language == EShLangGeometry && $1.qualifier == EvqVaryingOut) {
            // Change from in to out if specifyed
            if ($2 == EvqIn) {
                $1.qualifier = EvqVaryingIn;
            }
        }
#else	
        /* GEOMETRY SHADER CHECKS - BEGIN */
        /* Promote varying modifiers to varying if necessary */

        // Change from in to out if specified
        if ($1.qualifier == EvqVaryingIn && $2 == EvqOut) {
			$1.qualifier = EvqVaryingOut;
        }
        if ($1.qualifier == EvqVaryingOut && $2 == EvqIn) {
            $1.qualifier = EvqVaryingIn;
        }

        /* GEOMETRY SHADER CHECKS - END */
#endif
        
        if ($1.qualifier == EvqVaryingIn) {
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ($3.type == EbtBool || $3.type == EbtInt) {
                    parseContext.error($3.range, "cannot be bool or int", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ($3.type == EbtBool) {
                    parseContext.error($3.range, "cannot be bool", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                } else if (($3.type == EbtInt || $3.type == EbtUInt) && !($1.varyingModifier & EvmFlat)) {
                    parseContext.error($3.range, "needs to be flat shaded", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        if ($1.qualifier == EvqVaryingOut) {
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ($3.type == EbtBool || $3.type == EbtInt) {
                    parseContext.error($3.range, "cannot be bool or int", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ($3.type == EbtBool) {
                    parseContext.error($3.range, "cannot be bool", getQualifierString($1.qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }

        $$ = $3; 
        $$.qualifier = $1.qualifier;
        $$.varyingModifier = $1.varyingModifier;
        $$.range = addRange($1.range, $3.range);
    }
    ;
    
varying_geom_modifyer
    : {
        $$ = EvqInOut;
    }
    | IN_QUAL {
        $$ = EvqIn;
    }
    | OUT_QUAL {
        $$ = EvqOut;
    }
    ;


type_qualifier
    : CONST_QUAL {
        $$.setBasic(EbtVoid, EvqConst, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | ATTRIBUTE { 
        VERTEX_ONLY("attribute", $1.range);
        if (parseContext.globalErrorCheck($1.range, parseContext.symbolTable.atGlobalLevel(), "attribute"))
            parseContext.recover(__FILE__, __LINE__);
        $$.setBasic(EbtVoid, EvqAttribute, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | varying_modifier VARYING {
        if (parseContext.globalErrorCheck($2.range, parseContext.symbolTable.atGlobalLevel(), "varying"))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.language == EShLangVertex) {
            $$.setBasic(EbtVoid, EvqVaryingOut, $1, $2.range);
        } else if (parseContext.language == EShLangGeometry) {
            $$.setBasic(EbtVoid, EvqVaryingOut, $1, $2.range);
        } else {
            $$.setBasic(EbtVoid, EvqVaryingIn, $1, $2.range);
        }

        $$.range = $2.range;
    }
    | VARYING {
        if (parseContext.globalErrorCheck($1.range, parseContext.symbolTable.atGlobalLevel(), "varying"))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.language == EShLangVertex) {
            $$.setBasic(EbtVoid, EvqVaryingOut, EvmNone, $1.range);
        } else if (parseContext.language == EShLangGeometry) {
            $$.setBasic(EbtVoid, EvqVaryingOut, EvmNone, $1.range);
        } else {
            $$.setBasic(EbtVoid, EvqVaryingIn, EvmNone, $1.range);
        }

        $$.range = $1.range;
    }
    | UNIFORM {
        if (parseContext.globalErrorCheck($1.range, parseContext.symbolTable.atGlobalLevel(), "uniform"))
            parseContext.recover(__FILE__, __LINE__);
        $$.setBasic(EbtVoid, EvqUniform, EvmNone, $1.range);
        $$.range = $1.range;
    }
    ;

varying_modifier
    : INVARIANT {
        $$ = EvmInvariant;
    }
    | CENTROID {
        $$ = EvmCentroid;
    }
    | FLAT {
        $$ = EvmFlat;
    }
    | NOPERSPECTIVE {
        $$ = EvmNoperspective;
    }
    | varying_modifier INVARIANT {
        $$ |= EvmInvariant;
    }
    | varying_modifier CENTROID {
        if ($1 & EvmFlat) {
            parseContext.error($2.range, "varying flat cannot be used with centroid qualifier:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        $$ |= EvmCentroid;
    }
    | varying_modifier FLAT {
        if (parseContext.extensionErrorCheck($2.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        if ($1 & EvmFlat || $1 & EvmNoperspective) {
            parseContext.error($2.range, "varying flat cannot be used with other qualifiers:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        $$ |= EvmFlat;
    }
    | varying_modifier NOPERSPECTIVE {
        if (parseContext.extensionErrorCheck($2.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        if ($1 & EvmFlat) {
            parseContext.error($2.range, "varying flat cannot be used with noperspective qualifier:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        $$ |= EvmNoperspective;
    }
    ;

type_specifier
    : type_specifier_nonarray {
        $$ = $1;
    }
    | type_specifier_nonarray LEFT_BRACKET RIGHT_BRACKET {
        $$ = $1;
        
        if (parseContext.arrayTypeErrorCheck($2.range, $1)) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            $$.setArray(true, 0);
        }
        $$.range = addRange($1.range, $3.range);
    }
    | type_specifier_nonarray LEFT_BRACKET constant_expression RIGHT_BRACKET {
        $$ = $1;
        
        if (parseContext.arrayTypeErrorCheck($2.range, $1)) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int size;
            if (parseContext.arraySizeErrorCheck($2.range, $3, size)) {
                parseContext.recover(__FILE__, __LINE__);
            }
            $$.setArray(true, size);
        }
        $$.range = addRange($1.range, $4.range);
    }
    ;

type_specifier_nonarray
    : VOID_TYPE {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtVoid, qual, EvmNone, $1.range); 
        $$.range = $1.range;
    }
    | FLOAT_TYPE {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | INT_TYPE {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtInt, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | BOOL_TYPE {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtBool, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | UNSIGNED INT_TYPE { 
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUInt, qual, EvmNone, $1.range); 
        $$.range = $1.range;
    }
    | VEC2 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setAggregate(2);
        $$.range = $1.range;
    }
    | VEC3 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setAggregate(3);
        $$.range = $1.range;
    }
    | VEC4 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setAggregate(4);
        $$.range = $1.range;
    }
    | BVEC2 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtBool, qual, EvmNone, $1.range);
        $$.setAggregate(2);
        $$.range = $1.range;
    }
    | BVEC3 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtBool, qual, EvmNone, $1.range);
        $$.setAggregate(3);
        $$.range = $1.range;
    }
    | BVEC4 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtBool, qual, EvmNone, $1.range);
        $$.setAggregate(4);
        $$.range = $1.range;
    }
    | IVEC2 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtInt, qual, EvmNone, $1.range);
        $$.setAggregate(2);
        $$.range = $1.range;
    }
    | IVEC3 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtInt, qual, EvmNone, $1.range);
        $$.setAggregate(3);
        $$.range = $1.range;
    }
    | IVEC4 {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtInt, qual, EvmNone, $1.range);
        $$.setAggregate(4);
        $$.range = $1.range;
    }
    | UVEC2 {
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUInt, qual, EvmNone, $1.range);
        $$.setAggregate(2);
        $$.range = $1.range;
    }
    | UVEC3 {
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUInt, qual, EvmNone, $1.range);
        $$.setAggregate(3);
        $$.range = $1.range;
    }
    | UVEC4 {
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUInt, qual, EvmNone, $1.range);
        $$.setAggregate(4);
        $$.range = $1.range;
    }
    | MATRIX2 {
        FRAG_VERT_GEOM_ONLY("mat2", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(2, 2);
        $$.range = $1.range;
    }
    | MATRIX3 { 
        FRAG_VERT_GEOM_ONLY("mat3", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(3, 3);
        $$.range = $1.range;
    }
    | MATRIX4 { 
        FRAG_VERT_GEOM_ONLY("mat4", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(4, 4);
        $$.range = $1.range;
    }
    | MATRIX2X2 {
        FRAG_VERT_GEOM_ONLY("mat2x2", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(2, 2);
        $$.range = $1.range;
    }
    | MATRIX2X3 {
        FRAG_VERT_GEOM_ONLY("mat2x3", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(2, 3);
        $$.range = $1.range;
    }
    | MATRIX2X4 {
        FRAG_VERT_GEOM_ONLY("mat2x4", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(2, 4);
        $$.range = $1.range;
    }
    | MATRIX3X2 {
        FRAG_VERT_GEOM_ONLY("mat3x2", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(3, 2);
        $$.range = $1.range;
    }
    | MATRIX3X3 {
        FRAG_VERT_GEOM_ONLY("mat3x3", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(3, 3);
        $$.range = $1.range;
    }
    | MATRIX3X4 {
        FRAG_VERT_GEOM_ONLY("mat3x4", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(3, 4);
        $$.range = $1.range;
    }
    | MATRIX4X2 {
        FRAG_VERT_GEOM_ONLY("mat4x2", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(4, 2);
        $$.range = $1.range;
    }
    | MATRIX4X3 {
        FRAG_VERT_GEOM_ONLY("mat4x3", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(4, 3);
        $$.range = $1.range;
    }
    | MATRIX4X4 {
        FRAG_VERT_GEOM_ONLY("mat4x4", $1.range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtFloat, qual, EvmNone, $1.range);
        $$.setMatrix(4, 4);
        $$.range = $1.range;
    }
    | SAMPLER1D {
        FRAG_VERT_GEOM_ONLY("sampler1D", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler1D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLER1D {
        FRAG_VERT_GEOM_ONLY("isampler1D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler1D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLER1D {
        FRAG_VERT_GEOM_ONLY("usampler1D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler1D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER2D {
        FRAG_VERT_GEOM_ONLY("sampler2D", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLER2D {
        FRAG_VERT_GEOM_ONLY("isampler2D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler2D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLER2D {
        FRAG_VERT_GEOM_ONLY("usampler2D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler2D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER3D {
        FRAG_VERT_GEOM_ONLY("sampler3D", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler3D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLER3D {
        FRAG_VERT_GEOM_ONLY("isampler3D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler3D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLER3D {
        FRAG_VERT_GEOM_ONLY("usampler3D", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler3D, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLERCUBE {
        FRAG_VERT_GEOM_ONLY("samplerCube", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSamplerCube, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLERCUBE {
        FRAG_VERT_GEOM_ONLY("isamplerCube", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISamplerCube, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLERCUBE {
        FRAG_VERT_GEOM_ONLY("usamplerCube", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSamplerCube, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER1DSHADOW {
        FRAG_VERT_GEOM_ONLY("sampler1DShadow", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler1DShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER2DSHADOW {
        FRAG_VERT_GEOM_ONLY("sampler2DShadow", $1.range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2DShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER2DRECTARB {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("sampler2DRectARB", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_ARB_texture_rectangle"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2DRect, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | ISAMPLER2DRECT {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("isampler2DRectARB", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler2DRect, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | USAMPLER2DRECT {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("usampler2DRectARB", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler2DRect, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | SAMPLER2DRECTSHADOWARB {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("sampler2DRectShadowARB", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_ARB_texture_rectangle"))
            parseContext.recover(__FILE__, __LINE__);

        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2DRectShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    }
    | SAMPLER1DARRAY {
        FRAG_VERT_GEOM_ONLY("sampler1DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler1DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLER1DARRAY {
        FRAG_VERT_GEOM_ONLY("isampler1DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler1DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLER1DARRAY {
        FRAG_VERT_GEOM_ONLY("usampler1DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler1DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER2DARRAY {
        FRAG_VERT_GEOM_ONLY("sampler2DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLER2DARRAY {
        FRAG_VERT_GEOM_ONLY("isampler2DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISampler2DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLER2DARRAY {
        FRAG_VERT_GEOM_ONLY("usampler2DArray", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSampler2DArray, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLERBUFFER {
        FRAG_VERT_GEOM_ONLY("samplerBuffer", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSamplerBuffer, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | ISAMPLERBUFFER {
        FRAG_VERT_GEOM_ONLY("isamplerBuffer", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtISamplerBuffer, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | USAMPLERBUFFER {
        FRAG_VERT_GEOM_ONLY("usamplerBuffer", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtUSamplerBuffer, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER1DARRAYSHADOW {
        FRAG_VERT_GEOM_ONLY("sampler1DArrayShadow", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler1DArrayShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLER2DARRAYSHADOW {
        FRAG_VERT_GEOM_ONLY("sampler2DArrayShadow", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSampler2DArrayShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | SAMPLERCUBESHADOW {
        FRAG_VERT_GEOM_ONLY("samplerCubeShadow", $1.range);
        if (parseContext.extensionErrorCheck($1.range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtSamplerCubeShadow, qual, EvmNone, $1.range);
        $$.range = $1.range;
    } 
    | struct_specifier {
        FRAG_VERT_GEOM_ONLY("struct", $1.range);
        $$ = $1;
        $$.qualifier = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        if ($$.userDef) {
            $$.userDef->setSpecified(true);
        }
        $$.range = $1.range;
    }
    | TYPE_NAME {     
        //
        // This is for user defined type names.  The lexical phase looked up the 
        // type.
        //
        TType& structure = static_cast<TVariable*>($1.symbol)->getType();
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        $$.setBasic(EbtStruct, qual, EvmNone, $1.range);
        $$.userDef = &structure;
        if ($$.userDef) {
            $$.userDef->setSpecified(false);
        }
        $$.range = $1.range;
    }
    ;

struct_specifier
    : STRUCT IDENTIFIER LEFT_BRACE struct_declaration_list RIGHT_BRACE {
        
        TType* structure = new TType($4, *$2.string);
        TVariable* userTypeDef = new TVariable($2.string, *structure, true);
        if (! parseContext.symbolTable.insert(*userTypeDef)) {
            parseContext.error($2.range, "redefinition", $2.string->c_str(), "struct");
            parseContext.recover(__FILE__, __LINE__);
        }

        $$.setBasic(EbtStruct, EvqTemporary, EvmNone, $1.range);
        $$.userDef = structure;
        $$.range = addRange($1.range, $5.range);
    }
    | STRUCT LEFT_BRACE struct_declaration_list RIGHT_BRACE {
        TType* structure = new TType($3, TString(""));
        
        $$.setBasic(EbtStruct, EvqTemporary, EvmNone, $1.range);
        $$.userDef = structure;
        $$.range = addRange($1.range, $4.range);
    }
    ;
    
struct_declaration_list
    : struct_declaration {
        $$ = $1;
    }
    | struct_declaration_list struct_declaration {
        $$ = $1;
        for (unsigned int i = 0; i < $2->size(); ++i) {
            for (unsigned int j = 0; j < $$->size(); ++j) {
                if ((*$$)[j].type->getFieldName() == (*$2)[i].type->getFieldName()) {
                    parseContext.error((*$2)[i].range, "duplicate field name in structure:", "struct", (*$2)[i].type->getFieldName().c_str());
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
            $$->push_back((*$2)[i]);
        }
    }
    ;
    
struct_declaration
    : type_specifier struct_declarator_list SEMICOLON {
        $$ = $2;

        if (parseContext.arraySizeUnspecifiedErrorCheck($1.range, $1)) {
            parseContext.error($1.range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            $1.setArray(false);
        }
        if (parseContext.voidErrorCheck($1.range, (*$2)[0].type->getFieldName(), $1)) {
            parseContext.recover(__FILE__, __LINE__);
        }
        for (unsigned int i = 0; i < $$->size(); ++i) {
            //
            // Careful not to replace already know aspects of type, like array-ness
            //
            (*$$)[i].type->setType($1.type, $1.size, $1.matrixSize[0], $1.matrixSize[1], $1.matrix, $1.userDef);

            // don't allow arrays of arrays
            if ((*$$)[i].type->isArray()) {
                if (parseContext.arrayTypeErrorCheck($1.range, $1))
                    parseContext.recover(__FILE__, __LINE__);
            }
            if ($1.array)
                (*$$)[i].type->setArraySize($1.arraySize[0]);
            if ($1.userDef)
                (*$$)[i].type->setTypeName($1.userDef->getTypeName());

            if ( i == 0 &&
                 $1.type == EbtStruct &&
                 $1.userDef != 0 && 
                 $1.userDef->isSpecified() == true ) {

                    (*$$)[i].type->setSpecified(true);
            }
        }
    }
    ;
        
struct_declarator_list
    : struct_declarator {
        $$ = NewPoolTTypeList();
        $$->push_back($1);
    }
    | struct_declarator_list COMMA struct_declarator {
        $$->push_back($3);
    }
    ;
    
struct_declarator
    : IDENTIFIER {
        $$.type = new TType(EbtVoid);
        $$.type->setFieldName(*$1.string);
        $$.range = $1.range;
    }
    | IDENTIFIER LEFT_BRACKET constant_expression RIGHT_BRACKET {
        $$.type = new TType(EbtVoid);
        $$.type->setFieldName(*$1.string);
        
        int size;
        if (parseContext.arraySizeErrorCheck($2.range, $3, size))
            parseContext.recover(__FILE__, __LINE__);
        $$.type->setArraySize(size);
        $$.range = addRange($1.range, $4.range);
    }
    ;

initializer
    : assignment_expression { $$ = $1; }
    ;

declaration_statement 
    : declaration { $$ = $1; }
    ;

statement
    : compound_statement  { $$ = $1; }
    | simple_statement    { $$ = $1; }
    ;

// Grammar Note:  No labeled statements; 'goto' is not supported.

simple_statement 
    : declaration_statement { $$ = $1; }
    | expression_statement  { $$ = $1; }
    | selection_statement   { $$ = $1; }
    | switch_statement      { $$ = $1; }
    | case_label            { $$ = $1; }
    | iteration_statement   { $$ = $1; }
    | jump_statement        { $$ = $1; }
    ;

compound_statement
    : LEFT_BRACE RIGHT_BRACE { 
        $$ = 0;
    }
    | LEFT_BRACE { parseContext.symbolTable.push(); } statement_list { parseContext.symbolTable.pop(); } RIGHT_BRACE {
        if ($3 != 0)            
            $3->setOperator(EOpSequence); 
        $$ = $3;
        if ($$) $$->setRange(addRange($1.range, $5.range));
    }
    ;

statement_no_new_scope 
    : compound_statement_no_new_scope { $$ = $1; }
    | simple_statement                { $$ = $1; }
    ;

compound_statement_no_new_scope 
    // Statement that doesn't create a new scope, for selection_statement, iteration_statement 
    : LEFT_BRACE RIGHT_BRACE { 
        $$ = 0; 
    }
    | LEFT_BRACE statement_list RIGHT_BRACE { 
        if ($2)
            $2->setOperator(EOpSequence); 
        $$ = $2; 
        if ($$) $$->setRange(addRange($1.range, $3.range));
    }
    ;

statement_list
    : statement {
        $$ = parseContext.intermediate.makeAggregate($1, parseContext.extensionChanged);
        if ($$ && $1) $$->setRange($1->getRange());
    }
    | statement_list statement { 
        $$ = parseContext.intermediate.growAggregate($1, $2, parseContext.extensionChanged);
        if ($$ && $1 && $2) $$->setRange(addRange($1->getRange(), $2->getRange()));
    }
    ;

expression_statement
    : SEMICOLON  { $$ = 0; }
    | expression SEMICOLON  { $$ = static_cast<TIntermNode*>($1); }
    ;

selection_statement
    : IF LEFT_PAREN expression RIGHT_PAREN selection_rest_statement { 
        if (parseContext.boolErrorCheck($1.range, $3))
            parseContext.recover(__FILE__, __LINE__);
        $$ = parseContext.intermediate.addSelection($3, $5, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $4.range));
    }
    ;

selection_rest_statement 
    : statement ELSE statement {
        $$.node1 = $1;
        $$.node2 = $3;
    }
    | statement { 
        $$.node1 = $1;
        $$.node2 = 0;
    }
    ;

// Grammar Note: labeled statements for SWITCH only; 'goto' is not supported

condition
    // In 1996 c++ draft, conditions can include single declarations 
    : expression {
        $$ = $1;
        if (parseContext.boolErrorCheck($1->getRange(), $1))
            parseContext.recover(__FILE__, __LINE__);          
        if ($$ && $1) $$->setRange($1->getRange());
    }
    | fully_specified_type IDENTIFIER EQUAL initializer {
        TIntermNode* intermNode;
        if (parseContext.structQualifierErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.boolErrorCheck($2.range, $1))
            parseContext.recover(__FILE__, __LINE__);
        
        if (!parseContext.executeInitializer($2.range, *$2.string, $1, $4, intermNode))
            $$ = $4;
        else {
            parseContext.recover(__FILE__, __LINE__);
            $$ = 0;
        }
        if ($$ && $4) $$->setRange(addRange($1.range, $4->getRange()));
    }
    ;

switch_statement
    : SWITCH LEFT_PAREN { parseContext.symbolTable.push(); ++parseContext.switchNestingLevel; } expression RIGHT_PAREN LEFT_BRACE switch_statement_list RIGHT_BRACE {
        parseContext.symbolTable.pop();
        $$ = parseContext.intermediate.addSwitch($4, $7, $2.range, parseContext.extensionChanged);
        --parseContext.switchNestingLevel;
        if ($$) $$->setRange(addRange($2.range, $5.range));
    }
    ;

switch_statement_list
    : /* empty */ {
        $$ = 0;
    }
    | statement_list {
        $$ = $1;
        if ($$) $$->getAsAggregate()->setOperator(EOpSequence);
    }
    ;

case_label
    : CASE expression COLON {
        $$ = parseContext.intermediate.addCase($2, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $3.range));
    }
    | DEFAULT COLON {
        $$ = parseContext.intermediate.addCase(NULL, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    ;

iteration_statement
    : WHILE LEFT_PAREN { parseContext.symbolTable.push(); ++parseContext.loopNestingLevel; } condition RIGHT_PAREN statement_no_new_scope { 
        parseContext.symbolTable.pop();
        $$ = parseContext.intermediate.addLoop($6, NULL, $4, 0, LOOP_WHILE, $1.range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ($$) $$->setRange(addRange($1.range, $5.range));
    }
    | DO { ++parseContext.loopNestingLevel; } statement WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON {
        if (parseContext.boolErrorCheck($8.range, $6))
            parseContext.recover(__FILE__, __LINE__);
                    
        $$ = parseContext.intermediate.addLoop($3, NULL, $6, 0, LOOP_DO, $4.range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ($$) $$->setRange(addRange($4.range, $7.range));
    }
    | FOR LEFT_PAREN { parseContext.symbolTable.push(); ++parseContext.loopNestingLevel; } for_init_statement for_rest_statement RIGHT_PAREN statement_no_new_scope {
        parseContext.symbolTable.pop();
        /*
        $$ = parseContext.intermediate.makeAggregate($4, $2.range);
        $$ = parseContext.intermediate.growAggregate(
                $$,
                parseContext.intermediate.addLoop($7, reinterpret_cast<TIntermTyped*>($5.node1), reinterpret_cast<TIntermTyped*>($5.node2), true, $1.range),
                $1.range);
        $$->getAsAggregate()->setOperator(EOpSequence);
        */
        
        $$ = parseContext.intermediate.addLoop($7, $4, reinterpret_cast<TIntermTyped*>($5.node1), reinterpret_cast<TIntermTyped*>($5.node2), LOOP_FOR, $1.range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ($$) $$->setRange(addRange($1.range, $6.range));
    }
    ;

for_init_statement 
    : expression_statement {
        $$ = $1; 
    } 
    | declaration_statement {
        $$ = $1;
    }
    ;

conditionopt 
    : condition { 
        $$ = $1; 
    }
    | /* May be null */ { 
        $$ = 0; 
    }
    ;

for_rest_statement 
    : conditionopt SEMICOLON { 
        $$.node1 = $1;
        $$.node2 = 0;
    }
    | conditionopt SEMICOLON expression  {
        $$.node1 = $1;
        $$.node2 = $3;
    }
    ;

jump_statement
    : CONTINUE SEMICOLON {
        if (parseContext.loopNestingLevel <= 0) {
            parseContext.error($1.range, "continue statement only allowed in loops", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }        
        $$ = parseContext.intermediate.addBranch(EOpContinue, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    | BREAK SEMICOLON {
        if (parseContext.loopNestingLevel <= 0 &&
            parseContext.switchNestingLevel <= 0) {
            parseContext.error($1.range, "break statement only allowed in loops and switches", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }        
        $$ = parseContext.intermediate.addBranch(EOpBreak, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    | RETURN SEMICOLON {
        $$ = parseContext.intermediate.addBranch(EOpReturn, $1.range, parseContext.extensionChanged);
        if (parseContext.currentFunctionType->getBasicType() != EbtVoid) {
            parseContext.error($1.range, "non-void function must return a value", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }
    | RETURN expression SEMICOLON {        
        $$ = parseContext.intermediate.addBranch(EOpReturn, $2, $1.range, parseContext.extensionChanged);
        parseContext.functionReturnsValue = true;
        if (parseContext.currentFunctionType->getBasicType() == EbtVoid) {
            parseContext.error($1.range, "void function cannot return a value", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        } else if (*(parseContext.currentFunctionType) != $2->getType()) {
            parseContext.error($1.range, "function return is not matching type:", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ($$) $$->setRange(addRange($1.range, $3.range));
    }
    | DISCARD SEMICOLON {
        FRAG_ONLY("discard", $1.range);
        $$ = parseContext.intermediate.addBranch(EOpKill, $1.range, parseContext.extensionChanged);
        if ($$) $$->setRange(addRange($1.range, $2.range));
    }        
    ;

// Grammar Note:  No 'goto'.  Gotos are not supported.

translation_unit
    : external_declaration { 
        $$ = $1;
        parseContext.treeRoot = $$; 
    }
    | translation_unit external_declaration {
        $$ = parseContext.intermediate.growAggregate($1, $2, parseContext.extensionChanged);
        if ($$ && $1 && $2) $$->setRange(addRange($1->getRange(), $2->getRange()));
        parseContext.treeRoot = $$;
    }
    ;

external_declaration
    : function_definition { 
        $$ = $1; 
    }
    | declaration { 
        $$ = $1; 
    }
    ;

function_definition
    : function_prototype {
        TFunction& function = *($1.function);
        TFunction* prevDec = static_cast<TFunction*>(parseContext.symbolTable.find(function.getMangledName()));
        //
        // Note:  'prevDec' could be 'function' if this is the first time we've seen function
        // as it would have just been put in the symbol table.  Otherwise, we're looking up
        // an earlier occurance.
        //
        if (prevDec->isDefined()) {
            //
            // Then this function already has a body.
            //
            parseContext.error($1.range, "function already has a body", function.getName().c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        prevDec->setDefined();
        
        //
        // Raise error message if main function takes any parameters or return anything other than void
        //
        if (function.getName() == "main") {
            if (function.getParamCount() > 0) {
                parseContext.error($1.range, "function cannot take any parameter(s)", function.getName().c_str(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            if (function.getReturnType().getBasicType() != EbtVoid) {
                parseContext.error($1.range, "", function.getReturnType().getBasicString(), "main function cannot return a value");
                parseContext.recover(__FILE__, __LINE__);
            }            
        }
   
        //
        // New symbol table scope for body of function plus its arguments
        //
        parseContext.symbolTable.push();
        
        //
        // Remember the return type for later checking for RETURN statements.
        //
        parseContext.currentFunctionType = &(prevDec->getReturnType());
        parseContext.functionReturnsValue = false;
        
        // 
        // Insert parameters into the symbol table.
        // If the parameter has no name, it's not an error, just don't insert it 
        // (could be used for unused args).
        //
        // Also, accumulate the list of parameters into the HIL, so lower level code
        // knows where to find parameters.
        //
        TIntermAggregate* paramNodes = new TIntermAggregate;
        for (int i = 0; i < function.getParamCount(); i++) {
            TParameter& param = function[i];
            if (param.name != 0) {
                TVariable *variable = new TVariable(param.name, *param.type);
                // 
                // Insert the parameters with name in the symbol table.
                //
                if (! parseContext.symbolTable.insert(*variable)) {
                    parseContext.error($1.range, "redefinition", variable->getName().c_str(), "");
                    parseContext.recover(__FILE__, __LINE__);
                    delete variable;
                }
                //
                // Transfer ownership of name pointer to symbol table.
                //
                param.name = 0;
                
                //
                // Add the parameter to the HIL
                //                
                paramNodes = parseContext.intermediate.growAggregate(
                        paramNodes, 
                        parseContext.intermediate.addFuncParam(variable->getUniqueId(),
                                                               variable->getName(),
                                                               variable->getType(), $1.range,
                                                               parseContext.extensionChanged),
                        parseContext.extensionChanged);
            } else {
                paramNodes = parseContext.intermediate.growAggregate(
                        paramNodes, 
                        parseContext.intermediate.addFuncParam(0, "", *param.type, $1.range,
                                                               parseContext.extensionChanged),
                        parseContext.extensionChanged);
            }
        }
        parseContext.intermediate.setAggregateOperator(paramNodes, EOpParameters, $1.range, parseContext.extensionChanged);
        $1.intermAggregate = paramNodes;
        parseContext.loopNestingLevel = 0;
    }
    compound_statement_no_new_scope {
        //?? Check that all paths return a value if return type != void ?
        //   May be best done as post process phase on intermediate code
        if (parseContext.currentFunctionType->getBasicType() != EbtVoid && ! parseContext.functionReturnsValue) {
            parseContext.error($1.range, "function does not return a value:", "", $1.function->getName().c_str());
            parseContext.recover(__FILE__, __LINE__);
        }
        parseContext.symbolTable.pop();
        

        TIntermNode *body = NULL;
        // Add dummy node at the end of 'main' body for debugging the last statement
        if ($1.function->getName() == "main") {
            if ($3 != 0) {
                if ($3->getAsAggregate()) {
                    // Add dummy node to end of aggregate
                    TIntermDummy *dummy = parseContext.intermediate.addDummy($3->getRange(), parseContext.extensionChanged);
                    $3->getAsAggregate()->setOperator(EOpNull);
                    body = parseContext.intermediate.growAggregate($3->getAsAggregate(), dummy, parseContext.extensionChanged);
                    body->getAsAggregate()->setOperator(EOpSequence);
                } else {
                    // UNTESTED
                    TIntermDummy *dummy = parseContext.intermediate.addDummy($3->getRange(), parseContext.extensionChanged);
                    body = parseContext.intermediate.growAggregate($3, dummy, parseContext.extensionChanged);
                    body->getAsAggregate()->setOperator(EOpSequence);
                }
            } else {
                // UNTESTED
                TIntermDummy *dummy = parseContext.intermediate.addDummy($3->getRange(), parseContext.extensionChanged);
                body = parseContext.intermediate.growAggregate($3, dummy, parseContext.extensionChanged);
                body->getAsAggregate()->setOperator(EOpSequence);
            }
        } else {
            if ($3) {
                body = $3;
            } else {
                body = new TIntermAggregate;
                body->getAsAggregate()->setOperator(EOpSequence);
            }
        }
        
        $$ = parseContext.intermediate.growAggregate($1.intermAggregate, body, parseContext.extensionChanged);
        parseContext.intermediate.setAggregateOperator($$, EOpFunction, $1.range, parseContext.extensionChanged);
        $$->getAsAggregate()->setName($1.function->getMangledName().c_str());
        $$->getAsAggregate()->setType($1.function->getReturnType());
        
        // store the pragma information for debug and optimize and other vendor specific 
        // information. This information can be queried from the parse tree
        $$->getAsAggregate()->setOptimize(parseContext.contextPragma.optimize);
        $$->getAsAggregate()->setDebug(parseContext.contextPragma.debug);
        $$->getAsAggregate()->addToPragmaTable(parseContext.contextPragma.pragmaTable);
        if ($$ && body) $$->setRange(addRange($1.range, body->getRange()));
    }
    ;

%%
