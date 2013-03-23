#include "CodeInsertion.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "CodeTools.h"

#include "dbgprint.h"

#define CG_RESULT_PREFIX    "dbgResult"
#define CG_CONDITION_PREFIX "dbgCond"
#define CG_PARAMETER_PREFIX "dbgParam"
#define CG_LOOP_ITER_PREFIX "dbgIter"
#define CG_FUNCTION_POSTFIX "DBG"

#define CG_RANDOMIZED_POSTFIX_SIZE 3

#define CG_FRAGMENT_RESULT  "gl_FragColor"

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) < 0;
          }
};

typedef std::map<const char*, const char*, ltstr> strMap;
typedef std::list<const char*> strList;

static struct {
    ShVariable *result;
    ShVariable *condition;
    ShVariable *parameter;

    strMap  nameMap;
	strList loopIters;
    int     numLoopIters;
} g;


static ShVariable* createDefault(cgTypes type, EShLanguage l)
{
    int i;
    ShVariable *ret;

    ret = (ShVariable*) malloc(sizeof(ShVariable));
    ret->uniqueId   = -1;
    ret->builtin    = 0;
    ret->name       = NULL;
    ret->size       = 1;
    ret->isMatrix   = 0;
    ret->isArray    = 0;
    for (i=0; i<MAX_ARRAYS; i++) {
        ret->arraySize[i]  = 0;
    }
    ret->structName = NULL;
    ret->structSize = 0;
    ret->structSpec = NULL;
    
    switch (type) {
        case CG_TYPE_RESULT:
            ret->type       = SH_FLOAT;
            switch (l) {
                case EShLangVertex:
                case EShLangGeometry:
                    ret->qualifier  = SH_VARYING_OUT;
                    break;
                case EShLangFragment:
                default:
                    ret->qualifier  = SH_TEMPORARY;
                    break;
            }
            return ret;
        case CG_TYPE_CONDITION:
            ret->type       = SH_BOOL;
            ret->qualifier  = SH_TEMPORARY;
            return ret;
        case CG_TYPE_PARAMETER:
            dbgPrint(DBGLVL_WARNING, "CodeInsertion - cannot create default of CG_TYPE_PARAMETER\n");
            return NULL;
        default:
            return NULL;
    }
}

static void getUnusedNameByPrefix(char **name, 
                                  ShVariableList *vl, const char *prefix)
{
    if (!(findFirstShVariableFromName(vl, prefix))) {
        /* name is free to use */
        if (!(*name = (char*)malloc(strlen(prefix)+1))) {
            dbgPrint(DBGLVL_ERROR, "CodeInsertion - not enough memory for result name\n");
            exit(1);
        }
        strcpy(*name, prefix);
    } else {
        /* add randomized post-fix to name until its free */
        if (!(*name = (char*)malloc(strlen(prefix) + 1 +
                        CG_RANDOMIZED_POSTFIX_SIZE))) {
            dbgPrint(DBGLVL_ERROR, "CodeInsertion - not enough memory for result name\n");
            exit(1);
        }
        strcpy(*name, prefix);
        
        while (findFirstShVariableFromName(vl, *name)) {
            char ran;
            int i;
            
            (*name)[strlen(prefix)] = '\0';

            for (i=0; i<CG_RANDOMIZED_POSTFIX_SIZE; i++) {
                ran = (char)((rand()/(float)RAND_MAX)*('Z'-'A')+'A');
                strncat(*name, &ran, 1);
            }
        }
    }
}

void cgGetNewName(char **name, ShVariableList *vl, const char *prefix)
{
    getUnusedNameByPrefix(name, vl, prefix);
}

