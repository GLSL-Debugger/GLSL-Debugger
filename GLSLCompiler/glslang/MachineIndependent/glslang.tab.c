/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7.12-4996"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 39 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"


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

/* Line 371 of yacc.c  */
#line 185 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "glslang.tab.h".  */
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
/* Line 387 of yacc.c  */
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


/* Line 387 of yacc.c  */
#line 399 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.tab.c"
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

/* Copy the second part of user declarations.  */
/* Line 390 of yacc.c  */
#line 188 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"

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


/* Line 390 of yacc.c  */
#line 497 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif


/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  113
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2930

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  83
/* YYNRULES -- Number of rules.  */
#define YYNRULES  268
/* YYNRULES -- Number of states.  */
#define YYNSTATES  392

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   392

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    19,
      21,    26,    28,    32,    35,    38,    40,    42,    44,    48,
      51,    54,    57,    59,    62,    66,    69,    71,    73,    75,
      77,    80,    83,    86,    88,    90,    92,    94,    96,   100,
     104,   108,   110,   114,   118,   120,   124,   128,   130,   134,
     138,   142,   146,   148,   152,   156,   158,   162,   164,   168,
     170,   174,   176,   180,   182,   186,   188,   192,   194,   200,
     202,   206,   208,   210,   212,   214,   216,   218,   220,   222,
     224,   226,   228,   230,   234,   236,   239,   242,   245,   247,
     249,   252,   256,   260,   263,   269,   273,   276,   280,   283,
     284,   286,   288,   290,   292,   295,   299,   303,   308,   310,
     314,   319,   327,   336,   342,   344,   347,   351,   358,   366,
     371,   374,   376,   380,   381,   383,   385,   387,   389,   392,
     394,   396,   398,   400,   402,   404,   407,   410,   413,   416,
     418,   422,   427,   429,   431,   433,   435,   438,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   492,   494,   496,   498,   500,   502,
     504,   506,   508,   510,   512,   514,   516,   518,   520,   522,
     524,   526,   528,   530,   532,   534,   536,   538,   540,   542,
     544,   546,   548,   550,   556,   561,   563,   566,   570,   572,
     576,   578,   583,   585,   587,   589,   591,   593,   595,   597,
     599,   601,   603,   605,   608,   609,   610,   616,   618,   620,
     623,   627,   629,   632,   634,   637,   643,   647,   649,   651,
     656,   657,   666,   667,   669,   673,   676,   677,   684,   685,
     694,   695,   703,   705,   707,   709,   710,   713,   717,   720,
     723,   726,   730,   733,   735,   738,   740,   742,   743
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     217,     0,    -1,    57,    -1,   139,    -1,    60,    -1,   108,
      -1,    59,    -1,    61,    -1,    84,   166,    85,    -1,   140,
      -1,   141,    86,   142,    87,    -1,   143,    -1,   141,    90,
      62,    -1,   141,    65,    -1,   141,    66,    -1,   166,    -1,
     144,    -1,   145,    -1,   141,    90,   145,    -1,   147,    85,
      -1,   146,    85,    -1,   148,    47,    -1,   148,    -1,   148,
     164,    -1,   147,    91,   164,    -1,   149,    84,    -1,   184,
      -1,    57,    -1,    62,    -1,   141,    -1,    65,   150,    -1,
      66,   150,    -1,   151,   150,    -1,    98,    -1,    96,    -1,
      95,    -1,    97,    -1,   150,    -1,   152,    99,   150,    -1,
     152,   100,   150,    -1,   152,   101,   150,    -1,   152,    -1,
     153,    98,   152,    -1,   153,    96,   152,    -1,   153,    -1,
     154,    63,   153,    -1,   154,    64,   153,    -1,   154,    -1,
     155,   102,   154,    -1,   155,   103,   154,    -1,   155,    67,
     154,    -1,   155,    68,   154,    -1,   155,    -1,   156,    69,
     155,    -1,   156,    70,   155,    -1,   156,    -1,   157,   106,
     156,    -1,   157,    -1,   158,   105,   157,    -1,   158,    -1,
     159,   104,   158,    -1,   159,    -1,   160,    71,   159,    -1,
     160,    -1,   161,    73,   160,    -1,   161,    -1,   162,    72,
     161,    -1,   162,    -1,   162,   107,   166,    92,   164,    -1,
     163,    -1,   150,   165,   164,    -1,    93,    -1,    74,    -1,
      75,    -1,    77,    -1,    76,    -1,    83,    -1,    78,    -1,
      79,    -1,    80,    -1,    81,    -1,    82,    -1,   164,    -1,
     166,    91,   164,    -1,   163,    -1,   169,    94,    -1,   178,
      94,    -1,   170,    85,    -1,   172,    -1,   171,    -1,   172,
     174,    -1,   171,    91,   174,    -1,   180,    57,    84,    -1,
     184,    57,    -1,   184,    57,    86,   167,    87,    -1,   182,
     175,   173,    -1,   175,   173,    -1,   182,   175,   176,    -1,
     175,   176,    -1,    -1,    41,    -1,    42,    -1,    43,    -1,
     184,    -1,    86,    87,    -1,    86,   167,    87,    -1,    86,
      87,   177,    -1,    86,   167,    87,   177,    -1,   179,    -1,
     178,    91,    57,    -1,   178,    91,    57,   177,    -1,   178,
      91,    57,    86,    87,    93,   191,    -1,   178,    91,    57,
      86,   167,    87,    93,   191,    -1,   178,    91,    57,    93,
     191,    -1,   180,    -1,   180,    57,    -1,   180,    57,   177,
      -1,   180,    57,    86,    87,    93,   191,    -1,   180,    57,
      86,   167,    87,    93,   191,    -1,   180,    57,    93,   191,
      -1,     8,    57,    -1,   184,    -1,   182,   181,   184,    -1,
      -1,    41,    -1,    42,    -1,     4,    -1,     3,    -1,   183,
      45,    -1,    45,    -1,    44,    -1,     8,    -1,   112,    -1,
     111,    -1,   110,    -1,   183,     8,    -1,   183,   112,    -1,
     183,   111,    -1,   183,   110,    -1,   185,    -1,   185,    86,
      87,    -1,   185,    86,   167,    87,    -1,    47,    -1,     6,
      -1,     7,    -1,     5,    -1,   109,     7,    -1,    26,    -1,
      27,    -1,    28,    -1,    20,    -1,    21,    -1,    22,    -1,
      23,    -1,    24,    -1,    25,    -1,   113,    -1,   114,    -1,
     115,    -1,    29,    -1,    30,    -1,    31,    -1,    32,    -1,
      35,    -1,    38,    -1,    33,    -1,    36,    -1,    39,    -1,
      34,    -1,    37,    -1,    40,    -1,    49,    -1,   121,    -1,
     128,    -1,    50,    -1,   122,    -1,   129,    -1,    51,    -1,
     123,    -1,   130,    -1,    52,    -1,   124,    -1,   131,    -1,
      53,    -1,    54,    -1,    55,    -1,   125,    -1,   132,    -1,
      56,    -1,   116,    -1,   126,    -1,   133,    -1,   117,    -1,
     127,    -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,
     118,    -1,   119,    -1,   120,    -1,   186,    -1,    58,    -1,
      46,    57,    88,   187,    89,    -1,    46,    88,   187,    89,
      -1,   188,    -1,   187,   188,    -1,   184,   189,    94,    -1,
     190,    -1,   189,    91,   190,    -1,    57,    -1,    57,    86,
     167,    87,    -1,   164,    -1,   168,    -1,   195,    -1,   194,
      -1,   192,    -1,   201,    -1,   202,    -1,   205,    -1,   208,
      -1,   209,    -1,   216,    -1,    88,    89,    -1,    -1,    -1,
      88,   196,   200,   197,    89,    -1,   199,    -1,   194,    -1,
      88,    89,    -1,    88,   200,    89,    -1,   193,    -1,   200,
     193,    -1,    94,    -1,   166,    94,    -1,    14,    84,   166,
      85,   203,    -1,   193,    12,   193,    -1,   193,    -1,   166,
      -1,   180,    57,    93,   191,    -1,    -1,    17,    84,   206,
     166,    85,    88,   207,    89,    -1,    -1,   200,    -1,    18,
     166,    92,    -1,    19,    92,    -1,    -1,    48,    84,   210,
     204,    85,   198,    -1,    -1,    11,   211,   193,    48,    84,
     166,    85,    94,    -1,    -1,    13,    84,   212,   213,   215,
      85,   198,    -1,   201,    -1,   192,    -1,   204,    -1,    -1,
     214,    94,    -1,   214,    94,   166,    -1,    10,    94,    -1,
       9,    94,    -1,    16,    94,    -1,    16,   166,    94,    -1,
      15,    94,    -1,   218,    -1,   217,   218,    -1,   219,    -1,
     168,    -1,    -1,   169,   220,   199,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   337,   337,   377,   380,   402,   421,   427,   433,   440,
     443,   534,   537,   654,   665,   679,   687,   814,   817,   835,
     844,   851,   855,   862,   868,   877,   885,   975,   982,   992,
     995,  1006,  1017,  1040,  1041,  1042,  1043,  1054,  1055,  1065,
    1075,  1090,  1091,  1100,  1112,  1113,  1125,  1140,  1141,  1152,
    1163,  1174,  1188,  1189,  1200,  1214,  1215,  1230,  1231,  1246,
    1247,  1262,  1263,  1277,  1278,  1292,  1293,  1307,  1308,  1333,
    1334,  1348,  1349,  1350,  1351,  1357,  1358,  1359,  1366,  1373,
    1380,  1387,  1397,  1400,  1412,  1420,  1426,  1436,  1471,  1474,
    1481,  1489,  1510,  1529,  1545,  1579,  1585,  1596,  1602,  1613,
    1616,  1619,  1622,  1628,  1641,  1646,  1657,  1663,  1675,  1678,
    1738,  1819,  1891,  1965,  2006,  2028,  2085,  2185,  2262,  2343,
    2416,  2513,  2525,  2623,  2626,  2629,  2636,  2640,  2647,  2660,
    2673,  2682,  2685,  2688,  2691,  2694,  2697,  2704,  2715,  2729,
    2732,  2742,  2759,  2764,  2769,  2774,  2779,  2786,  2792,  2798,
    2804,  2810,  2816,  2822,  2828,  2834,  2840,  2848,  2856,  2864,
    2871,  2878,  2885,  2892,  2899,  2906,  2913,  2920,  2927,  2934,
    2941,  2948,  2954,  2963,  2972,  2978,  2987,  2996,  3002,  3011,
    3020,  3026,  3035,  3044,  3050,  3056,  3067,  3078,  3089,  3100,
    3109,  3118,  3127,  3136,  3145,  3154,  3163,  3172,  3181,  3190,
    3199,  3208,  3217,  3234,  3247,  3257,  3260,  3275,  3314,  3318,
    3324,  3329,  3342,  3346,  3350,  3351,  3357,  3358,  3359,  3360,
    3361,  3362,  3363,  3367,  3370,  3370,  3370,  3379,  3380,  3385,
    3388,  3397,  3401,  3408,  3409,  3413,  3422,  3426,  3436,  3442,
    3460,  3460,  3469,  3472,  3479,  3483,  3490,  3490,  3496,  3496,
    3504,  3504,  3522,  3525,  3531,  3534,  3540,  3544,  3551,  3559,
    3568,  3576,  3588,  3598,  3602,  3610,  3613,  3619,  3619
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTRIBUTE", "CONST_QUAL", "BOOL_TYPE",
  "FLOAT_TYPE", "INT_TYPE", "INVARIANT", "BREAK", "CONTINUE", "DO", "ELSE",
  "FOR", "IF", "DISCARD", "RETURN", "SWITCH", "CASE", "DEFAULT", "BVEC2",
  "BVEC3", "BVEC4", "IVEC2", "IVEC3", "IVEC4", "VEC2", "VEC3", "VEC4",
  "MATRIX2", "MATRIX3", "MATRIX4", "MATRIX2X2", "MATRIX3X2", "MATRIX4X2",
  "MATRIX2X3", "MATRIX3X3", "MATRIX4X3", "MATRIX2X4", "MATRIX3X4",
  "MATRIX4X4", "IN_QUAL", "OUT_QUAL", "INOUT_QUAL", "UNIFORM", "VARYING",
  "STRUCT", "VOID_TYPE", "WHILE", "SAMPLER1D", "SAMPLER2D", "SAMPLER3D",
  "SAMPLERCUBE", "SAMPLER1DSHADOW", "SAMPLER2DSHADOW", "SAMPLER2DRECTARB",
  "SAMPLER2DRECTSHADOWARB", "IDENTIFIER", "TYPE_NAME", "FLOATCONSTANT",
  "INTCONSTANT", "BOOLCONSTANT", "FIELD_SELECTION", "LEFT_OP", "RIGHT_OP",
  "INC_OP", "DEC_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP",
  "OR_OP", "XOR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "ADD_ASSIGN",
  "MOD_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN",
  "OR_ASSIGN", "SUB_ASSIGN", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
  "RIGHT_BRACKET", "LEFT_BRACE", "RIGHT_BRACE", "DOT", "COMMA", "COLON",
  "EQUAL", "SEMICOLON", "BANG", "DASH", "TILDE", "PLUS", "STAR", "SLASH",
  "PERCENT", "LEFT_ANGLE", "RIGHT_ANGLE", "VERTICAL_BAR", "CARET",
  "AMPERSAND", "QUESTION", "UINTCONSTANT", "UNSIGNED", "NOPERSPECTIVE",
  "FLAT", "CENTROID", "UVEC2", "UVEC3", "UVEC4", "SAMPLER1DARRAY",
  "SAMPLER2DARRAY", "SAMPLER1DARRAYSHADOW", "SAMPLER2DARRAYSHADOW",
  "SAMPLERCUBESHADOW", "ISAMPLER1D", "ISAMPLER2D", "ISAMPLER3D",
  "ISAMPLERCUBE", "ISAMPLER2DRECT", "ISAMPLER1DARRAY", "ISAMPLER2DARRAY",
  "USAMPLER1D", "USAMPLER2D", "USAMPLER3D", "USAMPLERCUBE",
  "USAMPLER2DRECT", "USAMPLER1DARRAY", "USAMPLER2DARRAY", "SAMPLERBUFFER",
  "ISAMPLERBUFFER", "USAMPLERBUFFER", "$accept", "variable_identifier",
  "primary_expression", "postfix_expression", "integer_expression",
  "function_call", "function_call_or_method", "function_call_generic",
  "function_call_header_no_parameters",
  "function_call_header_with_parameters", "function_call_header",
  "function_identifier", "unary_expression", "unary_operator",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_xor_expression",
  "logical_or_expression", "conditional_expression",
  "assignment_expression", "assignment_operator", "expression",
  "constant_expression", "declaration", "function_prototype",
  "function_declarator", "function_header_with_parameters",
  "function_header", "parameter_declarator", "parameter_declaration",
  "parameter_qualifier", "parameter_type_specifier",
  "array_declarator_suffix", "init_declarator_list", "single_declaration",
  "fully_specified_type", "varying_geom_modifyer", "type_qualifier",
  "varying_modifier", "type_specifier", "type_specifier_nonarray",
  "struct_specifier", "struct_declaration_list", "struct_declaration",
  "struct_declarator_list", "struct_declarator", "initializer",
  "declaration_statement", "statement", "simple_statement",
  "compound_statement", "$@1", "$@2", "statement_no_new_scope",
  "compound_statement_no_new_scope", "statement_list",
  "expression_statement", "selection_statement",
  "selection_rest_statement", "condition", "switch_statement", "$@3",
  "switch_statement_list", "case_label", "iteration_statement", "$@4",
  "$@5", "$@6", "for_init_statement", "conditionopt", "for_rest_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", "$@7", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   138,   139,   140,   140,   140,   140,   140,   140,   141,
     141,   141,   141,   141,   141,   142,   143,   144,   144,   145,
     145,   146,   146,   147,   147,   148,   149,   149,   149,   150,
     150,   150,   150,   151,   151,   151,   151,   152,   152,   152,
     152,   153,   153,   153,   154,   154,   154,   155,   155,   155,
     155,   155,   156,   156,   156,   157,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   162,   163,   163,   164,
     164,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   166,   166,   167,   168,   168,   169,   170,   170,
     171,   171,   172,   173,   173,   174,   174,   174,   174,   175,
     175,   175,   175,   176,   177,   177,   177,   177,   178,   178,
     178,   178,   178,   178,   179,   179,   179,   179,   179,   179,
     179,   180,   180,   181,   181,   181,   182,   182,   182,   182,
     182,   183,   183,   183,   183,   183,   183,   183,   183,   184,
     184,   184,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   186,   186,   187,   187,   188,   189,   189,
     190,   190,   191,   192,   193,   193,   194,   194,   194,   194,
     194,   194,   194,   195,   196,   197,   195,   198,   198,   199,
     199,   200,   200,   201,   201,   202,   203,   203,   204,   204,
     206,   205,   207,   207,   208,   208,   210,   209,   211,   209,
     212,   209,   213,   213,   214,   214,   215,   215,   216,   216,
     216,   216,   216,   217,   217,   218,   218,   220,   219
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     3,     1,
       4,     1,     3,     2,     2,     1,     1,     1,     3,     2,
       2,     2,     1,     2,     3,     2,     1,     1,     1,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     5,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     2,     2,     2,     1,     1,
       2,     3,     3,     2,     5,     3,     2,     3,     2,     0,
       1,     1,     1,     1,     2,     3,     3,     4,     1,     3,
       4,     7,     8,     5,     1,     2,     3,     6,     7,     4,
       2,     1,     3,     0,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     1,
       3,     4,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     5,     4,     1,     2,     3,     1,     3,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     0,     0,     5,     1,     1,     2,
       3,     1,     2,     1,     2,     5,     3,     1,     1,     4,
       0,     8,     0,     1,     3,     2,     0,     6,     0,     8,
       0,     7,     1,     1,     1,     0,     2,     3,     2,     2,
       2,     3,     2,     1,     2,     1,     1,     0,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,   127,   126,   145,   143,   144,   131,   150,   151,   152,
     153,   154,   155,   147,   148,   149,   159,   160,   161,   162,
     165,   168,   163,   166,   169,   164,   167,   170,   130,   129,
       0,   142,   171,   174,   177,   180,   183,   184,   185,   188,
     202,     0,   134,   133,   132,   156,   157,   158,   189,   192,
     198,   199,   200,   172,   175,   178,   181,   186,   190,   193,
     173,   176,   179,   182,   187,   191,   194,   195,   196,   197,
     266,   267,     0,    89,    99,     0,   108,   114,   123,     0,
     121,   139,   201,     0,   263,   265,   120,     0,     0,   146,
      85,     0,    87,    99,   131,   100,   101,   102,    90,     0,
      99,     0,    86,   115,   124,   125,     0,   135,   128,   138,
     137,   136,     0,     1,   264,     0,     0,     0,   205,     0,
     268,    91,    96,    98,   103,     0,   109,    92,     0,     0,
     116,   122,     2,     6,     4,     7,    28,     0,     0,     0,
     140,    35,    34,    36,    33,     5,     3,     9,    29,    11,
      16,    17,     0,     0,    22,     0,    37,     0,    41,    44,
      47,    52,    55,    57,    59,    61,    63,    65,    67,    84,
       0,    26,     0,   210,     0,   208,   204,   206,     0,     0,
     248,     0,     0,     0,     0,     0,     0,     0,     0,   224,
     229,   233,    37,    69,    82,     0,   213,     0,   121,   216,
     231,   215,   214,     0,   217,   218,   219,   220,   221,   222,
      93,    95,    97,     0,     0,   110,   104,     0,   212,   119,
      30,    31,     0,    13,    14,     0,     0,    20,    19,     0,
     142,    23,    25,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   141,   203,     0,     0,   207,   259,
     258,     0,   250,     0,   262,   260,     0,   240,     0,   245,
     246,   223,     0,    72,    73,    75,    74,    77,    78,    79,
      80,    81,    76,    71,     0,     0,   234,   230,   232,     0,
     104,     0,   113,     0,     0,   106,   105,     8,     0,    15,
      27,    12,    18,    24,    38,    39,    40,    43,    42,    45,
      46,    50,    51,    48,    49,    53,    54,    56,    58,    60,
      62,    64,    66,     0,     0,   209,     0,     0,     0,   261,
       0,   244,     0,   225,    70,    83,     0,     0,   105,   104,
       0,   117,     0,   107,    10,     0,   211,     0,   253,   252,
     255,     0,     0,   238,     0,     0,     0,    94,   111,     0,
     105,   118,    68,     0,   254,     0,     0,   237,   235,     0,
       0,     0,   226,   112,     0,   256,     0,     0,   242,     0,
     228,   247,   227,     0,   257,   251,   236,   243,     0,   239,
     249,   241
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   146,   147,   148,   298,   149,   150,   151,   152,   153,
     154,   155,   192,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   193,   194,   284,   195,   170,
     196,   197,    72,    73,    74,   122,    98,    99,   123,   295,
      75,    76,    77,   106,    78,    79,   171,    81,    82,   117,
     118,   174,   175,   219,   199,   200,   201,   202,   272,   356,
     381,   382,   203,   204,   205,   368,   355,   206,   330,   388,
     207,   208,   332,   261,   327,   350,   365,   366,   209,    83,
      84,    85,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -346
