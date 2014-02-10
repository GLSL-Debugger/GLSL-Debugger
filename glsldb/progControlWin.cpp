// Windows specific calls of progControl.cpp
// I just want to separate it from that big file for future progControl
// reimplementation. When the new interface will be there, I'll return code back.

#include <QtCore>
#include <windows.h>
#include "progControl.qt.h"
#include "utils/dbgprint.h"
#include <cstdlib>
#define DEBUGLIB "\\glsldebug.dll"

#ifdef UNICODE
	#define LOAD_LIB_NAME  "LoadLibraryW"
#else
	#define LOAD_LIB_NAME  "LoadLibraryA"
#endif // !UNICODE


DWORD injectLib(HANDLE hProcess, ATTACHMENT_INFORMATION &ai)
{
	HANDLE hThread;
	DWORD error;
	char dllPath[_MAX_PATH];		// Path to debugger preload DLL.
	void* debuggeeLib;   // The address (in the remote process) where
						  // dllPath will be copied to;
	HMODULE hKernel32 = ::GetModuleHandle("Kernel32");
	DWORD debuggeeAddr;

	// Get dll path
	HMODULE hThis = GetModuleHandleW(NULL);
	GetModuleFileNameA(hThis, dllPath, _MAX_PATH);
	char *insPos = strrchr(dllPath, '\\');
	strcpy(insPos ? insPos : dllPath, DEBUGLIB);

	// 1. Allocate memory in the remote process for szLibPath
	// 2. Write szLibPath to the allocated memory
	debuggeeLib = VirtualAllocEx(hProcess, NULL, sizeof(dllPath),
								 MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, debuggeeLib, (void*)dllPath,
					   sizeof(dllPath), NULL);

	// Load library into the remote process via CreateRemoteThread & LoadLibrary
	LPTHREAD_START_ROUTINE loadLib = (LPTHREAD_START_ROUTINE)GetProcAddress(
									hKernel32, LOAD_LIB_NAME);
	hThread = CreateRemoteThread(hProcess, NULL, 0, loadLib, debuggeeLib,
								 CREATE_SUSPENDED, NULL);
	error = GetLastError();
	if (!error) {
		ResumeThread(hThread);
		WaitForSingleObject(hThread, INFINITE);

		// Get attachment information of the loaded module
		ai.hProcess = hProcess;
		GetExitCodeThread(hThread, &debuggeeAddr);
		//GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		//					GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		//					reinterpret_cast<LPCTSTR>(&debuggeeAddr), &ai.hLibrary);

		// Clean up
		CloseHandle(hThread);
	} else {
		dbgPrint(DBGLVL_ERROR, "Injection failed, error code %i", error);
	}

	VirtualFreeEx(hProcess, debuggeeLib, sizeof(dllPath), MEM_RELEASE);
	return error;
}

pcErrorCode ProgramControl::runProgram(char **debuggedProgramArgs, char *workDir)
{
	pcErrorCode error = PCE_EXEC;
	STARTUPINFOA startupInfo;
	PROCESS_INFORMATION processInfo;
	LPSTR cmdLine; // Command line to execute.
	QStringList cmdArgs;

	/* Only size must be initialised. */
	ZeroMemory(&startupInfo, sizeof(STARTUPINFOA));
	startupInfo.cb = sizeof(STARTUPINFOA);

	/* Concatenate the command line. */
	for (int i = 0; debuggedProgramArgs[i] != 0; i++)
		cmdArgs << debuggedProgramArgs[i];
	cmdLine = strdup(cmdArgs.join(" ").toStdString().c_str());

	this->setDebugEnvVars();	// TODO dirty hack.
	LPVOID newEnv = GetEnvironmentStrings();


	if (CreateProcess(NULL, cmdLine, NULL, NULL, NULL,
					  CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE,
					  newEnv, workDir, &startupInfo, &processInfo)) {
		this->createEvents(processInfo.dwProcessId);
		_hDebuggedProgram = processInfo.hProcess;
		_debuggeePID = processInfo.dwProcessId;
		//::DebugActiveProcess(processInfo.dwProcessId);
		//::DebugBreakProcess(processInfo.hProcess);
		if(!injectLib(processInfo.hProcess, _ai)){
			ResumeThread(processInfo.hThread);
			CloseHandle(processInfo.hThread);
			error = PCE_NONE;
		}
	}
	free(cmdLine);

	dbgPrint(DBGLVL_INFO, "wait for debuggee\n");
	error = this->checkChildStatus();
	if (error != PCE_NONE) {
		this->killProgram(0);
		//_debuggeePID = 0;
		return error;
	}

	dbgPrint(DBGLVL_INFO, "send continue\n");
	error = this->executeDbgCommand();
	//error = dbgCommandCallOrig();
	if (error != PCE_NONE) {
		this->killProgram(0);
		//_debuggeePID = 0;
		return error;
	}

#ifdef DEBUG
	this->printCall();
#endif

	return error;
}

pcErrorCode ProgramControl::attachToProgram(const DWORD pid) {
	pcErrorCode retval = PCE_NONE;      // Method return value.
	char dllPath[_MAX_PATH];// Path to debugger preload DLL.
	char smName[_MAX_PATH];// Name of shared memory.

	HMODULE hThis = GetModuleHandleW(NULL);
	GetModuleFileNameA(hThis, dllPath, _MAX_PATH);
	char *insPos = strrchr(dllPath, '\\');
	strcpy(insPos ? insPos : dllPath, DEBUGLIB);

	this->createEvents(pid);
	::SetEvent(_hEvtDebuggee);

	this->setDebugEnvVars();	// TODO dirty hack.
	::GetEnvironmentVariableA("GLSL_DEBUGGER_SHMID", smName, _MAX_PATH);
	if (!::AttachToProcess(_ai, pid, PROCESS_ALL_ACCESS, dllPath, smName,
					_path_dbgfuncs.c_str())) {
		return PCE_UNKNOWN_ERROR;   // TODO
	}

	_hDebuggedProgram = _ai.hProcess;
	_debuggeePID = pid;  // TODO: Do we need this information?

	retval = this->checkChildStatus();

	return retval;
}
