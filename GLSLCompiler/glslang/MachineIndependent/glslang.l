/*
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
*/
/* Based on
ANSI C grammar, Lex specification

In 1985, Jeff Lee published this Lex specification together with a Yacc 
grammar for the April 30, 1985 ANSI C draft.  Tom Stockfisch reposted 
both to net.sources in 1987; that original, as mentioned in the answer 
to question 17.25 of the comp.lang.c FAQ, can be ftp'ed from ftp.uu.net, 
file usenet/net.sources/ansi.c.grammar.Z. 

I intend to keep this version as close to the current C Standard grammar 
as possible; please let me know if you discover discrepancies. 

Jutta Degener, 1995 
*/

D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
E           [Ee][+-]?{D}+
O           [0-7]
FS          (f|F)
US          (u|U)

%option nounput
%top{
/* begin top block */
#include <stdio.h>
#include <stdlib.h>
#include "ParseHelper.h"
#include "generated_glsl_parser.hpp"
extern YYSTYPE yylval;

/* windows only pragma */
#ifdef _MSC_VER
#pragma warning(disable : 4102)
#endif

typedef enum {
    YY_TOKEN_LEFT,
    YY_TOKEN_RIGHT
} YYTOKENPOS;

int yy_input(char* buf, int max_size);

TSourceLoc yylinenum;
int yycursor;
int yytokenpos[2];

//#ifdef _WIN32
//    extern int yyparse(TParseContext&);
//    #define YY_DECL int yylex(YYSTYPE* pyylval, TParseContext& parseContext)    
//#else
    extern int yyparse(void*);
    #define YY_DECL int yylex(YYSTYPE* pyylval, void* parseContextLocal)
    #define parseContext (*((TParseContext*)(parseContextLocal)))		
//#endif
 
#define YY_INPUT(buf,result,max_size) (result = yy_input(buf, max_size))

#define PREP_PPYLVAL(A) {\
                          pyylval->lex.range.left.line   = yylinenum; \
                          pyylval->lex.range.left.colum  = yytokenpos[YY_TOKEN_LEFT]; \
                          pyylval->lex.range.right.line  = yylinenum; \
                          pyylval->lex.range.right.colum = yytokenpos[YY_TOKEN_RIGHT]; \
                        }
/* end top block */
}

%option noyywrap
%option never-interactive
%x FIELDS

%%
<*>"//"[^\n]*"\n"     { /* ?? carriage and/or line-feed? */ };

"attribute"    {  PREP_PPYLVAL("attribute"); return(ATTRIBUTE); }
"unsigned"     {  PREP_PPYLVAL("unsigned");  return(UNSIGNED); }
"const"        {  PREP_PPYLVAL("const");     return(CONST_QUAL); }
"uniform"      {  PREP_PPYLVAL("uniform");   return(UNIFORM); }
"varying"      {  PREP_PPYLVAL("varying");   return(VARYING); }

"invariant"    {  PREP_PPYLVAL("invariant");          return(INVARIANT); }
"centroid"     {  PREP_PPYLVAL("centroid");           return(CENTROID); }
"flat"         {  PREP_PPYLVAL("flat");               return(FLAT); }
"noperspective" {  PREP_PPYLVAL("noperspective");      return(NOPERSPECTIVE); }

"break"        {  PREP_PPYLVAL("break");     return(BREAK); }
"continue"     {  PREP_PPYLVAL("continue");  return(CONTINUE); }
"do"           {  PREP_PPYLVAL("do");        return(DO); }
"for"          {  PREP_PPYLVAL("for");       return(FOR); }
"while"        {  PREP_PPYLVAL("while");     return(WHILE); }

"if"           {  PREP_PPYLVAL("if");        return(IF); }
"else"         {  PREP_PPYLVAL("else");      return(ELSE); }

"in"           {  PREP_PPYLVAL("in");        return(IN_QUAL); }
"out"          {  PREP_PPYLVAL("out");       return(OUT_QUAL); }
"inout"        {  PREP_PPYLVAL("inout");     return(INOUT_QUAL); }

"float"        {  PREP_PPYLVAL("float"); parseContext.lexAfterType = true; return(FLOAT_TYPE); }
"int"          {  PREP_PPYLVAL("int");   parseContext.lexAfterType = true; return(INT_TYPE); }
"void"         {  PREP_PPYLVAL("void");  parseContext.lexAfterType = true; return(VOID_TYPE); }
"bool"         {  PREP_PPYLVAL("bool");  parseContext.lexAfterType = true; return(BOOL_TYPE); }
"true"         {  PREP_PPYLVAL("true");  pyylval->lex.b = true;  return(BOOLCONSTANT); }
"false"        {  PREP_PPYLVAL("false"); pyylval->lex.b = false; return(BOOLCONSTANT); }

