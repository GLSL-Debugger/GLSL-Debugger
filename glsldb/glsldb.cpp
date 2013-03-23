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

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif /* _WIN32 */
#include <string.h>
#ifndef _WIN32
#include <sys/ptrace.h>
#include <sys/wait.h>
#endif /* _WIN32 */

#include <QtGui/QApplication>
#include "mainWindow.qt.h"
#include "dbgprint.h"

extern "C" {
  #include "GL/gl.h"
  #include "GL/glext.h"
  #include "debuglib.h"
  #include "glenumerants/glenumerants.h"
  #include "utils/p2pcopy.h"
}

static struct {
	int shmid;
	DbgRec *fcalls;
	char *debuggedProgram;
	char **debuggedProgramArgs;
	char *debuglib;
	char *dbgFunctionsPath;
	char *libdlsym;
#ifndef _WIN32
	pid_t debuggedProgramPID;
#else /* _WIN32 */
	DWORD debuggedProgramPID;
#endif /* _WIN32 */
} g;

void usage(const char *pname)
{
	printf("Usage: %s [OPTIONS] [executable-file arguments]\n", pname);
	printf("\n");
	printf("\t-h\n\t\tshow usage information\n");
	printf("\t-v\n\t\tverbose logging\n");
	printf("\t-vv\n\t\tmore verbose logging\n");
#ifdef DEBUG
	printf("\t-compilerInfo\n\t\tlog compiler info\n");
	printf("\t-debug\n\t\tfull debug logging\n");
#endif
	printf("\t-ld dirname\n\t\tlog to specified directory. Default: log to stderr\n");
}

#ifdef _WIN32
#  define DIRSEP '\\'
#else
#  define DIRSEP '/'
#endif

#if 0
static DbgRec *getThreadRecord(pid_t pid)
{
	int i;
	for (i = 0; i < SHM_MAX_THREADS; i++) {
		if (g.fcalls[i].threadId == 0 || g.fcalls[i].threadId == pid) {
			break;
		}
	}
	if (i == SHM_MAX_THREADS) {
		dbgPrint(DBGLVL_ERROR, "Error: max. number of debuggable threads exceeded!\n");
		exit(1);
	}
	return &g.fcalls[i];
}
#endif

static void parseArgs(int argc, char **argv, char ***debuggedProgramArgs)
{
	int i, j;

	*debuggedProgramArgs = NULL;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			usage(argv[0]);
			exit(1);
		} else if (!strcmp(argv[i], "-v")) {
			setMaxDebugOutputLevel(DBGLVL_WARNING);
		} else if (!strcmp(argv[i], "-vv")) {
			setMaxDebugOutputLevel(DBGLVL_INFO);
#ifdef DEBUG
		} else if (!strcmp(argv[i], "-compilerInfo")) {
			setMaxDebugOutputLevel(DBGLVL_COMPILERINFO);
		} else if (!strcmp(argv[i], "-debug")) {
			setMaxDebugOutputLevel(DBGLVL_DEBUG);
#endif
		} else if (!strcmp(argv[i], "-ld")) {
			++i;
			if (i < argc)
			{
				setLogDir(argv[i]);
			}
			else
			{
				fprintf(stderr, "-ld option requires an argument!");
				exit(1);
			}
		} else if (argv[i][0] == '-') {
			dbgPrint(DBGLVL_ERROR, "Unknow option: \"%s\"\n", argv[i]);
			usage(argv[0]);
			exit(1);
		} else {
			/* not a debugger command switch, assume the rest is program and
			 * program switches
			 */
			if (!(*debuggedProgramArgs = (char**) malloc((argc - i + 1)*sizeof(char*)))) {
				dbgPrint(DBGLVL_ERROR, "Error: allocation of debuggedProgramArgs failed\n");
				exit(1);
			}
			(*debuggedProgramArgs)[0] = strdup(argv[i]);
			for (j = 0; j < argc - i; j++) {
				if (!((*debuggedProgramArgs)[j] = strdup(argv[i + j]))) {
					dbgPrint(DBGLVL_ERROR,
							"Error: allocation of debuggedProgramArgs[%d] failed\n",
							j);
					exit(1);
				}
				dbgPrint(DBGLVL_INFO, "ARG[%i] = \"%s\"\n", j,
				         (*debuggedProgramArgs)[j]);
			}
			(*debuggedProgramArgs)[j] = NULL;
			break;
		}
	}
}

static void freeArgs(char ***debuggedProgramArgs)
{
	int i = 0;

	if (*debuggedProgramArgs) {
		while ((*debuggedProgramArgs)[i]) {
			free((*debuggedProgramArgs)[i]);
			i++;
		}
		free(*debuggedProgramArgs);
		*debuggedProgramArgs = NULL;
	}
}

