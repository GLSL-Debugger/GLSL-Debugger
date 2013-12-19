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

#ifndef _HASH_H
#define _HASH_H

#include <string.h>

#include "common.h"

typedef int (*HashFunc)(const void *key, int numBuckets);
typedef int (*CompFunc)(const void *key1, const void *key2);

typedef struct tHashNode {
	struct tHashNode *next;
	const void *key;
	void *data;
} HashNode;

typedef struct {
	int numBuckets;
	HashNode **table;
	HashFunc hashFunc;
	CompFunc compFunc;
	int freeDataPointers;
} Hash;

/* initialize hash */
UTILSLOCAL void hash_create(Hash *hash, HashFunc hashFunc,
		CompFunc compFunc, int numBuckets, int freeDataPointers);

/* free memory associated with this hash */
UTILSLOCAL void hash_free(Hash *hash);

/* insert (key, data) pair; if the key already exits data is replaced. In the
 * latter case 1 is returned, 0 else.
 */
UTILSLOCAL int hash_insert(Hash *hash, const void *key, void *data);

/* remove data item associated with key */
UTILSLOCAL void hash_remove(Hash *hash, void *key);

/* return data associated with key */
UTILSLOCAL void *hash_find(Hash *hash, const void *key);

/* return number of elements in hash */
UTILSLOCAL int hash_count(Hash *hash);

/* return the n-th elemnt in the hash */
UTILSLOCAL void *hash_element(Hash *hash, int n);

/* some common hash and comparison functions */
UTILSLOCAL int hashInt(const void *key, int numBuckets);
UTILSLOCAL int compInt(const void *key1, const void *key2);

UTILSLOCAL int hashString(const void *key, int numBuckets);
UTILSLOCAL int compString(const void *key1, const void *key2);

#endif
