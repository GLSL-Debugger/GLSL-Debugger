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

#include <stdlib.h>

#include "hash.h"

void hash_create(Hash *hash, HashFunc hashFunc, CompFunc compFunc,
                 int numBuckets, int freeDataPointers)
{
	int i;
	hash->numBuckets = numBuckets;
	hash->hashFunc = hashFunc;
	hash->compFunc = compFunc;
	hash->freeDataPointers = freeDataPointers;
	/* TODO: mem check */
	hash->table = (HashNode**)malloc(numBuckets*sizeof(HashNode*));
	for (i = 0; i < numBuckets; i++) {
		hash->table[i] = NULL;
	}
}

void hash_free(Hash *hash)
{
	int i;
	
	for (i = 0; i < hash->numBuckets; i++) {
		if (hash->table[i]) {
			HashNode *node = hash->table[i];
			while (node) {
				HashNode *next = node->next;
				if (hash->freeDataPointers && node->data) {
					free(node->data);
				}
				free(node);
				node = next;
			}
		}
	}
	free(hash->table);
	hash->table = NULL;
	hash->numBuckets = 0;
}

int hash_insert(Hash *hash, void *key, void *data)
{
	int n = hash->hashFunc(key, hash->numBuckets);
	HashNode *node = hash->table[n];
	HashNode *prev = NULL;
	while (node) {
		if (hash->compFunc(node->key, key)) {
			node->data = data;
			return 1;
		}
		prev = node;
		node = node->next;
	}
	if (prev == NULL) {
		/* TODO: mem check */
		hash->table[n] = (HashNode*)malloc(sizeof(HashNode));
		node = hash->table[n];
	} else {
		/* TODO: mem check */
		prev->next = (HashNode*)malloc(sizeof(HashNode));
		node = prev->next;
	}
	node->data = data;
	node->key = key;
	node->next = NULL;
	return 0;
}

void hash_remove(Hash *hash, void *key)
{
	int n = hash->hashFunc(key, hash->numBuckets);
	HashNode *node = hash->table[n];
	HashNode *prev = node;
	while (node) {
		if (hash->compFunc(node->key, key)) {
			if (prev == node) {
				hash->table[n] = node->next;
			} else {
				prev->next = node->next;
			}
			if (hash->freeDataPointers && node->data) {
				free(node->data);
			}
			free(node);
			return;
		}
		prev = node;
		node = node->next;
	}
}

void *hash_find(Hash *hash, void *key)
{
	HashNode *node = hash->table[hash->hashFunc(key, hash->numBuckets)];
	while (node) {
		if (hash->compFunc(node->key, key)) {
			return node->data;
		}
		node = node->next;
	}
	return NULL;
}

int hash_count(Hash *hash)
{
	int i, count = 0;

	for (i = 0; i < hash->numBuckets; i++) {
		if (hash->table[i]) {
			HashNode *node = hash->table[i];
			while (node) {
				node = node->next;
				count++;
			}
		}
	}
	return count;
}

void *hash_element(Hash *hash, int n)
{
	int i, count = 0;

	for (i = 0; i < hash->numBuckets; i++) {
		if (hash->table[i]) {
			HashNode *node = hash->table[i];
			while (node) {
				if (count == n) {
					return node->data;
				}
				node = node->next;
				count++;
			}
		}
	}
	return NULL;
}

/* some common hash and comparison functions */

int hashInt(void *key, int numBuckets)
{
	return *(int*)key % numBuckets;
}

int compInt(void *key1, void *key2)
{
	return *(int*)key1 == *(int*)key2;
}

int hashString(void *key, int numBuckets)
{
	char *s = (char *)key;
	/* universal hash function, R. Sedgewick, Algorithms in C++, p. 593 */
	int h, a = 31415, b = 27183;
	for (h = 0; *s != 0; s++, a = a*b % (numBuckets - 1)) {
		h = (a*h + *s) % numBuckets;
	}
	return (h < 0) ? (h + numBuckets) : h;
}

int compString(void *key1, void *key2)
{
	return !strcmp((const char*)key1, (const char*)key2);
}


