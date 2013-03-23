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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
//#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <tlhelp32.h> 
#endif /* _WIN32 */

#include "dbgprint.h"

static struct {
	int maxDebugOutputLevel;
	char* logDir;
	FILE *logfile;
} g = {0, NULL, NULL};

#define LOG_SUFFIX "-glsldevil.log"

void setMaxDebugOutputLevel(int level) {
    g.maxDebugOutputLevel = level;
}

int getMaxDebugOutputLevel(void) {
    return g.maxDebugOutputLevel;
}

void setLogDir(const char* dir)
{
	if (dir) {
#ifdef _WIN32
		g.logDir = _strdup(dir);
#else
		g.logDir = strdup(dir);
#endif
	} else {
		g.logDir = NULL;
	}
}

const char *getLogDir()
{
	return g.logDir;
}

void startLogging(const char* baseName)
{
    if (g.logDir) {
		char* logfileName;
		time_t epochTime = time(NULL); 
		struct tm *currentTime = localtime(&epochTime);

		if (baseName) {
			if (!(logfileName = malloc(strlen(g.logDir) + strlen(baseName) + 16 + strlen(LOG_SUFFIX)))) {
				perror("Error opening logfile");
				exit(1);
			}

			sprintf(logfileName, "%s/%02i%02i%02i-%02i%02i%02i-%s%s",
					g.logDir,
					currentTime->tm_year%100,
					currentTime->tm_mon + 1, 
					currentTime->tm_mday, 
					currentTime->tm_hour, 
					currentTime->tm_min, 
					currentTime->tm_sec, 
					baseName,
					LOG_SUFFIX);
		} else {
#ifdef _WIN32
			DWORD pid = GetCurrentProcessId();
#else
			pid_t pid = getpid();
#endif
			if (!(logfileName = malloc(strlen(g.logDir) + 1 + (int)log10(pid) + 16 +strlen(LOG_SUFFIX)))) {
				perror("Error opening logfile");
				exit(1);
			}

			sprintf(logfileName, "%s/%02i%02i%02i-%02i%02i%02i-%li%s",
					g.logDir,
					currentTime->tm_year%100,
					currentTime->tm_mon + 1, 
					currentTime->tm_mday, 
					currentTime->tm_hour, 
					currentTime->tm_min, 
					currentTime->tm_sec, 
					(long)pid,
					LOG_SUFFIX);
		}

        g.logfile = fopen(logfileName, "w");
		if (!g.logfile)	{
			perror("Error opening logfile");
			exit(1);
		}
		free(logfileName);
    } else {
		g.logfile = NULL;
	}
}

void quitLogging(void)
{
	if (g.logfile) {
		fclose(g.logfile);
		g.logfile = NULL;
	}
}

int _dbgPrint_(int level, int printPrefix, const char *fmt, ...)
{
    va_list list;
#ifdef _WIN32
    int cnt = 0;        /* Size of formatted message. */
    char *tmp = NULL;   /* Buffer for formatted message. */
#endif /* _WIN32 */
	const char* prefix = NULL;
	time_t epochTime = time(NULL); 

    if (level > g.maxDebugOutputLevel) {
        return 0;
    }

	if (printPrefix) {
		switch (level) {
		case DBGLVL_ERROR:
			prefix = "ERROR: ";
			break;
		case DBGLVL_WARNING:
			prefix = "WARNING: ";
			break;
		case DBGLVL_INFO:
			prefix = "INFO: ";
			break;
		case DBGLVL_INTERNAL_WARNING:
			prefix = "INTERNAL: ";
			break;
		case DBGLVL_COMPILERINFO:
			prefix = "COMPILER: ";
			break;
		case DBGLVL_DEBUG:
			prefix = "DEBUG: ";
			break;
		default:
			prefix = "UNCLASSIFIED: ";
			break;
		}
	}

#if _WIN32 && (defined(DEBUG) || defined(_DEBUG))
    /* Debug builds should write to attached debugger console: */
	if (printPrefix) {
		OutputDebugStringA(prefix);
	}
    va_start(list, fmt);
    cnt = _vscprintf(fmt, list) + 1;
    va_end(list);
    tmp = (char *) malloc(cnt * sizeof(char));
    if (tmp != NULL) {
        va_start(list, fmt);
        _vsnprintf_s(tmp, cnt, cnt, fmt, list);
        va_end(list);
        OutputDebugStringA(tmp);
        free(tmp);
    }
#else 

    /* Write to log file */
    if (g.logfile != NULL) {
		if (printPrefix) {
			fprintf(g.logfile, "%li - %s", (long)epochTime, prefix);
		}
        va_start(list, fmt);
        vfprintf(g.logfile, fmt, list);
        va_end(list);
        fflush(g.logfile);
    }
	else
	{
		va_start(list, fmt);
		vfprintf(stderr, fmt, list);
		va_end(list);
	}
#endif

    return 0;
}