static void getNewUnusedName(cgTypes type, char **name, ShVariableList *vl, EShLanguage l)
{
    switch (type) {
        case CG_TYPE_RESULT:
            switch (l) {
                case EShLangVertex:
                case EShLangGeometry:
                    if (!(*name = (char*)malloc(strlen(CG_RESULT_PREFIX)+1))) {
                        dbgPrint(DBGLVL_ERROR, "CodeInsertion - not enough memory for result name\n");
                        exit(1);
                    }
                    strcpy(*name, CG_RESULT_PREFIX);
                    break;
                case EShLangFragment:
                    getUnusedNameByPrefix(name, vl, CG_RESULT_PREFIX);
                    break;
                default:
                    break;
            }
            break;
        case CG_TYPE_CONDITION:
            getUnusedNameByPrefix(name, vl, CG_CONDITION_PREFIX);
            break;
        case CG_TYPE_PARAMETER:
            getUnusedNameByPrefix(name, vl, CG_PARAMETER_PREFIX);
            break;
        default:
            break;
    }
}

void cgInit(cgTypes type, ShVariable *src, ShVariableList *vl, EShLanguage l)
{
    ShVariable **var = NULL;

    switch (type) {
        case CG_TYPE_RESULT:
            var = &g.result;
            break;
        case CG_TYPE_CONDITION:
            var = &g.condition;
            if (src) {
                dbgPrint(DBGLVL_WARNING, "CodeInsertion - user defined condition types not supported\n");
            }
            break;
        case CG_TYPE_PARAMETER:
            var = &g.parameter;
            if (!src) {
                dbgPrint(DBGLVL_WARNING, "CodeInsertion - must provide user defined parameter types\n");
            }
            break;
        default:
            return;
    }

    if (*var) cgDestruct(type);
    
    if (!src) {
        *var = createDefault(type, l);
    } else {
        *var = copyShVariable(src);
    }

    /* Assign non-used name */
    getNewUnusedName(type, &((*var)->name), vl, l);

    ShDumpVariable(*var, 1);
}

static const char* getQualifierCode(ShVariable *v, EShLanguage l)
{
    switch (v->qualifier) {
        case SH_VARYING_OUT:
            if (l == EShLangGeometry) {
                return "varying out ";
            } else {
                return "varying ";
            }
        case SH_UNIFORM:
            return "uniform ";
        default:
            return "";
    }
}

static TString getTypeCode(ShVariable *v, bool reduceToScalar = false)
{
    if (v->size == 1 || reduceToScalar) {
        switch (v->type) {
            case SH_FLOAT:
                return "float";
            case SH_INT:
                return "int";
            case SH_UINT:
                return "unsigned int";
            case SH_BOOL:
                return "bool";
            default:
                dbgPrint(DBGLVL_WARNING, "CodeInsertion - queried type of invalid type\n");
                return "";
        }
    } else {
        TString result;
        char *out = (char*)malloc(100);
        switch (v->type) {
            case SH_FLOAT:
                sprintf(out, "vec%i", v->size);
                break;
            case SH_INT:
                sprintf(out, "ivec%i", v->size);
                break;
            case SH_UINT:
                sprintf(out, "uvec%i", v->size);
                return out;
            case SH_BOOL:
                sprintf(out, "bvec%i", v->size);
                break;
            default:
                dbgPrint(DBGLVL_WARNING, "CodeInsertion - queried type of invalid type\n");
                out[0] = '\0';
        }
        result = TString(out);
        free(out);
        return result;
    }
}

