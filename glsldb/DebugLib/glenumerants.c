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
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */


#if (defined(GLSLDB_LINUX) || defined(GLSLDB_OSX))
#  include "../GL/glx.h"
#  include "../GL/glxext.h"

//#  ifdef GLX_VERSION_1_4
//#    define GLX_SAMPLE_BUFFERS                 100000
//#    define GLX_SAMPLES                        100001
//#  endif
#include "glenumerants.h"
#include "generated/glenumerants.h"
#include "generated/glxenumerants.h"
#endif

#ifdef GLSLDB_WIN32
#  include <windows.h>
#  include <GL/wglext.h>
#  include "generated/wglenumerants.h"
#endif

static void concatenate(char **dst, const char *src)
{
    if (!src || strlen(src) == 0)
    {
        return;
    }

    if (*dst) {
        *dst = realloc(*dst, strlen(*dst) + strlen(src) + 1);
        if (!dst) {
            fprintf(stderr, "concatenate'ing strings failed\n");
            exit(1);
        }
        strcat(*dst, src);
    } else {
#ifdef _WIN32
        /* strdup is not ANSI but Microsoft-specific, _strdup is ISO C++. */
        if (!(*dst = _strdup(src))) {
#else /* _WIN32 */
        if (!(*dst = strdup(src))) {
#endif /* _WIN32 */
            fprintf(stderr, "concatenate'ing strings failed\n");
            exit(1);
        }
    }
}

const char *lookupEnum(GLenum e)
{
    int i = 0;
    while (glEnumerantsMap[i].string != NULL) {
        if (glEnumerantsMap[i].value == e) {
            /* assumes enums are unique! */
            return glEnumerantsMap[i].string;
        }
        i++;
    }
    return "UNKNOWN ENUM!";
}

char *lookupAllEnum(GLenum e)
{
    int i;
    char *result = NULL;

    i = 0;
    concatenate(&result, "{");
    while (glEnumerantsMap[i].string != NULL) {
        if (glEnumerantsMap[i].value == e) {
            /* assumes enums are unique! */
            const char *s = glEnumerantsMap[i].string;
            concatenate(&result, s);
            concatenate(&result, ",");
        }
        i++;
    }
#ifndef _WIN32
    i = 0;
    while (glxEnumerantsMap[i].string != NULL) {
        if (glxEnumerantsMap[i].value == e) {
            const char *s = glxEnumerantsMap[i].string;
#else
    while (wglEnumerantsMap[i].string != NULL) {
        if (wglEnumerantsMap[i].value == e) {
            const char *s = wglEnumerantsMap[i].string;
#endif
            /* assumes enums are unique! */
            concatenate(&result, s);
            concatenate(&result, ",");
        }
        i++;
    }
    if (strlen(result) == 1) {
        result = realloc(result, 14*sizeof(char));
        strcpy(result, "UNKNOWN ENUM!");
    } else {
        result[strlen(result)-1] = '}';
    }
    return result;
}

char *dissectBitfield(GLbitfield b)
{
    char *result = NULL;
    int i;

    /* find combinations */
    i = 0;
    while (glBitfieldMap[i].string != NULL) {
        if ((glBitfieldMap[i].value & b) == glBitfieldMap[i].value) {
            if (result != NULL) {
                concatenate(&result, "|");
            }
            concatenate(&result, glBitfieldMap[i].string);
        }
        i++;
    }
    return result;
}



#ifdef _WIN32

const char *lookupWGLEnum(int e)
{
    int i;

    i = 0;
    while (wglEnumerantsMap[i].string != NULL) {
        if (wglEnumerantsMap[i].value == e) {
            /* assumes enums are unique! */
            return wglEnumerantsMap[i].string;
        }
        i++;
    }
    return "UNKNOWN ENUM!";
}

#else

const char *lookupGLXEnum(int e)
{
    int i;

    i = 0;
    while (glxEnumerantsMap[i].string != NULL) {
        if (glxEnumerantsMap[i].value == e) {
            /* assumes enums are unique! */
            return glxEnumerantsMap[i].string;
        }
        i++;
    }
    return "UNKNOWN ENUM!";
}

#endif
