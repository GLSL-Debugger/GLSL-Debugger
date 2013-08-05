/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_MEDIA_SDA9_PROJECTS_GLSL_DEBUGGER_GLSLCOMPILER_GLSLANG_MACHINEINDEPENDENT_GLSLANG_TAB_H_INCLUDED
# define YY_YY_MEDIA_SDA9_PROJECTS_GLSL_DEBUGGER_GLSLCOMPILER_GLSLANG_MACHINEINDEPENDENT_GLSLANG_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ATTRIBUTE = 258,
     CONST_QUAL = 259,
     BOOL_TYPE = 260,
     FLOAT_TYPE = 261,
     INT_TYPE = 262,
     INVARIANT = 263,
     BREAK = 264,
     CONTINUE = 265,
     DO = 266,
     ELSE = 267,
     FOR = 268,
     IF = 269,
     DISCARD = 270,
     RETURN = 271,
     SWITCH = 272,
     CASE = 273,
     DEFAULT = 274,
     BVEC2 = 275,
     BVEC3 = 276,
     BVEC4 = 277,
     IVEC2 = 278,
     IVEC3 = 279,
     IVEC4 = 280,
     VEC2 = 281,
     VEC3 = 282,
     VEC4 = 283,
     MATRIX2 = 284,
     MATRIX3 = 285,
     MATRIX4 = 286,
     MATRIX2X2 = 287,
     MATRIX3X2 = 288,
     MATRIX4X2 = 289,
     MATRIX2X3 = 290,
     MATRIX3X3 = 291,
     MATRIX4X3 = 292,
     MATRIX2X4 = 293,
     MATRIX3X4 = 294,
     MATRIX4X4 = 295,
     IN_QUAL = 296,
     OUT_QUAL = 297,
     INOUT_QUAL = 298,
     UNIFORM = 299,
     VARYING = 300,
     STRUCT = 301,
     VOID_TYPE = 302,
     WHILE = 303,
     SAMPLER1D = 304,
     SAMPLER2D = 305,
     SAMPLER3D = 306,
     SAMPLERCUBE = 307,
     SAMPLER1DSHADOW = 308,
     SAMPLER2DSHADOW = 309,
     SAMPLER2DRECTARB = 310,
     SAMPLER2DRECTSHADOWARB = 311,
     IDENTIFIER = 312,
     TYPE_NAME = 313,
     FLOATCONSTANT = 314,
     INTCONSTANT = 315,
     BOOLCONSTANT = 316,
     FIELD_SELECTION = 317,
     LEFT_OP = 318,
     RIGHT_OP = 319,
     INC_OP = 320,
     DEC_OP = 321,
     LE_OP = 322,
     GE_OP = 323,
     EQ_OP = 324,
     NE_OP = 325,
     AND_OP = 326,
     OR_OP = 327,
     XOR_OP = 328,
     MUL_ASSIGN = 329,
     DIV_ASSIGN = 330,
     ADD_ASSIGN = 331,
     MOD_ASSIGN = 332,
     LEFT_ASSIGN = 333,
     RIGHT_ASSIGN = 334,
     AND_ASSIGN = 335,
     XOR_ASSIGN = 336,
     OR_ASSIGN = 337,
     SUB_ASSIGN = 338,
     LEFT_PAREN = 339,
     RIGHT_PAREN = 340,
     LEFT_BRACKET = 341,
     RIGHT_BRACKET = 342,
     LEFT_BRACE = 343,
     RIGHT_BRACE = 344,
     DOT = 345,
     COMMA = 346,
     COLON = 347,
     EQUAL = 348,
     SEMICOLON = 349,
     BANG = 350,
     DASH = 351,
     TILDE = 352,
     PLUS = 353,
     STAR = 354,
     SLASH = 355,
     PERCENT = 356,
     LEFT_ANGLE = 357,
     RIGHT_ANGLE = 358,
     VERTICAL_BAR = 359,
     CARET = 360,
     AMPERSAND = 361,
     QUESTION = 362,
     UINTCONSTANT = 363,
     UNSIGNED = 364,
     NOPERSPECTIVE = 365,
     FLAT = 366,
     CENTROID = 367,
     UVEC2 = 368,
     UVEC3 = 369,
     UVEC4 = 370,
     SAMPLER1DARRAY = 371,
     SAMPLER2DARRAY = 372,
     SAMPLER1DARRAYSHADOW = 373,
     SAMPLER2DARRAYSHADOW = 374,
     SAMPLERCUBESHADOW = 375,
     ISAMPLER1D = 376,
     ISAMPLER2D = 377,
     ISAMPLER3D = 378,
     ISAMPLERCUBE = 379,
     ISAMPLER2DRECT = 380,
     ISAMPLER1DARRAY = 381,
     ISAMPLER2DARRAY = 382,
     USAMPLER1D = 383,
     USAMPLER2D = 384,
     USAMPLER3D = 385,
     USAMPLERCUBE = 386,
     USAMPLER2DRECT = 387,
     USAMPLER1DARRAY = 388,
     USAMPLER2DARRAY = 389,
     SAMPLERBUFFER = 390,
     ISAMPLERBUFFER = 391,
     USAMPLERBUFFER = 392
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2053 of yacc.c  */
#line 155 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"

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


/* Line 2053 of yacc.c  */
#line 228 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_MEDIA_SDA9_PROJECTS_GLSL_DEBUGGER_GLSLCOMPILER_GLSLANG_MACHINEINDEPENDENT_GLSLANG_TAB_H_INCLUDED  */