void cgAddDeclaration(cgTypes type, TString &prog, EShLanguage l)
{
    switch (type) {
        case CG_TYPE_RESULT:
            if (g.result) {
                prog += getQualifierCode(g.result, l);
                prog += getTypeCode(g.result);
                prog += " ";
                prog += g.result->name;
                prog += ";\n";
            }
            break;
        case CG_TYPE_CONDITION:
            if (g.condition) {
                prog += getQualifierCode(g.condition, l);
                prog += getTypeCode(g.condition);
                prog += " ";
                prog += g.condition->name;
                prog += ";\n";
            }
            break;
        case CG_TYPE_PARAMETER:
            if (g.parameter) {
                prog += getQualifierCode(g.parameter, l);
                prog += getTypeCode(g.parameter);
                prog += " ";
                prog += g.parameter->name;
                if (g.parameter->isArray) {
                    char buf[100];
                    sprintf(buf, "[%i]", g.parameter->arraySize[0]);
                    prog += buf;
                }
                prog += ";\n";
            }
            break;
        case CG_TYPE_LOOP_ITERS:
            for (strList::iterator it = g.loopIters.begin(); it != g.loopIters.end(); it++) {
                prog += "int ";
                prog += *it;
                prog += ";\n";
            }
            break;
        case CG_TYPE_ALL:
            cgAddDeclaration(CG_TYPE_RESULT, prog, l);
            cgAddDeclaration(CG_TYPE_CONDITION, prog, l);
            cgAddDeclaration(CG_TYPE_PARAMETER, prog, l);
            cgAddDeclaration(CG_TYPE_LOOP_ITERS, prog, l);
            break;
    }
}

static void addInitializationCode(cgInitialization init, TString &prog, EShLanguage l)
{
    switch(init) {
        case CG_INIT_BLACK:
            prog += "0.0";
            break;
        case CG_INIT_WHITE:
            prog += "1.0";
            break;
        case CG_INIT_CHESS:
            prog += "(mod(floor(gl_FragCoord.x/8.0), 2.0) == ";
            prog += "mod(floor(gl_FragCoord.y/8.0), 2.0)) ? ";
            prog += "1.0 : 0.8";
            break;
        case CG_INIT_GEOMAP:
            prog += "0.0, 0.0, gl_PrimitiveIDIn";
            break;
    }
}

void cgAddInitialization(cgTypes type, cgInitialization init,
                         TString &prog, EShLanguage l)
{
    switch(type) {
        case CG_TYPE_RESULT:
            prog += g.result->name;
            prog += " = ";
            prog += getTypeCode(g.result);
            break;
        case CG_TYPE_CONDITION:
            prog += g.condition->name;
            prog += " = ";
            prog += getTypeCode(g.condition);
            break;
        case CG_TYPE_PARAMETER:
            prog += g.parameter->name;
            prog += " = ";
            prog += getTypeCode(g.parameter);
            break;
        default:
            break;
    }
    prog += "(";
    addInitializationCode(init, prog, l);
    prog += ")";
}

