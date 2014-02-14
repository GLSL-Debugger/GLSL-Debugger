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
// Create strings that declare built-in definitions, add built-ins that
// cannot be expressed in the files, and establish mappings between
// built-in functions and operators.
//

#include "../Include/intermediate.h"
#include "Initialize.h"

void TBuiltIns::initialize()
{
	//
	// Initialize all the built-in strings for parsing.
	//
	TString BuiltInFunctions;
	TString BuiltInFunctionsVertex;
	TString BuiltInFunctionsGeometry;
	TString BuiltInFunctionsFragment;
	TString StandardVertexVaryings;
	TString StandardVaryingOutGeometry;
	TString StandardVaryingInGeometry;
	TString StandardFragmentVaryings;
	TString StandardVertexAttributes;
	TString StandardUniforms;

	{
		//============================================================================
		//
		// Prototypes for built-in functions seen by both vertex and fragment shaders.
		//
		//============================================================================

		TString& s = BuiltInFunctions;

		//
		// Angle and Trigonometric Functions.
		//
		s.append(TString("float radians(float degrees);"));
		s.append(TString("vec2  radians(vec2  degrees);"));
		s.append(TString("vec3  radians(vec3  degrees);"));
		s.append(TString("vec4  radians(vec4  degrees);"));

		s.append(TString("float degrees(float radians);"));
		s.append(TString("vec2  degrees(vec2  radians);"));
		s.append(TString("vec3  degrees(vec3  radians);"));
		s.append(TString("vec4  degrees(vec4  radians);"));

		s.append(TString("float sin(float angle);"));
		s.append(TString("vec2  sin(vec2  angle);"));
		s.append(TString("vec3  sin(vec3  angle);"));
		s.append(TString("vec4  sin(vec4  angle);"));

		s.append(TString("float cos(float angle);"));
		s.append(TString("vec2  cos(vec2  angle);"));
		s.append(TString("vec3  cos(vec3  angle);"));
		s.append(TString("vec4  cos(vec4  angle);"));

		s.append(TString("float tan(float angle);"));
		s.append(TString("vec2  tan(vec2  angle);"));
		s.append(TString("vec3  tan(vec3  angle);"));
		s.append(TString("vec4  tan(vec4  angle);"));

		s.append(TString("float asin(float x);"));
		s.append(TString("vec2  asin(vec2  x);"));
		s.append(TString("vec3  asin(vec3  x);"));
		s.append(TString("vec4  asin(vec4  x);"));

		s.append(TString("float acos(float x);"));
		s.append(TString("vec2  acos(vec2  x);"));
		s.append(TString("vec3  acos(vec3  x);"));
		s.append(TString("vec4  acos(vec4  x);"));

		s.append(TString("float atan(float y, float x);"));
		s.append(TString("vec2  atan(vec2  y, vec2  x);"));
		s.append(TString("vec3  atan(vec3  y, vec3  x);"));
		s.append(TString("vec4  atan(vec4  y, vec4  x);"));

		s.append(TString("float atan(float y_over_x);"));
		s.append(TString("vec2  atan(vec2  y_over_x);"));
		s.append(TString("vec3  atan(vec3  y_over_x);"));
		s.append(TString("vec4  atan(vec4  y_over_x);"));

		//
		// Exponential Functions.
		//
		s.append(TString("float pow(float x, float y);"));
		s.append(TString("vec2  pow(vec2  x, vec2  y);"));
		s.append(TString("vec3  pow(vec3  x, vec3  y);"));
		s.append(TString("vec4  pow(vec4  x, vec4  y);"));

		s.append(TString("float exp(float x);"));
		s.append(TString("vec2  exp(vec2  x);"));
		s.append(TString("vec3  exp(vec3  x);"));
		s.append(TString("vec4  exp(vec4  x);"));

		s.append(TString("float log(float x);"));
		s.append(TString("vec2  log(vec2  x);"));
		s.append(TString("vec3  log(vec3  x);"));
		s.append(TString("vec4  log(vec4  x);"));

		s.append(TString("float exp2(float x);"));
		s.append(TString("vec2  exp2(vec2  x);"));
		s.append(TString("vec3  exp2(vec3  x);"));
		s.append(TString("vec4  exp2(vec4  x);"));

		s.append(TString("float log2(float x);"));
		s.append(TString("vec2  log2(vec2  x);"));
		s.append(TString("vec3  log2(vec3  x);"));
		s.append(TString("vec4  log2(vec4  x);"));

		s.append(TString("float sqrt(float x);"));
		s.append(TString("vec2  sqrt(vec2  x);"));
		s.append(TString("vec3  sqrt(vec3  x);"));
		s.append(TString("vec4  sqrt(vec4  x);"));

		s.append(TString("float inversesqrt(float x);"));
		s.append(TString("vec2  inversesqrt(vec2  x);"));
		s.append(TString("vec3  inversesqrt(vec3  x);"));
		s.append(TString("vec4  inversesqrt(vec4  x);"));

		//
		// Common Functions.
		//
		s.append(TString("float abs(float x);"));
		s.append(TString("vec2  abs(vec2  x);"));
		s.append(TString("vec3  abs(vec3  x);"));
		s.append(TString("vec4  abs(vec4  x);"));

		// EXT_gpu_shader4
		s.append(TString("int   abs(int   x);"));
		s.append(TString("ivec2 abs(ivec2 x);"));
		s.append(TString("ivec3 abs(ivec3 x);"));
		s.append(TString("ivec4 abs(ivec4 x);"));

		s.append(TString("float sign(float x);"));
		s.append(TString("vec2  sign(vec2  x);"));
		s.append(TString("vec3  sign(vec3  x);"));
		s.append(TString("vec4  sign(vec4  x);"));

		// EXT_gpu_shader4
		s.append(TString("int   sign(int   x);"));
		s.append(TString("ivec2 sign(ivec2 x);"));
		s.append(TString("ivec3 sign(ivec3 x);"));
		s.append(TString("ivec4 sign(ivec4 x);"));

		s.append(TString("float floor(float x);"));
		s.append(TString("vec2  floor(vec2  x);"));
		s.append(TString("vec3  floor(vec3  x);"));
		s.append(TString("vec4  floor(vec4  x);"));

		s.append(TString("float ceil(float x);"));
		s.append(TString("vec2  ceil(vec2  x);"));
		s.append(TString("vec3  ceil(vec3  x);"));
		s.append(TString("vec4  ceil(vec4  x);"));

		s.append(TString("float fract(float x);"));
		s.append(TString("vec2  fract(vec2  x);"));
		s.append(TString("vec3  fract(vec3  x);"));
		s.append(TString("vec4  fract(vec4  x);"));

		s.append(TString("float mod(float x, float y);"));
		s.append(TString("vec2  mod(vec2  x, float y);"));
		s.append(TString("vec3  mod(vec3  x, float y);"));
		s.append(TString("vec4  mod(vec4  x, float y);"));
		s.append(TString("vec2  mod(vec2  x, vec2  y);"));
		s.append(TString("vec3  mod(vec3  x, vec3  y);"));
		s.append(TString("vec4  mod(vec4  x, vec4  y);"));

		s.append(TString("float min(float x, float y);"));
		s.append(TString("vec2  min(vec2  x, float y);"));
		s.append(TString("vec3  min(vec3  x, float y);"));
		s.append(TString("vec4  min(vec4  x, float y);"));
		s.append(TString("vec2  min(vec2  x, vec2  y);"));
		s.append(TString("vec3  min(vec3  x, vec3  y);"));
		s.append(TString("vec4  min(vec4  x, vec4  y);"));

		// EXT_gpu_shader4
		s.append(TString("int   min(int   x, int   y);"));
		s.append(TString("ivec2 min(ivec2 x, ivec2 y);"));
		s.append(TString("ivec3 min(ivec3 x, ivec3 y);"));
		s.append(TString("ivec4 min(ivec4 x, ivec4 y);"));
		s.append(TString("ivec2 min(ivec2 x, int   y);"));
		s.append(TString("ivec3 min(ivec3 x, int   y);"));
		s.append(TString("ivec4 min(ivec4 x, int   y);"));
		s.append(TString("unsigned int min(unsigned int x, unsigned int y);"));
		s.append(TString("uvec2 min(uvec2 x, uvec2 y);"));
		s.append(TString("uvec3 min(uvec3 x, uvec3 y);"));
		s.append(TString("uvec4 min(uvec4 x, uvec4 y);"));
		s.append(TString("uvec2 min(uvec2 x, unsigned int y);"));
		s.append(TString("uvec3 min(uvec3 x, unsigned int y);"));
		s.append(TString("uvec4 min(uvec4 x, unsigned int y);"));

		s.append(TString("float max(float x, float y);"));
		s.append(TString("vec2  max(vec2  x, float y);"));
		s.append(TString("vec3  max(vec3  x, float y);"));
		s.append(TString("vec4  max(vec4  x, float y);"));
		s.append(TString("vec2  max(vec2  x, vec2  y);"));
		s.append(TString("vec3  max(vec3  x, vec3  y);"));
		s.append(TString("vec4  max(vec4  x, vec4  y);"));

		// EXT_gpu_shader4
		s.append(TString("int   max(int   x, int   y);"));
		s.append(TString("ivec2 max(ivec2 x, ivec2 y);"));
		s.append(TString("ivec3 max(ivec3 x, ivec3 y);"));
		s.append(TString("ivec4 max(ivec4 x, ivec4 y);"));
		s.append(TString("ivec2 max(ivec2 x, int   y);"));
		s.append(TString("ivec3 max(ivec3 x, int   y);"));
		s.append(TString("ivec4 max(ivec4 x, int   y);"));
		s.append(TString("unsigned int max(unsigned int x, unsigned int y);"));
		s.append(TString("uvec2 max(uvec2 x, uvec2 y);"));
		s.append(TString("uvec3 max(uvec3 x, uvec3 y);"));
		s.append(TString("uvec4 max(uvec4 x, uvec4 y);"));
		s.append(TString("uvec2 max(uvec2 x, unsigned int y);"));
		s.append(TString("uvec3 max(uvec3 x, unsigned int y);"));
		s.append(TString("uvec4 max(uvec4 x, unsigned int y);"));

		s.append(TString("float clamp(float x, float minVal, float maxVal);"));
		s.append(TString("vec2  clamp(vec2  x, float minVal, float maxVal);"));
		s.append(TString("vec3  clamp(vec3  x, float minVal, float maxVal);"));
		s.append(TString("vec4  clamp(vec4  x, float minVal, float maxVal);"));
		s.append(TString("vec2  clamp(vec2  x, vec2  minVal, vec2  maxVal);"));
		s.append(TString("vec3  clamp(vec3  x, vec3  minVal, vec3  maxVal);"));
		s.append(TString("vec4  clamp(vec4  x, vec4  minVal, vec4  maxVal);"));

		// EXT_gpu_shader4
		s.append(TString("int   clamp(int   x, int   minVal, int   maxVal);"));
		s.append(TString("ivec2 clamp(ivec2 x, ivec2 minVal, ivec2 maxVal);"));
		s.append(TString("ivec3 clamp(ivec3 x, ivec3 minVal, ivec3 maxVal);"));
		s.append(TString("ivec4 clamp(ivec4 x, ivec4 minVal, ivec4 maxVal);"));
		s.append(TString("int   clamp(int   x, int minVal, int maxVal);"));
		s.append(TString("ivec2 clamp(ivec2 x, int minVal, int maxVal);"));
		s.append(TString("ivec3 clamp(ivec3 x, int minVal, int maxVal);"));
		s.append(TString("ivec4 clamp(ivec4 x, int minVal, int maxVal);"));

		s.append(
				TString(
						"unsigned int clamp(unsigned int x, unsigned int minVal, unsigned int maxVal);"));
		s.append(TString("uvec2 clamp(uvec2 x, uvec2 minVal, uvec2 maxVal);"));
		s.append(TString("uvec3 clamp(uvec3 x, uvec3 minVal, uvec3 maxVal);"));
		s.append(TString("uvec4 clamp(uvec4 x, uvec4 minVal, uvec4 maxVal);"));
		s.append(
				TString(
						"unsigned int clamp(unsigned int x, unsigned int minVal, unsigned int maxVal);"));
		s.append(
				TString(
						"uvec2 clamp(uvec2 x, unsigned int minVal, unsigned int maxVal);"));
		s.append(
				TString(
						"uvec3 clamp(uvec3 x, unsigned int minVal, unsigned int maxVal);"));
		s.append(
				TString(
						"uvec4 clamp(uvec4 x, unsigned int minVal, unsigned int maxVal);"));

		s.append(TString("float mix(float x, float y, float a);"));
		s.append(TString("vec2  mix(vec2  x, vec2  y, float a);"));
		s.append(TString("vec3  mix(vec3  x, vec3  y, float a);"));
		s.append(TString("vec4  mix(vec4  x, vec4  y, float a);"));
		s.append(TString("vec2  mix(vec2  x, vec2  y, vec2  a);"));
		s.append(TString("vec3  mix(vec3  x, vec3  y, vec3  a);"));
		s.append(TString("vec4  mix(vec4  x, vec4  y, vec4  a);"));

		s.append(TString("float step(float edge, float x);"));
		s.append(TString("vec2  step(vec2  edge, vec2  x);"));
		s.append(TString("vec3  step(vec3  edge, vec3  x);"));
		s.append(TString("vec4  step(vec4  edge, vec4  x);"));
		s.append(TString("vec2  step(float edge, vec2  x);"));
		s.append(TString("vec3  step(float edge, vec3  x);"));
		s.append(TString("vec4  step(float edge, vec4  x);"));

		s.append(
				TString(
						"float smoothstep(float edge0, float edge1, float x);"));
		s.append(
				TString(
						"vec2  smoothstep(vec2  edge0, vec2  edge1, vec2  x);"));
		s.append(
				TString(
						"vec3  smoothstep(vec3  edge0, vec3  edge1, vec3  x);"));
		s.append(
				TString(
						"vec4  smoothstep(vec4  edge0, vec4  edge1, vec4  x);"));
		s.append(
				TString(
						"vec2  smoothstep(float edge0, float edge1, vec2  x);"));
		s.append(
				TString(
						"vec3  smoothstep(float edge0, float edge1, vec3  x);"));
		s.append(
				TString(
						"vec4  smoothstep(float edge0, float edge1, vec4  x);"));

		// EXT_gpu_shader4
		s.append(TString("float truncate(float x);"));
		s.append(TString("vec2  truncate(vec2  x);"));
		s.append(TString("vec3  truncate(vec3  x);"));
		s.append(TString("vec4  truncate(vec4  x);"));

		s.append(TString("float round(float x);"));
		s.append(TString("vec2  round(vec2  x);"));
		s.append(TString("vec3  round(vec3  x);"));
		s.append(TString("vec4  round(vec4  x);"));

		//
		// Geometric Functions.
		//
		s.append(TString("float length(float x);"));
		s.append(TString("float length(vec2  x);"));
		s.append(TString("float length(vec3  x);"));
		s.append(TString("float length(vec4  x);"));

		s.append(TString("float distance(float p0, float p1);"));
		s.append(TString("float distance(vec2  p0, vec2  p1);"));
		s.append(TString("float distance(vec3  p0, vec3  p1);"));
		s.append(TString("float distance(vec4  p0, vec4  p1);"));

		s.append(TString("float dot(float x, float y);"));
		s.append(TString("float dot(vec2  x, vec2  y);"));
		s.append(TString("float dot(vec3  x, vec3  y);"));
		s.append(TString("float dot(vec4  x, vec4  y);"));

		s.append(TString("vec3 cross(vec3 x, vec3 y);"));
		s.append(TString("float normalize(float x);"));
		s.append(TString("vec2  normalize(vec2  x);"));
		s.append(TString("vec3  normalize(vec3  x);"));
		s.append(TString("vec4  normalize(vec4  x);"));

		s.append(TString("float faceforward(float N, float I, float Nref);"));
		s.append(TString("vec2  faceforward(vec2  N, vec2  I, vec2  Nref);"));
		s.append(TString("vec3  faceforward(vec3  N, vec3  I, vec3  Nref);"));
		s.append(TString("vec4  faceforward(vec4  N, vec4  I, vec4  Nref);"));

		s.append(TString("float reflect(float I, float N);"));
		s.append(TString("vec2  reflect(vec2  I, vec2  N);"));
		s.append(TString("vec3  reflect(vec3  I, vec3  N);"));
		s.append(TString("vec4  reflect(vec4  I, vec4  N);"));

		s.append(TString("float refract(float I, float N, float eta);"));
		s.append(TString("vec2  refract(vec2  I, vec2  N, float eta);"));
		s.append(TString("vec3  refract(vec3  I, vec3  N, float eta);"));
		s.append(TString("vec4  refract(vec4  I, vec4  N, float eta);"));

		//
		// Matrix Functions.
		//
		s.append(TString("mat2 matrixCompMult(mat2 x, mat2 y);"));
		s.append(TString("mat3 matrixCompMult(mat3 x, mat3 y);"));
		s.append(TString("mat4 matrixCompMult(mat4 x, mat4 y);"));

		s.append(TString("mat2   outerProduct(vec2 x, vec2 y);"));
		s.append(TString("mat2x3 outerProduct(vec3 x, vec2 y);"));
		s.append(TString("mat2x4 outerProduct(vec4 x, vec2 y);"));
		s.append(TString("mat3x2 outerProduct(vec2 x, vec3 y);"));
		s.append(TString("mat3   outerProduct(vec3 x, vec3 y);"));
		s.append(TString("mat3x4 outerProduct(vec4 x, vec3 y);"));
		s.append(TString("mat4x2 outerProduct(vec2 x, vec4 y);"));
		s.append(TString("mat4x3 outerProduct(vec3 x, vec4 y);"));
		s.append(TString("mat4   outerProduct(vec4 x, vec4 y);"));

		s.append(TString("mat2   transpose(mat2 m);"));
		s.append(TString("mat3   transpose(mat3 m);"));
		s.append(TString("mat4   transpose(mat4 m);"));
		s.append(TString("mat2x3 transpose(mat3x2 m);"));
		s.append(TString("mat3x2 transpose(mat2x3 m);"));
		s.append(TString("mat2x4 transpose(mat4x2 m);"));
		s.append(TString("mat4x2 transpose(mat2x4 m);"));
		s.append(TString("mat3x4 transpose(mat4x3 m);"));
		s.append(TString("mat4x3 transpose(mat3x4 m);"));
		//
		// Vector relational functions.
		//
		s.append(TString("bvec2 lessThan(vec2 x, vec2 y);"));
		s.append(TString("bvec3 lessThan(vec3 x, vec3 y);"));
		s.append(TString("bvec4 lessThan(vec4 x, vec4 y);"));

		s.append(TString("bvec2 lessThan(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 lessThan(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 lessThan(ivec4 x, ivec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 lessThan(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 lessThan(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 lessThan(uvec4 x, uvec4 y);"));

		s.append(TString("bvec2 lessThanEqual(vec2 x, vec2 y);"));
		s.append(TString("bvec3 lessThanEqual(vec3 x, vec3 y);"));
		s.append(TString("bvec4 lessThanEqual(vec4 x, vec4 y);"));

		s.append(TString("bvec2 lessThanEqual(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 lessThanEqual(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 lessThanEqual(ivec4 x, ivec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 lessThanEqual(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 lessThanEqual(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 lessThanEqual(uvec4 x, uvec4 y);"));

		s.append(TString("bvec2 greaterThan(vec2 x, vec2 y);"));
		s.append(TString("bvec3 greaterThan(vec3 x, vec3 y);"));
		s.append(TString("bvec4 greaterThan(vec4 x, vec4 y);"));

		s.append(TString("bvec2 greaterThan(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 greaterThan(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 greaterThan(ivec4 x, ivec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 greaterThan(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 greaterThan(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 greaterThan(uvec4 x, uvec4 y);"));

		s.append(TString("bvec2 greaterThanEqual(vec2 x, vec2 y);"));
		s.append(TString("bvec3 greaterThanEqual(vec3 x, vec3 y);"));
		s.append(TString("bvec4 greaterThanEqual(vec4 x, vec4 y);"));

		s.append(TString("bvec2 greaterThanEqual(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 greaterThanEqual(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 greaterThanEqual(ivec4 x, ivec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 greaterThanEqual(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 greaterThanEqual(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 greaterThanEqual(uvec4 x, uvec4 y);"));

		s.append(TString("bvec2 equal(vec2 x, vec2 y);"));
		s.append(TString("bvec3 equal(vec3 x, vec3 y);"));
		s.append(TString("bvec4 equal(vec4 x, vec4 y);"));

		s.append(TString("bvec2 equal(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 equal(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 equal(ivec4 x, ivec4 y);"));

		s.append(TString("bvec2 equal(bvec2 x, bvec2 y);"));
		s.append(TString("bvec3 equal(bvec3 x, bvec3 y);"));
		s.append(TString("bvec4 equal(bvec4 x, bvec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 equal(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 equal(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 equal(uvec4 x, uvec4 y);"));

		s.append(TString("bvec2 notEqual(vec2 x, vec2 y);"));
		s.append(TString("bvec3 notEqual(vec3 x, vec3 y);"));
		s.append(TString("bvec4 notEqual(vec4 x, vec4 y);"));

		s.append(TString("bvec2 notEqual(ivec2 x, ivec2 y);"));
		s.append(TString("bvec3 notEqual(ivec3 x, ivec3 y);"));
		s.append(TString("bvec4 notEqual(ivec4 x, ivec4 y);"));

		s.append(TString("bvec2 notEqual(bvec2 x, bvec2 y);"));
		s.append(TString("bvec3 notEqual(bvec3 x, bvec3 y);"));
		s.append(TString("bvec4 notEqual(bvec4 x, bvec4 y);"));

		// EXT_gpu_shader4
		s.append(TString("bvec2 notEqual(uvec2 x, uvec2 y);"));
		s.append(TString("bvec3 notEqual(uvec3 x, uvec3 y);"));
		s.append(TString("bvec4 notEqual(uvec4 x, uvec4 y);"));

		s.append(TString("bool any(bvec2 x);"));
		s.append(TString("bool any(bvec3 x);"));
		s.append(TString("bool any(bvec4 x);"));

		s.append(TString("bool all(bvec2 x);"));
		s.append(TString("bool all(bvec3 x);"));
		s.append(TString("bool all(bvec4 x);"));

		s.append(TString("bvec2 not(bvec2 x);"));
		s.append(TString("bvec3 not(bvec3 x);"));
		s.append(TString("bvec4 not(bvec4 x);"));

		//
		// Texture Functions.
		//
		s.append(TString("vec4  texture1D(sampler1D  sampler, float coord);"));
		s.append(TString("ivec4 texture1D(isampler1D sampler, float coord);"));
		s.append(TString("uvec4 texture1D(usampler1D sampler, float coord);"));

		s.append(
				TString(
						"vec4  texture1DOffset(sampler1D  sampler, float coord, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DOffset(isampler1D sampler, float coord, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DOffset(usampler1D sampler, float coord, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProj(sampler1D  sampler, vec2 coord);"));
		s.append(
				TString(
						"ivec4 texture1DProj(isampler1D sampler, vec2 coord);"));
		s.append(
				TString(
						"uvec4 texture1DProj(usampler1D sampler, vec2 coord);"));

		s.append(
				TString(
						"vec4  texture1DProjOffset(sampler1D  sampler, vec2 coord, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjOffset(isampler1D sampler, vec2 coord, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjOffset(usampler1D sampler, vec2 coord, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProj(sampler1D  sampler, vec4 coord);"));
		s.append(
				TString(
						"ivec4 texture1DProj(isampler1D sampler, vec4 coord);"));
		s.append(
				TString(
						"uvec4 texture1DProj(usampler1D sampler, vec4 coord);"));

		s.append(
				TString(
						"vec4  texture1DProjOffset(sampler1D  sampler, vec4 coord, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjOffset(isampler1D sampler, vec4 coord, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjOffset(usampler1D sampler, vec4 coord, int offset);"));

		s.append(TString("vec4  texture2D(sampler2D  sampler, vec2 coord);"));
		s.append(TString("ivec4 texture2D(isampler2D sampler, vec2 coord);"));
		s.append(TString("uvec4 texture2D(usampler2D sampler, vec2 coord);"));

		s.append(
				TString(
						"vec4  texture2DOffset(sampler2D  sampler, vec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DOffset(isampler2D sampler, vec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DOffset(usampler2D sampler, vec2 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProj(sampler2D  sampler, vec3 coord);"));
		s.append(
				TString(
						"ivec4 texture2DProj(isampler2D sampler, vec3 coord);"));
		s.append(
				TString(
						"uvec4 texture2DProj(usampler2D sampler, vec3 coord);"));

		s.append(
				TString(
						"vec4  texture2DProjOffset(sampler2D  sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjOffset(isampler2D sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjOffset(usampler2D sampler, vec3 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProj(sampler2D  sampler, vec4 coord);"));
		s.append(
				TString(
						"ivec4 texture2DProj(isampler2D sampler, vec4 coord);"));
		s.append(
				TString(
						"uvec4 texture2DProj(usampler2D sampler, vec4 coord);"));

		s.append(
				TString(
						"vec4  texture2DProjOffset(sampler2D  sampler, vec4 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjOffset(isampler2D sampler, vec4 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjOffset(usampler2D sampler, vec4 coord, ivec2 offset);"));

		s.append(TString("vec4  texture3D(sampler3D  sampler, vec3 coord);"));
		s.append(TString("ivec4 texture3D(isampler3D sampler, vec3 coord);"));
		s.append(TString("uvec4 texture3D(usampler3D sampler, vec3 coord);"));

		s.append(
				TString(
						"vec4  texture3DOffset(sampler3D  sampler, vec3 coord, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DOffset(isampler3D sampler, vec3 coord, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DOffset(usampler3D sampler, vec3 coord, ivec3 offset);"));

		s.append(
				TString(
						"vec4  texture3DProj(sampler3D  sampler, vec4 coord);"));
		s.append(
				TString(
						"ivec4 texture3DProj(isampler3D sampler, vec4 coord);"));
		s.append(
				TString(
						"uvec4 texture3DProj(usampler3D sampler, vec4 coord);"));

		s.append(
				TString(
						"vec4  texture3DProjOffset(sampler3D  sampler, vec4 coord, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DProjOffset(isampler3D sampler, vec4 coord, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DProjOffset(usampler3D sampler, vec4 coord, ivec3 offset);"));

		s.append(
				TString(
						"vec4  textureCube(samplerCube  sampler, vec3 coord);"));
		s.append(
				TString(
						"ivec4 textureCube(isamplerCube sampler, vec3 coord);"));
		s.append(
				TString(
						"uvec4 textureCube(usamplerCube sampler, vec3 coord);"));

		s.append(
				TString("vec4 shadow1D(sampler1DShadow sampler, vec3 coord);"));
		s.append(
				TString(
						"vec4 shadow1DOffset(sampler1DShadow sampler, vec3 coord, int offset);"));
		s.append(
				TString("vec4 shadow2D(sampler2DShadow sampler, vec3 coord);"));
		s.append(
				TString(
						"vec4 shadow2DOffset(sampler2DShadow sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow1DProj(sampler1DShadow sampler, vec4 coord);"));
		s.append(
				TString(
						"vec4 shadow1DProjOffset(sampler1DShadow sampler, vec4 coord, int offset);"));
		s.append(
				TString(
						"vec4 shadow2DProj(sampler2DShadow sampler, vec4 coord);"));
		s.append(
				TString(
						"vec4 shadow2DProjOffset(sampler2DShadow sampler, vec4 coord, ivec2 offset);"));

		//
		// Texture Functions (formaly vertex shaders only! EXT_gpu_shader4 allowes general usage)
		//
		s.append(
				TString(
						"vec4  texture1DLod(sampler1D  sampler, float coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture1DLod(isampler1D sampler, float coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture1DLod(usampler1D sampler, float coord, float lod);"));

		s.append(
				TString(
						"vec4  texture1DLodOffset(sampler1D  sampler, float coord, float lod, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DLodOffset(isampler1D sampler, float coord, float lod, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DLodOffset(usampler1D sampler, float coord, float lod, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProjLod(sampler1D  sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture1DProjLod(isampler1D sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture1DProjLod(usampler1D sampler, vec2 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture1DProjLodOffset(sampler1D  sampler, vec2 coord, float lod, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjLodOffset(isampler1D sampler, vec2 coord, float lod, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjLodOffset(usampler1D sampler, vec2 coord, float lod, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProjLod(sampler1D  sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture1DProjLod(isampler1D sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture1DProjLod(usampler1D sampler, vec4 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture1DProjLodOffset(sampler1D  sampler, vec4 coord, float lod, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjLodOffset(isampler1D sampler, vec4 coord, float lod, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjLodOffset(usampler1D sampler, vec4 coord, float lod, int offset);"));

		s.append(
				TString(
						"vec4  texture2DLod(sampler2D  sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture2DLod(isampler2D sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture2DLod(usampler2D sampler, vec2 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture2DLodOffset(sampler2D  sampler, vec2 coord, float lod, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DLodOffset(isampler2D sampler, vec2 coord, float lod, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DLodOffset(usampler2D sampler, vec2 coord, float lod, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProjLod(sampler2D  sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture2DProjLod(isampler2D sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture2DProjLod(usampler2D sampler, vec3 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture2DProjLodOffset(sampler2D  sampler, vec3 coord, float lod, vec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjLodOffset(isampler2D sampler, vec3 coord, float lod, vec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjLodOffset(usampler2D sampler, vec3 coord, float lod, vec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProjLod(sampler2D  sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture2DProjLod(isampler2D sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture2DProjLod(usampler2D sampler, vec4 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture2DProjLodOffset(sampler2D  sampler, vec4 coord, float lod, vec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjLodOffset(isampler2D sampler, vec4 coord, float lod, vec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjLodOffset(usampler2D sampler, vec4 coord, float lod, vec2 offset);"));

		s.append(
				TString(
						"vec4  texture3DLod(sampler3D  sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture3DLod(isampler3D sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture3DLod(usampler3D sampler, vec3 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture3DLodOffset(sampler3D  sampler, vec3 coord, float lod, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DLodOffset(isampler3D sampler, vec3 coord, float lod, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DLodOffset(usampler3D sampler, vec3 coord, float lod, ivec3 offset);"));

		s.append(
				TString(
						"vec4  texture3DProjLod(sampler3D  sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture3DProjLod(isampler3D sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture3DProjLod(usampler3D sampler, vec4 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture3DProjLodOffset(sampler3D  sampler, vec4 coord, float lod, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DProjLodOffset(isampler3D sampler, vec4 coord, float lod, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DProjLodOffset(usampler3D sampler, vec4 coord, float lod, ivec3 offset);"));

		s.append(
				TString(
						"vec4  textureCubeLod(samplerCube  sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"ivec4 textureCubeLod(isamplerCube sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"uvec4 textureCubeLod(usamplerCube sampler, vec3 coord, float lod);"));

		s.append(
				TString(
						"vec4 shadow1DLod(sampler1DShadow sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"vec4 shadow1DLodOffset(sampler1DShadow sampler, vec3 coord, float lod, int offset);"));
		s.append(
				TString(
						"vec4 shadow2DLod(sampler2DShadow sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"vec4 shadow2DLodOffset(sampler2DShadow sampler, vec3 coord, float lod, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow1DProjLod(sampler1DShadow sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"vec4 shadow1DProjLodOffset(sampler1DShadow sampler, vec4 coord, float lod, int offset);"));
		s.append(
				TString(
						"vec4 shadow2DProjLod(sampler2DShadow sampler, vec4 coord, float lod);"));
		s.append(
				TString(
						"vec4 shadow2DProjLodOffset(sampler2DShadow sampler, vec4 coord, float lod, ivec2 offset);"));

		// ARB_texture_rectangle
		s.append(
				TString(
						"vec4  texture2DRect(sampler2DRect  sampler, vec2 coord);"));
		s.append(
				TString(
						"ivec4 texture2DRect(isampler2DRect sampler, vec2 coord);"));
		s.append(
				TString(
						"uvec4 texture2DRect(usampler2DRect sampler, vec2 coord);"));

		s.append(
				TString(
						"vec4  texture2DRectOffset(sampler2DRect  sampler, vec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectOffset(isampler2DRect sampler, vec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectOffset(usampler2DRect sampler, vec2 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DRectProj(sampler2DRect  sampler, vec3 coord);"));
		s.append(
				TString(
						"ivec4 texture2DRectProj(isampler2DRect sampler, vec3 coord);"));
		s.append(
				TString(
						"uvec4 texture2DRectProj(usampler2DRect sampler, vec3 coord);"));

		s.append(
				TString(
						"vec4  texture2DRectProjOffset(sampler2DRect  sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjOffset(isampler2DRect sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjOffset(usampler2DRect sampler, vec3 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DRectProj(sampler2DRect  sampler, vec4 coord);"));
		s.append(
				TString(
						"ivec4 texture2DRectProj(isampler2DRect sampler, vec4 coord);"));
		s.append(
				TString(
						"uvec4 texture2DRectProj(usampler2DRect sampler, vec4 coord);"));

		s.append(
				TString(
						"vec4  texture2DRectProjOffset(sampler2DRect  sampler, vec4 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjOffset(isampler2DRect sampler, vec4 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjOffset(usampler2DRect sampler, vec4 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4 shadow2DRect(sampler2DRectShadow sampler, vec3 coord);"));
		s.append(
				TString(
						"vec4 shadow2DRectOffset(sampler2DRectShadow sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow2DRectProj(sampler2DRectShadow sampler, vec4 coord);"));
		s.append(
				TString(
						"vec4 shadow2DRectProjOffset(sampler2DRectShadow sampler, vec4 coord, ivec2 offset);"));

		// EXT_gpu_shader4
		s.append(
				TString(
						"vec4  texelFetch1D(sampler1D  sampler, int coord, int lod);"));
		s.append(
				TString(
						"ivec4 texelFetch1D(isampler1D sampler, int coord, int lod);"));
		s.append(
				TString(
						"uvec4 texelFetch1D(usampler1D sampler, int coord, int lod);"));

		s.append(
				TString(
						"vec4  texelFetch1DOffset(sampler1D  sampler, int coord, int lod, int offset);"));
		s.append(
				TString(
						"ivec4 texelFetch1DOffset(isampler1D sampler, int coord, int lod, int offset);"));
		s.append(
				TString(
						"uvec4 texelFetch1DOffset(usampler1D sampler, int coord, int lod, int offset);"));

		s.append(
				TString(
						"vec4  texelFetch2D(sampler2D  sampler, ivec2 coord, int lod);"));
		s.append(
				TString(
						"ivec4 texelFetch2D(isampler2D sampler, ivec2 coord, int lod);"));
		s.append(
				TString(
						"uvec4 texelFetch2D(usampler2D sampler, ivec2 coord, int lod);"));

		s.append(
				TString(
						"vec4  texelFetch2DOffset(sampler2D  sampler, ivec2 coord, int lod, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texelFetch2DOffset(isampler2D sampler, ivec2 coord, int lod, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texelFetch2DOffset(usampler2D sampler, ivec2 coord, int lod, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texelFetch3D(sampler3D  sampler, ivec3 coord, int lod);"));
		s.append(
				TString(
						"ivec4 texelFetch3D(isampler3D sampler, ivec3 coord, int lod);"));
		s.append(
				TString(
						"uvec4 texelFetch3D(usampler3D sampler, ivec3 coord, int lod);"));

		s.append(
				TString(
						"vec4  texelFetch3DOffset(sampler3D  sampler, ivec3 coord, int lod, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texelFetch3DOffset(isampler3D sampler, ivec3 coord, int lod, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texelFetch3DOffset(usampler3D sampler, ivec3 coord, int lod, ivec3 offset);"));

		s.append(
				TString(
						"vec4  texelFetch2DRect(sampler2DRect  sampler, ivec2 coord);"));
		s.append(
				TString(
						"ivec4 texelFetch2DRect(isampler2DRect sampler, ivec2 coord);"));
		s.append(
				TString(
						"uvec4 texelFetch2DRect(usampler2DRect sampler, ivec2 coord);"));

		s.append(
				TString(
						"vec4  texelFetch2DRectOffset(sampler2DRect  sampler, ivec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texelFetch2DRectOffset(isampler2DRect sampler, ivec2 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texelFetch2DRectOffset(usampler2DRect sampler, ivec2 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texelFetch1DArray(sampler1DArray  sampler, ivec2 coord, int lod);"));
		s.append(
				TString(
						"ivec4 texelFetch1DArray(isampler1DArray sampler, ivec2 coord, int lod);"));
		s.append(
				TString(
						"uvec4 texelFetch1DArray(usampler1DArray sampler, ivec2 coord, int lod);"));

		s.append(
				TString(
						"vec4  texelFetch1DArrayOffset(sampler1DArray  sampler, ivec2 coord, int lod, int offset);"));
		s.append(
				TString(
						"ivec4 texelFetch1DArrayOffset(isampler1DArray sampler, ivec2 coord, int lod, int offset);"));
		s.append(
				TString(
						"uvec4 texelFetch1DArrayOffset(usampler1DArray sampler, ivec2 coord, int lod, int offset);"));

		s.append(
				TString(
						"vec4  texelFetch2DArray(sampler2DArray  sampler, ivec3 coord, int lod);"));
		s.append(
				TString(
						"ivec4 texelFetch2DArray(isampler2DArray sampler, ivec3 coord, int lod);"));
		s.append(
				TString(
						"uvec4 texelFetch2DArray(usampler2DArray sampler, ivec3 coord, int lod);"));

		s.append(
				TString(
						"vec4  texelFetch2DArrayOffset(sampler2DArray  sampler, ivec3 coord, int lod, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texelFetch2DArrayOffset(isampler2DArray sampler, ivec3 coord, int lod, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texelFetch2DArrayOffset(usampler2DArray sampler, ivec3 coord, int lod, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texelFetchBuffer(samplerBuffer  sampler, int coord);"));
		s.append(
				TString(
						"ivec4 texelFetchBuffer(isamplerBuffer sampler, int coord);"));
		s.append(
				TString(
						"uvec4 texelFetchBuffer(usamplerBuffer sampler, int coord);"));

		s.append(TString("int textureSizeBuffer(samplerBuffer  sampler);"));
		s.append(TString("int textureSizeBuffer(isamplerBuffer sampler);"));
		s.append(TString("int textureSizeBuffer(usamplerBuffer sampler);"));

		s.append(TString("int textureSize1D(sampler1D  sampler, int lod);"));
		s.append(TString("int textureSize1D(isampler1D sampler, int lod);"));
		s.append(TString("int textureSize1D(usampler1D sampler, int lod);"));

		s.append(TString("ivec2 textureSize2D(sampler2D  sampler, int lod);"));
		s.append(TString("ivec2 textureSize2D(isampler2D sampler, int lod);"));
		s.append(TString("ivec2 textureSize2D(usampler2D sampler, int lod);"));

		s.append(TString("ivec3 textureSize3D(sampler3D  sampler, int lod);"));
		s.append(TString("ivec3 textureSize3D(isampler3D sampler, int lod);"));
		s.append(TString("ivec3 textureSize3D(usampler3D sampler, int lod);"));

		s.append(
				TString(
						"ivec2 textureSizeCube(samplerCube  sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSizeCube(isamplerCube sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSizeCube(usamplerCube sampler, int lod);"));

		s.append(
				TString(
						"ivec2 textureSize2DRect(sampler2DRect  sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSize2DRect(isampler2DRect sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSize2DRect(usampler2DRect sampler, int lod);"));

		s.append(
				TString(
						"ivec2 textureSize1DArray(sampler1DArray  sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSize1DArray(isampler1DArray sampler, int lod);"));
		s.append(
				TString(
						"ivec2 textureSize1DArray(usampler1DArray sampler, int lod);"));

		s.append(
				TString(
						"ivec3 textureSize2DArray(sampler2DArray  sampler, int lod);"));
		s.append(
				TString(
						"ivec3 textureSize2DArray(isampler2DArray sampler, int lod);"));
		s.append(
				TString(
						"ivec3 textureSize2DArray(usampler2DArray sampler, int lod);"));

		s.append(
				TString(
						"vec4  texture1DArray(sampler1DArray  sampler, vec2 coord);"));
		s.append(
				TString(
						"ivec4 texture1DArray(isampler1DArray sampler, vec2 coord);"));
		s.append(
				TString(
						"uvec4 texture1DArray(usampler1DArray sampler, vec2 coord);"));

		s.append(
				TString(
						"vec4  texture1DArrayOffset(sampler1DArray  sampler, vec2 coord, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DArrayOffset(isampler1DArray sampler, vec2 coord, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DArrayOffset(usampler1DArray sampler, vec2 coord, int offset);"));

		s.append(
				TString(
						"vec4  texture1DArrayLod(sampler1DArray  sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture1DArrayLod(isampler1DArray sampler, vec2 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture1DArrayLod(usampler1DArray sampler, vec2 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture1DArrayLodOffset(sampler1DArray  sampler, vec2 coord, float lod, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DArrayLodOffset(isampler1DArray sampler, vec2 coord, float lod, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DArrayLodOffset(usampler1DArray sampler, vec2 coord, float lod, int offset);"));

		s.append(
				TString(
						"vec4  texture2DArray(sampler2DArray  sampler, vec3 coord);"));
		s.append(
				TString(
						"ivec4 texture2DArray(isampler2DArray sampler, vec3 coord);"));
		s.append(
				TString(
						"uvec4 texture2DArray(usampler2DArray sampler, vec3 coord);"));

		s.append(
				TString(
						"vec4  texture2DArrayOffset(sampler2DArray  sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DArrayOffset(isampler2DArray sampler, vec3 coord, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DArrayOffset(usampler2DArray sampler, vec3 coord, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DArrayLod(sampler2DArray  sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"ivec4 texture2DArrayLod(isampler2DArray sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"uvec4 texture2DArrayLod(usampler2DArray sampler, vec3 coord, float lod);"));

		s.append(
				TString(
						"vec4  texture2DArrayLodOffset(sampler2DArray  sampler, vec3 coord, float lod, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DArrayLodOffset(isampler2DArray sampler, vec3 coord, float lod, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DArrayLodOffset(usampler2DArray sampler, vec3 coord, float lod, ivec2 offset);"));

		s.append(
				TString(
						"vec4  shadow1DArray(sampler1DArrayShadow  sampler, vec3 coord);"));
		s.append(
				TString(
						"vec4  shadow1DArrayOffset(sampler1DArrayShadow  sampler, vec3 coord, int offset);"));
		s.append(
				TString(
						"vec4  shadow1DArrayLod(sampler1DArrayShadow  sampler, vec3 coord, float lod);"));
		s.append(
				TString(
						"vec4  shadow1DArrayLodOffset(sampler1DArrayShadow  sampler, vec3 coord, float lod, int offset);"));
		s.append(
				TString(
						"vec4  shadow2DArray(sampler2DArrayShadow  sampler, vec4 coord);"));
		s.append(
				TString(
						"vec4  shadow2DArrayOffset(sampler2DArrayShadow  sampler, vec4 coord, ivec2 offset);"));
		s.append(
				TString(
						"vec4  shadowCube(samplerCubeShadow  sampler, vec4 coord);"));

		//
		s.append(
				TString(
						"vec4  texture1DGrad(sampler1D  sampler, float coord, float ddx, float ddy);"));
		s.append(
				TString(
						"ivec4 texture1DGrad(isampler1D sampler, float coord, float ddx, float ddy);"));
		s.append(
				TString(
						"uvec4 texture1DGrad(usampler1D sampler, float coord, float ddx, float ddy);"));

		s.append(
				TString(
						"vec4  texture1DGradOffset(sampler1D  sampler, float coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DGradOffset(isampler1D sampler, float coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DGradOffset(usampler1D sampler, float coord, float ddx, float ddy, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProjGrad(sampler1D  sampler, vec2 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"ivec4 texture1DProjGrad(isampler1D sampler, vec2 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"uvec4 texture1DProjGrad(usampler1D sampler, vec2 coord, float ddx, float ddy);"));

		s.append(
				TString(
						"vec4  texture1DProjGradOffset(sampler1D  sampler, vec2 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjGradOffset(isampler1D sampler, vec2 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjGradOffset(usampler1D sampler, vec2 coord, float ddx, float ddy, int offset);"));

		s.append(
				TString(
						"vec4  texture1DProjGrad(sampler1D  sampler, vec4 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"ivec4 texture1DProjGrad(isampler1D sampler, vec4 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"uvec4 texture1DProjGrad(usampler1D sampler, vec4 coord, float ddx, float ddy);"));

		s.append(
				TString(
						"vec4  texture1DProjGradOffset(sampler1D  sampler, vec4 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DProjGradOffset(isampler1D sampler, vec4 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DProjGradOffset(usampler1D sampler, vec4 coord, float ddx, float ddy, int offset);"));

		s.append(
				TString(
						"vec4  texture1DGradArray(sampler1DArray  sampler, vec2 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"ivec4 texture1DGradArray(isampler1DArray sampler, vec2 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"uvec4 texture1DGradArray(usampler1DArray sampler, vec2 coord, float ddx, float ddy);"));

		s.append(
				TString(
						"vec4  texture1DGradArrayOffset(sampler1DArray  sampler, vec2 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"ivec4 texture1DGradArrayOffset(isampler1DArray sampler, vec2 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"uvec4 texture1DGradArrayOffset(usampler1DArray sampler, vec2 coord, float ddx, float ddy, int offset);"));

		s.append(
				TString(
						"vec4  texture2DGrad(sampler2D  sampler, vec2 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DGrad(isampler2D sampler, vec2 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DGrad(usampler2D sampler, vec2 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DGradOffset(sampler2D  sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DGradOffset(isampler2D sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DGradOffset(usampler2D sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProjGrad(sampler2D  sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DProjGrad(isampler2D sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DProjGrad(usampler2D sampler, vec3 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DProjGradOffset(sampler2D  sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjGradOffset(isampler2D sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjGradOffset(usampler2D sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DProjGrad(sampler2D  sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DProjGrad(isampler2D sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DProjGrad(usampler2D sampler, vec4 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DProjGradOffset(sampler2D  sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DProjGradOffset(isampler2D sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DProjGradOffset(usampler2D sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DGradArray(sampler2DArray  sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DGradArray(isampler2DArray sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DGradArray(usampler2DArray sampler, vec3 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DGradArrayOffset(sampler2DArray  sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DGradArrayOffset(isampler2DArray sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DGradArrayOffset(usampler2DArray sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture3DGrad(sampler3D  sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"ivec4 texture3DGrad(isampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"uvec4 texture3DGrad(usampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy);"));

		s.append(
				TString(
						"vec4  texture3DGradOffset(sampler3D  sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DGradOffset(isampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DGradOffset(usampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));

		s.append(
				TString(
						"vec4  texture3DProjGrad(sampler3D  sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"ivec4 texture3DProjGrad(isampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"uvec4 texture3DProjGrad(usampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy);"));

		s.append(
				TString(
						"vec4  texture3DProjGradOffset(sampler3D  sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));
		s.append(
				TString(
						"ivec4 texture3DProjGradOffset(isampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));
		s.append(
				TString(
						"uvec4 texture3DProjGradOffset(usampler3D sampler, vec3 coord, vec3 ddx, vec3 ddy, ivec3 offset);"));

		s.append(
				TString(
						"vec4  textureCubeGrad(samplerCube  sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"ivec4 textureCubeGrad(isamplerCube sampler, vec3 coord, vec3 ddx, vec3 ddy);"));
		s.append(
				TString(
						"uvec4 textureCubeGrad(usamplerCube sampler, vec3 coord, vec3 ddx, vec3 ddy);"));

		s.append(
				TString(
						"vec4 shadow1DGrad(sampler1DShadow sampler, vec3 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"vec4 shadow1DGradOffset(sampler1DShadow sampler, vec3 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"vec4 shadow1DProjGrad(sampler1DShadow sampler, vec4 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"vec4 shadow1DProjGradOffset(sampler1DShadow sampler, vec4 coord, float ddx, float ddy, int offset);"));
		s.append(
				TString(
						"vec4 shadow1DArrayGrad(sampler1DArrayShadow sampler, vec3 coord, float ddx, float ddy);"));
		s.append(
				TString(
						"vec4 shadow1DArrayGradOffset(sampler1DArrayShadow sampler, vec3 coord, float ddx, float ddy, int offset);"));

		s.append(
				TString(
						"vec4 shadow2DGrad(sampler2DShadow sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"vec4 shadow2DGradOffset(sampler2DShadow sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow2DProjGrad(sampler2DShadow sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"vec4 shadow2DProjGradOffset(sampler2DShadow sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow2DArrayGrad(sampler2DArrayShadow sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"vec4 shadow2DArrayGradOffset(sampler2DArrayShadow sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DRectGrad(sampler2DRect  sampler, vec2 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DRectGrad(isampler2DRect sampler, vec2 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DRectGrad(usampler2DRect sampler, vec2 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DRectGradOffset(sampler2DRect  sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectGradOffset(isampler2DRect sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectGradOffset(usampler2DRect sampler, vec2 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DRectProjGrad(sampler2DRect  sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjGrad(isampler2DRect sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjGrad(usampler2DRect sampler, vec3 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DRectProjGradOffset(sampler2DRect  sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjGradOffset(isampler2DRect sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjGradOffset(usampler2DRect sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  texture2DRectProjGrad(sampler2DRect  sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjGrad(isampler2DRect sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjGrad(usampler2DRect sampler, vec4 coord, vec2 ddx, vec2 ddy);"));

		s.append(
				TString(
						"vec4  texture2DRectProjGradOffset(sampler2DRect  sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"ivec4 texture2DRectProjGradOffset(isampler2DRect sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"uvec4 texture2DRectProjGradOffset(usampler2DRect sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4 shadow2DRectGrad(sampler2DRectShadow sampler, vec3 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"vec4 shadow2DRectGradOffset(sampler2DRectShadow sampler, vec3 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));
		s.append(
				TString(
						"vec4 shadow2DRectProjGrad(sampler2DRectShadow sampler, vec4 coord, vec2 ddx, vec2 ddy);"));
		s.append(
				TString(
						"vec4 shadow2DRectProjGradOffset(sampler2DRectShadow sampler, vec4 coord, vec2 ddx, vec2 ddy, ivec2 offset);"));

		s.append(
				TString(
						"vec4  shadowCubeGrad(samplerCubeShadow sampler, vec4 coord, vec3 ddx, vec3 ddy);"));

		//
		// Noise functions.
		//
		s.append(TString("float noise1(float x);"));
		s.append(TString("float noise1(vec2  x);"));
		s.append(TString("float noise1(vec3  x);"));
		s.append(TString("float noise1(vec4  x);"));

		s.append(TString("vec2 noise2(float x);"));
		s.append(TString("vec2 noise2(vec2  x);"));
		s.append(TString("vec2 noise2(vec3  x);"));
		s.append(TString("vec2 noise2(vec4  x);"));

		s.append(TString("vec3 noise3(float x);"));
		s.append(TString("vec3 noise3(vec2  x);"));
		s.append(TString("vec3 noise3(vec3  x);"));
		s.append(TString("vec3 noise3(vec4  x);"));

		s.append(TString("vec4 noise4(float x);"));
		s.append(TString("vec4 noise4(vec2  x);"));
		s.append(TString("vec4 noise4(vec3  x);"));
		s.append(TString("vec4 noise4(vec4  x);"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Prototypes for built-in functions seen by vertex shaders only.
		//
		//============================================================================

		TString& s = BuiltInFunctionsVertex;

		//
		// Geometric Functions.
		//
		s.append(TString("vec4 ftransform();"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Prototypes for built-in functions seen by geometry shaders only.
		//
		//============================================================================

		TString& s = BuiltInFunctionsGeometry;

		//
		// Geometric Functions.
		//
		s.append(TString("void EmitVertex();"));
		s.append(TString("void EndPrimitive();"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Prototypes for built-in functions seen by fragment shaders only.
		//
		//============================================================================

		TString& s = BuiltInFunctionsFragment;

		//
		// Texture Functions.
		//
		s.append(
				TString(
						"vec4  texture1D(sampler1D  sampler, float coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture1D(isampler1D sampler, float coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture1D(usampler1D sampler, float coord, float bias);"));

		s.append(
				TString(
						"vec4  texture1DOffset(sampler1D  sampler, float coord, int offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DOffset(isampler1D sampler, float coord, int offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DOffset(usampler1D sampler, float coord, int offset, float bias);"));

		s.append(
				TString(
						"vec4  texture1DProj(sampler1D  sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DProj(isampler1D sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DProj(usampler1D sampler, vec2 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture1DProjOffset(sampler1D  sampler, vec2 coord, int offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DProjOffset(isampler1D sampler, vec2 coord, int offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DProjOffset(usampler1D sampler, vec2 coord, int offset, float bias);"));

		s.append(
				TString(
						"vec4  texture1DProj(sampler1D  sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DProj(isampler1D sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DProj(usampler1D sampler, vec4 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture1DProjOffset(sampler1D  sampler, vec4 coord, int offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DProjOffset(isampler1D sampler, vec4 coord, int offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DProjOffset(usampler1D sampler, vec4 coord, int offset, float bias);"));

		s.append(
				TString(
						"vec4  texture2D(sampler2D  sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture2D(isampler2D sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture2D(usampler2D sampler, vec2 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture2DOffset(sampler2D  sampler, vec2 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DOffset(isampler2D sampler, vec2 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DOffset(usampler2D sampler, vec2 coord, ivec2 offset, float bias);"));

		s.append(
				TString(
						"vec4  texture2DProj(sampler2D  sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DProj(isampler2D sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DProj(usampler2D sampler, vec3 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture2DProjOffset(sampler2D  sampler, vec3 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DProjOffset(isampler2D sampler, vec3 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DProjOffset(usampler2D sampler, vec3 coord, ivec2 offset, float bias);"));

		s.append(
				TString(
						"vec4  texture2DProj(sampler2D  sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DProj(isampler2D sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DProj(usampler2D sampler, vec4 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture2DProjOffset(sampler2D  sampler, vec4 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DProjOffset(isampler2D sampler, vec4 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DProjOffset(usampler2D sampler, vec4 coord, ivec2 offset, float bias);"));

		s.append(
				TString(
						"vec4  texture3D(sampler3D  sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture3D(isampler3D sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture3D(usampler3D sampler, vec3 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture3DOffset(sampler3D  sampler, vec3 coord, ivec3 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture3DOffset(isampler3D sampler, vec3 coord, ivec3 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture3DOffset(usampler3D sampler, vec3 coord, ivec3 offset, float bias);"));

		s.append(
				TString(
						"vec4  texture3DProj(sampler3D  sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture3DProj(isampler3D sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture3DProj(usampler3D sampler, vec4 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture3DProjOffset(sampler3D  sampler, vec4 coord, ivec3 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture3DProjOffset(isampler3D sampler, vec4 coord, ivec3 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture3DProjOffset(usampler3D sampler, vec4 coord, ivec3 offset, float bias);"));

		s.append(
				TString(
						"vec4  textureCube(samplerCube  sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"ivec4 textureCube(isamplerCube sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"uvec4 textureCube(usamplerCube sampler, vec3 coord, float bias);"));

		s.append(
				TString(
						"vec4 shadow1D(sampler1DShadow sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"vec4 shadow1DOffset(sampler1DShadow sampler, vec3 coord, int offset, float bias);"));
		s.append(
				TString(
						"vec4 shadow2D(sampler2DShadow sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"vec4 shadow2DOffset(sampler2DShadow sampler, vec3 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"vec4 shadow1DProj(sampler1DShadow sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"vec4 shadow1DProjOffset(sampler1DShadow sampler, vec4 coord, int offset, float bias);"));
		s.append(
				TString(
						"vec4 shadow2DProj(sampler2DShadow sampler, vec4 coord, float bias);"));
		s.append(
				TString(
						"vec4 shadow2DProjOffset(sampler2DShadow sampler, vec4 coord, ivec2 offset, float bias);"));

		// EXT_gpu_shader4
		s.append(
				TString(
						"vec4  texture1DArray(sampler1DArray  sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DArray(isampler1DArray sampler, vec2 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DArray(usampler1DArray sampler, vec2 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture1DArrayOffset(sampler1DArray  sampler, vec2 coord, int offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture1DArrayOffset(isampler1DArray sampler, vec2 coord, int offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture1DArrayOffset(usampler1DArray sampler, vec2 coord, int offset, float bias);"));

		s.append(
				TString(
						"vec4  texture2DArray(sampler2DArray  sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DArray(isampler2DArray sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DArray(usampler2DArray sampler, vec3 coord, float bias);"));

		s.append(
				TString(
						"vec4  texture2DArrayOffset(sampler2DArray  sampler, vec3 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"ivec4 texture2DArrayOffset(isampler2DArray sampler, vec3 coord, ivec2 offset, float bias);"));
		s.append(
				TString(
						"uvec4 texture2DArrayOffset(usampler2DArray sampler, vec3 coord, ivec2 offset, float bias);"));

		s.append(
				TString(
						"vec4  shadow1DArray(sampler1DArrayShadow  sampler, vec3 coord, float bias);"));
		s.append(
				TString(
						"vec4  shadow1DArrayOffset(sampler1DArrayShadow  sampler, vec3 coord, int offset, float bias);"));

		s.append(TString("float dFdx(float p);"));
		s.append(TString("vec2  dFdx(vec2  p);"));
		s.append(TString("vec3  dFdx(vec3  p);"));
		s.append(TString("vec4  dFdx(vec4  p);"));

		s.append(TString("float dFdy(float p);"));
		s.append(TString("vec2  dFdy(vec2  p);"));
		s.append(TString("vec3  dFdy(vec3  p);"));
		s.append(TString("vec4  dFdy(vec4  p);"));

		s.append(TString("float fwidth(float p);"));
		s.append(TString("vec2  fwidth(vec2  p);"));
		s.append(TString("vec3  fwidth(vec3  p);"));
		s.append(TString("vec4  fwidth(vec4  p);"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Standard Uniforms
		//
		//============================================================================

		TString& s = StandardUniforms;

		//
		// OpenGL'uniform' state.  Page numbers are in reference to version
		// 1.4 of the OpenGL specification.
		//

		//
		// Matrix state. p. 31, 32, 37, 39, 40.
		//
		s.append(TString("uniform mat4  gl_ModelViewMatrix;"));
		s.append(TString("uniform mat4  gl_ProjectionMatrix;"));
		s.append(TString("uniform mat4  gl_ModelViewProjectionMatrix;"));

		//
		// Derived matrix state that provides inverse and transposed versions
		// of the matrices above.
		//
		s.append(TString("uniform mat3  gl_NormalMatrix;"));

		s.append(TString("uniform mat4  gl_ModelViewMatrixInverse;"));
		s.append(TString("uniform mat4  gl_ProjectionMatrixInverse;"));
		s.append(TString("uniform mat4  gl_ModelViewProjectionMatrixInverse;"));

		s.append(TString("uniform mat4  gl_ModelViewMatrixTranspose;"));
		s.append(TString("uniform mat4  gl_ProjectionMatrixTranspose;"));
		s.append(
				TString(
						"uniform mat4  gl_ModelViewProjectionMatrixTranspose;"));

		s.append(TString("uniform mat4  gl_ModelViewMatrixInverseTranspose;"));
		s.append(TString("uniform mat4  gl_ProjectionMatrixInverseTranspose;"));
		s.append(
				TString(
						"uniform mat4  gl_ModelViewProjectionMatrixInverseTranspose;"));

		//
		// Normal scaling p. 39.
		//
		s.append(TString("uniform float gl_NormalScale;"));

		//
		// Depth range in window coordinates, p. 33
		//
		s.append(TString("struct gl_DepthRangeParameters {"));
		s.append(TString("    float near;"));        // n
		s.append(TString("    float far;"));         // f
		s.append(TString("    float diff;"));        // f - n
		s.append(TString("};"));
		s.append(TString("uniform gl_DepthRangeParameters gl_DepthRange;"));

		//
		// Point Size, p. 66, 67.
		//
		s.append(TString("struct gl_PointParameters {"));
		s.append(TString("    float size;"));
		s.append(TString("    float sizeMin;"));
		s.append(TString("    float sizeMax;"));
		s.append(TString("    float fadeThresholdSize;"));
		s.append(TString("    float distanceConstantAttenuation;"));
		s.append(TString("    float distanceLinearAttenuation;"));
		s.append(TString("    float distanceQuadraticAttenuation;"));
		s.append(TString("};"));

		s.append(TString("uniform gl_PointParameters gl_Point;"));

		//
		// Material State p. 50, 55.
		//
		s.append(TString("struct gl_MaterialParameters {"));
		s.append(TString("    vec4  emission;"));    // Ecm
		s.append(TString("    vec4  ambient;"));     // Acm
		s.append(TString("    vec4  diffuse;"));     // Dcm
		s.append(TString("    vec4  specular;"));    // Scm
		s.append(TString("    float shininess;"));   // Srm
		s.append(TString("};"));
		s.append(TString("uniform gl_MaterialParameters  gl_FrontMaterial;"));
		s.append(TString("uniform gl_MaterialParameters  gl_BackMaterial;"));

		//
		// Light State p 50, 53, 55.
		//

		s.append(TString("struct gl_LightSourceParameters {"));
		s.append(TString("    vec4  ambient;"));             // Acli
		s.append(TString("    vec4  diffuse;"));             // Dcli
		s.append(TString("    vec4  specular;"));            // Scli
		s.append(TString("    vec4  position;"));            // Ppli
		s.append(TString("    vec4  halfVector;"));          // Derived: Hi
		s.append(TString("    vec3  spotDirection;"));       // Sdli
		s.append(TString("    float spotExponent;"));        // Srli
		s.append(TString("    float spotCutoff;"));          // Crli
															 // (range: [0.0,90.0], 180.0)
		s.append(TString("    float spotCosCutoff;"));     // Derived: cos(Crli)
														   // (range: [1.0,0.0],-1.0)
		s.append(TString("    float constantAttenuation;"));  // K0
		s.append(TString("    float linearAttenuation;"));   // K1
		s.append(TString("    float quadraticAttenuation;"));   // K2
		s.append(TString("};"));

		s.append(TString("struct gl_LightModelParameters {"));
		s.append(TString("    vec4  ambient;"));       // Acs
		s.append(TString("};"));

		s.append(TString("uniform gl_LightModelParameters  gl_LightModel;"));

		//
		// Derived state from products of light and material.
		//

		s.append(TString("struct gl_LightModelProducts {"));
		s.append(TString("    vec4  sceneColor;"));  // Derived. Ecm + Acm * Acs
		s.append(TString("};"));

		s.append(
				TString(
						"uniform gl_LightModelProducts gl_FrontLightModelProduct;"));
		s.append(
				TString(
						"uniform gl_LightModelProducts gl_BackLightModelProduct;"));

		s.append(TString("struct gl_LightProducts {"));
		s.append(TString("    vec4  ambient;"));        // Acm * Acli
		s.append(TString("    vec4  diffuse;"));        // Dcm * Dcli
		s.append(TString("    vec4  specular;"));       // Scm * Scli
		s.append(TString("};"));

		//
		// Fog p. 161
		//
		s.append(TString("struct gl_FogParameters {"));
		s.append(TString("    vec4  color;"));
		s.append(TString("    float density;"));
		s.append(TString("    float start;"));
		s.append(TString("    float end;"));
		s.append(TString("    float scale;"));  //  1 / (gl_FogEnd - gl_FogStart)
		s.append(TString("};"));

		s.append(TString("uniform gl_FogParameters gl_Fog;"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Vertex attributes, p. 19.
		//
		//============================================================================

		TString& s = StandardVertexAttributes;

		s.append(TString("attribute vec4  gl_Color;"));
		s.append(TString("attribute vec4  gl_SecondaryColor;"));
		s.append(TString("attribute vec3  gl_Normal;"));
		s.append(TString("attribute vec4  gl_Vertex;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord0;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord1;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord2;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord3;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord4;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord5;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord6;"));
		s.append(TString("attribute vec4  gl_MultiTexCoord7;"));
		s.append(TString("attribute float gl_FogCoord;"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Define the output varying interface from the vertex shader.
		//
		//============================================================================

		TString& s = StandardVertexVaryings;

		s.append(TString("varying vec4  gl_FrontColor;"));
		s.append(TString("varying vec4  gl_BackColor;"));
		s.append(TString("varying vec4  gl_FrontSecondaryColor;"));
		s.append(TString("varying vec4  gl_BackSecondaryColor;"));
		s.append(TString("varying vec4  gl_TexCoord[];"));
		s.append(TString("varying float gl_FogFragCoord;"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Define the input varying interface from the geometry shader.
		//
		//============================================================================

		TString& s = StandardVaryingOutGeometry;

		s.append(TString("varying out vec4  gl_FrontColor;"));
		s.append(TString("varying out vec4  gl_BackColor;"));
		s.append(TString("varying out vec4  gl_FrontSecondaryColor;"));
		s.append(TString("varying out vec4  gl_BackSecondaryColor;"));
		s.append(TString("varying out vec4  gl_TexCoord[];"));
		s.append(TString("varying out float gl_FogFragCoord;"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Define the input varying interface from the geometry shader.
		//
		//============================================================================

		TString& s = StandardVaryingInGeometry;

		s.append(TString("varying in vec4  gl_FrontColorIn[];"));
		s.append(TString("varying in vec4  gl_BackColorIn[];"));
		s.append(TString("varying in vec4  gl_FrontSecondaryColorIn[];"));
		s.append(TString("varying in vec4  gl_BackSecondaryColorIn[];"));
		s.append(TString("varying in vec4  gl_TexCoordIn[][];"));
		s.append(TString("varying in float gl_FogFragCoordIn[];"));
		s.append(TString("varying in vec4  gl_PositionIn[];"));
		s.append(TString("varying in float gl_PointSizeIn[];"));
		s.append(TString("varying in vec4  gl_ClipVertexIn[];"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Define the input varying interface to the fragment shader.
		//
		//============================================================================

		TString& s = StandardFragmentVaryings;

		s.append(TString("varying vec4  gl_Color;"));
		s.append(TString("varying vec4  gl_SecondaryColor;"));
		s.append(TString("varying vec4  gl_TexCoord[];"));
		s.append(TString("varying float gl_FogFragCoord;"));
		s.append(TString("varying vec2  gl_PointCoord;"));

		s.append(TString("\n"));
	}

	builtInStrings[EShLangFragment].push_back(BuiltInFunctions);
	builtInStrings[EShLangFragment].push_back(BuiltInFunctionsFragment);
	builtInStrings[EShLangFragment].push_back(StandardUniforms);
	builtInStrings[EShLangFragment].push_back(StandardFragmentVaryings);

	builtInStrings[EShLangGeometry].push_back(BuiltInFunctions);
	builtInStrings[EShLangGeometry].push_back(BuiltInFunctionsGeometry);
	builtInStrings[EShLangGeometry].push_back(StandardVaryingOutGeometry);
	builtInStrings[EShLangGeometry].push_back(StandardVaryingInGeometry);
	builtInStrings[EShLangGeometry].push_back(StandardUniforms);

	builtInStrings[EShLangVertex].push_back(BuiltInFunctions);
	builtInStrings[EShLangVertex].push_back(BuiltInFunctionsVertex);
	builtInStrings[EShLangVertex].push_back(StandardVertexVaryings);
	builtInStrings[EShLangVertex].push_back(StandardVertexAttributes);
	builtInStrings[EShLangVertex].push_back(StandardUniforms);
}

void TBuiltIns::initialize(const TBuiltInResource &resources)
{
	//
	// Initialize all the built-in strings for parsing.
	//
	TString StandardUniforms;
	TString GeometryUniforms;

	{
		//============================================================================
		//
		// Standard Uniforms
		//
		//============================================================================

		TString& s = StandardUniforms;

		//
		// Implementation dependent constants.  The example values below
		// are the minimum values allowed for these maximums.
		//
		char builtInConstant[80];
		sprintf(builtInConstant, "const int  gl_MaxLights = %d;",
				resources.maxLights);  // GL 1.0
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxClipPlanes = %d;",
				resources.maxClipPlanes);  // GL 1.0
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxTextureUnits = %d;",
				resources.maxTextureUnits);  // GL 1.2
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxTextureCoords = %d;",
				resources.maxTextureCoords);  // ARB_fragment_program
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxVertexAttribs = %d;",
				resources.maxVertexAttribs);  // ARB_vertex_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant,
				"const int  gl_MaxVertexUniformComponents = %d;",
				resources.maxVertexUniformComponents);  // ARB_vertex_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxVaryingFloats = %d;",
				resources.maxVaryingFloats);  // ARB_vertex_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant,
				"const int  gl_MaxVertexTextureImageUnits = %d;",
				resources.maxVertexTextureImageUnits);  // ARB_vertex_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant,
				"const int  gl_MaxCombinedTextureImageUnits = %d;",
				resources.maxCombinedTextureImageUnits);  // ARB_vertex_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxTextureImageUnits = %d;",
				resources.maxTextureImageUnits);  // ARB_fragment_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant,
				"const int  gl_MaxFragmentUniformComponents = %d;",
				resources.maxFragmentUniformComponents);  // ARB_fragment_shader
		s.append(TString(builtInConstant));

		sprintf(builtInConstant, "const int  gl_MaxDrawBuffers = %d;",
				resources.maxDrawBuffers);  // proposed ARB_draw_buffers
		s.append(TString(builtInConstant));

		//
		// OpenGL'uniform' state.  Page numbers are in reference to version
		// 1.4 of the OpenGL specification.
		//

		//
		// Matrix state. p. 31, 32, 37, 39, 40.
		//
		s.append(
				TString(
						"uniform mat4  gl_TextureMatrix[gl_MaxTextureCoords];"));

		//
		// Derived matrix state that provides inverse and transposed versions
		// of the matrices above.
		//
		s.append(
				TString(
						"uniform mat4  gl_TextureMatrixInverse[gl_MaxTextureCoords];"));

		s.append(
				TString(
						"uniform mat4  gl_TextureMatrixTranspose[gl_MaxTextureCoords];"));

		s.append(
				TString(
						"uniform mat4  gl_TextureMatrixInverseTranspose[gl_MaxTextureCoords];"));

		//
		// Clip planes p. 42.
		//
		s.append(TString("uniform vec4  gl_ClipPlane[gl_MaxClipPlanes];"));

		//
		// Light State p 50, 53, 55.
		//
		s.append(
				TString(
						"uniform gl_LightSourceParameters  gl_LightSource[gl_MaxLights];"));

		//
		// Derived state from products of light.
		//
		s.append(
				TString(
						"uniform gl_LightProducts gl_FrontLightProduct[gl_MaxLights];"));
		s.append(
				TString(
						"uniform gl_LightProducts gl_BackLightProduct[gl_MaxLights];"));

		//
		// Textureg Environment and Generation, p. 152, p. 40-42.
		//
		s.append(
				TString(
						"uniform vec4  gl_TextureEnvColor[gl_MaxTextureImageUnits];"));
		s.append(TString("uniform vec4  gl_EyePlaneS[gl_MaxTextureCoords];"));
		s.append(TString("uniform vec4  gl_EyePlaneT[gl_MaxTextureCoords];"));
		s.append(TString("uniform vec4  gl_EyePlaneR[gl_MaxTextureCoords];"));
		s.append(TString("uniform vec4  gl_EyePlaneQ[gl_MaxTextureCoords];"));
		s.append(
				TString("uniform vec4  gl_ObjectPlaneS[gl_MaxTextureCoords];"));
		s.append(
				TString("uniform vec4  gl_ObjectPlaneT[gl_MaxTextureCoords];"));
		s.append(
				TString("uniform vec4  gl_ObjectPlaneR[gl_MaxTextureCoords];"));
		s.append(
				TString("uniform vec4  gl_ObjectPlaneQ[gl_MaxTextureCoords];"));

		s.append(TString("\n"));
	}
	{
		//============================================================================
		//
		// Geometry Uniforms
		//
		//============================================================================

		TString& s = GeometryUniforms;

		char builtInConstant[80];
		sprintf(builtInConstant, "const int  gl_VerticesIn = %d;",
				resources.geoVerticesIn);
		s.append(TString(builtInConstant));

		s.append(TString("\n"));
	}

	builtInStrings[EShLangFragment].push_back(StandardUniforms);
	builtInStrings[EShLangGeometry].push_back(StandardUniforms);
	builtInStrings[EShLangGeometry].push_back(GeometryUniforms);
	builtInStrings[EShLangVertex].push_back(StandardUniforms);
}

void IdentifyBuiltIns(EShLanguage language, TSymbolTable& symbolTable)
{
	//
	// First, insert some special built-in variables that are not in
	// the built-in header files.
	//
	switch (language) {

	case EShLangFragment: {
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_FrontFacing"),
						TType(EbtBool, EvqFace, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_FragCoord"),
						TType(EbtFloat, EvqFragCoord, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_FragColor"),
						TType(EbtFloat, EvqFragColor, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_FragDepth"),
						TType(EbtFloat, EvqFragDepth, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_PrimitiveID"),
						TType(EbtInt, EvqPrimitiveID, EvmNone, 1)));
	}
		break;

	case EShLangGeometry:
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_PrimitiveIDIn"),
						TType(EbtInt, EvqPrimitiveIDIn, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_Position"),
						TType(EbtFloat, EvqPosition, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_PointSize"),
						TType(EbtFloat, EvqPointSize, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_ClipVertex"),
						TType(EbtFloat, EvqClipVertex, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_PrimitiveID"),
						TType(EbtInt, EvqPrimitiveID, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_Layer"),
						TType(EbtInt, EvqLayer, EvmNone, 1)));
		break;

	case EShLangVertex:
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_Position"),
						TType(EbtFloat, EvqPosition, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_PointSize"),
						TType(EbtFloat, EvqPointSize, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_ClipVertex"),
						TType(EbtFloat, EvqClipVertex, EvmNone, 4)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_VertexID"),
						TType(EbtInt, EvqVertexID, EvmNone, 1)));
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_InstanceID"),
						TType(EbtInt, EvqInstanceID, EvmNone, 1)));
		break;
	default:
		break;
	}

	//
	// Next, identify which built-ins from the already loaded headers have
	// a mapping to an operator.  Those that are not identified as such are
	// expected to be resolved through a library of functions, versus as
	// operations.
	//
	symbolTable.relateToOperator("not", EOpVectorLogicalNot);

	symbolTable.relateToOperator("matrixCompMult", EOpMul);
	symbolTable.relateToOperator("outerProduct", EOpMatrixOuterProduct);
	symbolTable.relateToOperator("transpose", EOpMatrixTranspose);
	symbolTable.relateToOperator("mod", EOpMod);

	symbolTable.relateToOperator("equal", EOpVectorEqual);
	symbolTable.relateToOperator("notEqual", EOpVectorNotEqual);
	symbolTable.relateToOperator("lessThan", EOpLessThan);
	symbolTable.relateToOperator("greaterThan", EOpGreaterThan);
	symbolTable.relateToOperator("lessThanEqual", EOpLessThanEqual);
	symbolTable.relateToOperator("greaterThanEqual", EOpGreaterThanEqual);

	symbolTable.relateToOperator("radians", EOpRadians);
	symbolTable.relateToOperator("degrees", EOpDegrees);
	symbolTable.relateToOperator("sin", EOpSin);
	symbolTable.relateToOperator("cos", EOpCos);
	symbolTable.relateToOperator("tan", EOpTan);
	symbolTable.relateToOperator("asin", EOpAsin);
	symbolTable.relateToOperator("acos", EOpAcos);
	symbolTable.relateToOperator("atan", EOpAtan);

	symbolTable.relateToOperator("pow", EOpPow);
	symbolTable.relateToOperator("exp2", EOpExp2);
	symbolTable.relateToOperator("log", EOpLog);
	symbolTable.relateToOperator("exp", EOpExp);
	symbolTable.relateToOperator("log2", EOpLog2);
	symbolTable.relateToOperator("sqrt", EOpSqrt);
	symbolTable.relateToOperator("inversesqrt", EOpInverseSqrt);

	symbolTable.relateToOperator("abs", EOpAbs);
	symbolTable.relateToOperator("sign", EOpSign);
	symbolTable.relateToOperator("floor", EOpFloor);
	symbolTable.relateToOperator("ceil", EOpCeil);
	symbolTable.relateToOperator("fract", EOpFract);
	symbolTable.relateToOperator("min", EOpMin);
	symbolTable.relateToOperator("max", EOpMax);
	symbolTable.relateToOperator("clamp", EOpClamp);
	symbolTable.relateToOperator("mix", EOpMix);
	symbolTable.relateToOperator("step", EOpStep);
	symbolTable.relateToOperator("smoothstep", EOpSmoothStep);
	symbolTable.relateToOperator("truncate", EOpTruncate);
	symbolTable.relateToOperator("round", EOpRound);

	symbolTable.relateToOperator("length", EOpLength);
	symbolTable.relateToOperator("distance", EOpDistance);
	symbolTable.relateToOperator("dot", EOpDot);
	symbolTable.relateToOperator("cross", EOpCross);
	symbolTable.relateToOperator("normalize", EOpNormalize);
	symbolTable.relateToOperator("forward", EOpFaceForward);
	symbolTable.relateToOperator("reflect", EOpReflect);
	symbolTable.relateToOperator("refract", EOpRefract);

	symbolTable.relateToOperator("any", EOpAny);
	symbolTable.relateToOperator("all", EOpAll);

	switch (language) {

	case EShLangVertex:
		break;
	case EShLangGeometry:
		break;

	case EShLangFragment:
		symbolTable.relateToOperator("dFdx", EOpDPdx);
		symbolTable.relateToOperator("dFdy", EOpDPdy);
		symbolTable.relateToOperator("fwidth", EOpFwidth);

		break;

	case EShLangPack:
	case EShLangUnpack:
		symbolTable.relateToOperator("itof", EOpItof);
		symbolTable.relateToOperator("ftoi", EOpFtoi);
		symbolTable.relateToOperator("skipPixels", EOpSkipPixels);
		symbolTable.relateToOperator("readInput", EOpReadInput);
		symbolTable.relateToOperator("writePixel", EOpWritePixel);
		symbolTable.relateToOperator("bitmapLSB", EOpBitmapLsb);
		symbolTable.relateToOperator("bitmapMSB", EOpBitmapMsb);
		symbolTable.relateToOperator("writeOutput", EOpWriteOutput);
		symbolTable.relateToOperator("readPixel", EOpReadPixel);
		break;
	default:
		assert(false && "Language not supported");
		break;
	}
}

void IdentifyBuiltIns(EShLanguage language, TSymbolTable& symbolTable,
		const TBuiltInResource &resources)
{
	//
	// First, insert some special built-in variables that are not in
	// the built-in header files.
	//
	switch (language) {

	case EShLangFragment: {
		// Set up gl_FragData.  The array size.
		TType fragData(EbtFloat, EvqFragData, EvmNone, 4, false, true);
		fragData.setArraySize(resources.maxDrawBuffers);
		symbolTable.insert(
				*new TVariable(NewPoolTString("gl_FragData"), fragData));
	}
		break;

	default:
		break;
	}
}

const char* GetPreprocessorBuiltinString()
{
	static const char *PreprocessorBuiltinString =
			"#define GL_ARB_draw_buffers 1\n"
					"#define GL_ARB_texture_rectangle 1\n"
					"#define GL_EXT_bindable_uniform 1\n"
					"#define GL_EXT_geometry_shader4 1\n"
					"#define GL_EXT_gpu_shader4 1\n"
					"#define GL_EXT_texture_array 1\n";

	return PreprocessorBuiltinString;
}