"discard"      {  PREP_PPYLVAL("discard"); return(DISCARD); }
"return"       {  PREP_PPYLVAL("return");  return(RETURN); }

"mat2"         {  PREP_PPYLVAL("mat2"); parseContext.lexAfterType = true; return(MATRIX2); }
"mat3"         {  PREP_PPYLVAL("mat3"); parseContext.lexAfterType = true; return(MATRIX3); }
"mat4"         {  PREP_PPYLVAL("mat4"); parseContext.lexAfterType = true; return(MATRIX4); }

 /*GLSL 1.20: Support non-square matrices */
"mat2x2"       {  PREP_PPYLVAL("mat2x2"); parseContext.lexAfterType = true; return(MATRIX2X2); }
"mat2x3"       {  PREP_PPYLVAL("mat2x3"); parseContext.lexAfterType = true; return(MATRIX2X3); }
"mat2x4"       {  PREP_PPYLVAL("mat2x4"); parseContext.lexAfterType = true; return(MATRIX2X4); }
"mat3x2"       {  PREP_PPYLVAL("mat3x2"); parseContext.lexAfterType = true; return(MATRIX3X2); }
"mat3x3"       {  PREP_PPYLVAL("mat3x3"); parseContext.lexAfterType = true; return(MATRIX3X3); }
"mat3x4"       {  PREP_PPYLVAL("mat3x4"); parseContext.lexAfterType = true; return(MATRIX3X4); }
"mat4x2"       {  PREP_PPYLVAL("mat4x2"); parseContext.lexAfterType = true; return(MATRIX4X2); }
"mat4x3"       {  PREP_PPYLVAL("mat4x3"); parseContext.lexAfterType = true; return(MATRIX4X3); }
"mat4x4"       {  PREP_PPYLVAL("mat4x4"); parseContext.lexAfterType = true; return(MATRIX4X4); }

"vec2"         {  PREP_PPYLVAL("vec2"); parseContext.lexAfterType = true; return (VEC2); }
"vec3"         {  PREP_PPYLVAL("vec3"); parseContext.lexAfterType = true; return (VEC3); }
"vec4"         {  PREP_PPYLVAL("vec4"); parseContext.lexAfterType = true; return (VEC4); }
"ivec2"        {  PREP_PPYLVAL("ivec2"); parseContext.lexAfterType = true; return (IVEC2); }
"ivec3"        {  PREP_PPYLVAL("ivec3"); parseContext.lexAfterType = true; return (IVEC3); }
"ivec4"        {  PREP_PPYLVAL("ivec4"); parseContext.lexAfterType = true; return (IVEC4); }
"uvec2"        {  PREP_PPYLVAL("uvec2"); parseContext.lexAfterType = true; return (UVEC2); }
"uvec3"        {  PREP_PPYLVAL("uvec3"); parseContext.lexAfterType = true; return (UVEC3); }
"uvec4"        {  PREP_PPYLVAL("uvec4"); parseContext.lexAfterType = true; return (UVEC4); }
"bvec2"        {  PREP_PPYLVAL("bvec2"); parseContext.lexAfterType = true; return (BVEC2); }
"bvec3"        {  PREP_PPYLVAL("bvec3"); parseContext.lexAfterType = true; return (BVEC3); }
"bvec4"        {  PREP_PPYLVAL("bvec4"); parseContext.lexAfterType = true; return (BVEC4); }

"sampler1D"       {  PREP_PPYLVAL("sampler1D"); parseContext.lexAfterType = true; return SAMPLER1D; }
"sampler2D"       {  PREP_PPYLVAL("sampler2D"); parseContext.lexAfterType = true; return SAMPLER2D; }
"sampler3D"       {  PREP_PPYLVAL("sampler3D"); parseContext.lexAfterType = true; return SAMPLER3D; }
"samplerCube"     {  PREP_PPYLVAL("samplerCube"); parseContext.lexAfterType = true; return SAMPLERCUBE; }
"sampler1DShadow" {  PREP_PPYLVAL("sampler1DShadow"); parseContext.lexAfterType = true; return SAMPLER1DSHADOW; }
"sampler2DShadow" {  PREP_PPYLVAL("sampler2DShadow"); parseContext.lexAfterType = true; return SAMPLER2DSHADOW; }

 /* EXT_gpu_shader4 */
