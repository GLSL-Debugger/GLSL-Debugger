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

#include "mappings.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

int getIntFromMapping(Mapping m)
{
	int r = 0;

	r = m.index << 2;
	r += m.type;

	return r;
}

Mapping getMappingFromInt(int i)
{
	Mapping m;

	m.type = (MapType) (i & 0x3);
	m.index = i >> 2;

	return m;
}

int getIntFromRangeMapping(RangeMapping m)
{
	int r = 0;

	r = m.index << 3;
	r += m.range;

	return r;
}

RangeMapping getRangeMappingFromInt(int i)
{
	RangeMapping m;

	m.range = (RangeMap) (i & 0x7);
	m.index = i >> 3;

	return m;
}

static float mapValueF(float v, float min, float max)
{
	return CLAMP((v - min)/(max - min), 0.0f, 1.0f);
}

static int mapValueI(float v, float min, float max)
{
	return CLAMP((int)((v - min)/(max - min)*255), 0, 255);
}

float getMappedValueF(float v, Mapping *mapping, RangeMapping *rangeMapping,
		float minmax[2])
{
	UNUSED_ARG(mapping)
	float value = 0.0f;
	switch (rangeMapping->range) {
	case RANGE_MAP_DEFAULT:
		value = mapValueF(v, minmax[0], minmax[1]);
		break;
	case RANGE_MAP_POSITIVE:
		if (v < 0.0f) {
			value = 0.0f;
		} else {
			value = mapValueF(v, minmax[0], minmax[1]);
		}
		break;
	case RANGE_MAP_NEGATIVE:
		if (v > 0.0f) {
			value = 0.0f;
		} else {
			value = mapValueF(v, minmax[0], minmax[1]);
		}
		break;
	case RANGE_MAP_ABSOLUTE:
		value = mapValueF(fabs(v), minmax[0], minmax[1]);
		break;
	}
	return value;
}

int getMappedValueI(float v, Mapping *mapping, RangeMapping *rangeMapping,
		float minmax[2])
{
	UNUSED_ARG(mapping)
	int value = 0;
	switch (rangeMapping->range) {
	case RANGE_MAP_DEFAULT:
		value = mapValueI(v, minmax[0], minmax[1]);
		break;
	case RANGE_MAP_POSITIVE:
		if (v < 0.0f) {
			value = 0;
		} else {
			value = mapValueI(v, minmax[0], minmax[1]);
		}
		break;
	case RANGE_MAP_NEGATIVE:
		if (v > 0.0f) {
			value = 0;
		} else {
			value = mapValueI(v, minmax[0], minmax[1]);
		}
		break;
	case RANGE_MAP_ABSOLUTE:
		value = mapValueI(fabs(v), minmax[0], minmax[1]);
		break;
	}
	return value;
}