void cgAddOutput(cgTypes type, TString &prog, EShLanguage l, TQualifier o)
{
    /* TODO: fill out other possibilities */

    switch (l) {
        case EShLangVertex:
            break;
        case EShLangGeometry:
            switch(type) {
                case CG_TYPE_RESULT:
                    prog += "EmitVertex(); ";
                    prog += "EndPrimitive();\n";
                    break;
                default:
                    break;
            }
            break;
        case EShLangFragment:
            switch(type) {
                case CG_TYPE_RESULT:
                    switch (o) {
                        case EvqFragColor:
                            prog += "gl_FragColor.x";
                            break;
                        case EvqFragData:
                            prog += "gl_FragData[0].x";
                            break;
                        default:
                            dbgPrint(DBGLVL_WARNING, "CodeInsertion - no valid output method set for fragment program.\n");
                            dbgPrint(DBGLVL_WARNING, "CodeInsertion - assume gl_FragColor for further usage.\n");
                            prog += "gl_FragColor.x";
                    }
                    prog += " = ";
                    prog += g.result->name;
                    prog += ";\n";
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

char* itoSwizzle(int i)
{
    char* swizzle = new char[2];

    switch(i) {
        case 0: strcpy(swizzle, "x"); break;
        case 1: strcpy(swizzle, "y"); break;
        case 2: strcpy(swizzle, "z"); break;
        case 3: strcpy(swizzle, "w"); break;
    }
    return swizzle;
}

char* itoMultiSwizzle(int i)
{
    int k, j;
    char* swizzle = new char[5];
    swizzle[0] = '\0';
    
    dbgPrint(DBGLVL_COMPILERINFO, "%i\n", i);
    
    for( k = (int)ceil(log10((float)i)), j=0; k>0; k--, j++) {
        int d = (int)(i/pow(10, (float)(k-1)));

        dbgPrint(DBGLVL_COMPILERINFO, "d:%i k:%i i:%i\n", d, k, i);
        
        switch (d-1) {
            case 0: swizzle[j] = 'x'; break;
            case 1: swizzle[j] = 'y'; break;
            case 2: swizzle[j] = 'z'; break;
            case 3: swizzle[j] = 'w'; break;
        }
        i -= (int)(d*(pow(10, (float)(k-1))));
    }
    swizzle[j] = '\0';

    return swizzle;
}

static void addVariableCode(TString &prog, ShChangeable *cgb, ShVariableList *vl)
{
    ShVariable *var;
    
    if (!cgb || !vl) {
        dbgPrint(DBGLVL_ERROR, "CodeInsertion - called getVariableType without valid parameter\n");
        exit(1);
    }
    
    var = findShVariableFromId(vl, cgb->id);

    if (!var) {
        dbgPrint(DBGLVL_ERROR, "CodeInsertion - called getVariableType without valid parameter\n");
        exit(1);
    }

    prog += var->name;

    if (!var->builtin &&
         var->qualifier != SH_VARYING_IN &&
         var->qualifier != SH_VARYING_OUT &&
         var->qualifier != SH_UNIFORM &&
         var->qualifier != SH_ATTRIBUTE) {

        char buf[300];
        sprintf(buf, "_%i", var->uniqueId);
        prog += buf;
    }

    int i;
    for (i=0; i<cgb->numIndices; i++) {
        ShChangeableIndex *idx = cgb->indices[i];

        switch (idx->type) {
            case SH_CGB_ARRAY_INDIRECT:
                prog += "[";
                char buf[200];
                sprintf(buf, "%i", idx->index);
                prog += buf;
                prog += "]";
                break;
            case SH_CGB_ARRAY_DIRECT:
                prog += ".";
                prog += itoSwizzle(idx->index);
                break;
            case SH_CGB_STRUCT:
                prog += ".";
                if (idx->index < var->structSize) {
                    var = var->structSpec[idx->index];
                    prog += var->name;
                } else {
                    dbgPrint(DBGLVL_ERROR, "CodeInsertion - struct and changeable do not match\n");
                    exit(1);
                }
                break;
            case SH_CGB_SWIZZLE:
                prog += ".";
                prog += itoMultiSwizzle(idx->index);
                break;
        }
    }
}

static int getVariableSizeByArrayIndices(ShVariable *var, int numOfArrayIndices)
{
    switch(numOfArrayIndices) {
        case 0:
            if (var->isArray) {
                return var->arraySize[0] * var->size;
            } else if (var->structSpec) {
                return var->structSize;
            } else if (var->isMatrix) {
                /* HINT: glsl1.2 requires change here */
                return var->size*var->size;
            } else {
                return var->size;
            }
        case 1:
            if (var->isArray) {
                if (var->structSpec) {
                    return var->structSize;
                } else {
                    return var->size;
                }
            } else if (var->isMatrix) {
                /* HINT: glsl1.2 requires change here */
                return var->size;
            } else if (var->size > 1) {
                return 1;
            } else {
                dbgPrint(DBGLVL_ERROR, "CodeInsertion - array subscript to a non-array variable\n");
                exit(1);
            }
        case 2:
            if (var->isArray) {
                if (var->isMatrix) {
                    /* HINT: glsl1.2 requires change here */
                    return var->size;
                } else if (var->size > 1) {
                    return 1;
                } else {
                    dbgPrint(DBGLVL_ERROR, "CodeInsertion - array subscript to a non-array variable\n");
                    exit(1);
                }
            } else if(var->isMatrix) {
                return 1;
            } else {
                    dbgPrint(DBGLVL_ERROR, "CodeInsertion - array subscript to a non-array variable\n");
                    exit(1);
            }
        case 3:
            if (var->isArray && var->isMatrix) {
                return 1;
            } else {
                    dbgPrint(DBGLVL_ERROR, "CodeInsertion - array subscript to a non-array variable\n");
                    exit(1);
            }
        default:
            dbgPrint(DBGLVL_ERROR, "CodeInsertion - too many array subscripts (%i)\n", numOfArrayIndices);
            exit(1);
    }
}

static int getShChangeableSize(ShChangeable *cgb, ShVariableList *vl)
{
    int size, i;
    int arraySub = 0;
    ShVariable *var;
    
    if (!cgb || !vl) {
        return 0;
    }
    var = findShVariableFromId(vl, cgb->id);
    if (!var) {
        return 0;
    }

    size = var->size;

    for (i=0; i<cgb->numIndices; i++) {
        ShChangeableIndex *idx = cgb->indices[i];
        switch (idx->type) {
            case SH_CGB_ARRAY_INDIRECT:
                arraySub++;
                size = getVariableSizeByArrayIndices(var, arraySub);
                break;
            case SH_CGB_ARRAY_DIRECT:
                /* TODO: check assumption that size resolves to '1' */
                size = 1;
                break;
            case SH_CGB_STRUCT:
                if (idx->index < var->structSize) {
                    var = var->structSpec[idx->index];
                    arraySub = 0;
                    size = getVariableSizeByArrayIndices(var, arraySub);
                } else {
                    dbgPrint(DBGLVL_ERROR, "CodeInsertion - struct and changeable do not match\n");
                    exit(1);
                }
                break;
            case SH_CGB_SWIZZLE:
                size = (int) ceil(log10((float)idx->index));
                break;
        }
    }
    return size;
}

static void addVariableCodeFromList(TString &prog, ShChangeableList *cgbl, 
                                    ShVariableList *vl, int targetSize)
{
    int id;
    int size = 0;

    if (!cgbl) {
        dbgPrint(DBGLVL_WARNING, "CodeInsertion - no changeable list given to generate code\n");
        return;
    }
    
    for(id=0; id<cgbl->numChangeables; id++) {
        addVariableCode(prog, cgbl->changeables[id], vl);
        size += getShChangeableSize(cgbl->changeables[id], vl);
        
        if (id < (cgbl->numChangeables - 1)) {
            /* Only add seperator if not last item */
            prog += ", ";
        }
    }
    
    if (size > 4) {
        dbgPrint(DBGLVL_WARNING, "CodeInsertion - given changeables exeed single request batch size by %i\n",
                size-4);
    }
    
    for(id=size; id<targetSize; id++) {
        prog += ", ";
        prog += "0.0";
    }
}

static bool hasLoop(TIntermNodeStack *stack) {
    TIntermNodeStack::iterator iter;

    for(iter = stack->begin(); iter != stack->end(); iter++) {
        if ((*iter)->getAsLoopNode() &&
            (*iter)->getAsLoopNode()->needDbgLoopIter()) {
            return true;
        }
    }

    return false;
}

static void addLoopHeader(TString &prog, TIntermNodeStack *stack)
{
    TIntermNodeStack::iterator iter;
    char buf[200];

    if (hasLoop(stack)) {
        prog += "(";

        /* for each loop node inside stack, e.g. dbgPath add condition */
        for(iter = stack->begin(); iter != stack->end(); iter++) {
            if ((*iter)->getAsLoopNode() &&
                    (*iter)->getAsLoopNode()->needDbgLoopIter()) {
                TIntermLoop *ln = (*iter)->getAsLoopNode();
                prog += ln->getDbgIterName();
                prog += "==";
                sprintf(buf, "%i", ln->getDbgIter());
                prog += buf;
                prog += " && ";
            }
        }
        
        prog += "(";
    }
}

static void addLoopFooter(TString &prog, TIntermNodeStack *stack)
{
    if (hasLoop(stack)) {
        prog += ", true))";
    }
}


/* 'option' semantics:
 *     DBG_CG_SELECTION_CONDITIONAL: branch (true or false)
 *     DBG_CG_GEOMETRY_MAP:          EmitVertex or EndPrimitive
 */
void cgAddDbgCode(cgTypes type, TString &prog, DbgCgOptions cgOptions, 
                  ShChangeableList *src, ShVariableList *vl, 
                  TIntermNodeStack *stack, int option, int outPrimType)
{

    /* TODO: fill out other possibilities */
    switch (type) {
        case CG_TYPE_RESULT:
            /* Add additional overhead if inside loop */
            if (cgOptions != DBG_CG_GEOMETRY_CHANGEABLE ||
				(option != CG_GEOM_CHANGEABLE_IN_SCOPE &&
                option != CG_GEOM_CHANGEABLE_NO_SCOPE)) {
                addLoopHeader(prog, stack);
            }
            switch (cgOptions) {
                case DBG_CG_COVERAGE:
                    prog += g.result->name;
                    prog += " = ";
                    prog += getTypeCode(g.result);
                    prog += "(1.0)";
                    break;
                case DBG_CG_SELECTION_CONDITIONAL:
                    prog += g.result->name;
                    prog += " = ";
                    prog += getTypeCode(g.result);
                    prog += "(";
                    prog += option ? "1.0" : "0.5";
                    prog += ")";
                    break;
                case DBG_CG_LOOP_CONDITIONAL:
                    prog += g.result->name;
                    prog += " = ";
                    prog += getTypeCode(g.result);
                    prog += "(";
                    prog += g.condition->name;
                    prog += ")";
                    break;
                case DBG_CG_CHANGEABLE:
                    prog += g.result->name;
                    prog += " = ";
                    prog += getTypeCode(g.result);
                    prog += "(";
                    addVariableCodeFromList(prog, src, vl, getVariableSizeByArrayIndices(g.result, 0));
                    prog += ")";
                    break;
                case DBG_CG_GEOMETRY_MAP:
                    /* option: '0' EmitVertex
                     *         '1' EndPrimitive */
                    prog += g.result->name;
                    if (option) {
                        prog += " = ";
                        prog += getTypeCode(g.result);
                        prog += "(dbgResult.x + 1, 0.0, gl_PrimitiveIDIn)";
                    } else {
                        prog += " = ";
                        prog += getTypeCode(g.result);
                        prog += "(dbgResult.x, dbgResult.y + 1, gl_PrimitiveIDIn)";
                    }
                    break;
                case DBG_CG_GEOMETRY_CHANGEABLE:
                    switch (option) {
                        case CG_GEOM_CHANGEABLE_AT_TARGET:
                            prog += g.result->name;
                            prog += " = ";
                            prog += getTypeCode(g.result);
                            prog += "(";
                            prog += "0.0";
                            prog += ")";
                            break;
                        case CG_GEOM_CHANGEABLE_IN_SCOPE:
                            prog += g.result->name;
                            prog += " = ";
                            prog += getTypeCode(g.result);
                            prog += "(";
                            addVariableCodeFromList(prog, src, vl, 0);
                            prog += ", abs(";
                            prog += g.result->name;
                            prog += ".y))";
                            break;
                        case CG_GEOM_CHANGEABLE_NO_SCOPE:
                            prog += g.result->name;
                            prog += " = ";
                            prog += getTypeCode(g.result);
                            prog += "(0.0";
                            prog += ", -abs(";
                            prog += g.result->name;
                            prog += ".y))";
                            break;
                    }
                    break;
                case DBG_CG_VERTEX_COUNT:
                    /* option: '0' EmitVertex
                     *         '1' EndPrimitive */
                    prog += g.result->name;
                    if (option) {
                        prog += " = ";
                        prog += getTypeCode(g.result);
                        switch (outPrimType) {
                            case 0x0000: /* GL_POINTS */
                                prog += "(dbgResult.y > 0 ? dbgResult.x + 1 : dbgResult.x, 0.0, gl_PrimitiveIDIn)";
                                break;
                            case 0x0003: /* GL_LINE_STRIP */
                                prog += "(dbgResult.y > 1 ? dbgResult.x + dbgResult.y : dbgResult.x, 0.0, gl_PrimitiveIDIn)";
                                break;
                            case 0x0005: /* GL_TRIANGLE_STRIP */
                                prog += "(dbgResult.y > 2 ? dbgResult.x + dbgResult.y : dbgResult.x, 0.0, gl_PrimitiveIDIn)";
                                break;
                        }
                    } else {
                        prog += " = ";
                        prog += getTypeCode(g.result);
                        prog += "(dbgResult.x, dbgResult.y + 1, gl_PrimitiveIDIn)";
                    }
                    break;
                default:
                    break;
            }
            /* Add additional overhead if inside loop */
            if (cgOptions != DBG_CG_GEOMETRY_CHANGEABLE ||
				(option != CG_GEOM_CHANGEABLE_IN_SCOPE &&
                option != CG_GEOM_CHANGEABLE_NO_SCOPE)) {
                addLoopFooter(prog, stack);
            }
            break;
        case CG_TYPE_PARAMETER:
            prog += g.parameter->name;
            break;
        case CG_TYPE_CONDITION:
            prog += g.condition->name;
            break;
        default:
            break;
    }

}

void cgDestruct(cgTypes type) {
    switch (type) {
        case CG_TYPE_RESULT:
            freeShVariable(&g.result);
            break;
        case CG_TYPE_CONDITION:
            freeShVariable(&g.condition);
            break;
        case CG_TYPE_PARAMETER:
            freeShVariable(&g.parameter);
            break;
        case CG_TYPE_LOOP_ITERS:
            break;
        case CG_TYPE_ALL:
            cgDestruct(CG_TYPE_RESULT);
            cgDestruct(CG_TYPE_CONDITION);
            cgDestruct(CG_TYPE_PARAMETER);
            break;
    }
}

static const char* getNewUnusedFunctionName(const char *input, TIntermNode *root)
{
    char *output;
    size_t baseLen;
    int i;

    if (!(output=(char*)malloc(strlen(getFunctionName(input)) +
                               strlen(CG_FUNCTION_POSTFIX) +
                               CG_RANDOMIZED_POSTFIX_SIZE+1))) {
        dbgPrint(DBGLVL_ERROR, "CodeInsertion - not enough memory to create debug function name\n");
        exit(1);
    }
    strcpy(output, getFunctionName(input));
    strcat(output, CG_FUNCTION_POSTFIX);
    baseLen = strlen(output);

    while (getFunctionBySignature(output, root)) {
        output[baseLen] = '\0';
        
        for (i=0; i<CG_RANDOMIZED_POSTFIX_SIZE; i++) {
            output[baseLen+i] = (char)((rand()/(float)RAND_MAX)*('Z'-'A')+'A');
        }
        output[baseLen+i] = '\0';
    }

    return output;
}

void cgInitNameMap(void) 
{
    g.nameMap.clear();
}

void cgInitLoopIter(void)
{
    g.numLoopIters = 0;
	g.loopIters.clear();
}

const char* cgGetDebugName(const char *input, TIntermNode *root)
{
    strMap::iterator it = g.nameMap.find(input);

    if (it != g.nameMap.end()) {
        /* Object already found */
        return it->second;
    } else {
        /* New object: 1. generate new name
         *             2. add to map */
        g.nameMap[input] = getNewUnusedFunctionName(input, root);
        return g.nameMap[input];
    }
}

void cgSetLoopIterName(char **name, ShVariableList *vl) {
    char prefix[200];

    sprintf(prefix, "%s%i", CG_LOOP_ITER_PREFIX, g.numLoopIters);
    getUnusedNameByPrefix(name, vl, prefix);

	g.loopIters.push_back(*name);
    g.numLoopIters++;
}

void cgResetLoopIterNames(void) {
    g.numLoopIters = 0;
	g.loopIters.clear();
}

