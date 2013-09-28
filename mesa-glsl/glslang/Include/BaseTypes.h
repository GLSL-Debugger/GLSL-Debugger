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

#ifndef _BASICTYPES_INCLUDED_
#define _BASICTYPES_INCLUDED_

#include <string.h>

#include "ShaderLang.h"

//
// Basic type.  Arrays, vectors, etc., are orthogonal to this.
//
enum TBasicType {
    EbtVoid,
    EbtFloat,
    EbtInt,
    EbtUInt,
    EbtBool,
    EbtGuardSamplerBegin,  // non type:  see implementation of IsSampler()
    EbtSampler1D,
    EbtISampler1D,         // EXT_gpu_shader4
    EbtUSampler1D,         // EXT_gpu_shader4
    EbtSampler2D,
    EbtISampler2D,         // EXT_gpu_shader4
    EbtUSampler2D,         // EXT_gpu_shader4
    EbtSampler3D,
    EbtISampler3D,         // EXT_gpu_shader4
    EbtUSampler3D,         // EXT_gpu_shader4
    EbtSamplerCube,
    EbtISamplerCube,       // EXT_gpu_shader4
    EbtUSamplerCube,       // EXT_gpu_shader4
    EbtSampler1DShadow,
    EbtSampler2DShadow,
    EbtSampler2DRect,      // ARB_texture_rectangle
    EbtISampler2DRect,     // EXT_gpu_shader4
    EbtUSampler2DRect,     // EXT_gpu_shader4
    EbtSampler2DRectShadow,// ARB_texture_rectangle
    EbtSampler1DArray,     // EXT_gpu_shader4
    EbtISampler1DArray,    // EXT_gpu_shader4
    EbtUSampler1DArray,    // EXT_gpu_shader4
    EbtSampler2DArray,     // EXT_gpu_shader4
    EbtISampler2DArray,    // EXT_gpu_shader4
    EbtUSampler2DArray,    // EXT_gpu_shader4
    EbtSamplerBuffer,      // EXT_gpu_shader4
    EbtISamplerBuffer,     // EXT_gpu_shader4
    EbtUSamplerBuffer,     // EXT_gpu_shader4
    EbtSampler1DArrayShadow,     // EXT_gpu_shader4
    EbtSampler2DArrayShadow,     // EXT_gpu_shader4
    EbtSamplerCubeShadow,  // EXT_gpu_shader4
    EbtGuardSamplerEnd,    // non type:  see implementation of IsSampler()
    EbtStruct,
    EbtAddress,            // should be deprecated??
    EbtSwizzle,
    EbtInvariant           // this is necessary for including glsl1.20.8 invariant together with EXT_gpu_shader4
};

__inline bool IsSampler(TBasicType type)
{
    return type > EbtGuardSamplerBegin && type < EbtGuardSamplerEnd;
}

//
// Qualifiers and built-ins.  These are mainly used to see what can be read
// or written, and by the machine dependent translator to know which registers
// to allocate variables in.  Since built-ins tend to go to different registers
// than varying or uniform, it makes sense they are peers, not sub-classes.
//
enum TQualifier {
    EvqTemporary,     // For temporaries (within a function), read/write
    EvqGlobal,        // For globals read/write
    EvqConst,         // User defined constants and non-output parameters in functions
    EvqConstNoValue,  // Constants that do not have a constantUnion assigned (necessary for non-constant initializers)
    EvqAttribute,     // Readonly
    EvqVaryingIn,     // readonly, fragment shaders only
    EvqVaryingOut,    // vertex shaders only  read/write
    EvqUniform,       // Readonly, vertex and fragment

    // pack/unpack input and output
    EvqInput,
    EvqOutput,

    // parameters
    EvqIn,
	EvqConstIn,
    EvqOut,
    EvqInOut,
    EvqConstReadOnly,

    // built-ins written by vertex shader
    EvqPosition,
    EvqPointSize,
    EvqClipVertex,

    // built-ins read by vertex shader (EXT_gpu_shader4)
    EvqVertexID,
    EvqInstanceID,

    // built-ins read by fragment shader
    EvqFace,
    EvqFragCoord,

    // built-ins read by fragment shader (EXT_gpu_shader4)
    // and written by geometry shader (EXT_geometry_shader4)
    EvqPrimitiveID,

    // built-ins read by geometry shader (EXT_geometry_shader4)
    EvqPrimitiveIDIn,

    // built-ins written by geometry shader (EXT_geometry_shader4)
    EvqLayer,

    // built-ins written by fragment shader
    EvqFragColor,
    EvqFragData,
    EvqFragDepth,

    // end of list
    EvqLast,
};

typedef int TVaryingModifier;

enum TVMTypes {
    EvmNone = 0,
    EvmInvariant = 1,
    EvmFlat = 2,
    EvmCentroid = 4,
    EvmNoperspective = 8
};

//
// This is just for debug print out, carried along with the definitions above.
//
__inline const char* getVaryingModifierString(TVaryingModifier vm)
{
    char *buf = new char[1024];
    buf[0] = '\0';

    if (vm & EvmInvariant) {
        strcat(buf, "invariant");
    }
    if (vm & EvmNoperspective) {
        if (strlen(buf) != 0) {
            strcat(buf, " ");
        }
        strcat(buf, "noperspective");
    }
    if (vm & EvmFlat) {
        if (strlen(buf) != 0) {
            strcat(buf, " ");
        }
        strcat(buf, "flat");
    }
    if (vm & EvmCentroid) {
        if (strlen(buf) != 0) {
            strcat(buf, " ");
        }
        strcat(buf, "centroid");
    }

    return buf;
}

__inline const char* getQualifierString(TQualifier q, EShLanguage l)
{
    switch (q) {
    case EvqTemporary:      return "Temporary";      break;
    case EvqGlobal:         return "Global";         break;
    case EvqConst:          return "const";          break;
    case EvqConstNoValue:   return "const";          break;
    case EvqConstReadOnly:  return "const";          break;
    case EvqAttribute:      return "attribute";      break;
    case EvqVaryingIn:
            if (l == EShLangGeometry) {
                            return "varying in";
            } else {
                            return "varying";
            }
            break;
    case EvqVaryingOut:
            if (l == EShLangGeometry) {
                            return "varying out";
			/* remove varying qualifier for varying outs in fargment shader to
			 * avoid having to bind varying out names to render target */
			} else if (l == EShLangFragment) {
                            return "";
            } else {
                            return "varying";
            }
            break;
    case EvqUniform:        return "uniform";        break;
    case EvqIn:             return "in";             break;
    case EvqConstIn:        return "const in";       break;
    case EvqOut:            return "out";            break;
    case EvqInOut:          return "inout";          break;
    case EvqInput:          return "input";          break;
    case EvqOutput:         return "output";         break;
    case EvqPosition:       return "Position";       break;
    case EvqPointSize:      return "PointSize";      break;
    case EvqClipVertex:     return "ClipVertex";     break;
    case EvqVertexID:       return "VertexID";       break;
    case EvqInstanceID:     return "InstanceID";     break;
    case EvqFace:           return "Face";           break;
    case EvqFragCoord:      return "FragCoord";      break;
    case EvqPrimitiveID:    return "PrimitiveID";    break;
    case EvqPrimitiveIDIn:  return "PrimitiveIDIn";  break;
    case EvqLayer:          return "Layer";          break;
    case EvqFragColor:      return "FragColor";      break;
    case EvqFragData:       return "FragData";       break;
    case EvqFragDepth:      return "FragDepth";      break;
    default:                return "unknown qualifier";
    }
}

#endif // _BASICTYPES_INCLUDED_