#if 0
void printArgument(void *addr, int type)
{
	char *s;
	/* FIXME */
	int *tmp = (int*) malloc(sizeof(double)+sizeof(long long));

	switch (type) {
	case DBG_TYPE_CHAR:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(char));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%i", *(char*)tmp); 
		break;
	case DBG_TYPE_UNSIGNED_CHAR:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(unsigned char));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%i", *(unsigned char*)tmp); 
		break;
	case DBG_TYPE_SHORT_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(short));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%i", *(short*)tmp); 
		break;
	case DBG_TYPE_UNSIGNED_SHORT_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(unsigned short));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%i", *(unsigned short*)tmp); 
		break;
	case DBG_TYPE_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(int));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%i", *(int*)tmp); 
		break;
	case DBG_TYPE_UNSIGNED_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(unsigned int));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%u", *(unsigned int*)tmp); 
		break;
	case DBG_TYPE_LONG_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(long));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%li", *(long*)tmp); 
		break;
	case DBG_TYPE_UNSIGNED_LONG_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(unsigned long));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%lu", *(unsigned long*)tmp); 
		break;
	case DBG_TYPE_LONG_LONG_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(long long));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%lli", *(long long*)tmp); 
		break;
	case DBG_TYPE_UNSIGNED_LONG_LONG_INT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(unsigned long long));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%llu", *(unsigned long long*)tmp); 
		break;
	case DBG_TYPE_FLOAT:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(float));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%f", *(float*)tmp); 
		break;
	case DBG_TYPE_DOUBLE:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(double));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%f", *(double*)tmp); 
		break;
	case DBG_TYPE_POINTER:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(void*));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%p", *(void**)tmp); 
		break;
	case DBG_TYPE_BOOLEAN:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(GLboolean));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%s", *(GLboolean*)tmp ? "TRUE" : "FALSE");
		break;
	case DBG_TYPE_BITFIELD:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(GLbitfield));
		s  = dissectBitfield(*(GLbitfield*)tmp);
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%s", s);
		free(s);
		break;
	case DBG_TYPE_ENUM:
		cpyFromProcess(g.debuggedProgramPID, tmp, addr, sizeof(GLbitfield));
		dbgPrintNoPrefix(DBGLVL_DEBUG, "%s", lookupEnum(*(GLenum*)tmp));
		break;
	case DBG_TYPE_STRUCT:
		dbgPrintNoPrefix(DBGLVL_DEBUG, "STRUCT");
		break;
	default:	
		dbgPrintNoPrefix(DBGLVL_DEBUG, "UNKNOWN TYPE [%i]", type);
	}
	free(tmp);
}					  
	
void printCall(void)
{
	int i;
	DbgRec *rec = getThreadRecord(g.debuggedProgramPID);
	dbgPrint(DBGLVL_DEBUG, "CALL: %s(", rec->fname);
	for (i = 0; i < rec->numItems; i++) {
		dbgPrintNoPrefix(DBGLVL_DEBUG, "(%p,%li)", (void*)rec->items[2*i], rec->items[2*i+1]);
		printArgument((void*)rec->items[2*i], rec->items[2*i+1]);
		if (i != rec->numItems - 1) {
			dbgPrintNoPrefix(DBGLVL_DEBUG, ", ");
		}
	}
	dbgPrintNoPrefix(DBGLVL_DEBUG, ")\n");
}

void printResult(void)
{
	DbgRec *rec = getThreadRecord(g.debuggedProgramPID);
	if (rec->operation == DBG_ERROR_CODE) {
		dbgPrint(DBGLVL_DEBUG, "ERROR: %i\n", (unsigned int)rec->items[0]);
	} else if (rec->operation == DBG_RETURN_VALUE) {
		dbgPrint(DBGLVL_DEBUG, "RESULT: (%p,%li) ", (void*)rec->items[0], rec->items[1]);
		printArgument((void*)rec->items[0], rec->items[1]);
		dbgPrint(DBGLVL_DEBUG, "\n");
	}
}
#endif

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
#ifdef _WIN32
	app.setStyle("windowsxp");
#else /* _WIN32 */
    app.setStyle("cleanlooks");
#endif /* _WIN32 */

	QCoreApplication::setOrganizationName("VIS");
	QCoreApplication::setOrganizationDomain("vis.uni-stuttgart.de");
	QCoreApplication::setApplicationName("glsldevil");

	setMaxDebugOutputLevel(DBGLVL_ERROR);               /* default for release builds */
	//setMaxDebugOutputLevel(DBGLVL_INTERNAL_WARNING);
	//setMaxDebugOutputLevel(DBGLVL_COMPILERINFO);
	//setMaxDebugOutputLevel(DBGLVL_DEBUG);             /* only effective in case of a DEBUG build */
	
    parseArgs(argc, argv, &g.debuggedProgramArgs);

	startLogging("glsldb");

    MainWindow *mainWin = new MainWindow(argv[0], g.debuggedProgramArgs);

	freeArgs(&g.debuggedProgramArgs);
	
    mainWin->show();

    int returnValue = app.exec();

	delete mainWin;

	quitLogging();

	return returnValue;
}