static const yytype_int16 yypact[] =
{
    1330,  -346,  -346,  -346,  -346,  -346,   -17,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
     -42,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,    60,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,     4,    31,    27,    29,   -50,  -346,    67,   -19,    11,
    -346,    45,  -346,   385,  -346,  -346,  -346,    48,  2793,  -346,
    -346,    64,  -346,    35,  -346,  -346,  -346,  -346,  -346,  2793,
      85,    72,  -346,   -29,  -346,  -346,  2793,  -346,  -346,  -346,
    -346,  -346,  1463,  -346,  -346,  2793,   106,  2527,  -346,   520,
    -346,  -346,  -346,  -346,   107,  2793,   -68,  -346,  1596,  2128,
    -346,  -346,    81,  -346,  -346,  -346,  -346,  2128,  2128,  2128,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,   -36,  -346,
    -346,  -346,    83,   -23,  2261,    82,  -346,  2128,    43,   -12,
      -4,   -55,    65,    70,    68,    66,   109,   110,   -58,  -346,
      95,  -346,  2660,    98,    -9,  -346,  -346,  -346,    92,    94,
    -346,   105,   108,    96,  1729,   111,  2128,    99,   112,   104,
    -346,  -346,   145,  -346,  -346,     2,  -346,     4,   116,  -346,
    -346,  -346,  -346,   655,  -346,  -346,  -346,  -346,  -346,  -346,
     115,  -346,  -346,  1862,  2128,  -346,   -65,   117,  -346,  -346,
    -346,  -346,   -22,  -346,  -346,  2128,  2394,  -346,  -346,  2128,
     120,  -346,  -346,  -346,  2128,  2128,  2128,  2128,  2128,  2128,
    2128,  2128,  2128,  2128,  2128,  2128,  2128,  2128,  2128,  2128,
    2128,  2128,  2128,  2128,  -346,  -346,  2128,   106,  -346,  -346,
    -346,   790,  -346,  2128,  -346,  -346,    10,  -346,    21,  -346,
    -346,  -346,   790,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  2128,  2128,  -346,  -346,  -346,  2128,
     -51,   119,  -346,  1995,  2128,  -346,   -41,  -346,   121,   118,
    -346,   123,  -346,  -346,  -346,  -346,  -346,    43,    43,   -12,
     -12,    -4,    -4,    -4,    -4,   -55,   -55,    65,    70,    68,
      66,   109,   110,    58,   124,  -346,   154,  1060,   -10,  -346,
    2128,  -346,  1195,   790,  -346,  -346,   125,  2128,   -28,   127,
     129,  -346,  2128,  -346,  -346,  2128,  -346,   130,  -346,  -346,
    1195,   790,     9,   118,   172,   146,   144,  -346,  -346,  2128,
     127,  -346,  -346,  2128,  -346,   140,   150,   225,  -346,   151,
     147,   925,  -346,  -346,    12,  2128,   925,   790,   790,  2128,
    -346,  -346,  -346,   149,   118,  -346,  -346,   790,   152,  -346,
    -346,  -346
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -346,  -346,  -346,  -346,  -346,  -346,  -346,    18,  -346,  -346,
    -346,  -346,    41,  -346,   -82,   -80,  -134,   -84,    -2,    -1,
      -3,     1,     5,     3,  -346,  -108,  -127,  -346,  -133,  -118,
       7,     8,  -346,  -346,  -346,   128,   155,   157,   134,  -102,
    -346,  -346,  -316,  -346,   -57,  -346,     0,  -346,  -346,   135,
    -106,  -346,    -8,  -205,   -67,  -200,  -345,  -346,  -346,  -346,
    -114,   173,  -267,   -64,  -346,  -346,   -85,  -346,  -346,  -346,
    -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,  -346,
     183,  -346,  -346
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -89
static const yytype_int16 yytable[] =
{
      80,   130,   218,   288,   169,   333,   222,    70,    71,   292,
     217,   177,   241,   242,   252,    87,   354,   100,   213,   107,
     169,   293,   104,   105,   215,   214,   380,   231,   294,   223,
     224,   380,     1,     2,   354,   293,   100,    94,     1,     2,
      86,   101,   337,    94,   102,   293,    88,   243,   244,   253,
     225,   266,   342,   268,   226,   127,   108,   128,   293,   239,
     240,   326,   228,   297,   129,   359,   177,    89,   229,   285,
      95,    96,    97,    28,    29,   351,    95,    96,    97,    28,
      29,   285,   257,    80,   237,   258,   238,   218,   116,   341,
      70,    71,   299,   285,   369,   291,   286,   383,    90,   124,
     285,   285,   303,   285,   329,   169,   131,   311,   312,   313,
     314,   387,   285,   331,   -88,   116,    92,   116,    93,   198,
     323,   109,   110,   111,   103,   124,    95,    96,    97,   126,
     328,   112,   358,   288,   245,   246,   115,   361,   324,    42,
      43,    44,   234,   235,   236,    42,    43,    44,   169,   285,
     345,   367,   119,   156,   373,   307,   308,   334,   335,   309,
     310,   315,   316,   173,   210,   -27,   232,   218,   227,   156,
     249,   336,   116,   248,   389,   340,   247,   386,   220,   221,
     250,   169,   254,   251,   256,   169,   259,   288,   260,   262,
     264,   269,   263,   271,   343,   267,   270,   352,   233,   353,
     -26,   289,   347,   198,   296,   -21,   338,   -28,   344,   285,
     218,   346,   357,   293,   363,   218,   360,   353,   362,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   370,
     374,   371,   218,   372,   375,   376,   343,   377,   283,   378,
     379,   391,   384,   390,   302,   317,   319,   318,   121,   325,
     172,   320,   218,   211,   156,   322,   321,   125,   343,   212,
     348,   198,   385,   349,   120,   364,   114,     0,     0,     0,
       0,     0,   198,     0,     0,   304,   305,   306,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,     0,     0,     0,   156,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   198,     0,     0,
     156,     0,   198,   198,   156,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     198,   198,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   198,     0,     0,     0,     0,   198,   198,   198,     0,
       0,     0,     0,     0,     0,   113,     0,   198,     1,     2,
       3,     4,     5,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,    28,
      29,    30,    31,     0,    32,    33,    34,    35,    36,    37,
      38,    39,     0,    40,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     1,     2,     3,     4,     5,     6,   178,
     179,   180,     0,   181,   182,   183,   184,   185,   186,   187,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,    28,    29,    30,    31,   188,    32,
      33,    34,    35,    36,    37,    38,    39,   132,    40,   133,
     134,   135,   136,     0,     0,   137,   138,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,     0,     0,     0,   189,   190,
       0,     0,     0,     0,   191,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     1,     2,
       3,     4,     5,     6,   178,   179,   180,     0,   181,   182,
     183,   184,   185,   186,   187,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,    28,
      29,    30,    31,   188,    32,    33,    34,    35,    36,    37,
      38,    39,   132,    40,   133,   134,   135,   136,     0,     0,
     137,   138,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   139,
       0,     0,     0,   189,   287,     0,     0,     0,     0,   191,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   145,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     1,     2,     3,     4,     5,     6,   178,
     179,   180,     0,   181,   182,   183,   184,   185,   186,   187,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,    28,    29,    30,    31,   188,    32,
      33,    34,    35,    36,    37,    38,    39,   132,    40,   133,
     134,   135,   136,     0,     0,   137,   138,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,     0,     0,     0,   189,     0,
       0,     0,     0,     0,   191,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     1,     2,
       3,     4,     5,     6,   178,   179,   180,     0,   181,   182,
     183,   184,   185,   186,   187,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,    28,
      29,    30,    31,   188,    32,    33,    34,    35,    36,    37,
      38,    39,   132,    40,   133,   134,   135,   136,     0,     0,
     137,   138,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   139,
       0,     0,     0,   119,     0,     0,     0,     0,     0,   191,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   145,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,    28,    29,    30,    31,     0,    32,
      33,    34,    35,    36,    37,    38,    39,   132,    40,   133,
     134,   135,   136,     0,     0,   137,   138,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   139,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   191,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     1,     2,
       3,     4,     5,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,    28,
      29,    30,    31,     0,    32,    33,    34,    35,    36,    37,
      38,    39,   132,    40,   133,   134,   135,   136,     0,     0,
     137,   138,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   145,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     1,     2,     3,     4,     5,     6,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,    28,    29,    30,    31,     0,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     3,     4,
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,     0,     0,     0,     0,    30,
      31,     0,    32,    33,    34,    35,    36,    37,    38,    39,
     132,    40,   133,   134,   135,   136,     0,     0,   137,   138,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   139,     0,     0,
     140,     0,     0,     0,     0,     0,     0,     0,   141,   142,
     143,   144,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   145,    41,     0,     0,     0,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,     3,     4,     5,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,     0,     0,
       0,     0,    30,    31,     0,    32,    33,    34,    35,    36,
      37,    38,    39,   132,    40,   133,   134,   135,   136,     0,
       0,   137,   138,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     139,     0,     0,   216,     0,     0,     0,     0,     0,     0,
       0,   141,   142,   143,   144,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   145,    41,     0,     0,     0,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,     3,     4,     5,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
       0,     0,     0,     0,     0,    30,    31,     0,    32,    33,
      34,    35,    36,    37,    38,    39,   132,    40,   133,   134,
     135,   136,     0,     0,   137,   138,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   139,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   265,   141,   142,   143,   144,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   145,    41,     0,
       0,     0,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,     3,     4,     5,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,     0,     0,     0,     0,     0,    30,    31,
       0,    32,    33,    34,    35,    36,    37,    38,    39,   132,
      40,   133,   134,   135,   136,     0,     0,   137,   138,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   139,     0,     0,   290,
       0,     0,     0,     0,     0,     0,     0,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     145,    41,     0,     0,     0,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
       3,     4,     5,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,     0,     0,     0,     0,
       0,    30,    31,     0,    32,    33,    34,    35,    36,    37,
      38,    39,   132,    40,   133,   134,   135,   136,     0,     0,
     137,   138,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   139,
       0,     0,   339,     0,     0,     0,     0,     0,     0,     0,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   145,    41,     0,     0,     0,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     3,     4,     5,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,     0,
       0,     0,     0,     0,    30,    31,     0,    32,    33,    34,
      35,    36,    37,    38,    39,   132,    40,   133,   134,   135,
     136,     0,     0,   137,   138,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   145,    41,     0,     0,
       0,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,     3,     4,     5,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,     0,     0,     0,     0,     0,    30,   230,     0,
      32,    33,    34,    35,    36,    37,    38,    39,   132,    40,
     133,   134,   135,   136,     0,     0,   137,   138,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   139,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   141,   142,   143,   144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   145,
      41,     0,     0,     0,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,     3,
       4,     5,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,     0,     0,     0,     0,     0,
      30,    31,     0,    32,    33,    34,    35,    36,    37,    38,
      39,   300,    40,     0,     0,     0,   301,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    41,     0,     0,     0,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,     3,     4,     5,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,     0,     0,
       0,     0,     0,    30,    31,     0,    32,    33,    34,    35,
      36,    37,    38,    39,     0,    40,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    41,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,     3,     4,     5,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,     0,     0,     0,     0,     0,    30,    31,     0,    32,
      33,    34,    35,    36,    37,    38,    39,     0,    40,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   255,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
       0,     0,     0,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     3,     4,
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,     0,     0,     0,     0,     0,    30,
      31,     0,    32,    33,    34,    35,    36,    37,    38,    39,
       0,    40,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    41,     0,     0,     0,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-346)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,   103,   129,   203,   112,   272,   139,     0,     0,   214,
     128,   117,    67,    68,    72,    57,   332,    74,    86,     8,
     128,    86,    41,    42,   126,    93,   371,   154,    93,    65,
      66,   376,     3,     4,   350,    86,    93,     8,     3,     4,
      57,    91,    93,     8,    94,    86,    88,   102,   103,   107,
      86,   184,    93,   186,    90,    84,    45,    86,    86,    63,
      64,   261,    85,    85,    93,    93,   172,     7,    91,    91,
      41,    42,    43,    44,    45,    85,    41,    42,    43,    44,
      45,    91,    91,    83,    96,    94,    98,   214,    88,   294,
      83,    83,   225,    91,    85,   213,    94,    85,    94,    99,
      91,    91,   229,    91,    94,   213,   106,   241,   242,   243,
     244,   378,    91,    92,    85,   115,    85,   117,    91,   119,
     253,   110,   111,   112,    57,   125,    41,    42,    43,    57,
     263,    86,   337,   333,    69,    70,    88,   342,   256,   110,
     111,   112,    99,   100,   101,   110,   111,   112,   256,    91,
      92,   351,    88,   112,   359,   237,   238,   284,   285,   239,
     240,   245,   246,    57,    57,    84,    84,   294,    85,   128,
     104,   289,   172,   105,   379,   293,   106,   377,   137,   138,
      71,   289,    87,    73,    86,   293,    94,   387,    94,    84,
      94,    92,    84,    89,   296,    84,    84,   330,   157,   332,
      84,    86,    48,   203,    87,    85,    87,    84,    87,    91,
     337,    87,    87,    86,    84,   342,    87,   350,   345,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    57,
     363,    85,   359,    89,    94,    85,   338,    12,    93,    88,
      93,    89,   375,    94,   226,   247,   249,   248,    93,   257,
     115,   250,   379,   125,   213,   252,   251,   100,   360,   125,
     327,   261,   376,   327,    91,   350,    83,    -1,    -1,    -1,
      -1,    -1,   272,    -1,    -1,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,    -1,    -1,    -1,   256,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,    -1,    -1,
     289,    -1,   332,   333,   293,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     350,   351,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   371,    -1,    -1,    -1,    -1,   376,   377,   378,    -1,
      -1,    -1,    -1,    -1,    -1,     0,    -1,   387,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    47,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    88,    89,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    88,    89,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    88,    -1,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    47,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    -1,    -1,    65,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     3,     4,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    47,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    44,    45,    46,    47,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     5,     6,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    46,
      47,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    -1,    -1,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,
      97,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,   109,    -1,    -1,    -1,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      -1,    -1,    46,    47,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      -1,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,    -1,    -1,    -1,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,     5,     6,     7,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      -1,    -1,    -1,    -1,    -1,    46,    47,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,    -1,
      -1,    -1,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,     5,     6,     7,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,    -1,    -1,    46,    47,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    65,    66,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,    -1,    -1,    -1,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      -1,    46,    47,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      95,    96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,   109,    -1,    -1,    -1,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     5,     6,     7,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    46,    47,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    96,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,   109,    -1,    -1,
      -1,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    -1,    -1,    -1,    -1,    -1,    46,    47,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    -1,    -1,    65,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,    -1,    -1,    -1,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,     5,
       6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      46,    47,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    62,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,    -1,    -1,    -1,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      -1,    -1,    -1,    46,    47,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    -1,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,    -1,    -1,    -1,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,     5,     6,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    46,    47,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
      -1,    -1,    -1,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     5,     6,
       7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    46,
      47,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    -1,    -1,    -1,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    44,    45,
      46,    47,    49,    50,    51,    52,    53,    54,    55,    56,
      58,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     168,   169,   170,   171,   172,   178,   179,   180,   182,   183,
     184,   185,   186,   217,   218,   219,    57,    57,    88,     7,
      94,   220,    85,    91,     8,    41,    42,    43,   174,   175,
     182,    91,    94,    57,    41,    42,   181,     8,    45,   110,
     111,   112,    86,     0,   218,    88,   184,   187,   188,    88,
     199,   174,   173,   176,   184,   175,    57,    84,    86,    93,
     177,   184,    57,    59,    60,    61,    62,    65,    66,    84,
      87,    95,    96,    97,    98,   108,   139,   140,   141,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     167,   184,   187,    57,   189,   190,    89,   188,     9,    10,
      11,    13,    14,    15,    16,    17,    18,    19,    48,    88,
      89,    94,   150,   163,   164,   166,   168,   169,   184,   192,
     193,   194,   195,   200,   201,   202,   205,   208,   209,   216,
      57,   173,   176,    86,    93,   177,    87,   167,   164,   191,
     150,   150,   166,    65,    66,    86,    90,    85,    85,    91,
      47,   164,    84,   150,    99,   100,   101,    96,    98,    63,
      64,    67,    68,   102,   103,    69,    70,   106,   105,   104,
      71,    73,    72,   107,    87,    89,    86,    91,    94,    94,
      94,   211,    84,    84,    94,    94,   166,    84,   166,    92,
      84,    89,   196,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    93,   165,    91,    94,    89,   193,    86,
      87,   167,   191,    86,    93,   177,    87,    85,   142,   166,
      57,    62,   145,   164,   150,   150,   150,   152,   152,   153,
     153,   154,   154,   154,   154,   155,   155,   156,   157,   158,
     159,   160,   161,   166,   167,   190,   193,   212,   166,    94,
     206,    92,   210,   200,   164,   164,   167,    93,    87,    87,
     167,   191,    93,   177,    87,    92,    87,    48,   192,   201,
     213,    85,   166,   166,   180,   204,   197,    87,   191,    93,
      87,   191,   164,    84,   204,   214,   215,   193,   203,    85,
      57,    85,    89,   191,   166,    94,    85,    12,    88,    93,
     194,   198,   199,    85,   166,   198,   193,   200,   207,   191,
      94,    89
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YYUSE (yytype);
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1787 of yacc.c  */
#line 337 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        
        // The symbol table search was done in the lexical phase
        const TSymbol* symbol = (yyvsp[(1) - (1)].lex).symbol;
        const TVariable* variable;
        if (symbol == 0) {
            parseContext.error((yyvsp[(1) - (1)].lex).range, "undeclared identifier", (yyvsp[(1) - (1)].lex).string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
            TType type(EbtFloat);
            TVariable* fakeVariable = new TVariable((yyvsp[(1) - (1)].lex).string, type);
            parseContext.symbolTable.insert(*fakeVariable);
            variable = fakeVariable;
        } else {
            // This identifier can only be a variable type symbol 
            if (! symbol->isVariable()) {
                parseContext.error((yyvsp[(1) - (1)].lex).range, "variable expected", (yyvsp[(1) - (1)].lex).string->c_str(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            variable = static_cast<const TVariable*>(symbol);
        }

        // don't delete $1.string, it's used by error recovery, and the pool
        // pop will reclaim the memory

        if (variable->getType().getQualifier() == EvqConst ) {
            constUnion* constArray = variable->getConstPointer();
            TType t(variable->getType());
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(constArray, t, (yyvsp[(1) - (1)].lex).range, parseContext.extensionChanged);
        } else {
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addSymbol(variable->getUniqueId(), 
                                                     variable->getName(), 
                                                     variable->getType(), 
                                                     (yyvsp[(1) - (1)].lex).range,
                                                     parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].lex).range);
    }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 377 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 380 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        //
        // INT_TYPE is only 16-bit plus sign bit for vertex/fragment shaders, 
        // check for overflow for constants
        //
		fprintf(stderr, "I:%d\n", (yyvsp[(1) - (1)].lex).i);
        if (abs((yyvsp[(1) - (1)].lex).i) >= (1 << 16)) {
            if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                parseContext.error((yyvsp[(1) - (1)].lex).range, " integer constant overflow", "", "");
                parseContext.recover(__FILE__, __LINE__);
            } /* else {
                if (abs($1.i) >= (1 << 32)) {
                    parseContext.error($1.range, " integer constant overflow", "", "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } */
        }
        constUnion *unionArray = new constUnion[1];
        unionArray->setIConst((yyvsp[(1) - (1)].lex).i);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(1) - (1)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].lex).range);
    }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 402 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }

        /*
        if (abs($1.i) >= (1 << 32)) {
            parseContext.error($1.range, " integer constant overflow", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        */

        constUnion *unionArray = new constUnion[1];
		fprintf(stderr, "UI:%u\n", (yyvsp[(1) - (1)].lex).ui);
        unionArray->setUIConst((yyvsp[(1) - (1)].lex).ui);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtUInt, EvqConst), (yyvsp[(1) - (1)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].lex).range);

    }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 421 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        constUnion *unionArray = new constUnion[1];
        unionArray->setFConst((yyvsp[(1) - (1)].lex).f);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), (yyvsp[(1) - (1)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].lex).range);
    }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 427 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        constUnion *unionArray = new constUnion[1];
        unionArray->setBConst((yyvsp[(1) - (1)].lex).b);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(1) - (1)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].lex).range);
    }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 433 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(2) - (3)].interm.intermTypedNode);
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].lex).range));
    }
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 440 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 443 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (!(yyvsp[(1) - (4)].interm.intermTypedNode)->isArray() && !(yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix() && !(yyvsp[(1) - (4)].interm.intermTypedNode)->isVector()) {
            if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getAsSymbolNode())
                parseContext.error((yyvsp[(2) - (4)].lex).range, " left of '[' is not of type array, matrix, or vector ", (yyvsp[(1) - (4)].interm.intermTypedNode)->getAsSymbolNode()->getSymbol().c_str(), "");
            else
                parseContext.error((yyvsp[(2) - (4)].lex).range, " left of '[' is not of type array, matrix, or vector ", "expression", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getQualifier() == EvqConst && (yyvsp[(3) - (4)].interm.intermTypedNode)->getQualifier() == EvqConst) {
            if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isArray()) { // constant folding for arrays
                (yyval.interm.intermTypedNode) = parseContext.addConstArrayNode((yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), (yyvsp[(1) - (4)].interm.intermTypedNode), (yyvsp[(2) - (4)].lex).range);
            } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isVector()) {  // constant folding for vectors
                TVectorFields fields;                
                fields.num = 1;
                fields.offsets[0] = (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst(); // need to do it this way because v.xy sends fields integer array
                (yyval.interm.intermTypedNode) = parseContext.addConstVectorNode(fields, (yyvsp[(1) - (4)].interm.intermTypedNode), (yyvsp[(2) - (4)].lex).range);
            } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix()) { // constant folding for matrices
                (yyval.interm.intermTypedNode) = parseContext.addConstMatrixNode((yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), (yyvsp[(1) - (4)].interm.intermTypedNode), (yyvsp[(2) - (4)].lex).range);
            } 
        } else {
            if ((yyvsp[(3) - (4)].interm.intermTypedNode)->getQualifier() == EvqConst) {
                if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isVector() && (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getNominalSize() <= (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst() && !(yyvsp[(1) - (4)].interm.intermTypedNode)->isArray() ) {
                    parseContext.error((yyvsp[(2) - (4)].lex).range, "", "[", "field selection out of range '%d'", (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                    parseContext.recover(__FILE__, __LINE__);
                } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix() && (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getMatrixSize(1) <= (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst() && !(yyvsp[(1) - (4)].interm.intermTypedNode)->isArray() ) {
                    parseContext.error((yyvsp[(2) - (4)].lex).range, "", "[", "matrix field selection out of range '%d'", (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                    parseContext.recover(__FILE__, __LINE__);
                } else {
                    if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isArray()) {
                        if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getArraySize() == 0) {
                            if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getMaxArraySize() <= (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst()) {
                                if (parseContext.arraySetMaxSize((yyvsp[(1) - (4)].interm.intermTypedNode)->getAsSymbolNode(), (yyvsp[(1) - (4)].interm.intermTypedNode)->getTypePointer(), (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst(), true, (yyvsp[(2) - (4)].lex).range))
                                    parseContext.recover(__FILE__, __LINE__); 
                            } else {
                                if (parseContext.arraySetMaxSize((yyvsp[(1) - (4)].interm.intermTypedNode)->getAsSymbolNode(), (yyvsp[(1) - (4)].interm.intermTypedNode)->getTypePointer(), 0, false, (yyvsp[(2) - (4)].lex).range))
                                    parseContext.recover(__FILE__, __LINE__); 
                            }
                        } else if ( (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst() >= (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getArraySize()) {
                            parseContext.error((yyvsp[(2) - (4)].lex).range, "", "[", "array index out of range '%d'", (yyvsp[(3) - (4)].interm.intermTypedNode)->getAsConstantUnion()->getUnionArrayPointer()->getIConst());
                            parseContext.recover(__FILE__, __LINE__);
                        }
                    }
                    /* TODO: changed this to indirect!!! Check again if valid */
                    (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexIndirect, (yyvsp[(1) - (4)].interm.intermTypedNode), (yyvsp[(3) - (4)].interm.intermTypedNode), (yyvsp[(2) - (4)].lex).range, parseContext.extensionChanged);
                }
            } else {
                if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isArray() && (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getArraySize() == 0) {
                    parseContext.error((yyvsp[(2) - (4)].lex).range, "", "[", "array must be redeclared with a size before being indexed with a variable");
                    parseContext.recover(__FILE__, __LINE__);
                }
                
                (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexIndirect, (yyvsp[(1) - (4)].interm.intermTypedNode), (yyvsp[(3) - (4)].interm.intermTypedNode), (yyvsp[(2) - (4)].lex).range, parseContext.extensionChanged);
            }
        } 
        if ((yyval.interm.intermTypedNode) == 0) {
            constUnion *unionArray = new constUnion[1];
            unionArray->setFConst(0.0f);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), (yyvsp[(2) - (4)].lex).range, parseContext.extensionChanged);
        } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isArray()) {
            if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getStruct()) {
                (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getStruct(), (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getTypeName()));
            } else {
                if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix()) {
                    (yyval.interm.intermTypedNode) ->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqTemporary, EvmNone, 1,
                                       (yyvsp[(1) - (4)].interm.intermTypedNode)->getMatrixSize(0), (yyvsp[(1) - (4)].interm.intermTypedNode)->getMatrixSize(1), (yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix()));
                } else {
                    (yyval.interm.intermTypedNode) ->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqTemporary, EvmNone, (yyvsp[(1) - (4)].interm.intermTypedNode)->getNominalSize(),
                                       1,1, (yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix()));
               }
            }
            if ((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getQualifier() == EvqConst) {
                (yyval.interm.intermTypedNode)->getTypePointer()->changeQualifier(EvqConst);
            }
            // add possible remaining arrays
            int i;
            for (i=1; i<(yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getNumArrays(); i++) {
                (yyval.interm.intermTypedNode)->getTypePointer()->setArraySize((yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getArraySize(i), i-1);
            }
        } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix() && (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getQualifier() == EvqConst) {
            (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqConst, EvmNone, (yyvsp[(1) - (4)].interm.intermTypedNode)->getMatrixSize(1)));
        } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isMatrix()) {
            (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqTemporary, EvmNone, (yyvsp[(1) - (4)].interm.intermTypedNode)->getMatrixSize(1)));
        } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isVector() && (yyvsp[(1) - (4)].interm.intermTypedNode)->getType().getQualifier() == EvqConst) {
            (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqConst));
        } else if ((yyvsp[(1) - (4)].interm.intermTypedNode)->isVector()) {
            (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (4)].interm.intermTypedNode)->getBasicType(), EvqTemporary));
        } else {
            (yyval.interm.intermTypedNode)->setType((yyvsp[(1) - (4)].interm.intermTypedNode)->getType());
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (4)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (4)].interm.intermTypedNode)->getRange(), (yyvsp[(4) - (4)].lex).range));
    }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 534 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 537 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {        
        if ((yyvsp[(1) - (3)].interm.intermTypedNode)->isArray()) {
            parseContext.error((yyvsp[(3) - (3)].lex).range, "cannot apply dot operator to an array", ".", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        if ((yyvsp[(1) - (3)].interm.intermTypedNode)->isVector()) {
            TVectorFields fields;
            if (! parseContext.parseVectorFields(*(yyvsp[(3) - (3)].lex).string, (yyvsp[(1) - (3)].interm.intermTypedNode)->getNominalSize(), fields, (yyvsp[(3) - (3)].lex).range)) {
                fields.num = 1;
                fields.offsets[0] = 0;
                parseContext.recover(__FILE__, __LINE__);
            }

            if ((yyvsp[(1) - (3)].interm.intermTypedNode)->getType().getQualifier() == EvqConst) { // constant folding for vector fields
                (yyval.interm.intermTypedNode) = parseContext.addConstVectorNode(fields, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].lex).range);
                if ((yyval.interm.intermTypedNode) == 0) {
                    parseContext.recover(__FILE__, __LINE__);
                    (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
                }
                else
                    (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType(), EvqConst, EvmNone, (int) (*(yyvsp[(3) - (3)].lex).string).size()));
            } else {
                if (fields.num == 1) {
                    constUnion *unionArray = new constUnion[1];
                    /* TODO: Changed to setSConst from setIConst! Check if valid */
                    unionArray->setSConst(fields.offsets[0]);
                    TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(3) - (3)].lex).range, parseContext.extensionChanged);
                    (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexDirect, (yyvsp[(1) - (3)].interm.intermTypedNode), index, (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
                    (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType()));
                } else {
                    TString vectorString = *(yyvsp[(3) - (3)].lex).string;
                    TIntermTyped* index = parseContext.intermediate.addSwizzle(fields, (yyvsp[(3) - (3)].lex).range, parseContext.extensionChanged);                
                    (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpVectorSwizzle, (yyvsp[(1) - (3)].interm.intermTypedNode), index, (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
                    (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType(),EvqTemporary, EvmNone, (int) vectorString.size()));  
                }
            }
        } else if ((yyvsp[(1) - (3)].interm.intermTypedNode)->isMatrix()) {
            TMatrixFields fields;
            if (! parseContext.parseMatrixFields(*(yyvsp[(3) - (3)].lex).string, (yyvsp[(1) - (3)].interm.intermTypedNode)->getMatrixSize(0), (yyvsp[(1) - (3)].interm.intermTypedNode)->getMatrixSize(1), fields, (yyvsp[(3) - (3)].lex).range)) {
                fields.wholeRow = false;
                fields.wholeCol = false;
                fields.row = 0;
                fields.col = 0;
                parseContext.recover(__FILE__, __LINE__);
            }

            if (fields.wholeRow || fields.wholeCol) {
                parseContext.error((yyvsp[(2) - (3)].lex).range, " non-scalar fields not implemented yet", ".", "");
                parseContext.recover(__FILE__, __LINE__);
                constUnion *unionArray = new constUnion[1];
                /* TODO: Changed to setSConst from setIConst! Check if valid */
                unionArray->setSConst(0);
                TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(3) - (3)].lex).range, parseContext.extensionChanged);
                (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexDirect, (yyvsp[(1) - (3)].interm.intermTypedNode), index, (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
                if (fields.wholeRow) {
                    (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType(), EvqTemporary, EvmNone, (yyvsp[(1) - (3)].interm.intermTypedNode)->getMatrixSize(0)));
                } else {
                    (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType(), EvqTemporary, EvmNone, (yyvsp[(1) - (3)].interm.intermTypedNode)->getMatrixSize(1)));
                }
            } else {
                constUnion *unionArray = new constUnion[1];
                /* TODO: Changed to setSConst from setIConst! Check if valid */
                unionArray->setSConst(fields.col * (yyvsp[(1) - (3)].interm.intermTypedNode)->getMatrixSize(1) + fields.row);
                TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(3) - (3)].lex).range, parseContext.extensionChanged);
                (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexDirect, (yyvsp[(1) - (3)].interm.intermTypedNode), index, (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
                (yyval.interm.intermTypedNode)->setType(TType((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType()));
            }
        } else if ((yyvsp[(1) - (3)].interm.intermTypedNode)->getBasicType() == EbtStruct) {
            bool fieldFound = false;
            TTypeList* fields = (yyvsp[(1) - (3)].interm.intermTypedNode)->getType().getStruct();
            if (fields == 0) {
                parseContext.error((yyvsp[(2) - (3)].lex).range, "structure has no fields", "Internal Error", "");
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
            } else {
                unsigned int i;
                for (i = 0; i < fields->size(); ++i) {
                    if ((*fields)[i].type->getFieldName() == *(yyvsp[(3) - (3)].lex).string) {
                        fieldFound = true;
                        break;
                    }                
                }
                if (fieldFound) {
                    if ((yyvsp[(1) - (3)].interm.intermTypedNode)->getType().getQualifier() == EvqConst) {
                        (yyval.interm.intermTypedNode) = parseContext.addConstStruct(*(yyvsp[(3) - (3)].lex).string, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range);
                        if ((yyval.interm.intermTypedNode) == 0) {
                            parseContext.recover(__FILE__, __LINE__);
                            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
                        }
                        else {
                            (yyval.interm.intermTypedNode)->setType(*(*fields)[i].type);
                            // change the qualifier of the return type, not of the structure field
                            // as the structure definition is shared between various structures.
                            (yyval.interm.intermTypedNode)->getTypePointer()->changeQualifier(EvqConst);
                        }
                    } else {
                        constUnion *unionArray = new constUnion[1];
                        unionArray->setIConst(i);
                        TIntermTyped* index = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(3) - (3)].lex).range, parseContext.extensionChanged);
                        (yyval.interm.intermTypedNode) = parseContext.intermediate.addIndex(EOpIndexDirectStruct, (yyvsp[(1) - (3)].interm.intermTypedNode), index, (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
                        (yyval.interm.intermTypedNode)->setType(*(*fields)[i].type);
                    }
                } else {
                    parseContext.error((yyvsp[(2) - (3)].lex).range, " no such field in structure", (yyvsp[(3) - (3)].lex).string->c_str(), "");
                    parseContext.recover(__FILE__, __LINE__);
                    (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
                }
            }
        } else {
            parseContext.error((yyvsp[(2) - (3)].lex).range, " field selection requires structure, vector, or matrix on left hand side", (yyvsp[(3) - (3)].lex).string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].lex).range));
        // don't delete $3.string, it's from the pool
    }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 654 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.lValueErrorCheck((yyvsp[(2) - (2)].lex).range, "++", (yyvsp[(1) - (2)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath(EOpPostIncrement, (yyvsp[(1) - (2)].interm.intermTypedNode), (yyvsp[(2) - (2)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.unaryOpError((yyvsp[(2) - (2)].lex).range, "++", (yyvsp[(1) - (2)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (2)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (2)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (2)].interm.intermTypedNode)->getRange(), (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 665 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.lValueErrorCheck((yyvsp[(2) - (2)].lex).range, "--", (yyvsp[(1) - (2)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath(EOpPostDecrement, (yyvsp[(1) - (2)].interm.intermTypedNode), (yyvsp[(2) - (2)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.unaryOpError((yyvsp[(2) - (2)].lex).range, "--", (yyvsp[(1) - (2)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (2)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (2)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (2)].interm.intermTypedNode)->getRange(), (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 679 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.integerErrorCheck((yyvsp[(1) - (1)].interm.intermTypedNode), "[]"))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 687 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TFunction* fnCall = (yyvsp[(1) - (1)].interm).function;
        TOperator op = fnCall->getBuiltInOp();
        
        if (op == EOpArrayLength) {
            if ((yyvsp[(1) - (1)].interm).intermNode->getAsTyped() == 0 || (yyvsp[(1) - (1)].interm).intermNode->getAsTyped()->getType().getArraySize() == 0) {
                parseContext.error((yyvsp[(1) - (1)].interm).range, "", fnCall->getName().c_str(), "array must be declared with a size before using this method");
                parseContext.recover(__FILE__, __LINE__);
            }

            constUnion *unionArray = new constUnion[1];
            unionArray->setIConst((yyvsp[(1) - (1)].interm).intermNode->getAsTyped()->getType().getArraySize());
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtInt, EvqConst), (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
        } else if (op != EOpNull) {
            
            //
            // Then this should be a constructor.
            // Don't go through the symbol table for constructors.  
            // Their parameters will be verified algorithmically.
            //
            TType type(EbtVoid);  // use this to get the type back

            if (parseContext.constructorErrorCheck((yyvsp[(1) - (1)].interm).range, (yyvsp[(1) - (1)].interm).intermNode, *fnCall, op, &type)) {
                (yyval.interm.intermTypedNode) = 0;
            } else {
                //
                // It's a constructor, of type 'type'.
                //
                (yyval.interm.intermTypedNode) = parseContext.addConstructor((yyvsp[(1) - (1)].interm).intermNode, &type, op, fnCall, (yyvsp[(1) - (1)].interm).range);
            }
            if ((yyval.interm.intermTypedNode) == 0) {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm.intermTypedNode) = parseContext.intermediate.setAggregateOperator(0, op, (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
            }
            (yyval.interm.intermTypedNode)->setType(type);
            
        } else {
            //
            // Not a constructor.  Find it in the symbol table.
            //
            const TFunction* fnCandidate;
            bool builtIn;
            fnCandidate = parseContext.findFunction((yyvsp[(1) - (1)].interm).range, fnCall, &builtIn);
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
                        (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath(op, (yyvsp[(1) - (1)].interm).intermNode, 
                            TSourceRangeInit, parseContext.symbolTable, parseContext.extensionChanged);
                        if ((yyval.interm.intermTypedNode) == 0)  {
                            parseContext.error((yyvsp[(1) - (1)].interm).intermNode->getRange(), " wrong operand type", "Internal Error", 
                                "built in unary operator function.  Type: %s",
                                static_cast<TIntermTyped*>((yyvsp[(1) - (1)].interm).intermNode)->getCompleteString().c_str());
                            YYERROR;
                        }
                        (yyval.interm.intermTypedNode)->setType(fnCandidate->getReturnType());
                    } else {
                        TIntermTyped *constantInterm = parseContext.intermediate.foldAggregate((yyvsp[(1) - (1)].interm).intermAggregate, op, fnCandidate->getReturnType(), (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
                        if (!constantInterm) {
                            (yyval.interm.intermTypedNode) = parseContext.intermediate.setAggregateOperator((yyvsp[(1) - (1)].interm).intermAggregate, op, (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
                            (yyval.interm.intermTypedNode)->setType(fnCandidate->getReturnType());
                        } else {
                            constantInterm->getType().changeQualifier(EvqConst);
                            (yyval.interm.intermTypedNode) = constantInterm;
                            // bail out for not changeing the type anymore!
                            TType newType = TType(fnCandidate->getReturnType());
                            newType.changeQualifier(EvqConst);
                            (yyval.interm.intermTypedNode)->setType(newType);
                        }
                    }
                } else {
                    // This is a real function call
                    
                    (yyval.interm.intermTypedNode) = parseContext.intermediate.setAggregateOperator((yyvsp[(1) - (1)].interm).intermAggregate, EOpFunctionCall, (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
                    (yyval.interm.intermTypedNode)->setType(fnCandidate->getReturnType());                   
                    
                    // this is how we know whether the given function is a builtIn function or a user defined function
                    // if builtIn == false, it's a userDefined -> could be an overloaded builtIn function also
                    // if builtIn == true, it's definitely a builtIn function with EOpNull
                    if (!builtIn) {
                        (yyval.interm.intermTypedNode)->getAsAggregate()->setUserDefined(); 
                    } else {
                        if (strcmp(fnCandidate->getMangledName().c_str(), EMIT_VERTEX_SIG) == 0) {
                            (yyval.interm.intermTypedNode)->setEmitVertex();
                        }
                    }
                    (yyval.interm.intermTypedNode)->getAsAggregate()->setName(fnCandidate->getMangledName());

                    TQualifier qual;
                    TQualifierList& qualifierList = (yyval.interm.intermTypedNode)->getAsAggregate()->getQualifier();
                    for (int i = 0; i < fnCandidate->getParamCount(); ++i) {
                        qual = (*fnCandidate)[i].type->getQualifier();
                        if (qual == EvqOut || qual == EvqInOut) {
                            if (parseContext.lValueErrorCheck((yyval.interm.intermTypedNode)->getRange(), "assign", (yyval.interm.intermTypedNode)->getAsAggregate()->getSequence()[i]->getAsTyped())) {
                                parseContext.error((yyvsp[(1) - (1)].interm).intermNode->getRange(), "Constant value cannot be passed for 'out' or 'inout' parameters.", "Error", "");
                                parseContext.recover(__FILE__, __LINE__);
                            }
                        }
                        qualifierList.push_back(qual);
                    }
                    (yyval.interm.intermTypedNode)->setType(fnCandidate->getReturnType());
                }
            } else {
                // error message was put out by PaFindFunction()
                // Put on a dummy node for error recovery
                constUnion *unionArray = new constUnion[1];
                unionArray->setFConst(0.0f);
                (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtFloat, EvqConst), (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
                parseContext.recover(__FILE__, __LINE__);
            }
        }
        delete fnCall;
        if ((yyval.interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].interm).range);
    }
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 814 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(1) - (1)].interm);
    }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 817 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(1) - (3)].interm.intermTypedNode)->isArray() && (yyvsp[(3) - (3)].interm).function->getName() == "length") {
            //
            // implement array.length()
            //
            (yyval.interm) = (yyvsp[(3) - (3)].interm);
            (yyval.interm).intermNode = (yyvsp[(1) - (3)].interm.intermTypedNode);
            (yyval.interm).function->relateToOperator(EOpArrayLength);
        } else {
            parseContext.error((yyvsp[(3) - (3)].interm).range, "methods are not supported", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm) = (yyvsp[(3) - (3)].interm);
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm).range);
    }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 835 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(1) - (2)].interm).function->getBuiltInOp() != EOpNull &&
            (yyvsp[(1) - (2)].interm).function->getReturnTypePointer()->isArray() && 
            (yyvsp[(1) - (2)].interm).function->getReturnTypePointer()->getArraySize() == 0) {
                (yyvsp[(1) - (2)].interm).function->getReturnTypePointer()->setArraySize((yyvsp[(1) - (2)].interm).function->getParamCount());
        }
        (yyval.interm) = (yyvsp[(1) - (2)].interm);
        (yyval.interm).range = addRange((yyvsp[(1) - (2)].interm).range, (yyvsp[(2) - (2)].lex).range);
    }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 844 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(1) - (2)].interm);
        (yyval.interm).range = addRange((yyvsp[(1) - (2)].interm).range, (yyvsp[(2) - (2)].lex).range);
    }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 851 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).function = (yyvsp[(1) - (2)].interm.function);
        (yyval.interm).intermNode = 0;
    }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 855 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).function = (yyvsp[(1) - (1)].interm.function);
        (yyval.interm).intermNode = 0;
    }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 862 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TParameter param = { 0, new TType((yyvsp[(2) - (2)].interm.intermTypedNode)->getType()) };
        (yyvsp[(1) - (2)].interm.function)->addParameter(param);
        (yyval.interm).function = (yyvsp[(1) - (2)].interm.function);
        (yyval.interm).intermNode = (yyvsp[(2) - (2)].interm.intermTypedNode);
    }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 868 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TParameter param = { 0, new TType((yyvsp[(3) - (3)].interm.intermTypedNode)->getType()) };
        (yyvsp[(1) - (3)].interm).function->addParameter(param);
        (yyval.interm).function = (yyvsp[(1) - (3)].interm).function;
        (yyval.interm).intermNode = parseContext.intermediate.growAggregate((yyvsp[(1) - (3)].interm).intermNode, (yyvsp[(3) - (3)].interm.intermTypedNode), parseContext.extensionChanged);
    }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 877 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.function) = (yyvsp[(1) - (2)].interm.function);
    }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 885 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        
        //
        // Constructor
        //
        if ((yyvsp[(1) - (1)].interm.type).userDef) {
            TString tempString = "";
            TType type((yyvsp[(1) - (1)].interm.type));
            TFunction *function = new TFunction(&tempString, type, EOpConstructStruct);
            (yyval.interm.function) = function;
        } else {
            TOperator op = EOpNull;
            switch ((yyvsp[(1) - (1)].interm.type).type) {
            case EbtFloat:
                if ((yyvsp[(1) - (1)].interm.type).matrix) {
                    switch((yyvsp[(1) - (1)].interm.type).matrixSize[0]) {
                        case 2:
                            switch((yyvsp[(1) - (1)].interm.type).matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat2;    break;
                                case 3:
                                                                op = EOpConstructMat2x3;  break;
                                case 4:
                                                                op = EOpConstructMat2x4;  break;
                            }
                        case 3:
                            switch((yyvsp[(1) - (1)].interm.type).matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat3x2;  break;
                                case 3:
                                                                op = EOpConstructMat3;    break;
                                case 4:
                                                                op = EOpConstructMat3x4;  break;
                            }
                        case 4:
                            switch((yyvsp[(1) - (1)].interm.type).matrixSize[1]) {
                                case 2:
                                                                op = EOpConstructMat4x2;  break;
                                case 3:
                                                                op = EOpConstructMat4x3;  break;
                                case 4:
                                                                op = EOpConstructMat4;    break;
                            }
                    }         
                } else {      
                    switch((yyvsp[(1) - (1)].interm.type).size) {
                    case 1:                                     op = EOpConstructFloat; break;
                    case 2:                                     op = EOpConstructVec2;  break;
                    case 3:                                     op = EOpConstructVec3;  break;
                    case 4:                                     op = EOpConstructVec4;  break;
                    }       
                }  
                break;               
            case EbtInt:
                switch((yyvsp[(1) - (1)].interm.type).size) {
                case 1:                                         op = EOpConstructInt;   break;
                case 2:       FRAG_VERT_GEOM_ONLY("ivec2", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructIVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("ivec3", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructIVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("ivec4", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructIVec4; break;
                }         
                break;    
            case EbtUInt:
                switch((yyvsp[(1) - (1)].interm.type).size) {
                case 1:                                          op = EOpConstructUInt;  break;
                case 2:       FRAG_VERT_GEOM_ONLY("uvec2", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructUVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("uvec3", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructUVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("uvec4", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructUVec4; break;
                }         
                break;    
            case EbtBool:
                switch((yyvsp[(1) - (1)].interm.type).size) {
                case 1:                                         op = EOpConstructBool;  break;
                case 2:       FRAG_VERT_GEOM_ONLY("bvec2", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructBVec2; break;
                case 3:       FRAG_VERT_GEOM_ONLY("bvec3", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructBVec3; break;
                case 4:       FRAG_VERT_GEOM_ONLY("bvec4", (yyvsp[(1) - (1)].interm.type).range); op = EOpConstructBVec4; break;
                }         
                break;
            }
            if (op == EOpNull) {                    
                parseContext.error((yyvsp[(1) - (1)].interm.type).range, "cannot construct this type", TType::getBasicString((yyvsp[(1) - (1)].interm.type).type), "");
                parseContext.recover(__FILE__, __LINE__);
                (yyvsp[(1) - (1)].interm.type).type = EbtFloat;
                op = EOpConstructFloat;
            }            
            TString tempString = "";
            TType type((yyvsp[(1) - (1)].interm.type));
            TFunction *function = new TFunction(&tempString, type, op);
            (yyval.interm.function) = function;
        }
    }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 975 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.reservedErrorCheck((yyvsp[(1) - (1)].lex).range, *(yyvsp[(1) - (1)].lex).string)) 
            parseContext.recover(__FILE__, __LINE__);
        TType type(EbtVoid);
        TFunction *function = new TFunction((yyvsp[(1) - (1)].lex).string, type);
        (yyval.interm.function) = function;
    }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 982 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.reservedErrorCheck((yyvsp[(1) - (1)].lex).range, *(yyvsp[(1) - (1)].lex).string)) 
            parseContext.recover(__FILE__, __LINE__);
        TType type(EbtVoid);
        TFunction *function = new TFunction((yyvsp[(1) - (1)].lex).string, type);
        (yyval.interm.function) = function;
    }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 992 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 995 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.lValueErrorCheck((yyvsp[(1) - (2)].lex).range, "++", (yyvsp[(2) - (2)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath(EOpPreIncrement, (yyvsp[(2) - (2)].interm.intermTypedNode), (yyvsp[(1) - (2)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.unaryOpError((yyvsp[(1) - (2)].lex).range, "++", (yyvsp[(2) - (2)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(2) - (2)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(2) - (2)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 1006 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.lValueErrorCheck((yyvsp[(1) - (2)].lex).range, "--", (yyvsp[(2) - (2)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath(EOpPreDecrement, (yyvsp[(2) - (2)].interm.intermTypedNode), (yyvsp[(1) - (2)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.unaryOpError((yyvsp[(1) - (2)].lex).range, "--", (yyvsp[(2) - (2)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(2) - (2)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(2) - (2)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 1017 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(1) - (2)].interm).op != EOpNull) {
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addUnaryMath((yyvsp[(1) - (2)].interm).op, (yyvsp[(2) - (2)].interm.intermTypedNode), (yyvsp[(1) - (2)].interm).range, parseContext.symbolTable, parseContext.extensionChanged);
            if ((yyval.interm.intermTypedNode) == 0) {
                const char* errorOp = "";
                switch((yyvsp[(1) - (2)].interm).op) {
                case EOpNegative:   errorOp = "-"; break;
                case EOpLogicalNot: errorOp = "!"; break;
                case EOpBitwiseNot: errorOp = "~"; break;
				default: break;
                }
                parseContext.unaryOpError((yyvsp[(1) - (2)].interm).range, errorOp, (yyvsp[(2) - (2)].interm.intermTypedNode)->getCompleteString());
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm.intermTypedNode) = (yyvsp[(2) - (2)].interm.intermTypedNode);
            }
        } else
            (yyval.interm.intermTypedNode) = (yyvsp[(2) - (2)].interm.intermTypedNode);
        if ((yyval.interm.intermTypedNode) && (yyvsp[(2) - (2)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (2)].interm).range, (yyvsp[(2) - (2)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 1040 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpNull; }
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 1041 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpNegative; }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 1042 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpLogicalNot; }
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 1043 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
              if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                  parseContext.recover(__FILE__, __LINE__);
              }
              (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
              (yyval.interm).op = EOpBitwiseNot; 
            }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 1054 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 1055 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("*", (yyvsp[(2) - (3)].lex).range);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpMul, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "*", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 1065 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("/", (yyvsp[(2) - (3)].lex).range); 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpDiv, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "/", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 1075 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
                parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpMod, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "%", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 1090 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 1091 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {  
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpAdd, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "+", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 1100 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpSub, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "-", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        } 
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 1112 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 1113 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLeftShift, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "<<", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 1125 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpRightShift, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, ">>", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 1140 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 1141 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLessThan, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "<", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 1152 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpGreaterThan, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, ">", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 50:
/* Line 1787 of yacc.c  */
#line 1163 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLessThanEqual, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "<=", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 1174 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpGreaterThanEqual, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, ">=", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 1188 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 1189 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpEqual, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "==", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 1200 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpNotEqual, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "!=", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 1214 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 1215 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpAnd, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "&", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 1230 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 1231 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpExclusiveOr, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "^", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 1246 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 1247 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpInclusiveOr, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "|", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 1262 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 1263 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLogicalAnd, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "&&", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 1277 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 1278 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLogicalXor, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "^^", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 1292 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 1293 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addBinaryMath(EOpLogicalOr, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.symbolTable, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, "||", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            constUnion *unionArray = new constUnion[1];
            unionArray->setBConst(false);
            (yyval.interm.intermTypedNode) = parseContext.intermediate.addConstantUnion(unionArray, TType(EbtBool, EvqConst), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 1307 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 1308 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
       if (parseContext.boolErrorCheck((yyvsp[(2) - (5)].lex).range, (yyvsp[(1) - (5)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm.intermTypedNode) = parseContext.intermediate.addSelection((yyvsp[(1) - (5)].interm.intermTypedNode), (yyvsp[(3) - (5)].interm.intermTypedNode), (yyvsp[(5) - (5)].interm.intermTypedNode), (yyvsp[(2) - (5)].lex).range, parseContext.extensionChanged);

        /* GLSL 1.20 does not require the expressions to have the same type,
         * as long as there is a conversion to one of the expressions to make
         * their types match. The resulting matching is the type of the
         * entire expression */
        /*
        if ($3->getType() != $5->getType())
            $$ = 0;
        */

        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (5)].lex).range, ":", (yyvsp[(3) - (5)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(5) - (5)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(5) - (5)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (5)].interm.intermTypedNode) && (yyvsp[(5) - (5)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (5)].interm.intermTypedNode)->getRange(), (yyvsp[(5) - (5)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 1333 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 1334 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {        
        if (parseContext.lValueErrorCheck((yyvsp[(2) - (3)].interm).range, "assign", (yyvsp[(1) - (3)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addAssign((yyvsp[(2) - (3)].interm).op, (yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].interm).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.assignError((yyvsp[(2) - (3)].interm).range, "assign", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(1) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 1348 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {                                    (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpAssign; }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 1349 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { FRAG_VERT_GEOM_ONLY("*=", (yyvsp[(1) - (1)].lex).range);     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpMulAssign; }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 1350 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { FRAG_VERT_GEOM_ONLY("/=", (yyvsp[(1) - (1)].lex).range);     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpDivAssign; }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 1351 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpModAssign; 
                   }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 1357 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpAddAssign; }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 1358 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; (yyval.interm).op = EOpSubAssign; }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 1359 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
                     (yyval.interm).op = EOpLeftShiftAssign; 
                   }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 1366 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
                     (yyval.interm).op = EOpRightShiftAssign; 
                   }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 1373 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
                     (yyval.interm).op = EOpAndAssign; 
                   }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 1380 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
                     (yyval.interm).op = EOpExclusiveOrAssign; 
                   }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 1387 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
                     if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
                        parseContext.recover(__FILE__, __LINE__);
                     }
                     (yyval.interm).range = (yyvsp[(1) - (1)].lex).range; 
                     (yyval.interm).op = EOpInclusiveOrAssign; 
                   }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 1397 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 1400 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = parseContext.intermediate.addComma((yyvsp[(1) - (3)].interm.intermTypedNode), (yyvsp[(3) - (3)].interm.intermTypedNode), (yyvsp[(2) - (3)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermTypedNode) == 0) {
            parseContext.binaryOpError((yyvsp[(2) - (3)].lex).range, ",", (yyvsp[(1) - (3)].interm.intermTypedNode)->getCompleteString(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getCompleteString());
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = (yyvsp[(3) - (3)].interm.intermTypedNode);
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (3)].interm.intermTypedNode) && (yyvsp[(3) - (3)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (3)].interm.intermTypedNode)->getRange(), (yyvsp[(3) - (3)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 1412 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.constErrorCheck((yyvsp[(1) - (1)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
    }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 1420 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TIntermNode *funcDecl;
        funcDecl = parseContext.intermediate.addFuncDeclaration((yyvsp[(1) - (2)].interm).range, (yyvsp[(1) - (2)].interm).function, parseContext.extensionChanged);
        (yyval.interm.intermNode) = funcDecl;
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].interm).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 1426 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        if (((yyvsp[(1) - (2)].interm).intermAggregate) && ((yyvsp[(1) - (2)].interm).intermAggregate->getOp() == EOpNull)) {
                (yyvsp[(1) - (2)].interm).intermAggregate->setOperator(EOpDeclaration);
        }
        (yyval.interm.intermNode) = (yyvsp[(1) - (2)].interm).intermAggregate; 
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].interm).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 1436 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        //
        // Multiple declarations of the same function are allowed.
        //
        // If this is a definition, the definition production code will check for redefinitions 
        // (we don't know at this point if it's a definition or not).
        //
        // Redeclarations are allowed.  But, return types and parameter qualifiers must match.
        //
        TFunction* prevDec = static_cast<TFunction*>(parseContext.symbolTable.find((yyvsp[(1) - (2)].interm.function)->getMangledName()));
        if (prevDec) {
            if (prevDec->getReturnType() != (yyvsp[(1) - (2)].interm.function)->getReturnType()) {
                parseContext.error((yyvsp[(2) - (2)].lex).range, "overloaded functions must have the same return type", (yyvsp[(1) - (2)].interm.function)->getReturnType().getBasicString(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            for (int i = 0; i < prevDec->getParamCount(); ++i) {
                if ((*prevDec)[i].type->getQualifier() != (*(yyvsp[(1) - (2)].interm.function))[i].type->getQualifier()) {
                    parseContext.error((yyvsp[(2) - (2)].lex).range, "overloaded functions must have the same parameter qualifiers", (*(yyvsp[(1) - (2)].interm.function))[i].type->getQualifierString(), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        
        //
        // If this is a redeclaration, it could also be a definition,
        // in which case, we want to use the variable names from this one, and not the one that's
        // being redeclared.  So, pass back up this declaration, not the one in the symbol table.
        //
        (yyval.interm).function = (yyvsp[(1) - (2)].interm.function);

        parseContext.symbolTable.insert(*(yyval.interm).function);
    }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 1471 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.function) = (yyvsp[(1) - (1)].interm.function);
    }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 1474 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.function) = (yyvsp[(1) - (1)].interm.function);
    }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 1481 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        // Add the parameter
        (yyval.interm.function) = (yyvsp[(1) - (2)].interm.function);
        if ((yyvsp[(2) - (2)].interm).param.type->getBasicType() != EbtVoid)
            (yyvsp[(1) - (2)].interm.function)->addParameter((yyvsp[(2) - (2)].interm).param);
        else
            delete (yyvsp[(2) - (2)].interm).param.type;
    }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 1489 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        //
        // Only first parameter of one-parameter functions can be void
        // The check for named parameters not being void is done in parameter_declarator 
        //
        if ((yyvsp[(3) - (3)].interm).param.type->getBasicType() == EbtVoid) {
            //
            // This parameter > first is void
            //
            parseContext.error((yyvsp[(2) - (3)].lex).range, "cannot be an argument type except for '(void)'", "void", "");
            parseContext.recover(__FILE__, __LINE__);
            delete (yyvsp[(3) - (3)].interm).param.type;
        } else {
            // Add the parameter 
            (yyval.interm.function) = (yyvsp[(1) - (3)].interm.function); 
            (yyvsp[(1) - (3)].interm.function)->addParameter((yyvsp[(3) - (3)].interm).param);
        }
    }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 1510 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(1) - (3)].interm.type).qualifier != EvqGlobal && (yyvsp[(1) - (3)].interm.type).qualifier != EvqTemporary) {
            parseContext.error((yyvsp[(2) - (3)].lex).range, "no qualifiers allowed for function return", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        // make sure a sampler is not involved as well...
        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (3)].lex).range, (yyvsp[(1) - (3)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        // Add the function as a prototype after parsing it (we do not support recursion) 
        TFunction *function;
        TType type((yyvsp[(1) - (3)].interm.type));
        function = new TFunction((yyvsp[(2) - (3)].lex).string, type);
        (yyval.interm.function) = function;
    }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 1529 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(1) - (2)].interm.type).range, (yyvsp[(1) - (2)].interm.type))) {
            parseContext.error((yyvsp[(1) - (2)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (2)].interm.type).setArray(false);
        }
        if ((yyvsp[(1) - (2)].interm.type).type == EbtVoid) {
            parseContext.error((yyvsp[(2) - (2)].lex).range, "illegal use of type 'void'", (yyvsp[(2) - (2)].lex).string->c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if (parseContext.reservedErrorCheck((yyvsp[(2) - (2)].lex).range, *(yyvsp[(2) - (2)].lex).string))
            parseContext.recover(__FILE__, __LINE__);
        TParameter param = {(yyvsp[(2) - (2)].lex).string, new TType((yyvsp[(1) - (2)].interm.type))};
        (yyval.interm).range = (yyvsp[(2) - (2)].lex).range;
        (yyval.interm).param = param;
    }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 1545 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(1) - (5)].interm.type).range, (yyvsp[(1) - (5)].interm.type))) {
            parseContext.error((yyvsp[(1) - (5)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (5)].interm.type).setArray(false);
        }
        // Check that we can make an array out of this type
        if (parseContext.arrayTypeErrorCheck((yyvsp[(3) - (5)].lex).range, (yyvsp[(1) - (5)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.reservedErrorCheck((yyvsp[(2) - (5)].lex).range, *(yyvsp[(2) - (5)].lex).string))
            parseContext.recover(__FILE__, __LINE__);

        int size;
        if (parseContext.arraySizeErrorCheck((yyvsp[(3) - (5)].lex).range, (yyvsp[(4) - (5)].interm.intermTypedNode), size))
            parseContext.recover(__FILE__, __LINE__);
        (yyvsp[(1) - (5)].interm.type).setArray(true, size);

        TType* type = new TType((yyvsp[(1) - (5)].interm.type));
        TParameter param = { (yyvsp[(2) - (5)].lex).string, type };
        (yyval.interm).range = addRange((yyvsp[(2) - (5)].lex).range, (yyvsp[(5) - (5)].lex).range);
        (yyval.interm).param = param;
    }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 1579 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(3) - (3)].interm);
        if (parseContext.paramErrorCheck((yyvsp[(3) - (3)].interm).range, (yyvsp[(1) - (3)].interm.type).qualifier, (yyvsp[(2) - (3)].interm.qualifier), (yyval.interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm).range = addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm).range);
    }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 1585 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(2) - (2)].interm);
        if (parseContext.parameterSamplerErrorCheck((yyvsp[(2) - (2)].interm).range, (yyvsp[(1) - (2)].interm.qualifier), *(yyvsp[(2) - (2)].interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.paramErrorCheck((yyvsp[(2) - (2)].interm).range, EvqTemporary, (yyvsp[(1) - (2)].interm.qualifier), (yyval.interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm).range = (yyvsp[(2) - (2)].interm).range;
    }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 1596 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(3) - (3)].interm);
        if (parseContext.paramErrorCheck((yyvsp[(3) - (3)].interm).range, (yyvsp[(1) - (3)].interm.type).qualifier, (yyvsp[(2) - (3)].interm.qualifier), (yyval.interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm).range = addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm).range);
    }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 1602 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(2) - (2)].interm);
        if (parseContext.parameterSamplerErrorCheck((yyvsp[(2) - (2)].interm).range, (yyvsp[(1) - (2)].interm.qualifier), *(yyvsp[(2) - (2)].interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.paramErrorCheck((yyvsp[(2) - (2)].interm).range, EvqTemporary, (yyvsp[(1) - (2)].interm.qualifier), (yyval.interm).param.type))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm).range = (yyvsp[(2) - (2)].interm).range;
    }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 1613 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqIn;
    }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 1616 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqIn;
    }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 1619 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqOut;
    }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 1622 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqInOut;
    }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 1628 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(1) - (1)].interm.type).range, (yyvsp[(1) - (1)].interm.type))) {
            parseContext.error((yyvsp[(1) - (1)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (1)].interm.type).setArray(false);
        }
        TParameter param = { 0, new TType((yyvsp[(1) - (1)].interm.type)) };
        (yyval.interm).param = param;
        (yyval.interm).range = (yyvsp[(1) - (1)].interm.type).range;
    }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 1641 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type).setBasic(EbtVoid, EvqConst, EvmNone, (yyvsp[(1) - (2)].lex).range);
        (yyval.interm.type).setArray(true);
        (yyval.interm.type).range = addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range);
    }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 1646 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type).setBasic(EbtVoid, EvqConst, EvmNone, (yyvsp[(1) - (3)].lex).range);

        int size;
        if (parseContext.arraySizeErrorCheck((yyvsp[(1) - (3)].lex).range, (yyvsp[(2) - (3)].interm.intermTypedNode), size))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm.type).setArray(true, size);

        (yyval.interm.type).range = addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].lex).range);
    }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 1657 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        GEOM_ONLY("multiple array suffixes", (yyvsp[(3) - (3)].interm.type).range);
        (yyval.interm.type) = (yyvsp[(3) - (3)].interm.type);
        (yyval.interm.type).insertArray();
        (yyval.interm.type).range = addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].interm.type).range);
    }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 1663 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        GEOM_ONLY("multiple array suffixes", (yyvsp[(3) - (4)].lex).range);
        (yyval.interm.type) = (yyvsp[(4) - (4)].interm.type);
        int size;
        if (parseContext.arraySizeErrorCheck((yyvsp[(1) - (4)].lex).range, (yyvsp[(2) - (4)].interm.intermTypedNode), size))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.type).insertArray(size);
        (yyval.interm.type).range = addRange((yyvsp[(1) - (4)].lex).range, (yyvsp[(4) - (4)].interm.type).range);
    }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 1675 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(1) - (1)].interm);
    }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 1678 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm) = (yyvsp[(1) - (3)].interm);

        if ((yyvsp[(1) - (3)].interm).type.type != EbtInvariant) {

            if (parseContext.structQualifierErrorCheck((yyvsp[(3) - (3)].lex).range, (yyval.interm).type))
                parseContext.recover(__FILE__, __LINE__);

            if (parseContext.nonInitConstErrorCheck((yyvsp[(3) - (3)].lex).range, *(yyvsp[(3) - (3)].lex).string, (yyval.interm).type))
                parseContext.recover(__FILE__, __LINE__);

            if (parseContext.nonInitErrorCheck((yyvsp[(3) - (3)].lex).range, *(yyvsp[(3) - (3)].lex).string, (yyval.interm).type))
                parseContext.recover(__FILE__, __LINE__);

            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (3)].lex).string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(3) - (3)].lex).range, (TVariable*)sym, NULL, 
                                                         parseContext.extensionChanged);

            /* Special care taken for structs */
            if ( (yyvsp[(1) - (3)].interm).type.type == EbtStruct 
                 && (yyvsp[(1) - (3)].interm).type.userDef != 0 
                 && (yyvsp[(1) - (3)].interm).type.userDef->isSpecified() ) {
                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ((yyvsp[(1) - (3)].interm).intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  intermNode->getAsDeclarationNode(), 
                                  parseContext);
            } else {
                /* Add declaration normally to the tree */
                (yyval.interm).intermAggregate = 
                    parseContext.intermediate.growAggregate((yyvsp[(1) - (3)].interm).intermNode, intermNode, 
                                                            parseContext.extensionChanged);
            }
        } else {
            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (3)].lex).string);
            if (sym && sym->isVariable()) {

                TVariable *var = static_cast<TVariable*>(sym);
                var->getType().addVaryingModifier(EvmInvariant);

                TString *newName = new TString(var->getName());
                TType newType(EbtInvariant);
                TPublicType newPType;
                newPType.setBasic(EbtInvariant, EvqTemporary);
                TVariable *newVar = new TVariable(newName, newType);

                TIntermNode *intermNode = 
                    parseContext.intermediate.addDeclaration((yyvsp[(3) - (3)].lex).range, newVar, NULL, 
                                                             parseContext.extensionChanged);
                /* Add declaration normally to the tree */
                (yyval.interm).intermAggregate = 
                    parseContext.intermediate.growAggregate((yyvsp[(1) - (3)].interm).intermNode, intermNode, 
                                                            parseContext.extensionChanged);
            }
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (3)].interm).range, (yyvsp[(3) - (3)].lex).range);
    }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 1738 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.structQualifierErrorCheck((yyvsp[(3) - (4)].lex).range, (yyvsp[(1) - (4)].interm).type))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.nonInitConstErrorCheck((yyvsp[(3) - (4)].lex).range, *(yyvsp[(3) - (4)].lex).string, (yyvsp[(1) - (4)].interm).type))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm) = (yyvsp[(1) - (4)].interm);

        if (parseContext.arrayTypeErrorCheck((yyvsp[(4) - (4)].interm.type).range, (yyvsp[(1) - (4)].interm).type) || parseContext.arrayQualifierErrorCheck((yyvsp[(4) - (4)].interm.type).range, (yyvsp[(1) - (4)].interm).type)) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int i;
            for (i=0; i<MAX_ARRAYS; i++) {
                (yyvsp[(1) - (4)].interm).type.addArray(true, (yyvsp[(4) - (4)].interm.type).arraySize[i], i);
            }

            TVariable* variable;
            if (parseContext.arrayErrorCheck((yyvsp[(4) - (4)].interm.type).range, *(yyvsp[(3) - (4)].lex).string, (yyvsp[(1) - (4)].interm).type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        /* Check for multi-dimensional arrays usage */
        if ((yyvsp[(4) - (4)].interm.type).getNumArrays() > 1 && (yyvsp[(1) - (4)].interm).type.qualifier != EvqVaryingIn) {
            parseContext.error((yyvsp[(4) - (4)].interm.type).range, "multi-dimensional array usage error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && (yyvsp[(1) - (4)].interm).qualifier == EvqVaryingIn) {

            // First, find 'gl_VerticesIn' in symbol table
            int vertexInSize = 0;
            TVariable* variable = NULL;
            TSymbol* symbol = parseContext.symbolTable.find("gl_VerticesIn");
            if (symbol) {
                variable = static_cast<TVariable*>(symbol);
                constUnion* varUnion      = variable->getConstPointer();
                vertexInSize = variable->getConstPointer()[0].getIConst();

                if ((yyvsp[(4) - (4)].interm.type).arraySize[0] == 0) {
                    // Use gl_VertexIn for initialization of the array
                    (yyvsp[(1) - (4)].interm).type.setArray(true, vertexInSize);

                    if (parseContext.arrayErrorCheck((yyvsp[(4) - (4)].interm.type).range, *(yyvsp[(3) - (4)].lex).string, (yyvsp[(1) - (4)].interm).type, variable))
                        parseContext.recover(__FILE__, __LINE__);
                } else {
                    if (parseContext.arraySizeGeometryVaryingInErrorCheck((yyvsp[(4) - (4)].interm.type).range, vertexInSize, (yyvsp[(1) - (4)].interm).type.arraySize[0])) {
                        parseContext.recover(__FILE__, __LINE__);
                    }
                }
            }

            if (parseContext.arrayFullDefinedGeometryVaryingInErrorCheck((yyvsp[(4) - (4)].interm.type).range, *(yyvsp[(3) - (4)].lex).string, (yyvsp[(1) - (4)].interm).type)) {
                parseContext.recover(__FILE__, __LINE__);
            }
        }

        TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (4)].lex).string);
        TIntermNode *intermNode = 
            parseContext.intermediate.addDeclaration((yyvsp[(3) - (4)].lex).range, (TVariable*)sym, NULL, parseContext.extensionChanged);

        /* Special care taken for structs */
        if ( (yyvsp[(1) - (4)].interm).type.type == EbtStruct 
             && (yyvsp[(1) - (4)].interm).type.userDef != 0 
             && (yyvsp[(1) - (4)].interm).type.userDef->isSpecified() ) {
            /* Add declaration to instances of stuct */
            TIntermSpecification* specificationNode = 
                ((yyvsp[(1) - (4)].interm).intermAggregate->getSequence())[0]->getAsSpecificationNode();

            addStructInstance(specificationNode,
                              intermNode->getAsDeclarationNode(), 
                              parseContext);
        } else {
            /* Add declaration normally to the tree */
            (yyval.interm).intermAggregate = 
                parseContext.intermediate.growAggregate((yyvsp[(1) - (4)].interm).intermNode, intermNode, parseContext.extensionChanged);
            (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (4)].interm).range, (yyvsp[(4) - (4)].interm.type).range));
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (4)].interm).range, (yyvsp[(4) - (4)].interm.type).range);
    }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 1819 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.structQualifierErrorCheck((yyvsp[(3) - (7)].lex).range, (yyvsp[(1) - (7)].interm).type))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm) = (yyvsp[(1) - (7)].interm);

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck((yyvsp[(4) - (7)].lex).range, (yyvsp[(1) - (7)].interm).type) || parseContext.arrayQualifierErrorCheck((yyvsp[(4) - (7)].lex).range, (yyvsp[(1) - (7)].interm).type))
            parseContext.recover(__FILE__, __LINE__);
        else {
			(yyvsp[(1) - (7)].interm).type.setArray(true, (yyvsp[(7) - (7)].interm.intermTypedNode)->getType().getArraySize());
            if (parseContext.arrayErrorCheck((yyvsp[(4) - (7)].lex).range, *(yyvsp[(3) - (7)].lex).string, (yyvsp[(1) - (7)].interm).type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (7)].lex).string);
        TIntermNode *intermNode = 
            parseContext.intermediate.addDeclaration((yyvsp[(3) - (7)].lex).range, (TVariable*)sym, NULL, parseContext.extensionChanged);
        /* Special care taken for structs */
        if ( (yyvsp[(1) - (7)].interm).type.type == EbtStruct && (yyvsp[(1) - (7)].interm).type.userDef != 0 && (yyvsp[(1) - (7)].interm).type.userDef->isSpecified() ) {

            TIntermNode* initNode;
            if (!parseContext.executeInitializer((yyvsp[(3) - (7)].lex).range, *(yyvsp[(3) - (7)].lex).string, (yyvsp[(1) - (7)].interm).type, (yyvsp[(7) - (7)].interm.intermTypedNode), initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (7)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (7)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (7)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ((yyvsp[(1) - (7)].interm).intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode, 
                                  parseContext);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        } else {
            /* Add declaration normally to the tree */
            TIntermNode* initNode;
            if (!parseContext.executeInitializer((yyvsp[(3) - (7)].lex).range, *(yyvsp[(3) - (7)].lex).string, (yyvsp[(1) - (7)].interm).type, (yyvsp[(7) - (7)].interm.intermTypedNode), initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (7)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (7)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (7)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                (yyval.interm).intermAggregate =
                    parseContext.intermediate.growAggregate((yyval.interm).intermAggregate, decNode, parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (7)].interm).range, (yyvsp[(7) - (7)].interm.intermTypedNode)->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (7)].interm).range, (yyvsp[(7) - (7)].interm.intermTypedNode)->getRange());
    }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 1891 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.structQualifierErrorCheck((yyvsp[(3) - (8)].lex).range, (yyvsp[(1) - (8)].interm).type))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm) = (yyvsp[(1) - (8)].interm);

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck((yyvsp[(4) - (8)].lex).range, (yyvsp[(1) - (8)].interm).type) || parseContext.arrayQualifierErrorCheck((yyvsp[(4) - (8)].lex).range, (yyvsp[(1) - (8)].interm).type))
            parseContext.recover(__FILE__, __LINE__);
        else {
            int size;
            if (parseContext.arraySizeErrorCheck((yyvsp[(4) - (8)].lex).range, (yyvsp[(5) - (8)].interm.intermTypedNode), size))
                parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (8)].interm).type.setArray(true, size);
            if (parseContext.arrayErrorCheck((yyvsp[(4) - (8)].lex).range, *(yyvsp[(3) - (8)].lex).string, (yyvsp[(1) - (8)].interm).type, variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (8)].lex).string);
        TIntermNode *intermNode =
            parseContext.intermediate.addDeclaration((yyvsp[(3) - (8)].lex).range, (TVariable*)sym, NULL, parseContext.extensionChanged);
        /* Special care taken for structs */
        if ( (yyvsp[(1) - (8)].interm).type.type == EbtStruct && (yyvsp[(1) - (8)].interm).type.userDef != 0 && (yyvsp[(1) - (8)].interm).type.userDef->isSpecified() ) {
            TIntermNode* initNode;
            if (!parseContext.executeInitializer((yyvsp[(3) - (8)].lex).range, *(yyvsp[(3) - (8)].lex).string, (yyvsp[(1) - (8)].interm).type, (yyvsp[(8) - (8)].interm.intermTypedNode), initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (8)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (8)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (8)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ((yyvsp[(1) - (8)].interm).intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode, 
                                  parseContext);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        } else {
            /* Add declaration normally to the tree */
            TIntermNode* initNode;
            if (!parseContext.executeInitializer((yyvsp[(3) - (8)].lex).range, *(yyvsp[(3) - (8)].lex).string, (yyvsp[(1) - (8)].interm).type, (yyvsp[(8) - (8)].interm.intermTypedNode), initNode, variable)) {
                //
                // build the intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (8)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (8)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(3) - (8)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                }

                (yyval.interm).intermAggregate =
                    parseContext.intermediate.growAggregate((yyval.interm).intermAggregate, decNode, parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (8)].interm).range, (yyvsp[(8) - (8)].interm.intermTypedNode)->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (8)].interm).range, (yyvsp[(8) - (8)].interm.intermTypedNode)->getRange());
    }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 1965 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.structQualifierErrorCheck((yyvsp[(3) - (5)].lex).range, (yyvsp[(1) - (5)].interm).type))
            parseContext.recover(__FILE__, __LINE__);
        
        (yyval.interm) = (yyvsp[(1) - (5)].interm);
        
        TIntermNode* intermNode;
        if (!parseContext.executeInitializer((yyvsp[(3) - (5)].lex).range, *(yyvsp[(3) - (5)].lex).string, (yyvsp[(1) - (5)].interm).type, (yyvsp[(5) - (5)].interm.intermTypedNode), intermNode)) {
            //
            // build the intermediate representation
            //
            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(3) - (5)].lex).string);
            TIntermNode *decNode;
            decNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(3) - (5)].lex).range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
            /* Special care taken for structs */
           if ( (yyvsp[(1) - (5)].interm).type.type == EbtStruct 
                 && (yyvsp[(1) - (5)].interm).type.userDef != 0 
                 && (yyvsp[(1) - (5)].interm).type.userDef->isSpecified() ) {
                /* Add declaration to instances of stuct */
                TIntermSpecification* specificationNode = 
                    ((yyvsp[(1) - (5)].interm).intermAggregate->getSequence())[0]->getAsSpecificationNode();

                addStructInstance(specificationNode,
                                  decNode->getAsDeclarationNode(), 
                                  parseContext);
            } else {
                /* Add declaration normally to the tree */
                (yyval.interm).intermAggregate = 
                    parseContext.intermediate.growAggregate((yyvsp[(1) - (5)].interm).intermNode, decNode, parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (5)].interm).range, (yyvsp[(5) - (5)].interm.intermTypedNode)->getRange()));
            }
         } else {
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm).intermAggregate = 0;
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (5)].interm).range, (yyvsp[(5) - (5)].interm.intermTypedNode)->getRange());
    }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 2006 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).type = (yyvsp[(1) - (1)].interm.type);
        (yyval.interm).intermAggregate = 0;

        if ( (yyvsp[(1) - (1)].interm.type).type == EbtStruct &&
             (yyvsp[(1) - (1)].interm.type).userDef != 0 && 
             (yyvsp[(1) - (1)].interm.type).userDef->isSpecified() == true ) {
            TIntermNode *intermNode = 
                parseContext.intermediate.addSpecification((yyvsp[(1) - (1)].interm.type).range, (yyvsp[(1) - (1)].interm.type).userDef, parseContext.extensionChanged);
                
            processStruct((yyvsp[(1) - (1)].interm.type).userDef->getStruct(), 
                          intermNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);

            (yyval.interm).intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);

            if ((yyval.interm).intermAggregate)
                (yyval.interm).intermAggregate->setOperator(EOpSpecification); 
        }
        (yyval.interm).range = (yyvsp[(1) - (1)].interm.type).range;
    }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 2028 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).intermAggregate = 0;
        (yyval.interm).type = (yyvsp[(1) - (2)].interm.type);
    
        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (2)].lex).range, (yyval.interm).type))
            parseContext.recover(__FILE__, __LINE__);
        
        if (parseContext.nonInitConstErrorCheck((yyvsp[(2) - (2)].lex).range, *(yyvsp[(2) - (2)].lex).string, (yyval.interm).type))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.nonInitErrorCheck((yyvsp[(2) - (2)].lex).range, *(yyvsp[(2) - (2)].lex).string, (yyval.interm).type))
            parseContext.recover(__FILE__, __LINE__);

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && (yyvsp[(1) - (2)].interm.type).qualifier == EvqVaryingIn) {
            if (parseContext.nonArrayGeometryVaryingInErrorCheck((yyvsp[(2) - (2)].lex).range, (yyvsp[(1) - (2)].interm.type), *(yyvsp[(2) - (2)].lex).string))
                parseContext.recover(__FILE__, __LINE__);
        }
        
        /* Special handling for structs */
        if ( (yyvsp[(1) - (2)].interm.type).type == EbtStruct  && (yyvsp[(1) - (2)].interm.type).userDef != 0 && (yyvsp[(1) - (2)].interm.type).userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification((yyvsp[(1) - (2)].interm.type).range, (yyvsp[(1) - (2)].interm.type).userDef, parseContext.extensionChanged);
                
            processStruct((yyvsp[(1) - (2)].interm.type).userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (2)].lex).string);
            TIntermNode *declarationNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(2) - (2)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            (yyval.interm).intermAggregate = specificationAggregate;

        } else {
            /* None-struct declarations: just add declaration node */
            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (2)].lex).string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(2) - (2)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
            ((TIntermDeclaration*)intermNode)->setFirst(true);
            (yyval.interm).intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (2)].interm.type).range, (yyvsp[(2) - (2)].lex).range);
    }
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 2085 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).intermAggregate = 0;
        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (3)].lex).range, (yyvsp[(1) - (3)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        if (parseContext.nonInitConstErrorCheck((yyvsp[(2) - (3)].lex).range, *(yyvsp[(2) - (3)].lex).string, (yyvsp[(1) - (3)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm).type = (yyvsp[(1) - (3)].interm.type);
        
        if (parseContext.arrayTypeErrorCheck((yyvsp[(3) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type)) || parseContext.arrayQualifierErrorCheck((yyvsp[(3) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type))) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int i;
            for (i=0; i<MAX_ARRAYS; i++) {
                (yyvsp[(1) - (3)].interm.type).addArray(true, (yyvsp[(3) - (3)].interm.type).arraySize[i], i);
            }
            
            TVariable* variable;
            if (parseContext.arrayErrorCheck((yyvsp[(3) - (3)].interm.type).range, *(yyvsp[(2) - (3)].lex).string, (yyvsp[(1) - (3)].interm.type), variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        /* Check for multi-dimensional arrays usage */
        if ((yyvsp[(3) - (3)].interm.type).getNumArrays() > 1 && (yyvsp[(1) - (3)].interm.type).qualifier != EvqVaryingIn) {
            parseContext.error((yyvsp[(3) - (3)].interm.type).range, "multi-dimensional array usage error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }

        /* Additional checks for geometry shaders */
        if (parseContext.language == EShLangGeometry && (yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingIn) {
            
            // First, find 'gl_VerticesIn' in symbol table
            int vertexInSize = 0;
            TVariable* variable = NULL;
            TSymbol* symbol = parseContext.symbolTable.find("gl_VerticesIn");
            if (symbol) {
                variable = static_cast<TVariable*>(symbol);
                constUnion* varUnion      = variable->getConstPointer();
                vertexInSize = variable->getConstPointer()[0].getIConst();
            
                if ((yyvsp[(3) - (3)].interm.type).arraySize[0] == 0) {
                    // Use gl_VertexIn for initialization of the array
                    (yyvsp[(1) - (3)].interm.type).setArray(true, vertexInSize);
    
                    if (parseContext.arrayErrorCheck((yyvsp[(3) - (3)].interm.type).range, *(yyvsp[(2) - (3)].lex).string, (yyvsp[(1) - (3)].interm.type), variable))
                        parseContext.recover(__FILE__, __LINE__);
                } else {
                    if (parseContext.arraySizeGeometryVaryingInErrorCheck((yyvsp[(3) - (3)].interm.type).range, vertexInSize, (yyvsp[(1) - (3)].interm.type).arraySize[0])) {
                        parseContext.recover(__FILE__, __LINE__);
                    }
                }
            }

            if (parseContext.arrayFullDefinedGeometryVaryingInErrorCheck((yyvsp[(3) - (3)].interm.type).range, *(yyvsp[(2) - (3)].lex).string, (yyvsp[(1) - (3)].interm.type))) {
                parseContext.recover(__FILE__, __LINE__);
            }
        }
 
        /* Special handling for structs */
        if ( (yyvsp[(1) - (3)].interm.type).type == EbtStruct  && (yyvsp[(1) - (3)].interm.type).userDef != 0 && (yyvsp[(1) - (3)].interm.type).userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type).userDef, parseContext.extensionChanged);
                
            processStruct((yyvsp[(1) - (3)].interm.type).userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (3)].lex).string);
            TIntermNode *declarationNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(2) - (3)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            (yyval.interm).intermAggregate = specificationAggregate;

        } else {
            /* None-struct declarations: just add declaration node */
            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (3)].lex).string);
            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(2) - (3)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
            intermNode->getAsDeclarationNode()->setFirst(true);
            (yyval.interm).intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, 
                                                        parseContext.extensionChanged);
            (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm.type).range));
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm.type).range);

    }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 2185 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).intermAggregate = 0;

        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (6)].lex).range, (yyvsp[(1) - (6)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm).type = (yyvsp[(1) - (6)].interm.type);

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck((yyvsp[(3) - (6)].lex).range, (yyvsp[(1) - (6)].interm.type)) || parseContext.arrayQualifierErrorCheck((yyvsp[(3) - (6)].lex).range, (yyvsp[(1) - (6)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);
        else {
            (yyvsp[(1) - (6)].interm.type).setArray(true, (yyvsp[(6) - (6)].interm.intermTypedNode)->getType().getArraySize());
            if (parseContext.arrayErrorCheck((yyvsp[(3) - (6)].lex).range, *(yyvsp[(2) - (6)].lex).string, (yyvsp[(1) - (6)].interm.type), variable))
                parseContext.recover(__FILE__, __LINE__);
        }
        
        /* Special handling for structs */
        if ( (yyvsp[(1) - (6)].interm.type).type == EbtStruct  && (yyvsp[(1) - (6)].interm.type).userDef != 0 && (yyvsp[(1) - (6)].interm.type).userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification((yyvsp[(1) - (6)].interm.type).range, (yyvsp[(1) - (6)].interm.type).userDef, parseContext.extensionChanged);
                
            processStruct((yyvsp[(1) - (6)].interm.type).userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TIntermNode* initNode;
            TIntermNode *declarationNode;
            if (!parseContext.executeInitializer((yyvsp[(2) - (6)].lex).range, *(yyvsp[(2) - (6)].lex).string, (yyvsp[(1) - (6)].interm.type), (yyvsp[(6) - (6)].interm.intermTypedNode), initNode, variable)) {
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (6)].lex).string);
                declarationNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (6)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                declarationNode = NULL;
            }

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            (yyval.interm).intermAggregate = specificationAggregate;

        } else {
            TIntermNode* intermNode;
            if (!parseContext.executeInitializer((yyvsp[(2) - (6)].lex).range, *(yyvsp[(2) - (6)].lex).string, (yyvsp[(1) - (6)].interm.type), (yyvsp[(6) - (6)].interm.intermTypedNode), intermNode, variable)) {
                //
                // Build intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (6)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (6)].lex).range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (6)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                }

                (yyval.interm).intermAggregate = parseContext.intermediate.makeAggregate(decNode, 
                                                                   parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (6)].interm.type).range, (yyvsp[(6) - (6)].interm.intermTypedNode)->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (6)].interm.type).range, (yyvsp[(6) - (6)].interm.intermTypedNode)->getRange());
    }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 2262 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm).intermAggregate = 0;

        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (7)].lex).range, (yyvsp[(1) - (7)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm).type = (yyvsp[(1) - (7)].interm.type);

        TVariable* variable = 0;
        if (parseContext.arrayTypeErrorCheck((yyvsp[(3) - (7)].lex).range, (yyvsp[(1) - (7)].interm.type)) || parseContext.arrayQualifierErrorCheck((yyvsp[(3) - (7)].lex).range, (yyvsp[(1) - (7)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);
        else {
            int size;
            if (parseContext.arraySizeErrorCheck((yyvsp[(3) - (7)].lex).range, (yyvsp[(4) - (7)].interm.intermTypedNode), size))
                parseContext.recover(__FILE__, __LINE__);
            
            (yyvsp[(1) - (7)].interm.type).setArray(true, size);
            if (parseContext.arrayErrorCheck((yyvsp[(3) - (7)].lex).range, *(yyvsp[(2) - (7)].lex).string, (yyvsp[(1) - (7)].interm.type), variable))
                parseContext.recover(__FILE__, __LINE__);
        }

        
        /* Special handling for structs */
        if ( (yyvsp[(1) - (7)].interm.type).type == EbtStruct  && (yyvsp[(1) - (7)].interm.type).userDef != 0 && (yyvsp[(1) - (7)].interm.type).userDef->isSpecified() ) {
            /* Struct declarations: add a Specification node to the parse tree */
            TIntermNode *specificationNode =
                parseContext.intermediate.addSpecification((yyvsp[(1) - (7)].interm.type).range, (yyvsp[(1) - (7)].interm.type).userDef, parseContext.extensionChanged);
                
            processStruct((yyvsp[(1) - (7)].interm.type).userDef->getStruct(),
                          specificationNode->getAsSpecificationNode()->getParameterPointer(),
                          parseContext);
            
            TIntermAggregate *specificationAggregate = 
                parseContext.intermediate.makeAggregate(specificationNode, 
                                                        parseContext.extensionChanged);
        
            if (specificationAggregate)
                specificationAggregate->setOperator(EOpSpecification);

            TIntermNode* initNode;
            TIntermNode *declarationNode;
            if (!parseContext.executeInitializer((yyvsp[(2) - (7)].lex).range, *(yyvsp[(2) - (7)].lex).string, (yyvsp[(1) - (7)].interm.type), (yyvsp[(7) - (7)].interm.intermTypedNode), initNode, variable)) {
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (7)].lex).string);
                declarationNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (7)].lex).range, (TVariable*) sym, initNode, parseContext.extensionChanged);
            } else {
                parseContext.recover(__FILE__, __LINE__);
                declarationNode = NULL;
            }

            addStructInstance(specificationNode->getAsSpecificationNode(),
                              declarationNode->getAsDeclarationNode(), 
                              parseContext);
    
            (yyval.interm).intermAggregate = specificationAggregate;

        } else {
            TIntermNode* intermNode;
            if (!parseContext.executeInitializer((yyvsp[(2) - (7)].lex).range, *(yyvsp[(2) - (7)].lex).string, (yyvsp[(1) - (7)].interm.type), (yyvsp[(7) - (7)].interm.intermTypedNode), intermNode, variable)) {
                //
                // Build intermediate representation
                //
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (7)].lex).string);
                TIntermNode *decNode;

                if (intermNode) {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (7)].lex).range, (TVariable*) sym, intermNode, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                } else {
                    decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (7)].lex).range, (TVariable*) sym, NULL, parseContext.extensionChanged);
                    ((TIntermDeclaration*)decNode)->setFirst(true);
                }

                (yyval.interm).intermAggregate = parseContext.intermediate.makeAggregate(decNode, parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (7)].interm.type).range, (yyvsp[(7) - (7)].interm.intermTypedNode)->getRange()));
            } else {
                parseContext.recover(__FILE__, __LINE__);
                (yyval.interm).intermAggregate = 0;
            }
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (7)].interm.type).range, (yyvsp[(7) - (7)].interm.intermTypedNode)->getRange());
    }
    break;

  case 119:
