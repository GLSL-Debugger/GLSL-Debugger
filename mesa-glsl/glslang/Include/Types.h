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

#ifndef _TYPES_INCLUDED
#define _TYPES_INCLUDED

#include <stdlib.h>

#include "../Include/Common.h"
#include "../Include/BaseTypes.h"
#include "../Public/ShaderLang.h"


#define UNNAMED_STRUCT "unnamed_struct"

//
// Need to have association of line numbers to types in a list for building structs.
//
class TType;
struct TTypeRange {
    TType* type;
    TSourceRange range;
};
typedef TVector<TTypeRange> TTypeList;

inline TTypeList* NewPoolTTypeList()
{
	void* memory = GlobalPoolAllocator.allocate(sizeof(TTypeList));
	return new(memory) TTypeList;
}

//
// This is a workaround for a problem with the yacc stack,  It can't have
// types that it thinks have non-trivial constructors.  It should 
// just be used while recognizing the grammar, not anything else.  Pointers
// could be used, but also trying to avoid lots of memory management overhead.
//
// Not as bad as it looks, there is no actual assumption that the fields
// match up or are name the same or anything like that.
//
class TPublicType {
public:
    TBasicType type;
    TQualifier qualifier;
    TVaryingModifier varyingModifier;
    int size;          // size of vector
    bool matrix;
    int matrixSize[2]; // size of matrix
    bool array;
    int arraySize[MAX_ARRAYS];     // size of array
    TType* userDef;
    TSourceRange range;

    void setBasic(TBasicType bt, TQualifier q, TVaryingModifier vm = EvmNone, TSourceRange r = TSourceRangeInit)
    {
        int i;
        type = bt;
        qualifier = q;
        varyingModifier = vm;
        size = 1;
        matrix = false;
        matrixSize[0] = 1;
        matrixSize[1] = 1;
        array = false;
        for (i=0; i<MAX_ARRAYS; i++) {
            arraySize[i] = -1;
        }
        userDef = 0;
        range = r;
    }

    void setAggregate(int s)
    {
        matrix = false;
        size = s;
    }

    void setMatrix(int mS1, int mS2)
    {
        matrix = true;
        matrixSize[0] = mS1;
        matrixSize[1] = mS2;
        /* Matrices are always of non-vector type */
        size = 1;
    }

    void setArray(bool a, int s = 0)
    {
        array = a;
        arraySize[0] = s;
    }

    void addArray(bool a, int s = 0, int i = 0)
    {
        array = a;
        arraySize[i] = s;
    }
    void insertArray(int s = 0)
    {
        int i;
        for (i=1; i<MAX_ARRAYS; i++) {
            arraySize[i] = arraySize[i-1];
        }
        arraySize[0] = s;
    }

    int getNumArrays()
    {
        int i;
        int n = 0;
        for (i=1; i<MAX_ARRAYS; i++) {
            if (arraySize[i] != -1) {
                n++;
            } else {
                return n;
            }
        }
        return n;
    }
};

