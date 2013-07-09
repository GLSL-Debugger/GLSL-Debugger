/*
Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/
/****************************************************************************\
Copyright (c) 2002, NVIDIA Corporation.

NVIDIA Corporation("NVIDIA") supplies this software to you in
consideration of your agreement to the following terms, and your use,
installation, modification or redistribution of this NVIDIA software
constitutes acceptance of these terms.  If you do not agree with these
terms, please do not use, install, modify or redistribute this NVIDIA
software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, NVIDIA grants you a personal, non-exclusive
license, under NVIDIA's copyrights in this original NVIDIA software (the
"NVIDIA Software"), to use, reproduce, modify and redistribute the
NVIDIA Software, with or without modifications, in source and/or binary
forms; provided that if you redistribute the NVIDIA Software, you must
retain the copyright notice of NVIDIA, this notice and the following
text and disclaimers in all such redistributions of the NVIDIA Software.
Neither the name, trademarks, service marks nor logos of NVIDIA
Corporation may be used to endorse or promote products derived from the
NVIDIA Software without specific prior written permission from NVIDIA.
Except as expressly stated in this notice, no other rights or licenses
express or implied, are granted by NVIDIA herein, including but not
limited to any patent rights that may be infringed by your derivative
works or by other works in which the NVIDIA Software may be
incorporated. No hardware is licensed hereunder.

THE NVIDIA SOFTWARE IS BEING PROVIDED ON AN "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED,
INCLUDING WITHOUT LIMITATION, WARRANTIES OR CONDITIONS OF TITLE,
NON-INFRINGEMENT, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR
ITS USE AND OPERATION EITHER ALONE OR IN COMBINATION WITH OTHER
PRODUCTS.

IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT,
INCIDENTAL, EXEMPLARY, CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, LOST PROFITS; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) OR ARISING IN ANY WAY
OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE
NVIDIA SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT,
TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined (_WIN32)
#include <windows.h>
 typedef UINT_PTR uintptr_t;
#else
 #include <stdint.h>
 /*typedef unsigned int uintptr_t; */
#endif

#include "memory.h"

/* default alignment and chunksize, if called with 0 arguments */
#define CHUNKSIZE       (64*1024)
#define ALIGN           8

/* we need to call the `real' malloc and free, not our replacements */
#undef malloc
#undef free

struct chunk {
    struct chunk        *next;
};

struct cleanup {
    struct cleanup      *next;
    void                (*fn)(void *);
    void                *arg;
};

struct MemoryPool_rec {
    struct chunk        *next;
    uintptr_t           free, end;
    size_t              chunksize;
    uintptr_t           alignmask;
    struct cleanup      *cleanup;
};

MemoryPool *mem_CreatePool(size_t chunksize, unsigned int align)
{
    MemoryPool  *pool;

    if (align == 0) align = ALIGN;
    if (chunksize == 0) chunksize = CHUNKSIZE;
    if (align & (align-1)) return 0;
    if (chunksize < sizeof(MemoryPool)) return 0;
    if (chunksize & (align-1)) return 0;
    if (!(pool = (MemoryPool *)malloc(chunksize))) return 0;
    pool->next = 0;
    pool->chunksize = chunksize;
    pool->alignmask = (uintptr_t)(align)-1;
    pool->free = ((uintptr_t)(pool + 1U) + pool->alignmask) & ~pool->alignmask;
    pool->end = (uintptr_t)pool + chunksize;
    pool->cleanup = 0;
    return pool;
}

void mem_FreePool(MemoryPool *pool)
{
    struct cleanup      *cleanup;
    struct chunk        *p, *next;

    for (cleanup = pool->cleanup; cleanup; cleanup = cleanup->next) {
        cleanup->fn(cleanup->arg);
    }
    for (p = (struct chunk *)pool; p; p = next) {
        next = p->next;
        free(p);
    }
}

void *mem_Alloc(MemoryPool *pool, size_t size)
{
    struct chunk *ch;
    void *rv = (void *)pool->free;
    size = (size + pool->alignmask) & ~pool->alignmask;
    if (size <= 0) size = pool->alignmask;
    pool->free += size;
    if (pool->free > pool->end || pool->free < (uintptr_t)rv) {
        size_t minreq = (size + sizeof(struct chunk) + pool->alignmask)
                      & ~pool->alignmask;
        pool->free = (uintptr_t)rv;
        if (minreq >= pool->chunksize) {
            /* request size is too big for the chunksize, so allocate it as
               a single chunk of the right size */
            ch = (struct chunk *)malloc(minreq);
            if (!ch) return 0;
        } else {
            ch = (struct chunk *)malloc(pool->chunksize);
            if (!ch) return 0;
            pool->free = (uintptr_t)ch + minreq;
            pool->end = (uintptr_t)ch + pool->chunksize;
        }
        ch->next = pool->next;
        pool->next = ch;
        rv = (void *)(((uintptr_t)(ch+1) + pool->alignmask) & ~pool->alignmask);
    }
    return rv;
}

int mem_AddCleanup(MemoryPool *pool, void (*fn)(void *), void *arg) {
    struct cleanup *cleanup;

    pool->free = (pool->free + sizeof(void *) - 1) & ~(sizeof(void *)-1);
    cleanup = (struct cleanup *)mem_Alloc(pool, sizeof(struct cleanup));
    if (!cleanup) return -1;
    cleanup->next = pool->cleanup;
    cleanup->fn = fn;
    cleanup->arg = arg;
    pool->cleanup = cleanup;
    return 0;
}