"isampler1D"      {  PREP_PPYLVAL("isampler1D"); parseContext.lexAfterType = true; return ISAMPLER1D; }
"usampler1D"      {  PREP_PPYLVAL("usampler1D"); parseContext.lexAfterType = true; return USAMPLER1D; }
"isampler2D"      {  PREP_PPYLVAL("isampler2D"); parseContext.lexAfterType = true; return ISAMPLER2D; }
"usampler2D"      {  PREP_PPYLVAL("usampler2D"); parseContext.lexAfterType = true; return USAMPLER2D; }
"isampler3D"      {  PREP_PPYLVAL("isampler3D"); parseContext.lexAfterType = true; return ISAMPLER3D; }
"usampler3D"      {  PREP_PPYLVAL("usampler3D"); parseContext.lexAfterType = true; return USAMPLER3D; }
"isamplerCube"    {  PREP_PPYLVAL("isamplerCube"); parseContext.lexAfterType = true; return ISAMPLERCUBE; }
"usamplerCube"    {  PREP_PPYLVAL("usamplerCube"); parseContext.lexAfterType = true; return USAMPLERCUBE; }
"isampler2DRect"  {  PREP_PPYLVAL("isampler2DRect"); parseContext.lexAfterType = true; return ISAMPLER2DRECT; }
"usampler2DRect"  {  PREP_PPYLVAL("usampler2DRect"); parseContext.lexAfterType = true; return USAMPLER2DRECT; }
"sampler1DArray"  {  PREP_PPYLVAL("sampler1DArray"); parseContext.lexAfterType = true; return SAMPLER1DARRAY; }
"isampler1DArray" {  PREP_PPYLVAL("isampler1DArray"); parseContext.lexAfterType = true; return ISAMPLER1DARRAY; }
"usampler1DArray" {  PREP_PPYLVAL("usampler1DArray"); parseContext.lexAfterType = true; return USAMPLER1DARRAY; }
"sampler2DArray"  {  PREP_PPYLVAL("sampler2DArray"); parseContext.lexAfterType = true; return SAMPLER2DARRAY; }
"isampler2DArray" {  PREP_PPYLVAL("isampler2DArray"); parseContext.lexAfterType = true; return ISAMPLER2DARRAY; }
"usampler2DArray" {  PREP_PPYLVAL("usampler2DArray"); parseContext.lexAfterType = true; return USAMPLER2DARRAY; }
"samplerBuffer"   {  PREP_PPYLVAL("samplerBuffer"); parseContext.lexAfterType = true; return SAMPLERBUFFER; }
"isamplerBuffer"  {  PREP_PPYLVAL("isamplerBuffer"); parseContext.lexAfterType = true; return ISAMPLERBUFFER; }
"usamplerBuffer"  {  PREP_PPYLVAL("usamplerBuffer"); parseContext.lexAfterType = true; return USAMPLERBUFFER; }
"sampler1DArrayShadow"  {  PREP_PPYLVAL("sampler1DArrayShadow"); parseContext.lexAfterType = true; return SAMPLER1DARRAYSHADOW; }
"sampler2DArrayShadow"  {  PREP_PPYLVAL("sampler2DArrayShadow"); parseContext.lexAfterType = true; return SAMPLER2DARRAYSHADOW; }
"samplerCubeShadow"     {  PREP_PPYLVAL("samplerCubeShadow"); parseContext.lexAfterType = true; return SAMPLERCUBESHADOW; }




"sampler2DRect"        { PREP_PPYLVAL("sampler2DRect"); parseContext.lexAfterType = true; 
                         return SAMPLER2DRECTARB; /* ARB_texture_rectangle */ }
"sampler2DRectShadow"  { PREP_PPYLVAL("sampler2DRectShadow"); parseContext.lexAfterType = true; 
                         return SAMPLER2DRECTSHADOWARB; /* ARB_texture_rectangle */ }

"struct"       {  PREP_PPYLVAL("struct"); return(STRUCT); }

"asm"          {  PaReservedWord(); return 0; }

"class"        {  PaReservedWord(); return 0; }
"union"        {  PaReservedWord(); return 0; }
"enum"         {  PaReservedWord(); return 0; }
"typedef"      {  PaReservedWord(); return 0; }
"template"     {  PaReservedWord(); return 0; }
"this"         {  PaReservedWord(); return 0; }
"packed"       {  PaReservedWord(); return 0; }

"goto"         {  PaReservedWord(); return 0; }

"switch"       {  PREP_PPYLVAL("switch"); return(SWITCH); }
"case"         {  PREP_PPYLVAL("case"); return(CASE); }
"default"      {  PREP_PPYLVAL("default"); return (DEFAULT); }