/* Line 1787 of yacc.c  */
#line 2343 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        
        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(1) - (4)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);

        (yyval.interm).type = (yyvsp[(1) - (4)].interm.type);

        TIntermNode* intermNode;
        if (!parseContext.executeInitializer((yyvsp[(2) - (4)].lex).range, *(yyvsp[(2) - (4)].lex).string, (yyvsp[(1) - (4)].interm.type), (yyvsp[(4) - (4)].interm.intermTypedNode), intermNode)) {
            //
            // Build intermediate representation
            //
            /* Special handling for structs */
            if ( (yyvsp[(1) - (4)].interm.type).type == EbtStruct  && (yyvsp[(1) - (4)].interm.type).userDef != 0 && (yyvsp[(1) - (4)].interm.type).userDef->isSpecified() ) {
                /* Struct declarations: add a Specification node to the parse tree */
                TIntermNode *specificationNode =
                    parseContext.intermediate.addSpecification((yyvsp[(1) - (4)].interm.type).range, (yyvsp[(1) - (4)].interm.type).userDef, parseContext.extensionChanged);
                    
                processStruct((yyvsp[(1) - (4)].interm.type).userDef->getStruct(),
                              specificationNode->getAsSpecificationNode()->getParameterPointer(),
                              parseContext);
                
                TIntermAggregate *specificationAggregate = 
                    parseContext.intermediate.makeAggregate(specificationNode, parseContext.extensionChanged);
            
                if (specificationAggregate)
                    specificationAggregate->setOperator(EOpSpecification);

                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (4)].lex).string);
                TIntermNode *declarationNode = 
                    parseContext.intermediate.addDeclaration((yyvsp[(2) - (4)].lex).range, (TVariable*) sym, 
                                                             intermNode, parseContext.extensionChanged);

                addStructInstance(specificationNode->getAsSpecificationNode(),
                                  declarationNode->getAsDeclarationNode(), 
                                  parseContext);
        
                (yyval.interm).intermAggregate = specificationAggregate;

            } else {
                /* None-struct declarations: just add declaration node */
                TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (4)].lex).string);
                TIntermNode *decNode = 
                    parseContext.intermediate.addDeclaration((yyvsp[(2) - (4)].lex).range, (TVariable*) sym, 
                                                             intermNode, parseContext.extensionChanged);
                decNode->getAsDeclarationNode()->setFirst(true);
                (yyval.interm).intermAggregate = 
                    parseContext.intermediate.makeAggregate(decNode, parseContext.extensionChanged);
                (yyval.interm).intermAggregate->setRange(addRange((yyvsp[(1) - (4)].interm.type).range, (yyvsp[(4) - (4)].interm.intermTypedNode)->getRange()));
            }