typedef std::map<TTypeList*, TTypeList*> TStructureMap;
typedef std::map<TTypeList*, TTypeList*>::iterator TStructureMapIterator;
//
// Base class for things that have a type.
//
class TType {
public:
    POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)
    explicit TType(TBasicType t, TQualifier q = EvqTemporary, TVaryingModifier vm = EvmNone, 
            int s = 1, int mS1 = 1, int mS2 = 1, bool m = false, bool a = false) :
        type(t), qualifier(q), varyingModifier(vm), size(s), matrix(m), array(a),
        structure(0), structureSize(0), arrayInformationType(0), fieldName(0), mangled(0), typeName(0), specified(0) {
            int i;
            matrixSize[0] = mS1;
            matrixSize[1] = mS2;
            for (i=0; i<MAX_ARRAYS; i++) {
                arraySize[i] = -1;
                maxArraySize[i] = 0;
            }
        }
    explicit TType(const TPublicType &p) :
        type(p.type), qualifier(p.qualifier), varyingModifier(p.varyingModifier), size(p.size), matrix(p.matrix), 
        array(p.array),
        structure(0), structureSize(0), arrayInformationType(0), fieldName(0), mangled(0), typeName(0), specified(0) {
            int i;
            matrixSize[0] = p.matrixSize[0];
            matrixSize[1] = p.matrixSize[1];
            if (p.userDef) {
                structure = p.userDef->getStruct();
                typeName = NewPoolTString(p.userDef->getTypeName().c_str());
            }
            for (i=0; i<MAX_ARRAYS; i++) {
                arraySize[i] = p.arraySize[i];
                maxArraySize[i] = 0;
            }
        }
    explicit TType(TTypeList* userDef, const TString& n) :
        type(EbtStruct), qualifier(EvqTemporary), varyingModifier(EvmNone), size(1), matrix(false), array(false),
        structure(userDef), arrayInformationType(0), fieldName(0), mangled(0), specified(0) {
            int i;
            matrixSize[0] = 1;
            matrixSize[1] = 1;
            typeName = NewPoolTString(n.c_str());
            for (i=0; i<MAX_ARRAYS; i++) {
                arraySize[i] = -1;
                maxArraySize[i] = 0;
            }
        }
    explicit TType() { }
    virtual ~TType() {}
    
    TType(const TType& type) { *this = type; }

    void copyType(const TType& copyOf, TStructureMap& remapper)
    {
        int i;
        type = copyOf.type;
        qualifier = copyOf.qualifier;
        varyingModifier = copyOf.varyingModifier;
        size = copyOf.size;
        matrix = copyOf.matrix;
        matrixSize[0] = copyOf.matrixSize[0];
        matrixSize[1] = copyOf.matrixSize[1];
        array = copyOf.array;
        for (i=0; i<MAX_ARRAYS; i++) {
            arraySize[i] = copyOf.arraySize[i];
            maxArraySize[i] = copyOf.maxArraySize[i];
        }
        specified = copyOf.specified;
        
        TStructureMapIterator iter;
        if (copyOf.structure) {
	        if ((iter = remapper.find(structure)) == remapper.end()) {
                // create the new structure here
                structure = NewPoolTTypeList();
                for (unsigned int i = 0; i < copyOf.structure->size(); ++i) {
                    TTypeRange typeRange;
                    typeRange.range = (*copyOf.structure)[i].range;
                    typeRange.type = (*copyOf.structure)[i].type->clone(remapper);
                    structure->push_back(typeRange);
                }
            } else {
                structure = iter->second;
            }
        } else
            structure = 0;
        
        fieldName = 0;
        if (copyOf.fieldName)
            fieldName = NewPoolTString(copyOf.fieldName->c_str());
        typeName = 0;
        if (copyOf.typeName)
            typeName = NewPoolTString(copyOf.typeName->c_str());
        
        mangled = 0;
        if (copyOf.mangled)
            mangled = NewPoolTString(copyOf.mangled->c_str());
        
        structureSize = copyOf.structureSize;
        assert(copyOf.arrayInformationType == 0);
        arrayInformationType = 0; // arrayInformationType should not be set for builtIn symbol table level
    }
    
    TType* clone(TStructureMap& remapper)
    {
        TType *newType = new TType();
        newType->copyType(*this, remapper);
        
        return newType;
    }
    
    virtual void setType(TBasicType t, int s = 1, int mS1 = 1, int mS2 = 1, bool m = false, bool a = false, int aS = 0)
    {
        type = t;
        size = s;
        matrix = m;
        matrixSize[0] = mS1;
        matrixSize[1] = mS2;
        array = a;
        arraySize[0] = aS; 
    }
    virtual void setType(TBasicType t, int s = 1, int mS1 = 1, int mS2 = 1, bool m = false, TType* userDef = 0)
    { 
        type = t;
        size = s;
        matrix = m;
        matrixSize[0] = mS1;
        matrixSize[1] = mS2;
        if (userDef)
            structure = userDef->getStruct(); 
        // leave array information intact.
    }

    virtual void setTypeName(const TString& n) { typeName = NewPoolTString(n.c_str()); }
    virtual void setFieldName(const TString& n) { fieldName = NewPoolTString(n.c_str()); }
    virtual const TString& getTypeName() const
    { 
        assert(typeName);    		
        return *typeName; 
    }
    
    virtual const TString& getFieldName() const
    { 
        assert(fieldName);
        return *fieldName; 
    }
    
    virtual TBasicType getBasicType() const { return type; }
    virtual TQualifier getQualifier() const { return qualifier; }
    virtual TVaryingModifier getVaryingModifier() const { return varyingModifier; }
    virtual void changeQualifier(TQualifier q) { qualifier = q; }
    virtual void changeVaryingModifier(TVaryingModifier vm) { varyingModifier = vm; }
    virtual void addVaryingModifier(TVaryingModifier vm) { varyingModifier |= vm; }
    
    // One-dimensional size of single instance type
    virtual int getNominalSize() const { return size; }  
    
    // Full-dimensional size of single instance of type
    virtual int getInstanceSize() const  
    {
        if (matrix) {
            return matrixSize[0] * matrixSize[1];
        } else {
            return size;
        }
    }

    // Get matrix sizes
    virtual int getMatrixSize(int i) const { return i==0?matrixSize[0]:matrixSize[1]; }
    
    virtual bool isMatrix() const { return matrix ? true : false; }
    virtual bool isArray() const  { return array ? true : false; }
    int getArraySize(int i = 0) const { return arraySize[i]; }
    void setArraySize(int s, int i = 0) { array = true; arraySize[i] = s; }
    void setMaxArraySize (int s, int i=0) { maxArraySize[i] = s; }
    int getMaxArraySize (int i=0) const { return maxArraySize[i]; }
    int getNumArrays() const {
        int i = 0;
        int n = 0;
        for (i=0; i<MAX_ARRAYS; i++) {
            if (arraySize[i] != -1) {
                n++;
            } else {
                return n;
            }
        }
        return n;
    }
    void clearArrayness() { 
        int i;
        array = false; 
        for (i=0; i<MAX_ARRAYS; i++) {
            arraySize[i] = -1;
            maxArraySize[i] = 0; 
        }
    }
    void setArrayInformationType(TType* t) { arrayInformationType = t; }
    TType* getArrayInformationType() { return arrayInformationType; }
    virtual bool isVector() const { return size > 1 && !matrix; }
    static const char* getBasicString(TBasicType t) {
        switch (t) {
            case EbtVoid:              return "void";              break;
            case EbtFloat:             return "float";             break;
            case EbtInt:               return "int";               break;
            case EbtUInt:              return "unsigned int";      break;
            case EbtBool:              return "bool";              break;
            case EbtSampler1D:         return "sampler1D";         break;
            case EbtISampler1D:        return "isampler1D";        break; // EXT_gpu_shader4
            case EbtUSampler1D:        return "usampler1D";        break; // EXT_gpu_shader4
            case EbtSampler2D:         return "sampler2D";         break;
            case EbtISampler2D:        return "isampler2D";        break; // EXT_gpu_shader4
            case EbtUSampler2D:        return "usampler2D";        break; // EXT_gpu_shader4
            case EbtSampler3D:         return "sampler3D";         break;
            case EbtISampler3D:        return "isampler3D";        break; // EXT_gpu_shader4
            case EbtUSampler3D:        return "usampler3D";        break; // EXT_gpu_shader4
            case EbtSamplerCube:       return "samplerCube";       break;
            case EbtISamplerCube:      return "isamplerCube";      break; // EXT_gpu_shader4
            case EbtUSamplerCube:      return "usamplerCube";      break; // EXT_gpu_shader4
            case EbtSampler1DShadow:   return "sampler1DShadow";   break;
            case EbtSampler2DShadow:   return "sampler2DShadow";   break;
            case EbtSampler2DRect:     return "sampler2DRect";     break; // ARB_texture_rectangle
            case EbtISampler2DRect:    return "isampler2DRect";    break; // EXT_gpu_shader4
            case EbtUSampler2DRect:    return "usampler2DRect";    break; // EXT_gpu_shader4
            case EbtSampler2DRectShadow:     return "samplerRectShadow"; break; // ARB_texture_rectangle
            case EbtSampler1DArray:    return "sampler1DArray";    break; // EXT_gpu_shader4
            case EbtISampler1DArray:   return "isampler1DArray";   break; // EXT_gpu_shader4
            case EbtUSampler1DArray:   return "usampler1DArray";   break; // EXT_gpu_shader4
            case EbtSampler2DArray:    return "sampler2DArray";    break; // EXT_gpu_shader4
            case EbtISampler2DArray:   return "isampler2DArray";   break; // EXT_gpu_shader4
            case EbtUSampler2DArray:   return "usampler2DArray";   break; // EXT_gpu_shader4
            case EbtSamplerBuffer:     return "samplerBuffer";     break; // EXT_gpu_shader4
            case EbtISamplerBuffer:    return "isamplerBuffer";    break; // EXT_gpu_shader4
            case EbtUSamplerBuffer:    return "usamplerBuffer";    break; // EXT_gpu_shader4
            case EbtStruct:            return "structure";         break;
            case EbtSampler1DArrayShadow:    return "sampler1DArrayShadow";    break; // EXT_gpu_shader4
            case EbtSampler2DArrayShadow:    return "sampler2DArrayShadow";    break; // EXT_gpu_shader4
            case EbtSamplerCubeShadow:       return "samplerCubeShadow";       break; // EXT_gpu_shader4
            case EbtSwizzle:           return "swizzle";           break;
            default:                   return "unknown type";
        }
    }
    const char* getBasicString() const { return TType::getBasicString(type); }
    const char* getQualifierString(EShLanguage l = EShLangFragment) const { return ::getQualifierString(qualifier, l); }
    const char* getVaryingModifierString() const { return ::getVaryingModifierString(varyingModifier); }
    TTypeList* getStruct() { return structure; }
    
    int getObjectSize() const
    {
        int totalSize;
        
        if (getBasicType() == EbtStruct)
            totalSize = getStructSize();
        else if (matrix)
            totalSize = matrixSize[0] * matrixSize[1];
        else 
            totalSize = size;
        
        if (isArray())
            totalSize *= Max(getArraySize(), getMaxArraySize());
        
        return totalSize;
    }
    
    TTypeList* getStruct() const { return structure; }
    TString& getMangledName() {
        if (!mangled) {
            mangled = NewPoolTString("");
            buildMangledName(*mangled);            
            *mangled += ';' ;
        }
        
        return *mangled;
    }
    bool sameElementType(const TType& right) const {
        return
            type == right.type   &&
            matrix == right.matrix &&
            (matrix || size == right.size) &&
            (!matrix || (matrixSize[0] == right.matrixSize[0] && matrixSize[1] == right.matrixSize[1])) &&
            structure == right.structure;
    }
    bool operator==(const TType& right) const {
        // don't check the qualifier, it's not ever what's being sought after
        if (!array) {
            return
                type == right.type   &&
                matrix == right.matrix &&
                (matrix || size == right.size) &&
                (!matrix || (matrixSize[0] == right.matrixSize[0] && matrixSize[1] == right.matrixSize[1])) &&
                array == right.array  && 
                structure == right.structure;

        } else {
            int i;
            for (i=0; i<MAX_ARRAYS; i++) {
                if (arraySize[i] != right.arraySize[i]) {
                    return false;
                }
            }
            return
                type == right.type   &&
                matrix == right.matrix &&
                (matrix || size == right.size) &&
                (!matrix || (matrixSize[0] == right.matrixSize[0] && matrixSize[1] == right.matrixSize[1])) &&
                array == right.array  && 
                structure == right.structure;
        }
    }
    bool operator!=(const TType& right) const {
        return !operator==(right);
    }
    TString getCompleteString() const;
    TString getCodeString(bool withQualifier, EShLanguage l) const;

    bool isSpecified() { return specified; }
    void setSpecified(bool sp) { specified = sp; }
        

    variableType getShBasicType(void) {
        switch (type) {
            case EbtFloat:
                return SH_FLOAT;
            case EbtInt:
                return SH_INT;
            case EbtUInt:
                return SH_UINT;
            case EbtBool:
                return SH_BOOL;
            case EbtStruct:
                return SH_STRUCT;
            case EbtSampler1D:
                return SH_SAMPLER_1D;
            case EbtISampler1D:
                return SH_ISAMPLER_1D;
            case EbtUSampler1D:
                return SH_USAMPLER_1D;
            case EbtSampler2D:
                return SH_SAMPLER_2D;
            case EbtISampler2D:
                return SH_ISAMPLER_2D;
            case EbtUSampler2D:
                return SH_USAMPLER_2D;
            case EbtSampler3D:
                return SH_SAMPLER_3D;
            case EbtISampler3D:
                return SH_ISAMPLER_3D;
            case EbtUSampler3D:
                return SH_USAMPLER_3D;
            case EbtSamplerCube:
                return SH_SAMPLER_CUBE;
            case EbtISamplerCube:
                return SH_ISAMPLER_CUBE;
            case EbtUSamplerCube:
                return SH_USAMPLER_CUBE;
            case EbtSampler1DShadow:
                return SH_SAMPLER_1D_SHADOW;
            case EbtSampler2DShadow:
                return SH_SAMPLER_2D_SHADOW;
            case EbtSampler2DRect:
                return SH_SAMPLER_2D_RECT;
            case EbtISampler2DRect:
                return SH_ISAMPLER_2D_RECT;
            case EbtUSampler2DRect:
                return SH_USAMPLER_2D_RECT;
            case EbtSampler2DRectShadow:
                return SH_SAMPLER_2D_RECT_SHADOW;
            case EbtSampler1DArray:
                return SH_SAMPLER_1D_ARRAY;
            case EbtISampler1DArray:
                return SH_ISAMPLER_1D_ARRAY;
            case EbtUSampler1DArray:
                return SH_USAMPLER_1D_ARRAY;
            case EbtSampler2DArray:
                return SH_SAMPLER_2D_ARRAY;
            case EbtISampler2DArray:
                return SH_ISAMPLER_2D_ARRAY;
            case EbtUSampler2DArray:
                return SH_USAMPLER_2D_ARRAY;
            case EbtSamplerBuffer:
                return SH_SAMPLER_BUFFER;
            case EbtISamplerBuffer:
                return SH_ISAMPLER_BUFFER;
            case EbtUSamplerBuffer:
                return SH_USAMPLER_BUFFER;
            case EbtSampler1DArrayShadow:
                return SH_SAMPLER_1D_ARRAY_SHADOW;
            case EbtSampler2DArrayShadow:
                return SH_SAMPLER_2D_ARRAY_SHADOW;
            case EbtSamplerCubeShadow:
                return SH_SAMPLER_CUBE_SHADOW;
            default:
                fprintf(stderr, "E! ShVariable does not support type %i\n",
                        type);
                exit(1);
        }
    }
    
    variableQualifier getShQualifier(void) {
        switch (qualifier) {
            case EvqTemporary:
                return SH_TEMPORARY;
            case EvqGlobal:
                return SH_GLOBAL;
            case EvqConst:
            case EvqConstNoValue:
                return SH_CONST;
            case EvqAttribute:
                return SH_ATTRIBUTE;
            case EvqVaryingIn:
                return SH_VARYING_IN;
            case EvqVaryingOut:
                return SH_VARYING_OUT;
            case EvqUniform:
                return SH_UNIFORM;
            case EvqIn:
			case EvqConstIn: /* check back with Magnus */
                return SH_PARAM_IN;
            case EvqOut:
                return SH_PARAM_OUT;
            case EvqInOut:
                return SH_PARAM_INOUT;
            case EvqFace:
            case EvqFragCoord:
            /* TODO: These should not be allowed always, but so far we trust in the user (: We should not! */
            case EvqVertexID:
            case EvqInstanceID:
            case EvqPrimitiveID:
            case EvqPrimitiveIDIn:
            case EvqLayer:
                return SH_BUILTIN_READ;
            case EvqPosition:
            case EvqPointSize:
            case EvqClipVertex:
            case EvqFragColor:
            case EvqFragData:
            case EvqFragDepth:
                return SH_BUILTIN_WRITE;
            default:
                return SH_UNSET;
        }
    }

    variableVaryingModifier getShVaryingModifier(void) {
        return varyingModifier;
    }
    
    ShVariable* getShVariable(void) {
        int i;
        ShVariable *v = (ShVariable*) malloc(sizeof(ShVariable));
        
        // Type has no identifier! To be filled in later by TVariable
        v->uniqueId = -1;
        
        v->builtin = false;
        
        if (!fieldName) {
            v->name = NULL;
        } else {
            v->name = (char*) malloc(strlen(fieldName->c_str())+1);
            strcpy(v->name, fieldName->c_str());
        }
        
        // Type of variable (SH_FLOAT/SH_INT/SH_BOOL/SH_STRUCT)
        v->type = getShBasicType();
        
        // Qualifier of variable
        v->qualifier = getShQualifier();

        // Varying modifier
        v->varyingModifier = getShVaryingModifier();
        
        // Scalar/Vector size
        v->size = getNominalSize();

        // Matrix handling
        v->isMatrix = isMatrix();
        v->matrixSize[0] = getMatrixSize(0);
        v->matrixSize[1] = getMatrixSize(1);

        // Array handling
        v->isArray = isArray();
        for (i=0; i<MAX_ARRAYS; i++) {
            v->arraySize[i] = getArraySize(i);
        }

        if (type == EbtStruct) {
            int i;

            //
            // Append structure to ShVariable
            //·
            v->structSize = (int)structure->size();
            v->structSpec = (ShVariable**) malloc(v->structSize *
                    sizeof(ShVariable*));
            
            if (strlen(typeName->c_str()) != 0) {
                v->structName = (char*) malloc(strlen(typeName->c_str())+1);
                strcpy(v->structName, typeName->c_str());
            } else {
                v->structName = (char*) malloc(strlen(UNNAMED_STRUCT)+1);
                strcpy(v->structName, UNNAMED_STRUCT);
            }
            
            TTypeList::iterator tli = structure->begin();
            i = 0;
            while (tli != structure->end()) {
                v->structSpec[i] = tli->type->getShVariable();
                i++;
                tli++;
            }
        } else {
            v->structName = NULL;
            v->structSize = 0;
            v->structSpec = NULL;
        }
        return v;
    }

protected:
    void buildMangledName(TString&);
    int getStructSize() const;

    TBasicType type      ;//: 6;
    TQualifier qualifier ;//: 7;
    TVaryingModifier varyingModifier;
    int size             ;//: 8; // size of vector or matrix, not size of array
    unsigned int matrix  ;//: 1;
    unsigned int array   ;//: 1;
    int arraySize[MAX_ARRAYS];
    int matrixSize[2];
    

    TTypeList* structure;      // 0 unless this is a struct
    mutable int structureSize;
    int maxArraySize[MAX_ARRAYS];
    TType* arrayInformationType;
    TString *fieldName;         // for structure field names
    TString *mangled;
    TString *typeName;          // for structure field type name
    
    bool specified;
};

#endif // _TYPES_INCLUDED_