"inline"       {  PaReservedWord(); return 0; }
"noinline"     {  PaReservedWord(); return 0; }
"volatile"     {  PaReservedWord(); return 0; }
"public"       {  PaReservedWord(); return 0; }
"static"       {  PaReservedWord(); return 0; }
"extern"       {  PaReservedWord(); return 0; }
"external"     {  PaReservedWord(); return 0; }
"interface"    {  PaReservedWord(); return 0; }

"long"         {  PaReservedWord(); return 0; }
"short"        {  PaReservedWord(); return 0; }
"double"       {  PaReservedWord(); return 0; }
"half"         {  PaReservedWord(); return 0; }
"fixed"        {  PaReservedWord(); return 0; }

"input"        {  PaReservedWord(); return 0; }
"output"       {  PaReservedWord(); return 0; }

"hvec2"        {  PaReservedWord(); return 0; }
"hvec3"        {  PaReservedWord(); return 0; }
"hvec4"        {  PaReservedWord(); return 0; }
"fvec2"        {  PaReservedWord(); return 0; }
"fvec3"        {  PaReservedWord(); return 0; }
"fvec4"        {  PaReservedWord(); return 0; }
"dvec2"        {  PaReservedWord(); return 0; }
"dvec3"        {  PaReservedWord(); return 0; }
"dvec4"        {  PaReservedWord(); return 0; }

"sampler3DRect"        {  PaReservedWord(); return 0; }

"sizeof"       {  PaReservedWord(); return 0; }
"cast"         {  PaReservedWord(); return 0; }

"namespace"    {  PaReservedWord(); return 0; }
"using"        {  PaReservedWord(); return 0; }

"lowp"         {  PaReservedWord(); return 0; }
"mediump"      {  PaReservedWord(); return 0; }
"highp"        {  PaReservedWord(); return 0; }
"precision"    {  PaReservedWord(); return 0; }

{L}({L}|{D})*       {
   PREP_PPYLVAL("PaIdentOrType");
   pyylval->lex.string = NewPoolTString(yytext); 
   return PaIdentOrType(*pyylval->lex.string, parseContext, pyylval->lex.symbol); 
}

 /* GLSL: 1.20 const ints */
0[xX]{H}+{US}?    { PREP_PPYLVAL("INTCONSTANT"); 
                    if (yytext[strlen(yytext)-1] == 'u' || yytext[strlen(yytext)-1] == 'U') {
						pyylval->lex.ui = strtoul(yytext, 0, 16);
                        return(UINTCONSTANT); 
                    } else {
						pyylval->lex.i = strtol(yytext, 0, 16);
                        return(INTCONSTANT); 
                    }
                  }
0{O}+{US}?        { PREP_PPYLVAL("INTCONSTANT"); 
                    if (yytext[strlen(yytext)-1] == 'u' || yytext[strlen(yytext)-1] == 'U') {
						pyylval->lex.ui = strtoul(yytext, 0, 8);
                        return(UINTCONSTANT); 
                    } else {
						pyylval->lex.i = strtol(yytext, 0, 8);
                        return(INTCONSTANT); 
                    }
                  }
0{D}+{US}?        { TSourceRange range;
                    range.left.line = yylinenum;
                    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
                    range.right.line = yylinenum;
                    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
                    parseContext.error(range, "Invalid Octal number.", yytext, "", ""); 
                    parseContext.recover(__FILE__, __LINE__); return 0; 
                  }
{D}+{US}?         { PREP_PPYLVAL("INTCONSTANT"); 
                    if (yytext[strlen(yytext)-1] == 'u' || yytext[strlen(yytext)-1] == 'U') {
						pyylval->lex.ui = strtoul(yytext, 0, 0); 
                        return(UINTCONSTANT); 
                    } else {
						pyylval->lex.i = strtol(yytext, 0, 0); 
                        return(INTCONSTANT); 
                    }
                  }

 /* GLSL 1.20: Accept 'f' as part of a floating-point constants */
{D}+{E}{FS}?           { PREP_PPYLVAL("FLOATCONSTANT"); pyylval->lex.f = static_cast<float>(atof(yytext)); 
                         return(FLOATCONSTANT); }
{D}+"."{D}*({E})?{FS}? { PREP_PPYLVAL("FLOATCONSTANT"); pyylval->lex.f = static_cast<float>(atof(yytext)); 
                         return(FLOATCONSTANT); }
"."{D}+({E})?{FS}?     { PREP_PPYLVAL("FLOATCONSTANT"); pyylval->lex.f = static_cast<float>(atof(yytext)); 
                         return(FLOATCONSTANT); }

"/*"            {  int ret = PaParseComment(pyylval->lex.range.left.line, parseContext); if (!ret) return ret; }   