#if 0
            TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (4)].lex).string);
            TIntermNode *decNode;
            
            if (intermNode) {
                decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (4)].lex).range,
                                                                   (TVariable*) sym,
                                                                   intermNode);
            } else {
                decNode = parseContext.intermediate.addDeclaration((yyvsp[(2) - (4)].lex).range,
                                                                   (TVariable*) sym, 
                                                                   NULL);
            }
            decNode->getAsDeclarationNode()->setFirst(true);
            
            (yyval.interm).intermAggregate = parseContext.intermediate.makeAggregate(decNode, (yyvsp[(2) - (4)].lex).range);
#endif
        } else {
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm).intermAggregate = 0;
        }
        (yyval.interm).range = addRange((yyvsp[(1) - (4)].interm.type).range, (yyvsp[(4) - (4)].interm.intermTypedNode)->getRange());
    }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 2416 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TSymbol *sym = parseContext.symbolTable.find(*(yyvsp[(2) - (2)].lex).string);
        if (sym && sym->isVariable()) {
            
            TVariable *var = static_cast<TVariable*>(sym);
            var->getType().addVaryingModifier(EvmInvariant);
            
            TString *newName = new TString(var->getName());
            TType newType(EbtInvariant);
            TPublicType newPType;
            newPType.setBasic(EbtInvariant, EvqTemporary);
            TVariable *newVar = new TVariable(newName, newType);

            TIntermNode *intermNode = 
                parseContext.intermediate.addDeclaration((yyvsp[(2) - (2)].lex).range, newVar, NULL, 
                                                         parseContext.extensionChanged);
            ((TIntermDeclaration*)intermNode)->setFirst(true);
            (yyval.interm).intermAggregate = 
                parseContext.intermediate.makeAggregate(intermNode, parseContext.extensionChanged);
            (yyval.interm).type = newPType;
        } else {
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm).intermAggregate = 0;
        }
        (yyval.interm).range = (yyvsp[(2) - (2)].lex).range;
    }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 2513 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(1) - (1)].interm.type).range, (yyvsp[(1) - (1)].interm.type))) {
            parseContext.error((yyvsp[(1) - (1)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (1)].interm.type).setArray(false);
        }
        (yyval.interm.type) = (yyvsp[(1) - (1)].interm.type);
    }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 2525 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.varyingModifyerErrorCheck((yyvsp[(3) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type), (yyvsp[(2) - (3)].interm.qualifier))) {
            parseContext.error((yyvsp[(3) - (3)].interm.type).range, "varying modifier syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(3) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm.type))) {
            parseContext.error((yyvsp[(3) - (3)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(3) - (3)].interm.type).setArray(false);
        }
        if ((yyvsp[(3) - (3)].interm.type).array && parseContext.arrayQualifierErrorCheck((yyvsp[(3) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type))) {
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(3) - (3)].interm.type).setArray(false);
        }
        
        if ((yyvsp[(1) - (3)].interm.type).qualifier == EvqAttribute){
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool || (yyvsp[(3) - (3)].interm.type).type == EbtInt) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool or int", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        
        
#if 0 // OLD     
	 	/* Promote varying modifier to varyings in case of geometry shaders */
        if (parseContext.language == EShLangGeometry && (yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingOut) {
            // Change from in to out if specifyed
            if ((yyvsp[(2) - (3)].interm.qualifier) == EvqIn) {
                (yyvsp[(1) - (3)].interm.type).qualifier = EvqVaryingIn;
            }
        }
#else	
        /* GEOMETRY SHADER CHECKS - BEGIN */
        /* Promote varying modifiers to varying if necessary */

        // Change from in to out if specified
        if ((yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingIn && (yyvsp[(2) - (3)].interm.qualifier) == EvqOut) {
			(yyvsp[(1) - (3)].interm.type).qualifier = EvqVaryingOut;
        }
        if ((yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingOut && (yyvsp[(2) - (3)].interm.qualifier) == EvqIn) {
            (yyvsp[(1) - (3)].interm.type).qualifier = EvqVaryingIn;
        }

        /* GEOMETRY SHADER CHECKS - END */
#endif
        
        if ((yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingIn) {
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool || (yyvsp[(3) - (3)].interm.type).type == EbtInt) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool or int", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                } else if (((yyvsp[(3) - (3)].interm.type).type == EbtInt || (yyvsp[(3) - (3)].interm.type).type == EbtUInt) && !((yyvsp[(1) - (3)].interm.type).varyingModifier & EvmFlat)) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "needs to be flat shaded", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }
        if ((yyvsp[(1) - (3)].interm.type).qualifier == EvqVaryingOut) {
            if (!parseContext.extensionActiveCheck("GL_EXT_gpu_shader4")) {
                // GLSL 1.20.8
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool || (yyvsp[(3) - (3)].interm.type).type == EbtInt) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool or int", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            } else {
                // GL_EXT_gpu_shader4 rev.10
                if ((yyvsp[(3) - (3)].interm.type).type == EbtBool) {
                    parseContext.error((yyvsp[(3) - (3)].interm.type).range, "cannot be bool", getQualifierString((yyvsp[(1) - (3)].interm.type).qualifier, parseContext.language), "");
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
        }

        (yyval.interm.type) = (yyvsp[(3) - (3)].interm.type); 
        (yyval.interm.type).qualifier = (yyvsp[(1) - (3)].interm.type).qualifier;
        (yyval.interm.type).varyingModifier = (yyvsp[(1) - (3)].interm.type).varyingModifier;
        (yyval.interm.type).range = addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].interm.type).range);
    }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 2623 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqInOut;
    }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 2626 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqIn;
    }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 2629 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.qualifier) = EvqOut;
    }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 2636 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type).setBasic(EbtVoid, EvqConst, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 2640 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        VERTEX_ONLY("attribute", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.globalErrorCheck((yyvsp[(1) - (1)].lex).range, parseContext.symbolTable.atGlobalLevel(), "attribute"))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.type).setBasic(EbtVoid, EvqAttribute, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 2647 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.globalErrorCheck((yyvsp[(2) - (2)].lex).range, parseContext.symbolTable.atGlobalLevel(), "varying"))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.language == EShLangVertex) {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingOut, (yyvsp[(1) - (2)].interm.varyingModifier), (yyvsp[(2) - (2)].lex).range);
        } else if (parseContext.language == EShLangGeometry) {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingOut, (yyvsp[(1) - (2)].interm.varyingModifier), (yyvsp[(2) - (2)].lex).range);
        } else {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingIn, (yyvsp[(1) - (2)].interm.varyingModifier), (yyvsp[(2) - (2)].lex).range);
        }

        (yyval.interm.type).range = (yyvsp[(2) - (2)].lex).range;
    }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 2660 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.globalErrorCheck((yyvsp[(1) - (1)].lex).range, parseContext.symbolTable.atGlobalLevel(), "varying"))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.language == EShLangVertex) {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingOut, EvmNone, (yyvsp[(1) - (1)].lex).range);
        } else if (parseContext.language == EShLangGeometry) {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingOut, EvmNone, (yyvsp[(1) - (1)].lex).range);
        } else {
            (yyval.interm.type).setBasic(EbtVoid, EvqVaryingIn, EvmNone, (yyvsp[(1) - (1)].lex).range);
        }

        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 2673 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.globalErrorCheck((yyvsp[(1) - (1)].lex).range, parseContext.symbolTable.atGlobalLevel(), "uniform"))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.type).setBasic(EbtVoid, EvqUniform, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 2682 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.varyingModifier) = EvmInvariant;
    }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 2685 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.varyingModifier) = EvmCentroid;
    }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 2688 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.varyingModifier) = EvmFlat;
    }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 2691 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.varyingModifier) = EvmNoperspective;
    }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 2694 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.varyingModifier) |= EvmInvariant;
    }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 2697 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(1) - (2)].interm.varyingModifier) & EvmFlat) {
            parseContext.error((yyvsp[(2) - (2)].lex).range, "varying flat cannot be used with centroid qualifier:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        (yyval.interm.varyingModifier) |= EvmCentroid;
    }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 2704 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(2) - (2)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        if ((yyvsp[(1) - (2)].interm.varyingModifier) & EvmFlat || (yyvsp[(1) - (2)].interm.varyingModifier) & EvmNoperspective) {
            parseContext.error((yyvsp[(2) - (2)].lex).range, "varying flat cannot be used with other qualifiers:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        (yyval.interm.varyingModifier) |= EvmFlat;
    }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 2715 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(2) - (2)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        if ((yyvsp[(1) - (2)].interm.varyingModifier) & EvmFlat) {
            parseContext.error((yyvsp[(2) - (2)].lex).range, "varying flat cannot be used with noperspective qualifier:", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        
        (yyval.interm.varyingModifier) |= EvmNoperspective;
    }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 2729 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type) = (yyvsp[(1) - (1)].interm.type);
    }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 2732 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type) = (yyvsp[(1) - (3)].interm.type);
        
        if (parseContext.arrayTypeErrorCheck((yyvsp[(2) - (3)].lex).range, (yyvsp[(1) - (3)].interm.type))) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            (yyval.interm.type).setArray(true, 0);
        }
        (yyval.interm.type).range = addRange((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(3) - (3)].lex).range);
    }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 2742 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.type) = (yyvsp[(1) - (4)].interm.type);
        
        if (parseContext.arrayTypeErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(1) - (4)].interm.type))) {
            parseContext.recover(__FILE__, __LINE__);
        } else {
            int size;
            if (parseContext.arraySizeErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(3) - (4)].interm.intermTypedNode), size)) {
                parseContext.recover(__FILE__, __LINE__);
            }
            (yyval.interm.type).setArray(true, size);
        }
        (yyval.interm.type).range = addRange((yyvsp[(1) - (4)].interm.type).range, (yyvsp[(4) - (4)].lex).range);
    }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 2759 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtVoid, qual, EvmNone, (yyvsp[(1) - (1)].lex).range); 
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 2764 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 2769 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 2774 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtBool, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 2779 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (2)].lex).range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUInt, qual, EvmNone, (yyvsp[(1) - (2)].lex).range); 
        (yyval.interm.type).range = (yyvsp[(1) - (2)].lex).range;
    }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 2786 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 2792 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 2798 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 2804 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtBool, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 151:
