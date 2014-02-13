
#ifndef _WIN32
#error Only windows supports this test interface
#endif

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils/dbgprint.h"
#include "initLib.h"
#include "debuglib.h"

#ifdef GLSLDB_OSX
#	define SHM_SIZE (2*1024*1024)
#else
#	define SHM_SIZE (32*1024*1024)
#endif


static struct {
	HANDLE hEvtDebugee; /* wait for debugger */
	HANDLE hEvtDebugger; /* signal debugger */
	HANDLE hShMem; /* shared memory handle */
	DbgRec *fcalls;
	int numDbgFunctions;
} g = {NULL, NULL, NULL, NULL, 0};

int execute_command(enum DBG_OPERATIONS command)
{
	DWORD exitCode = STILL_ACTIVE;
	g.fcalls->operation = command;
	switch(command) {
	case DBG_EXECUTE:
		g.fcalls->items[0] = DBG_EXECUTE_RUN;
		g.fcalls->items[1] = 1;
	case DBG_STOP_EXECUTION:
	case DBG_CALL_ORIGFUNCTION:
	case DBG_DONE:
	default:
		break;
	}

	SetEvent(g.hEvtDebugee);
	while (exitCode == STILL_ACTIVE) {
		switch (WaitForSingleObject(g.hEvtDebugger, 2000)) {
		case WAIT_OBJECT_0:
			return 0;

		case WAIT_TIMEOUT:
			break;

		default:
			return -1;
		}
	} /* end while (exitCode == STILL_ACTIVE) */

	return 0;
}

int main(int argv, char* argc[])
{
	DWORD pid;
	char line[10];
	char shMemName[64];
	int command;
	setMaxDebugOutputLevel(DBGLVL_ALL);

	if (argv < 2){
		printf("No process pid to connect.\n");
		return 1;
	}
	pid = atoi(argc[1]);


	if (!openEventsProc(&g.hEvtDebugee, &g.hEvtDebugger, pid))
		return 1;
	printf("Events opened.\n");

	_snprintf(shMemName, 64, "%uSHM", pid);
	SetEnvironmentVariableA("GLSL_DEBUGGER_SHMID", shMemName);
	/* Attach to shared mem segment */
	if (!openSharedMemory(&g.hShMem, &g.fcalls, SHM_SIZE))
		return 1;
	printf("Memory opened.\n");


	while(1) {
		printf("Waiting for command.\n");
		gets(line);
		// Remove newline
		line[strlen(line)] = '\0';
		command = atoi(line);
		if (command < 0)
			break;
		if (command == 0) {
			execute_command(DBG_CALL_ORIGFUNCTION);
			execute_command(DBG_DONE);
		} else {
			execute_command(command);
		}

	}


	return 0;
}