"+="            {  PREP_PPYLVAL("+="); return(ADD_ASSIGN); }
"-="            {  PREP_PPYLVAL("-="); return(SUB_ASSIGN); }
"*="            {  PREP_PPYLVAL("*="); return(MUL_ASSIGN); }
"/="            {  PREP_PPYLVAL("/="); return(DIV_ASSIGN); }
"%="            {  PREP_PPYLVAL("%%="); return(MOD_ASSIGN); }
"<<="           {  PREP_PPYLVAL("<<="); return(LEFT_ASSIGN); }
">>="           {  PREP_PPYLVAL(">>="); return(RIGHT_ASSIGN); }
"&="            {  PREP_PPYLVAL("&="); return(AND_ASSIGN); }
"^="            {  PREP_PPYLVAL("^="); return(XOR_ASSIGN); }
"|="            {  PREP_PPYLVAL("|="); return(OR_ASSIGN); }

"++"            {  PREP_PPYLVAL("++"); return(INC_OP); }
"--"            {  PREP_PPYLVAL("--"); return(DEC_OP); }
"&&"            {  PREP_PPYLVAL("&&"); return(AND_OP); }
"||"            {  PREP_PPYLVAL("||"); return(OR_OP); }
"^^"            {  PREP_PPYLVAL("^^"); return(XOR_OP); }
"<="            {  PREP_PPYLVAL("<="); return(LE_OP); }
">="            {  PREP_PPYLVAL(">="); return(GE_OP); }
"=="            {  PREP_PPYLVAL("=="); return(EQ_OP); }
"!="            {  PREP_PPYLVAL("!="); return(NE_OP); }
"<<"            {  PREP_PPYLVAL("<<"); return(LEFT_OP); }
">>"            {  PREP_PPYLVAL(">>"); return(RIGHT_OP); }
";"         {  PREP_PPYLVAL(";"); parseContext.lexAfterType = false; return(SEMICOLON); }
("{"|"<%")      {  PREP_PPYLVAL("{|<%%"); parseContext.lexAfterType = false; return(LEFT_BRACE); }
("}"|"%>")      {  PREP_PPYLVAL("}|%%>"); return(RIGHT_BRACE); }
","         {  PREP_PPYLVAL(","); if (parseContext.inTypeParen) parseContext.lexAfterType = false; return(COMMA); }
":"         {  PREP_PPYLVAL(":"); return(COLON); }
"="         {  PREP_PPYLVAL("="); parseContext.lexAfterType = false; return(EQUAL); }
"("         {  PREP_PPYLVAL("("); parseContext.lexAfterType = false; parseContext.inTypeParen = true; return(LEFT_PAREN); }
")"         {  PREP_PPYLVAL(")"); parseContext.inTypeParen = false; return(RIGHT_PAREN); }
("["|"<:")      {  PREP_PPYLVAL("[|<:"); return(LEFT_BRACKET); }
("]"|":>")      {  PREP_PPYLVAL("]|:>"); return(RIGHT_BRACKET); }
"."         { BEGIN(FIELDS);  return(DOT); }
"!"         {  PREP_PPYLVAL("!"); return(BANG); }
"-"         {  PREP_PPYLVAL("-"); return(DASH); }
"~"         {  PREP_PPYLVAL("~"); return(TILDE); }
"+"         {  PREP_PPYLVAL("+"); return(PLUS); }
"*"         {  PREP_PPYLVAL("*"); return(STAR); }
"/"         {  PREP_PPYLVAL("/"); return(SLASH); }
"%"         {  PREP_PPYLVAL("%%"); return(PERCENT); }
"<"         {  PREP_PPYLVAL("<"); return(LEFT_ANGLE); }
">"         {  PREP_PPYLVAL(">"); return(RIGHT_ANGLE); }
"|"         {  PREP_PPYLVAL("|"); return(VERTICAL_BAR); }
"^"         {  PREP_PPYLVAL("^"); return(CARET); }
"&"         {  PREP_PPYLVAL("&"); return(AMPERSAND); }
"?"         {  PREP_PPYLVAL("?"); return(QUESTION); }

<FIELDS>{L}({L}|{D})* { 
BEGIN(INITIAL);      
    PREP_PPYLVAL("FIELD_SELECTION");     
    pyylval->lex.string = NewPoolTString(yytext); 
    return FIELD_SELECTION; }
<FIELDS>[ \t\v\f\r] {}

[ \t\v\n\f\r]   {  }
<*><<EOF>> { (&parseContext)->AfterEOF = true; yy_delete_buffer(YY_CURRENT_BUFFER); yyterminate();}
<*>.    { parseContext.infoSink.info << "FLEX: Unknown char " << yytext << "\n";
          return 0; }