/* Line 1787 of yacc.c  */
#line 2810 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtBool, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 2816 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtBool, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 153:
/* Line 1787 of yacc.c  */
#line 2822 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 2828 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 2834 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 2840 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 157:
/* Line 1787 of yacc.c  */
#line 2848 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 2856 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUInt, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setAggregate(4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 2864 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat2", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(2, 2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 2871 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        FRAG_VERT_GEOM_ONLY("mat3", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(3, 3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 2878 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        FRAG_VERT_GEOM_ONLY("mat4", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(4, 4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 2885 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat2x2", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(2, 2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 2892 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat2x3", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(2, 3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 2899 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat2x4", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(2, 4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 165:
/* Line 1787 of yacc.c  */
#line 2906 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat3x2", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(3, 2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 166:
/* Line 1787 of yacc.c  */
#line 2913 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat3x3", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(3, 3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 2920 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat3x4", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(3, 4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 2927 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat4x2", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(4, 2);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 2934 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat4x3", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(4, 3);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 2941 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("mat4x4", (yyvsp[(1) - (1)].lex).range); 
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtFloat, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).setMatrix(4, 4);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 2948 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler1D", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler1D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 2954 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isampler1D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler1D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 2963 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usampler1D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler1D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 2972 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler2D", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 2978 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isampler2D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler2D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 2987 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usampler2D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler2D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 2996 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler3D", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler3D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 3002 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isampler3D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler3D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 3011 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usampler3D", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler3D, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 3020 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("samplerCube", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSamplerCube, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 3026 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isamplerCube", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISamplerCube, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 182:
/* Line 1787 of yacc.c  */
#line 3035 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usamplerCube", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSamplerCube, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 3044 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler1DShadow", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler1DShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 184:
/* Line 1787 of yacc.c  */
#line 3050 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler2DShadow", (yyvsp[(1) - (1)].lex).range);
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2DShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 185:
/* Line 1787 of yacc.c  */
#line 3056 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("sampler2DRectARB", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_ARB_texture_rectangle"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2DRect, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 3067 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("isampler2DRectARB", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler2DRect, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 3078 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("usampler2DRectARB", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "EXT_gpu_shader4"))
            parseContext.recover(__FILE__, __LINE__);
        
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler2DRect, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 3089 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        // ARB_texture_rectangle

        FRAG_VERT_GEOM_ONLY("sampler2DRectShadowARB", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_ARB_texture_rectangle"))
            parseContext.recover(__FILE__, __LINE__);

        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2DRectShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 3100 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler1DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler1DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 3109 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isampler1DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler1DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 3118 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usampler1DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler1DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 3127 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler2DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 3136 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isampler2DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISampler2DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 3145 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usampler2DArray", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSampler2DArray, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 3154 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("samplerBuffer", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSamplerBuffer, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 3163 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("isamplerBuffer", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtISamplerBuffer, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 3172 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("usamplerBuffer", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtUSamplerBuffer, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 3181 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler1DArrayShadow", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler1DArrayShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 3190 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("sampler2DArrayShadow", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSampler2DArrayShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 200:
/* Line 1787 of yacc.c  */
#line 3199 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("samplerCubeShadow", (yyvsp[(1) - (1)].lex).range);
        if (parseContext.extensionErrorCheck((yyvsp[(1) - (1)].lex).range, "GL_EXT_gpu_shader4")) {
            parseContext.recover(__FILE__, __LINE__);
        }
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtSamplerCubeShadow, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 201:
/* Line 1787 of yacc.c  */
#line 3208 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_VERT_GEOM_ONLY("struct", (yyvsp[(1) - (1)].interm.type).range);
        (yyval.interm.type) = (yyvsp[(1) - (1)].interm.type);
        (yyval.interm.type).qualifier = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        if ((yyval.interm.type).userDef) {
            (yyval.interm.type).userDef->setSpecified(true);
        }
        (yyval.interm.type).range = (yyvsp[(1) - (1)].interm.type).range;
    }
    break;

  case 202:
/* Line 1787 of yacc.c  */
#line 3217 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {     
        //
        // This is for user defined type names.  The lexical phase looked up the 
        // type.
        //
        TType& structure = static_cast<TVariable*>((yyvsp[(1) - (1)].lex).symbol)->getType();
        TQualifier qual = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
        (yyval.interm.type).setBasic(EbtStruct, qual, EvmNone, (yyvsp[(1) - (1)].lex).range);
        (yyval.interm.type).userDef = &structure;
        if ((yyval.interm.type).userDef) {
            (yyval.interm.type).userDef->setSpecified(false);
        }
        (yyval.interm.type).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 203:
/* Line 1787 of yacc.c  */
#line 3234 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        
        TType* structure = new TType((yyvsp[(4) - (5)].interm.typeList), *(yyvsp[(2) - (5)].lex).string);
        TVariable* userTypeDef = new TVariable((yyvsp[(2) - (5)].lex).string, *structure, true);
        if (! parseContext.symbolTable.insert(*userTypeDef)) {
            parseContext.error((yyvsp[(2) - (5)].lex).range, "redefinition", (yyvsp[(2) - (5)].lex).string->c_str(), "struct");
            parseContext.recover(__FILE__, __LINE__);
        }

        (yyval.interm.type).setBasic(EbtStruct, EvqTemporary, EvmNone, (yyvsp[(1) - (5)].lex).range);
        (yyval.interm.type).userDef = structure;
        (yyval.interm.type).range = addRange((yyvsp[(1) - (5)].lex).range, (yyvsp[(5) - (5)].lex).range);
    }
    break;

  case 204:
/* Line 1787 of yacc.c  */
#line 3247 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TType* structure = new TType((yyvsp[(3) - (4)].interm.typeList), TString(""));
        
        (yyval.interm.type).setBasic(EbtStruct, EvqTemporary, EvmNone, (yyvsp[(1) - (4)].lex).range);
        (yyval.interm.type).userDef = structure;
        (yyval.interm.type).range = addRange((yyvsp[(1) - (4)].lex).range, (yyvsp[(4) - (4)].lex).range);
    }
    break;

  case 205:
