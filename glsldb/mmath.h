/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef MMATH_H
#define MMATH_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SQR
#  define SQR(x) ((x)*(x))
#endif

#ifndef M_PI
#  define M_PI 3.141592765
#endif

#define EPS 1e-5f

typedef struct {
	float x, y, z;
} Vector3;

typedef struct {
	float x, y, z, w;
} Quaternion;

Vector3 Vector3_new(float x, float y, float z);

Vector3 Vector3_add(Vector3 u, Vector3 v);

Vector3 Vector3_sub(Vector3 u, Vector3 v);

Vector3 Vector3_cross(Vector3 u, Vector3 v);

float Vector3_dot(Vector3 u, Vector3 v);

Vector3 Vector3_mult(Vector3 u, Vector3 v);

Vector3 Vector3_smult(float s, Vector3 v);

float Vector3_normalize(Vector3 *v);

Vector3 Vector3_neg(Vector3 v);

void Vector3_stderr(char *s, Vector3 v);

Quaternion Quaternion_new(float w, float x, float y, float z);

Quaternion Quaternion_fromAngleAxis(float angle, Vector3 axis);

Quaternion Quaternion_mult(Quaternion p, Quaternion q);

void Quaternion_getAngleAxis(const Quaternion q, float *angle, Vector3 *axis);

void Quaternion_normalize(Quaternion *q);

Quaternion Quaternion_inverse(Quaternion q);

Vector3 Quaternion_multVector3(Quaternion q, Vector3 v);

void Quaternion_stderr(char *s, Quaternion q);

void mul4x4(float *dst, float *src1, float *src2);
void mul4x4_v3(Vector3 *dst, float *m, Vector3 v);
void mul4x4d_v3(Vector3 *dst, double *m, Vector3 v);
void mul4x4d_d3(double *dst, double *m, double *v);

#ifdef __cplusplus
}
#endif


#endif