%%

//Including Pre-processor.
extern "C" {
  #include "./preprocessor/preprocess.h"
}
//
// The YY_INPUT macro just calls this.  Maybe this could be just put into
// the macro directly.
//

int yy_input(char* buf, int max_size)
{
     int len;

    if ((len = yylex_CPP(buf, max_size)) == 0)
        return 0;
    if (len >= max_size) 
        YY_FATAL_ERROR( "input buffer overflow, can't enlarge buffer because scanner uses REJECT" );

    buf[len] = ' ';
	return len+1;
}


//
// Parse an array of strings using yyparse.  We set up globals used by
// yywrap.
//
// Returns 0 for success, as per yyparse().
//
int PaParseStrings(char* argv[], int strLen[], int argc, TParseContext& parseContextLocal)
{
    int argv0len;
    
    ScanFromString(argv[0]); 
    
    //Storing the Current Compiler Parse context into the cpp structure.
	cpp->pC = (void*)&parseContextLocal;
	
	if (!argv || argc == 0)
        return 1;
    
    for (int i = 0; i < argc; ++i) {
        if (!argv[i]) {
            parseContextLocal.error(TSourceRangeInit, "Null shader source string", "", "");
            parseContextLocal.recover(__FILE__, __LINE__);
            return 1;
        }
    }
    
    if (!strLen) {
        argv0len = (int) strlen(argv[0]);
        strLen   = &argv0len;
    }
    yyrestart(0);
    (&parseContextLocal)->AfterEOF = false;
    cpp->PaWhichStr = 0;
    cpp->PaArgv     = argv;
    cpp->PaArgc     = argc;
    cpp->PaStrLen   = strLen;
    cpp->notAVersionToken = 0;
    yylinenum   = 1;
    yycursor    = 1;
    yytokenpos[YY_TOKEN_LEFT] = 1;
    yytokenpos[YY_TOKEN_RIGHT] = 1;
   
    if (*cpp->PaStrLen >= 0) {    
        //#ifdef _WIN32
        //    yyparse(parseContextLocal);            
        //#else
            yyparse((void*)(&parseContextLocal));
        //#endif
        if (cpp->CompileError == 1 || parseContextLocal.recoveredFromError || parseContextLocal.numErrors > 0) {
            return 1;
        } else {
             return 0;
        }
    }
    else
        return 0;
}
void yyerror(void* /*parseContextLocal*/, const char *s) 
{
    
    TSourceRange range;
    range.left.line = yylinenum;
    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
    range.right.line = yylinenum;
    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
    if (((TParseContext *)cpp->pC)->AfterEOF) {
        if (cpp->tokensBeforeEOF == 1) {
            GlobalParseContext->error(range, "syntax error", "pre-mature EOF", s, "");
            GlobalParseContext->recover(__FILE__, __LINE__);
        }
    } else {
        GlobalParseContext->error(range, "syntax error", yytext, s, "");
        GlobalParseContext->recover(__FILE__, __LINE__);
    }            
}
void PaReservedWord()
{
    TSourceRange range;
    range.left.line = yylinenum;
    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
    range.right.line = yylinenum;
    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
    GlobalParseContext->error(range, "Reserved word.", yytext, "", "");
    GlobalParseContext->recover(__FILE__, __LINE__);
}

int PaIdentOrType(TString& id, TParseContext& parseContextLocal, TSymbol*& symbol)
{
    
    symbol = parseContextLocal.symbolTable.find(id);
    if (parseContextLocal.lexAfterType == false && symbol && symbol->isVariable()) {
        TVariable* variable = static_cast<TVariable*>(symbol);
        if (variable->isUserType()) {
            parseContextLocal.lexAfterType = true;
            return TYPE_NAME;
        }
    }
    
    return IDENTIFIER;
}

int PaParseComment(int &lineno, TParseContext& parseContextLocal)
{
    int transitionFlag = 0;
    int nextChar;
    
    while (transitionFlag != 2) {
        nextChar = yyinput();
        if (nextChar == '\n')
             lineno++;
        switch (nextChar) {
        case '*' :
            transitionFlag = 1;
            break;
        case '/' :  /* if star is the previous character, then it is the end of comment */
            if (transitionFlag == 1) {
                return 1 ;
            }
            break;
        case EOF :
            {
                /* Raise error message here */
                TSourceRange range;
                range.left.line = yylinenum;
                range.left.colum = yytokenpos[YY_TOKEN_LEFT];
                range.right.line = yylinenum;
                range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
                parseContextLocal.error(range, "End of shader found before end of comment.", "", "", "");
                GlobalParseContext->recover(__FILE__, __LINE__);
                return YY_NULL; 
            }
        default :  /* Any other character will be a part of the comment */
            transitionFlag = 0;
        }
    }
    return 1;
}