/* Line 1787 of yacc.c  */
#line 3257 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeList) = (yyvsp[(1) - (1)].interm.typeList);
    }
    break;

  case 206:
/* Line 1787 of yacc.c  */
#line 3260 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeList) = (yyvsp[(1) - (2)].interm.typeList);
        for (unsigned int i = 0; i < (yyvsp[(2) - (2)].interm.typeList)->size(); ++i) {
            for (unsigned int j = 0; j < (yyval.interm.typeList)->size(); ++j) {
                if ((*(yyval.interm.typeList))[j].type->getFieldName() == (*(yyvsp[(2) - (2)].interm.typeList))[i].type->getFieldName()) {
                    parseContext.error((*(yyvsp[(2) - (2)].interm.typeList))[i].range, "duplicate field name in structure:", "struct", (*(yyvsp[(2) - (2)].interm.typeList))[i].type->getFieldName().c_str());
                    parseContext.recover(__FILE__, __LINE__);
                }
            }
            (yyval.interm.typeList)->push_back((*(yyvsp[(2) - (2)].interm.typeList))[i]);
        }
    }
    break;

  case 207:
/* Line 1787 of yacc.c  */
#line 3275 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeList) = (yyvsp[(2) - (3)].interm.typeList);

        if (parseContext.arraySizeUnspecifiedErrorCheck((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type))) {
            parseContext.error((yyvsp[(1) - (3)].interm.type).range, "array syntax error", "", "");
            parseContext.recover(__FILE__, __LINE__);
            (yyvsp[(1) - (3)].interm.type).setArray(false);
        }
        if (parseContext.voidErrorCheck((yyvsp[(1) - (3)].interm.type).range, (*(yyvsp[(2) - (3)].interm.typeList))[0].type->getFieldName(), (yyvsp[(1) - (3)].interm.type))) {
            parseContext.recover(__FILE__, __LINE__);
        }
        for (unsigned int i = 0; i < (yyval.interm.typeList)->size(); ++i) {
            //
            // Careful not to replace already know aspects of type, like array-ness
            //
            (*(yyval.interm.typeList))[i].type->setType((yyvsp[(1) - (3)].interm.type).type, (yyvsp[(1) - (3)].interm.type).size, (yyvsp[(1) - (3)].interm.type).matrixSize[0], (yyvsp[(1) - (3)].interm.type).matrixSize[1], (yyvsp[(1) - (3)].interm.type).matrix, (yyvsp[(1) - (3)].interm.type).userDef);

            // don't allow arrays of arrays
            if ((*(yyval.interm.typeList))[i].type->isArray()) {
                if (parseContext.arrayTypeErrorCheck((yyvsp[(1) - (3)].interm.type).range, (yyvsp[(1) - (3)].interm.type)))
                    parseContext.recover(__FILE__, __LINE__);
            }
            if ((yyvsp[(1) - (3)].interm.type).array)
                (*(yyval.interm.typeList))[i].type->setArraySize((yyvsp[(1) - (3)].interm.type).arraySize[0]);
            if ((yyvsp[(1) - (3)].interm.type).userDef)
                (*(yyval.interm.typeList))[i].type->setTypeName((yyvsp[(1) - (3)].interm.type).userDef->getTypeName());

            if ( i == 0 &&
                 (yyvsp[(1) - (3)].interm.type).type == EbtStruct &&
                 (yyvsp[(1) - (3)].interm.type).userDef != 0 && 
                 (yyvsp[(1) - (3)].interm.type).userDef->isSpecified() == true ) {

                    (*(yyval.interm.typeList))[i].type->setSpecified(true);
            }
        }
    }
    break;

  case 208:
