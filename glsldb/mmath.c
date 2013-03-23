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

#include <math.h>
#include <stdio.h>
#include "mmath.h"

Vector3 Vector3_new(float x, float y, float z)
{
	Vector3 result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	
	return result;
}

Vector3 Vector3_add(Vector3 u, Vector3 v)
{
	Vector3 result;

	result.x = u.x + v.x;
	result.y = u.y + v.y;
	result.z = u.z + v.z;

	return result;
}	

Vector3 Vector3_sub(Vector3 u, Vector3 v)
{
	Vector3 result;
	
	result.x = u.x - v.x;
	result.y = u.y - v.y;
	result.z = u.z - v.z;

	return result;
}	

Vector3 Vector3_smult(float s, Vector3 v)
{
	Vector3 result;
	
	result.x = s * v.x;
	result.y = s * v.y;
	result.z = s * v.z;
	
	return result;	
}

Vector3 Vector3_mult(Vector3 u, Vector3 v)
{
	Vector3 result;
	
	result.x = u.x * v.x;
	result.y = u.y * v.y;
	result.z = u.z * v.z;
	
	return result;	
}

Vector3 Vector3_cross(Vector3 u, Vector3 v) 
{
	Vector3 result;

	result.x = u.y * v.z - u.z * v.y;
	result.y = u.z * v.x - u.x * v.z;
	result.z = u.x * v.y - u.y * v.x;

	return result;
}

float Vector3_dot(Vector3 u, Vector3 v) 
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

float Vector3_normalize(Vector3 *v)
{
	float d = (float)sqrt(SQR(v->x) + SQR(v->y) + SQR(v->z));

	if (d > EPS) {
		v->x /= d;
		v->y /= d;
		v->z /= d;
		return d;
	} else {
		v->x = v->y = v->z = 0.f;
		return 0.f;
	}
}

void Vector3_stderr(char *s, Vector3 v)
{
	fprintf(stderr, "%s <%f, %f, %f>\n", s, v.x, v.y, v.z);
}

Quaternion Quaternion_new(float w, float x, float y, float z)
{
	Quaternion result;

	result.x = w;
	result.y = x;
	result.z = y;
	result.w = z;
	
	return result;
}

Quaternion Quaternion_fromAngleAxis(float angle, Vector3 axis)
{
	Quaternion q;
	float l = Vector3_normalize(&axis);

	if(l > EPS){
		l = (float)sin(0.5f * angle);
		q.x = axis.x * l;
		q.y = axis.y * l;
		q.z = axis.z * l;
		q.w = (float)cos(0.5f * angle);
	} else {
		q.x = 0.f;
		q.y = 0.f;
		q.z = 0.f;
		q.w = 1.f;
	}

	return q;
} 

Quaternion Quaternion_mult(Quaternion p, Quaternion q)
{
	Quaternion result;

	result.w = p.w * q.w - (p.x * q.x + p.y * q.y + p.z * q.z);
	result.x = p.w * q.x + q.w * p.x + p.y * q.z - p.z * q.y;
	result.y = p.w * q.y + q.w * p.y + p.z * q.x - p.x * q.z;
	result.z = p.w * q.z + q.w * p.z + p.x * q.y - p.y * q.x;

	return result;
}

void Quaternion_getAngleAxis(const Quaternion q, float *angle, Vector3 *axis)
{
	float d = (float)sqrt(SQR(q.x) + SQR(q.y) + SQR(q.z));
 
	if(d > EPS){
		d = 1.f / d;
		axis->x = q.x * d;
		axis->y = q.y * d;
		axis->z = q.z * d;
		*angle   = 2.f * (float)acos(q.w);
	} else {
		axis->x = 0.f;
		axis->y = 0.f;
		axis->z = 1.f;
		*angle  = 0.f;
	}     
} 

void Quaternion_normalize(Quaternion *q)
{
	float d = (float)sqrt(SQR(q->w) + SQR(q->x) + SQR(q->y) + SQR(q->z));
	if (d > EPS) {
		d = 1.f / d;
		q->w *= d;
		q->x *= d;
		q->y *= d;
		q->z *= d;
	} else {
		q->w = 1.f;
		q->x = q->y = q->z = 0.f;
	}
}

Vector3 Vector3_neg(Vector3 v)
{
    return Vector3_new(-v.x, -v.y, -v.z);
}

Quaternion Quaternion_inverse(Quaternion q)
{
	Quaternion result;
	float d = SQR(q.w) + SQR(q.x) + SQR(q.y) + SQR(q.z);
	if (d > EPS) {
		d = 1.f / (float)sqrt(d);
		result.w = q.w * d;
		result.x = -q.x * d;
		result.y = -q.y * d;
		result.z = -q.z * d;
	} else {
		result.w = 1.f;
		result.x = result.y = result.z = 0.f;
	}
	return result;
}

Vector3 Quaternion_multVector3(Quaternion q, Vector3 v)
{
	Vector3 result, u;
	float uu, uv;

	u.x = q.x;
	u.y = q.y;
	u.z = q.z;	

	uu = Vector3_dot(u, u);
	uv = Vector3_dot(u,v);

	result = Vector3_smult(2.f, Vector3_add(Vector3_smult(uv, u), 
						   Vector3_smult(q.w, Vector3_cross(u, v))));
	result = Vector3_add(result, Vector3_smult(SQR(q.w) - uu, v));

	return result;
}


void Quaternion_stderr(char *s, Quaternion q)
{
	fprintf(stderr, "%s (%f <%f, %f, %f>)\n", s, q.w, q.x, q.y, q.z);
}

void mul4x4(float *dst, float *src1, float *src2) 
{
   int i, j, k;
   for(i = 0; i < 4; i++) {
       for (j = 0; j < 4; j++) {
           dst[4*i + j] = 0.0;
		   for (k = 0; k < 4; k++) {
               dst[4*i +j] += src1[4*i + k] * src2[4*k + j];
		   }
       }
   }
}

void mul4x4_v3(Vector3 *dst, float *m, Vector3 v) 
{
	float w;
	dst->x = m[0]*v.x + m[4]*v.y + m[8]*v.z + m[12];
	dst->y = m[1]*v.x + m[5]*v.y + m[9]*v.z + m[13];
	dst->z = m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14];
	w = m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15];
	dst->x /= w;
	dst->y /= w;
	dst->z /= w;
}

void mul4x4d_v3(Vector3 *dst, double *m, Vector3 v) 
{
	double w;
	dst->x = (float)(m[0]*v.x + m[4]*v.y + m[8]*v.z + m[12]);
	dst->y = (float)(m[1]*v.x + m[5]*v.y + m[9]*v.z + m[13]);
	dst->z = (float)(m[2]*v.x + m[6]*v.y + m[10]*v.z + m[14]);
	w = m[3]*v.x + m[7]*v.y + m[11]*v.z + m[15];
	dst->x /= (float)w;
	dst->y /= (float)w;
	dst->z /= (float)w;
}

void mul4x4d_d3(double *dst, double *m, double *v) 
{
	double w;
	dst[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12];
	dst[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13];
	dst[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14];
	w = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15];
	dst[0] /= w;
	dst[1] /= w;
	dst[2] /= w;
}