void CPPDebugLogMsg(const char *msg)
{
    ((TParseContext *)cpp->pC)->infoSink.debug.message(EPrefixNone, msg);
}

void CPPWarningToInfoLog(const char *msg)
{
    TSourceRange range;
    range.left.line = yylinenum;
    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
    range.right.line = yylinenum;
    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
    ((TParseContext *)cpp->pC)->infoSink.info.message(EPrefixWarning, msg, range); 
}

void CPPShInfoLogMsg(const char *msg)
{
    TSourceRange range;
    range.left.line = yylinenum;
    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
    range.right.line = yylinenum;
    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
    ((TParseContext *)cpp->pC)->error(range,"", "",msg,"");
    GlobalParseContext->recover(__FILE__, __LINE__);
}

void CPPErrorToInfoLog(char *msg)
{
    TSourceRange range;
    range.left.line = yylinenum;
    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
    range.right.line = yylinenum;
    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
    ((TParseContext *)cpp->pC)->error(range,"syntax error", "",msg,"");
    GlobalParseContext->recover(__FILE__, __LINE__);
}

void SetLineNumber(int line)
{
    yylinenum &= ~SourceLocLineMask;
    yylinenum |= line;
}

void SetStringNumber(int string)
{
    yylinenum = (string << SourceLocStringShift) | (yylinenum & SourceLocLineMask);
}

int GetStringNumber(void)
{
    return yylinenum >> 16;
}

int GetLineNumber(void)
{
    return yylinenum & SourceLocLineMask;
}

void IncLineNumber(void)
{
    if ((yylinenum & SourceLocLineMask) <= SourceLocLineMask)
        ++yylinenum;
}

void DecLineNumber(void)
{
    if ((yylinenum & SourceLocLineMask) > 0)
        --yylinenum;
}
extern "C" {
void SetCursor(int col)
{
    yycursor = col;
}

int GetCursor(void)
{
    return yycursor;
}

void IncCursor(void)
{
    yycursor++;
}

void DecCursor(void)
{
    yycursor--;
}

void SetTokenPos(YYTOKENPOS t, int col)
{
    yytokenpos[t] = col;
}

int GetTokenPos(YYTOKENPOS t)
{
    return yytokenpos[t];
}

void IncCurserCol(YYTOKENPOS t)
{
    yytokenpos[t]++;
}

void DecCurserCol(YYTOKENPOS t)
{
    yytokenpos[t]--;
}

void setVersionNumber(int version) {
    ((TParseContext *)cpp->pC)->versionNumber = version;
}

} /* extern "C" */

void HandlePragma(const char **tokens, int numTokens)
{    
    if (!strcmp(tokens[0], "optimize")) {
        if (numTokens != 4) {
            CPPShInfoLogMsg("optimize pragma syntax is incorrect");
            return;
        }
        
        if (strcmp(tokens[1], "(")) {
            CPPShInfoLogMsg("\"(\" expected after 'optimize' keyword");
            return;
        }
            
        if (!strcmp(tokens[2], "on"))
            ((TParseContext *)cpp->pC)->contextPragma.optimize = true;
        else if (!strcmp(tokens[2], "off"))
            ((TParseContext *)cpp->pC)->contextPragma.optimize = false;
        else {
            CPPShInfoLogMsg("\"on\" or \"off\" expected after '(' for 'optimize' pragma");
            return;
        }
        
        if (strcmp(tokens[3], ")")) {
            CPPShInfoLogMsg("\")\" expected to end 'optimize' pragma");
            return;
        }
    } else if (!strcmp(tokens[0], "debug")) {
        if (numTokens != 4) {
            CPPShInfoLogMsg("debug pragma syntax is incorrect");
            return;
        }
        
        if (strcmp(tokens[1], "(")) {
            CPPShInfoLogMsg("\"(\" expected after 'debug' keyword");
            return;
        }
            
        if (!strcmp(tokens[2], "on"))
            ((TParseContext *)cpp->pC)->contextPragma.debug = true;
        else if (!strcmp(tokens[2], "off"))
            ((TParseContext *)cpp->pC)->contextPragma.debug = false;
        else {
            CPPShInfoLogMsg("\"on\" or \"off\" expected after '(' for 'debug' pragma");
            return;
        }
        
        if (strcmp(tokens[3], ")")) {
            CPPShInfoLogMsg("\")\" expected to end 'debug' pragma");
            return;
        }
    } else {

#ifdef PRAGMA_TABLE
        //
        // implementation specific pragma
        // use ((TParseContext *)cpp->pC)->contextPragma.pragmaTable to store the information about pragma
        // For now, just ignore the pragma that the implementation cannot recognize
        // An Example of one such implementation for a pragma that has a syntax like
        // #pragma pragmaname(pragmavalue)
        // This implementation stores the current pragmavalue against the pragma name in pragmaTable.
        //        
        if (numTokens == 4 && !strcmp(tokens[1], "(") && !strcmp(tokens[3], ")")) {              
            TPragmaTable& pragmaTable = ((TParseContext *)cpp->pC)->contextPragma.pragmaTable;
            TPragmaTable::iterator iter;
            iter = pragmaTable.find(TString(tokens[0]));
            if (iter != pragmaTable.end()) {
                iter->second = tokens[2];
            } else {
                pragmaTable[tokens[0] ] = tokens[2];
            }        
        } else if (numTokens >= 2) {
            TPragmaTable& pragmaTable = ((TParseContext *)cpp->pC)->contextPragma.pragmaTable;
            TPragmaTable::iterator iter;
            iter = pragmaTable.find(TString(tokens[0]));
            if (iter != pragmaTable.end()) {
                iter->second = tokens[1];
            } else {
                pragmaTable[tokens[0] ] = tokens[1];
            }
        }
#endif // PRAGMA_TABLE
    }
}