/* Line 1787 of yacc.c  */
#line 3314 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeList) = NewPoolTTypeList();
        (yyval.interm.typeList)->push_back((yyvsp[(1) - (1)].interm.typeRange));
    }
    break;

  case 209:
/* Line 1787 of yacc.c  */
#line 3318 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeList)->push_back((yyvsp[(3) - (3)].interm.typeRange));
    }
    break;

  case 210:
/* Line 1787 of yacc.c  */
#line 3324 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeRange).type = new TType(EbtVoid);
        (yyval.interm.typeRange).type->setFieldName(*(yyvsp[(1) - (1)].lex).string);
        (yyval.interm.typeRange).range = (yyvsp[(1) - (1)].lex).range;
    }
    break;

  case 211:
/* Line 1787 of yacc.c  */
#line 3329 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.typeRange).type = new TType(EbtVoid);
        (yyval.interm.typeRange).type->setFieldName(*(yyvsp[(1) - (4)].lex).string);
        
        int size;
        if (parseContext.arraySizeErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(3) - (4)].interm.intermTypedNode), size))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.typeRange).type->setArraySize(size);
        (yyval.interm.typeRange).range = addRange((yyvsp[(1) - (4)].lex).range, (yyvsp[(4) - (4)].lex).range);
    }
    break;

  case 212:
/* Line 1787 of yacc.c  */
#line 3342 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); }
    break;

  case 213:
/* Line 1787 of yacc.c  */
#line 3346 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 214:
/* Line 1787 of yacc.c  */
#line 3350 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermAggregate); }
    break;

  case 215:
/* Line 1787 of yacc.c  */
#line 3351 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 216:
/* Line 1787 of yacc.c  */
#line 3357 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 217:
/* Line 1787 of yacc.c  */
#line 3358 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 218:
/* Line 1787 of yacc.c  */
#line 3359 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 219:
/* Line 1787 of yacc.c  */
#line 3360 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 220:
/* Line 1787 of yacc.c  */
#line 3361 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 221:
/* Line 1787 of yacc.c  */
#line 3362 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 222:
/* Line 1787 of yacc.c  */
#line 3363 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 223:
/* Line 1787 of yacc.c  */
#line 3367 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermAggregate) = 0;
    }
    break;

  case 224:
/* Line 1787 of yacc.c  */
#line 3370 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { parseContext.symbolTable.push(); }
    break;

  case 225:
/* Line 1787 of yacc.c  */
#line 3370 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { parseContext.symbolTable.pop(); }
    break;

  case 226:
/* Line 1787 of yacc.c  */
#line 3370 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if ((yyvsp[(3) - (5)].interm.intermAggregate) != 0)            
            (yyvsp[(3) - (5)].interm.intermAggregate)->setOperator(EOpSequence); 
        (yyval.interm.intermAggregate) = (yyvsp[(3) - (5)].interm.intermAggregate);
        if ((yyval.interm.intermAggregate)) (yyval.interm.intermAggregate)->setRange(addRange((yyvsp[(1) - (5)].lex).range, (yyvsp[(5) - (5)].lex).range));
    }
    break;

  case 227:
