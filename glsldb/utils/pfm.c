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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_WIN32)
#   include <inttypes.h>
#endif
#include <math.h>

#include "pfm.h"

#define MAGIC_NUMBER_1_COMPOMENTS "Pf"
#define MAGIC_NUMBER_3_COMPOMENTS "PF"

static int readToken(FILE *fp, char *token)
{
	int comment = 0;
	char c;

	do {
		if (fread(&c, 1, 1, fp) != 1) {
			perror("reading token failed");
			exit(1);
		}
		if (c == '\n') {
			comment = 0;
		} else if (c == '#') {
			comment = 1;
		}
	} while (isspace(c) || comment);

	*token++ = c;

	do {
		if (fread(&c, 1, 1, fp) != 1) {
			perror("reading token failed");
			return -1;
		}
		*token++ = c;
	} while (!isspace(c));

	*--token = '\0';
	return 0;
}

static int isLittleEndian(void)
{
#if defined(_WIN32)
	unsigned __int16 word = 0x0001;
	unsigned __int8 *byte = (unsigned __int8 *)&word;
#else
	uint16_t word = 0x0001;
	uint8_t *byte = (uint8_t *) &word;
#endif
	return *byte > 0;
}

static void swapByteOrder32(void *data, unsigned long size)
{
	unsigned long i;
#if defined(_WIN32)
	unsigned __int32 v, sv;
	unsigned __int32 *idata = (unsigned __int32 *)data;
#else
	uint32_t v, sv;
	uint32_t *idata = (uint32_t *) data;
#endif
	for (i = 0; i < size; i++) {
		v = idata[i];
		sv = (v & 0x000000FF);
		sv = ((v & 0x0000FF00) >> 0x08) | (sv << 0x08);
		sv = ((v & 0x00FF0000) >> 0x10) | (sv << 0x08);
		sv = ((v & 0xFF000000) >> 0x18) | (sv << 0x08);
		idata[i] = sv;
	}
}

int pfmRead(const char *filename, PFMFile *pfmFile)
{
	char token[100], msg[100];
	int dataSize;
	FILE *fp;

	if (!(fp = fopen(filename, "rb"))) {
		sprintf(msg, "opening image file \"%s\" failed", filename);
		perror(msg);
		return -1;
	}

	/* Read magic number */
	if (readToken(fp, token)) {
		return -1;
	}
	if (!strcmp(MAGIC_NUMBER_1_COMPOMENTS, token)) {
		pfmFile->components = 1;
	} else if (!strcmp(MAGIC_NUMBER_3_COMPOMENTS, token)) {
		pfmFile->components = 3;
	} else {
		fprintf(stderr, "invalid magic number: %s\n", token);
		return -1;
	}

	/* Read dimensions */
	if (readToken(fp, token)) {
		return -1;
	}
	if (sscanf(token, "%i", &pfmFile->width) != 1) {
		fprintf(stderr, "invalid width: %s\n", token);
		return -1;
	}
	if (readToken(fp, token)) {
		return -1;
	}
	if (sscanf(token, "%i", &pfmFile->height) != 1) {
		fprintf(stderr, "invalid height: %s\n", token);
		return -1;
	}

	/* Allocate memory for image data */
	dataSize = pfmFile->width * pfmFile->height * pfmFile->components;
	if (!(pfmFile->data = (float *) malloc(dataSize * sizeof(float)))) {
		fprintf(stderr, "not enough memory for image data\n");
		return -1;
	}

	/* Read scale value */
	if (readToken(fp, token)) {
		return -1;
	}
	if (sscanf(token, "%f", &pfmFile->scale) != 1) {
		fprintf(stderr, "invalid scale value: %s\n", token);
		free(pfmFile->data);
		pfmFile->data = NULL;
		return -1;
	}

	/* Read image data */
	if (fread(pfmFile->data, sizeof(float), dataSize, fp) != dataSize) {
		perror("reading image data failed");
		free(pfmFile->data);
		pfmFile->data = NULL;
		return -1;
	}

	/* does not work for scale == 0.0; who cares? */
	if ((pfmFile->scale < 0.0 && !isLittleEndian())
			|| (pfmFile->scale > 0.0 && isLittleEndian())) {
		swapByteOrder32(pfmFile->data,
				pfmFile->width * pfmFile->height * pfmFile->components);
	}
	pfmFile->scale = (float) fabs(pfmFile->scale);

	fclose(fp);

	return 0;
}

int pfmWrite(const char *filename, PFMFile *pfmFile)
{

	int dataSize;
	FILE *fp;

	if (!(fp = fopen(filename, "wb"))) {
		perror("opening image file failed");
		return -1;
	}

	/* Write magic number */
	if (pfmFile->components == 3) {
		fprintf(fp, "%s\n", MAGIC_NUMBER_3_COMPOMENTS);
	} else if (pfmFile->components == 1) {
		fprintf(fp, "%s\n", MAGIC_NUMBER_1_COMPOMENTS);
	} else {
		fprintf(stderr, "PFM Write: unsupported number of components: %d\n",
				pfmFile->components);
		return -1;
	}

	/* Write dimensions */
	fprintf(fp, "%i %i\n", pfmFile->width, pfmFile->height);

	/* Write maximum value */
	dataSize = pfmFile->width * pfmFile->height * pfmFile->components;
	fprintf(fp, "%f\n", pfmFile->scale * (isLittleEndian() ? -1.0 : 1.0));

	/* Write image data */
	if (fwrite(pfmFile->data, sizeof(float), dataSize, fp) != dataSize) {
		fprintf(stderr, "writing image data failed\n");
		return -1;
	}

	fclose(fp);

	return 0;
}