void StoreStr(char *string)
{
    TString strSrc;
    strSrc = TString(string);

    ((TParseContext *)cpp->pC)->HashErrMsg = ((TParseContext *)cpp->pC)->HashErrMsg + " " + strSrc;
}

const char* GetStrfromTStr(void)
{
    cpp->ErrMsg = (((TParseContext *)cpp->pC)->HashErrMsg).c_str();
    return cpp->ErrMsg;
}

void ResetTString(void)
{
    ((TParseContext *)cpp->pC)->HashErrMsg = "";
}

TBehavior GetBehavior(const char* behavior)
{
    if (!strcmp("require", behavior))
        return EBhRequire;
    else if (!strcmp("enable", behavior))
        return EBhEnable;
    else if (!strcmp("disable", behavior))
        return EBhDisable;
    else if (!strcmp("warn", behavior))
        return EBhWarn;
    else {
        CPPShInfoLogMsg((TString("behavior '") + behavior + "' is not supported").c_str());
        return EBhDisable;
    }        
}

void  updateExtensionBehavior(const char* extName, const char* behavior)
{
    TBehavior behaviorVal = GetBehavior(behavior);
    TMap<TString, TBehavior>:: iterator iter;
    TString msg;
    
    // special cased for all extension
    if (!strcmp(extName, "all")) {
        if (behaviorVal == EBhRequire || behaviorVal == EBhEnable) {
            CPPShInfoLogMsg("extension 'all' cannot have 'require' or 'enable' behavior");  
            return;
        } else {
            for (iter = ((TParseContext *)cpp->pC)->extensionBehavior.begin(); 
                 iter != ((TParseContext *)cpp->pC)->extensionBehavior.end(); 
                 ++iter) {
                iter->second = behaviorVal;
            }
            ((TParseContext *)cpp->pC)->extensionChanged.push_back(
                TExtensionPair(extName, behaviorVal));
        }
    } else {
        iter = ((TParseContext *)cpp->pC)->extensionBehavior.find(TString(extName));
        if (iter == ((TParseContext *)cpp->pC)->extensionBehavior.end()) {
            switch (behaviorVal) {
            case EBhRequire:
                CPPShInfoLogMsg((TString("extension '") + extName + "' is not supported").c_str());  
                break;
            case EBhEnable:
            case EBhWarn:
            case EBhDisable:
                {
                    TSourceRange range;
                    range.left.line = yylinenum;
                    range.left.colum = yytokenpos[YY_TOKEN_LEFT];
                    range.right.line = yylinenum;
                    range.right.colum = yytokenpos[YY_TOKEN_RIGHT];
                    msg = TString("extension '") + extName + "' is not supported";
                    ((TParseContext *)cpp->pC)->infoSink.info.message(EPrefixWarning, msg.c_str(), range);
                    break;
                }
            }
            return;
        } else {
            iter->second = behaviorVal;
            ((TParseContext *)cpp->pC)->extensionChanged.push_back(
                TExtensionPair(extName, behaviorVal));
        }
    }
}


        

void setInitialState()
{
    yy_start = 1;
}