/* Line 1787 of yacc.c  */
#line 3379 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 228:
/* Line 1787 of yacc.c  */
#line 3380 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); }
    break;

  case 229:
/* Line 1787 of yacc.c  */
#line 3385 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermNode) = 0; 
    }
    break;

  case 230:
/* Line 1787 of yacc.c  */
#line 3388 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        if ((yyvsp[(2) - (3)].interm.intermAggregate))
            (yyvsp[(2) - (3)].interm.intermAggregate)->setOperator(EOpSequence); 
        (yyval.interm.intermNode) = (yyvsp[(2) - (3)].interm.intermAggregate); 
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].lex).range));
    }
    break;

  case 231:
/* Line 1787 of yacc.c  */
#line 3397 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermAggregate) = parseContext.intermediate.makeAggregate((yyvsp[(1) - (1)].interm.intermNode), parseContext.extensionChanged);
        if ((yyval.interm.intermAggregate) && (yyvsp[(1) - (1)].interm.intermNode)) (yyval.interm.intermAggregate)->setRange((yyvsp[(1) - (1)].interm.intermNode)->getRange());
    }
    break;

  case 232:
/* Line 1787 of yacc.c  */
#line 3401 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermAggregate) = parseContext.intermediate.growAggregate((yyvsp[(1) - (2)].interm.intermAggregate), (yyvsp[(2) - (2)].interm.intermNode), parseContext.extensionChanged);
        if ((yyval.interm.intermAggregate) && (yyvsp[(1) - (2)].interm.intermAggregate) && (yyvsp[(2) - (2)].interm.intermNode)) (yyval.interm.intermAggregate)->setRange(addRange((yyvsp[(1) - (2)].interm.intermAggregate)->getRange(), (yyvsp[(2) - (2)].interm.intermNode)->getRange()));
    }
    break;

  case 233:
/* Line 1787 of yacc.c  */
#line 3408 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = 0; }
    break;

  case 234:
/* Line 1787 of yacc.c  */
#line 3409 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { (yyval.interm.intermNode) = static_cast<TIntermNode*>((yyvsp[(1) - (2)].interm.intermTypedNode)); }
    break;

  case 235:
/* Line 1787 of yacc.c  */
#line 3413 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        if (parseContext.boolErrorCheck((yyvsp[(1) - (5)].lex).range, (yyvsp[(3) - (5)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
        (yyval.interm.intermNode) = parseContext.intermediate.addSelection((yyvsp[(3) - (5)].interm.intermTypedNode), (yyvsp[(5) - (5)].interm.nodePair), (yyvsp[(1) - (5)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (5)].lex).range, (yyvsp[(4) - (5)].lex).range));
    }
    break;

  case 236:
/* Line 1787 of yacc.c  */
#line 3422 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.nodePair).node1 = (yyvsp[(1) - (3)].interm.intermNode);
        (yyval.interm.nodePair).node2 = (yyvsp[(3) - (3)].interm.intermNode);
    }
    break;

  case 237:
/* Line 1787 of yacc.c  */
#line 3426 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.nodePair).node1 = (yyvsp[(1) - (1)].interm.intermNode);
        (yyval.interm.nodePair).node2 = 0;
    }
    break;

  case 238:
/* Line 1787 of yacc.c  */
#line 3436 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode);
        if (parseContext.boolErrorCheck((yyvsp[(1) - (1)].interm.intermTypedNode)->getRange(), (yyvsp[(1) - (1)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);          
        if ((yyval.interm.intermTypedNode) && (yyvsp[(1) - (1)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange((yyvsp[(1) - (1)].interm.intermTypedNode)->getRange());
    }
    break;

  case 239:
/* Line 1787 of yacc.c  */
#line 3442 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TIntermNode* intermNode;
        if (parseContext.structQualifierErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(1) - (4)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);
        if (parseContext.boolErrorCheck((yyvsp[(2) - (4)].lex).range, (yyvsp[(1) - (4)].interm.type)))
            parseContext.recover(__FILE__, __LINE__);
        
        if (!parseContext.executeInitializer((yyvsp[(2) - (4)].lex).range, *(yyvsp[(2) - (4)].lex).string, (yyvsp[(1) - (4)].interm.type), (yyvsp[(4) - (4)].interm.intermTypedNode), intermNode))
            (yyval.interm.intermTypedNode) = (yyvsp[(4) - (4)].interm.intermTypedNode);
        else {
            parseContext.recover(__FILE__, __LINE__);
            (yyval.interm.intermTypedNode) = 0;
        }
        if ((yyval.interm.intermTypedNode) && (yyvsp[(4) - (4)].interm.intermTypedNode)) (yyval.interm.intermTypedNode)->setRange(addRange((yyvsp[(1) - (4)].interm.type).range, (yyvsp[(4) - (4)].interm.intermTypedNode)->getRange()));
    }
    break;

  case 240:
/* Line 1787 of yacc.c  */
#line 3460 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { parseContext.symbolTable.push(); ++parseContext.switchNestingLevel; }
    break;

  case 241:
/* Line 1787 of yacc.c  */
#line 3460 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        parseContext.symbolTable.pop();
        (yyval.interm.intermNode) = parseContext.intermediate.addSwitch((yyvsp[(4) - (8)].interm.intermTypedNode), (yyvsp[(7) - (8)].interm.intermAggregate), (yyvsp[(2) - (8)].lex).range, parseContext.extensionChanged);
        --parseContext.switchNestingLevel;
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(2) - (8)].lex).range, (yyvsp[(5) - (8)].lex).range));
    }
    break;

  case 242:
/* Line 1787 of yacc.c  */
#line 3469 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermAggregate) = 0;
    }
    break;

  case 243:
/* Line 1787 of yacc.c  */
#line 3472 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermAggregate) = (yyvsp[(1) - (1)].interm.intermAggregate);
        if ((yyval.interm.intermAggregate)) (yyval.interm.intermAggregate)->getAsAggregate()->setOperator(EOpSequence);
    }
    break;

  case 244:
/* Line 1787 of yacc.c  */
#line 3479 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = parseContext.intermediate.addCase((yyvsp[(2) - (3)].interm.intermTypedNode), (yyvsp[(1) - (3)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].lex).range));
    }
    break;

  case 245:
/* Line 1787 of yacc.c  */
#line 3483 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = parseContext.intermediate.addCase(NULL, (yyvsp[(1) - (2)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 246:
/* Line 1787 of yacc.c  */
#line 3490 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { parseContext.symbolTable.push(); ++parseContext.loopNestingLevel; }
    break;

  case 247:
/* Line 1787 of yacc.c  */
#line 3490 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        parseContext.symbolTable.pop();
        (yyval.interm.intermNode) = parseContext.intermediate.addLoop((yyvsp[(6) - (6)].interm.intermNode), NULL, (yyvsp[(4) - (6)].interm.intermTypedNode), 0, LOOP_WHILE, (yyvsp[(1) - (6)].lex).range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (6)].lex).range, (yyvsp[(5) - (6)].lex).range));
    }
    break;

  case 248:
/* Line 1787 of yacc.c  */
#line 3496 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { ++parseContext.loopNestingLevel; }
    break;

  case 249:
/* Line 1787 of yacc.c  */
#line 3496 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.boolErrorCheck((yyvsp[(8) - (8)].lex).range, (yyvsp[(6) - (8)].interm.intermTypedNode)))
            parseContext.recover(__FILE__, __LINE__);
                    
        (yyval.interm.intermNode) = parseContext.intermediate.addLoop((yyvsp[(3) - (8)].interm.intermNode), NULL, (yyvsp[(6) - (8)].interm.intermTypedNode), 0, LOOP_DO, (yyvsp[(4) - (8)].lex).range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(4) - (8)].lex).range, (yyvsp[(7) - (8)].lex).range));
    }
    break;

  case 250:
/* Line 1787 of yacc.c  */
#line 3504 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { parseContext.symbolTable.push(); ++parseContext.loopNestingLevel; }
    break;

  case 251:
/* Line 1787 of yacc.c  */
#line 3504 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        parseContext.symbolTable.pop();
        /*
        $$ = parseContext.intermediate.makeAggregate($4, $2.range);
        $$ = parseContext.intermediate.growAggregate(
                $$,
                parseContext.intermediate.addLoop($7, reinterpret_cast<TIntermTyped*>($5.node1), reinterpret_cast<TIntermTyped*>($5.node2), true, $1.range),
                $1.range);
        $$->getAsAggregate()->setOperator(EOpSequence);
        */
        
        (yyval.interm.intermNode) = parseContext.intermediate.addLoop((yyvsp[(7) - (7)].interm.intermNode), (yyvsp[(4) - (7)].interm.intermNode), reinterpret_cast<TIntermTyped*>((yyvsp[(5) - (7)].interm.nodePair).node1), reinterpret_cast<TIntermTyped*>((yyvsp[(5) - (7)].interm.nodePair).node2), LOOP_FOR, (yyvsp[(1) - (7)].lex).range, parseContext.extensionChanged);
        --parseContext.loopNestingLevel;
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (7)].lex).range, (yyvsp[(6) - (7)].lex).range));
    }
    break;

  case 252:
/* Line 1787 of yacc.c  */
#line 3522 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); 
    }
    break;

  case 253:
/* Line 1787 of yacc.c  */
#line 3525 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode);
    }
    break;

  case 254:
/* Line 1787 of yacc.c  */
#line 3531 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = (yyvsp[(1) - (1)].interm.intermTypedNode); 
    }
    break;

  case 255:
/* Line 1787 of yacc.c  */
#line 3534 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermTypedNode) = 0; 
    }
    break;

  case 256:
/* Line 1787 of yacc.c  */
#line 3540 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.nodePair).node1 = (yyvsp[(1) - (2)].interm.intermTypedNode);
        (yyval.interm.nodePair).node2 = 0;
    }
    break;

  case 257:
/* Line 1787 of yacc.c  */
#line 3544 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.nodePair).node1 = (yyvsp[(1) - (3)].interm.intermTypedNode);
        (yyval.interm.nodePair).node2 = (yyvsp[(3) - (3)].interm.intermTypedNode);
    }
    break;

  case 258:
/* Line 1787 of yacc.c  */
#line 3551 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.loopNestingLevel <= 0) {
            parseContext.error((yyvsp[(1) - (2)].lex).range, "continue statement only allowed in loops", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }        
        (yyval.interm.intermNode) = parseContext.intermediate.addBranch(EOpContinue, (yyvsp[(1) - (2)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 259:
/* Line 1787 of yacc.c  */
#line 3559 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        if (parseContext.loopNestingLevel <= 0 &&
            parseContext.switchNestingLevel <= 0) {
            parseContext.error((yyvsp[(1) - (2)].lex).range, "break statement only allowed in loops and switches", "", "");
            parseContext.recover(__FILE__, __LINE__);
        }        
        (yyval.interm.intermNode) = parseContext.intermediate.addBranch(EOpBreak, (yyvsp[(1) - (2)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 260:
/* Line 1787 of yacc.c  */
#line 3568 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = parseContext.intermediate.addBranch(EOpReturn, (yyvsp[(1) - (2)].lex).range, parseContext.extensionChanged);
        if (parseContext.currentFunctionType->getBasicType() != EbtVoid) {
            parseContext.error((yyvsp[(1) - (2)].lex).range, "non-void function must return a value", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 261:
/* Line 1787 of yacc.c  */
#line 3576 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {        
        (yyval.interm.intermNode) = parseContext.intermediate.addBranch(EOpReturn, (yyvsp[(2) - (3)].interm.intermTypedNode), (yyvsp[(1) - (3)].lex).range, parseContext.extensionChanged);
        parseContext.functionReturnsValue = true;
        if (parseContext.currentFunctionType->getBasicType() == EbtVoid) {
            parseContext.error((yyvsp[(1) - (3)].lex).range, "void function cannot return a value", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        } else if (*(parseContext.currentFunctionType) != (yyvsp[(2) - (3)].interm.intermTypedNode)->getType()) {
            parseContext.error((yyvsp[(1) - (3)].lex).range, "function return is not matching type:", "return", "");
            parseContext.recover(__FILE__, __LINE__);
        }
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (3)].lex).range, (yyvsp[(3) - (3)].lex).range));
    }
    break;

  case 262:
/* Line 1787 of yacc.c  */
#line 3588 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        FRAG_ONLY("discard", (yyvsp[(1) - (2)].lex).range);
        (yyval.interm.intermNode) = parseContext.intermediate.addBranch(EOpKill, (yyvsp[(1) - (2)].lex).range, parseContext.extensionChanged);
        if ((yyval.interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].lex).range, (yyvsp[(2) - (2)].lex).range));
    }
    break;

  case 263:
/* Line 1787 of yacc.c  */
#line 3598 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode);
        parseContext.treeRoot = (yyval.interm.intermNode); 
    }
    break;

  case 264:
/* Line 1787 of yacc.c  */
#line 3602 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        (yyval.interm.intermNode) = parseContext.intermediate.growAggregate((yyvsp[(1) - (2)].interm.intermNode), (yyvsp[(2) - (2)].interm.intermNode), parseContext.extensionChanged);
        if ((yyval.interm.intermNode) && (yyvsp[(1) - (2)].interm.intermNode) && (yyvsp[(2) - (2)].interm.intermNode)) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (2)].interm.intermNode)->getRange(), (yyvsp[(2) - (2)].interm.intermNode)->getRange()));
        parseContext.treeRoot = (yyval.interm.intermNode);
    }
    break;

  case 265:
/* Line 1787 of yacc.c  */
#line 3610 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); 
    }
    break;

  case 266:
/* Line 1787 of yacc.c  */
#line 3613 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    { 
        (yyval.interm.intermNode) = (yyvsp[(1) - (1)].interm.intermNode); 
    }
    break;

  case 267:
/* Line 1787 of yacc.c  */
#line 3619 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        TFunction& function = *((yyvsp[(1) - (1)].interm).function);
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
            parseContext.error((yyvsp[(1) - (1)].interm).range, "function already has a body", function.getName().c_str(), "");
            parseContext.recover(__FILE__, __LINE__);
        }
        prevDec->setDefined();
        
        //
        // Raise error message if main function takes any parameters or return anything other than void
        //
        if (function.getName() == "main") {
            if (function.getParamCount() > 0) {
                parseContext.error((yyvsp[(1) - (1)].interm).range, "function cannot take any parameter(s)", function.getName().c_str(), "");
                parseContext.recover(__FILE__, __LINE__);
            }
            if (function.getReturnType().getBasicType() != EbtVoid) {
                parseContext.error((yyvsp[(1) - (1)].interm).range, "", function.getReturnType().getBasicString(), "main function cannot return a value");
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
                    parseContext.error((yyvsp[(1) - (1)].interm).range, "redefinition", variable->getName().c_str(), "");
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
                                                               variable->getType(), (yyvsp[(1) - (1)].interm).range,
                                                               parseContext.extensionChanged),
                        parseContext.extensionChanged);
            } else {
                paramNodes = parseContext.intermediate.growAggregate(
                        paramNodes, 
                        parseContext.intermediate.addFuncParam(0, "", *param.type, (yyvsp[(1) - (1)].interm).range,
                                                               parseContext.extensionChanged),
                        parseContext.extensionChanged);
            }
        }
        parseContext.intermediate.setAggregateOperator(paramNodes, EOpParameters, (yyvsp[(1) - (1)].interm).range, parseContext.extensionChanged);
        (yyvsp[(1) - (1)].interm).intermAggregate = paramNodes;
        parseContext.loopNestingLevel = 0;
    }
    break;

  case 268:
/* Line 1787 of yacc.c  */
#line 3709 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"
    {
        //?? Check that all paths return a value if return type != void ?
        //   May be best done as post process phase on intermediate code
        if (parseContext.currentFunctionType->getBasicType() != EbtVoid && ! parseContext.functionReturnsValue) {
            parseContext.error((yyvsp[(1) - (3)].interm).range, "function does not return a value:", "", (yyvsp[(1) - (3)].interm).function->getName().c_str());
            parseContext.recover(__FILE__, __LINE__);
        }
        parseContext.symbolTable.pop();
        

        TIntermNode *body = NULL;
        // Add dummy node at the end of 'main' body for debugging the last statement
        if ((yyvsp[(1) - (3)].interm).function->getName() == "main") {
            if ((yyvsp[(3) - (3)].interm.intermNode) != 0) {
                if ((yyvsp[(3) - (3)].interm.intermNode)->getAsAggregate()) {
                    // Add dummy node to end of aggregate
                    TIntermDummy *dummy = parseContext.intermediate.addDummy((yyvsp[(3) - (3)].interm.intermNode)->getRange(), parseContext.extensionChanged);
                    (yyvsp[(3) - (3)].interm.intermNode)->getAsAggregate()->setOperator(EOpNull);
                    body = parseContext.intermediate.growAggregate((yyvsp[(3) - (3)].interm.intermNode)->getAsAggregate(), dummy, parseContext.extensionChanged);
                    body->getAsAggregate()->setOperator(EOpSequence);
                } else {
                    // UNTESTED
                    TIntermDummy *dummy = parseContext.intermediate.addDummy((yyvsp[(3) - (3)].interm.intermNode)->getRange(), parseContext.extensionChanged);
                    body = parseContext.intermediate.growAggregate((yyvsp[(3) - (3)].interm.intermNode), dummy, parseContext.extensionChanged);
                    body->getAsAggregate()->setOperator(EOpSequence);
                }
            } else {
                // UNTESTED
                TIntermDummy *dummy = parseContext.intermediate.addDummy((yyvsp[(3) - (3)].interm.intermNode)->getRange(), parseContext.extensionChanged);
                body = parseContext.intermediate.growAggregate((yyvsp[(3) - (3)].interm.intermNode), dummy, parseContext.extensionChanged);
                body->getAsAggregate()->setOperator(EOpSequence);
            }
        } else {
            if ((yyvsp[(3) - (3)].interm.intermNode)) {
                body = (yyvsp[(3) - (3)].interm.intermNode);
            } else {
                body = new TIntermAggregate;
                body->getAsAggregate()->setOperator(EOpSequence);
            }
        }
        
        (yyval.interm.intermNode) = parseContext.intermediate.growAggregate((yyvsp[(1) - (3)].interm).intermAggregate, body, parseContext.extensionChanged);
        parseContext.intermediate.setAggregateOperator((yyval.interm.intermNode), EOpFunction, (yyvsp[(1) - (3)].interm).range, parseContext.extensionChanged);
        (yyval.interm.intermNode)->getAsAggregate()->setName((yyvsp[(1) - (3)].interm).function->getMangledName().c_str());
        (yyval.interm.intermNode)->getAsAggregate()->setType((yyvsp[(1) - (3)].interm).function->getReturnType());
        
        // store the pragma information for debug and optimize and other vendor specific 
        // information. This information can be queried from the parse tree
        (yyval.interm.intermNode)->getAsAggregate()->setOptimize(parseContext.contextPragma.optimize);
        (yyval.interm.intermNode)->getAsAggregate()->setDebug(parseContext.contextPragma.debug);
        (yyval.interm.intermNode)->getAsAggregate()->addToPragmaTable(parseContext.contextPragma.pragmaTable);
        if ((yyval.interm.intermNode) && body) (yyval.interm.intermNode)->setRange(addRange((yyvsp[(1) - (3)].interm).range, body->getRange()));
    }
    break;


/* Line 1787 of yacc.c  */
#line 7108 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2050 of yacc.c  */
#line 3764 "/media/sda9/projects/GLSL-Debugger/GLSLCompiler/glslang/MachineIndependent/glslang.y"

