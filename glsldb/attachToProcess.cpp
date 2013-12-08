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

#include "attachToProcess.qt.h"

#ifdef _WIN32
#include <cassert>
#include <psapi.h>
#include <detours.h>
#include <tchar.h>
#include "asprintf.h"
#endif /* _WIN32 */

#include <cstdarg>
#include <stdexcept>

#include <QtCore/QVariant>

#include "dbgprint.h"


#ifdef _WIN32
/*
 * ProcessSnapshotModel::Item::toQString
 */
QString ProcessSnapshotModel::Item::toQString(const wchar_t *str) {
    QString retval;

    if (str != NULL) {
        int bufLen = ::WideCharToMultiByte(CP_THREAD_ACP, 0, str, -1, NULL, 0,
            NULL, NULL);
        char *tmp = new char[bufLen];
        ::WideCharToMultiByte(CP_THREAD_ACP, 0, str, -1, tmp, bufLen, NULL,
            NULL);
        retval = QString(tmp);
        delete[] tmp;
    }

    return retval;
}
#endif /* _WIN32 */


/*
 * ProcessSnapshotModel::Item::Item
 */
ProcessSnapshotModel::Item::Item(const char *exe, const char *owner, PID pid,
        const bool isAttachable, Item *parent)
        : child(NULL), parent(parent), exe(exe), owner(owner),
        isAttachable(isAttachable), pid(pid) {
}


#ifdef _WIN32
/*
 * ProcessSnapshotModel::Item::Item
 */
ProcessSnapshotModel::Item::Item(PROCESSENTRY32& pe, Item *parent)
        : child(NULL), exe(toQString(pe.szExeFile)), parent(parent),
        isAttachable(false), pid(pe.th32ProcessID) {
    HANDLE hProcess = NULL;     // Process handle.
    HANDLE hToken = NULL;       // Process security token.
    HANDLE hSnapshot = NULL;    // Snapshot of process modules.
    MODULEENTRY32 me;           // For iterating modules.

    if ((hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
            this->pid)) != NULL) {
        // TODO: System UNC path is too weird.
        //char tmp[MAX_PATH];
        //if (::GetProcessImageFileNameA(hProcess, tmp, MAX_PATH)) {
        //    this->exe = QString(tmp);
        //}

        if (::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {

            /* User user information about process token. */
            DWORD userInfoLen = 0;
            if (::GetTokenInformation(hToken, TokenUser, NULL, 0, &userInfoLen)
                    || (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
                char *userInfo = new char[userInfoLen];

                if (::GetTokenInformation(hToken, TokenUser, userInfo,
                        userInfoLen, &userInfoLen)) {
                    PSID sid = reinterpret_cast<TOKEN_USER *>(
                        userInfo)->User.Sid;

                    /* Lookup the user name of the SID. */
                    DWORD userNameLen = 0;
                    DWORD domainLen = 0;
                    SID_NAME_USE snu = SidTypeUnknown;
                    if (::LookupAccountSidA(NULL, sid, NULL, &userNameLen,
                            NULL, &domainLen, &snu) || (::GetLastError()
                            == ERROR_INSUFFICIENT_BUFFER)) {
                        char *userName = new char[userNameLen];
                        char *domain = new char[domainLen];
                        if (::LookupAccountSidA(NULL, sid, userName,
                                &userNameLen, domain, &domainLen, &snu)) {
                            this->owner = QString(domain) + "\\"
                                + QString(userName);
                            this->isAttachable = true;
                            // Note: If we cannot get the owner SID, the owner
                            // is probably SYSTEM and we cannot attach to
                            // system processes.
                        }
                        delete[] userName;
                        delete[] domain;
                    }
                }

                delete[] userInfo;
            }
            ::CloseHandle(hToken);
        }

        ::CloseHandle(hProcess);
    }

    /* Check whether the process has OpenGL loaded. */
    if (this->isAttachable) {
        this->isAttachable = false;

        // Note: If we cannot enumerate the modules, we probably cannot attach,
        // too, because of insufficient privileges.
        if ((hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
                this->pid)) != INVALID_HANDLE_VALUE) {
            me.dwSize = sizeof(me);

            if (::Module32First(hSnapshot, &me)) {
                do {
                    QString name = toQString(me.szModule);
                    if (name.contains("opengl", Qt::CaseInsensitive)) {
                        this->isAttachable = true;
                        break;
                    }
                } while (::Module32Next(hSnapshot, &me));
            }
        }

        ::CloseHandle(hSnapshot);
    }

    /* Attaching to the debugger is forbidden, too. */
    if (this->pid == ::GetCurrentProcessId()) {
        this->isAttachable = false;
    }

    // TODO: Should it be forbidden to attach to process that have the
    // debug library already loaded?
}
#endif /* _WIN32 */


/*
 * ProcessSnapshotModel::Item::~Item
 */
ProcessSnapshotModel::Item::~Item(void) {
    delete this->child;
    this->child = NULL;
}


/*
 * ProcessSnapshotModel::ProcessSnapshotModel
 */
ProcessSnapshotModel::ProcessSnapshotModel(QObject *parent)
        : Super(parent), root(NULL) {
    this->Update();
}


/*
 * ProcessSnapshotModel::~ProcessSnapshotModel
 */
ProcessSnapshotModel::~ProcessSnapshotModel(void) {
    delete this->root;
}


/*
 * ProcessSnapshotModel::Update
 */
bool ProcessSnapshotModel::Update(void) {
    delete this->root;
    this->root = NULL;

#ifdef _WIN32
    HANDLE hSnapshot = NULL;
    PROCESSENTRY32 pe;
    Item *item = NULL;

    if ((hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0))
            == INVALID_HANDLE_VALUE) {
        return false;
    }

    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!::Process32First(hSnapshot, &pe)) {
        ::CloseHandle(hSnapshot);
        return false;
    }
    this->root = item = new Item(pe);

    while (::Process32Next(hSnapshot, &pe)) {
        item->child = new Item(pe, item);
        item = item->child;
    }

    ::CloseHandle(hSnapshot);
    this->reset();
    return true;

#else /* _WIN32 */
    // TODO
    return false;
#endif /* _WIN32 */
}


/*
 * ProcessSnapshotModel::columnCount
 */
int ProcessSnapshotModel::columnCount(const QModelIndex& parent) const {
    UNUSED_ARG(parent)
    return 3;
}


/*
 * ProcessSnapshotModel::data
 */
QVariant ProcessSnapshotModel::data(const QModelIndex& index, int role) const {
    Item *item = (index.isValid())
        ? static_cast<Item *>(index.internalPointer()) : NULL;

    if (item != NULL) {
        switch (role) {
            case Qt::DisplayRole:
                switch (index.column()) {
                    case 0: return item->exe;
                    case 1: return QVariant(static_cast<uint>(item->pid));
                    case 2: return item->owner;
                }
                /* falls through. */

            default:
                return QVariant();
        }
    } else {
        return QVariant();
    }
}


/*
 * ProcessSnapshotModel::flags
 */
Qt::ItemFlags ProcessSnapshotModel::flags(const QModelIndex& index) const {
    Item *item = (index.isValid())
        ? static_cast<Item *>(index.internalPointer()) : NULL;

    if ((item != NULL) && (item->isAttachable)) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return 0;
    }
}


/*
 * ProcessSnapshotModel::headerData
 */
QVariant ProcessSnapshotModel::headerData(int section,
        Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole:
                switch (section) {
                    case 0: return QString("Process Image");
                    case 1: return QString("PID");
                    case 2: return QString("Owner");
                }
                /* falls through. */

            default:
                return QVariant();
        }
    } else {
        return QVariant();
    }
}


/*
 * ProcessSnapshotModel::index
 */
QModelIndex ProcessSnapshotModel::index(int row, int column,
        const QModelIndex& parent) const {
    Item *item = (parent.isValid())
        ? static_cast<Item *>(parent.internalPointer()) : this->root;

    for (int r = 0; (r < row) && (item != NULL); r++) {
        item = item->child;
    }

    if (item != NULL) {
        return this->createIndex(row, column, item);
    } else {
        return QModelIndex();
    }
}


/*
 * ProcessSnapshotModel::rowCount
 */
int ProcessSnapshotModel::rowCount(const QModelIndex& parent) const {
    Item *item = (parent.isValid())
        ? static_cast<Item *>(parent.internalPointer()) : this->root;
    int retval = 0;

    if (parent.isValid()) return 0;

    item = this->root;

    while (item != NULL) {
        retval++;
        item = item->child;
    }

    return retval;
}





#ifdef _WIN32
//typedef LONG ( NTAPI *_NtSuspendProcess )( IN HANDLE ProcessHandle );
//typedef LONG ( NTAPI *_NtResumeProcess )( IN HANDLE ProcessHandle );


/**
 * This function runs a remote thread in 'hProcess' to explicity load the
 * library specified by 'libPath'.
 */
static HMODULE RemoteLoadLibrary(HANDLE hProcess, const char *libPath) {
    DWORD exitCode = 0;             // Exit code of remote thread.
    DWORD remoteMemSize = 0;        // Size of allocated remote memory.
    HANDLE hThread = NULL;          // Handle to remote thread.
    HMODULE hKernel32 = NULL;       // Module handle of kernel32.dll.
    HMODULE retval = NULL;          // Handle to loaded remote library.
    FARPROC loadLibrary = NULL;     // Function pointer of LoadLibrary.
    void *remoteLibPath = NULL;     // Remote memory for path to library.

    /* Sanity checks. */
    if (hProcess == NULL) {
        goto cleanup;
    }
    if (libPath == NULL) {
        goto cleanup;
    }

    /* Get Function pointer to LoadLibrary. */
    if ((hKernel32 = ::GetModuleHandleA("kernel32")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "Module handle of \"kernel32\" could not be retrieved: "
            "%u.\n", ::GetLastError());
        goto cleanup;
    }
    if ((loadLibrary = ::GetProcAddress(hKernel32, "LoadLibraryA")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "\"LoadLibrary\" could not be found: %u.\n",
            ::GetLastError());
        goto cleanup;
    }

    /* Allocate memory for library path in remote process. */
    remoteMemSize = ::strlen(libPath) + 1;
    if ((remoteLibPath = ::VirtualAllocEx(hProcess, NULL, remoteMemSize,
            MEM_COMMIT, PAGE_READWRITE)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "VirtualAllocEx failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Copy library path to remote process. */
    if (!::WriteProcessMemory(hProcess, remoteLibPath, libPath,
            remoteMemSize, NULL)) {
        dbgPrint(DBGLVL_ERROR, "WriteProcessMemory failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Load the debug library into the remote process. */
    if ((hThread = ::CreateRemoteThread(hProcess, NULL, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary),
            remoteLibPath, 0, NULL)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "CreateRemoteThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Wait for LoadLibrary to complete. */
    if (::WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0) {
        dbgPrint(DBGLVL_ERROR, "WaitForSingleObject failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Get exit code, which is the module handle of our remote debug library. */
    // TODO: Das könnte unter 64 bit kriminell sein ...
    if (!::GetExitCodeThread(hThread, &exitCode)) {
        dbgPrint(DBGLVL_ERROR, "GetExitCodeThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }
    retval = reinterpret_cast<HMODULE>(exitCode);

    /* Clean up. */
cleanup:
    if (hThread != NULL) {
        ::CloseHandle(hThread);
    }
    if (remoteLibPath != NULL) {
        ::VirtualFreeEx(hProcess, remoteLibPath, remoteMemSize, MEM_RELEASE);
    }
    return retval;
}


/**
 * Free the library designated by 'hRemoteModule' in process 'hProcess'.
 */
static bool RemoteFreeLibrary(HANDLE hProcess, HANDLE hRemoteModule) {
    bool retval = false;            // Result of overall operation.
    DWORD exitCode = 0;             // Exit code of remote thread.
    HANDLE hThread = NULL;          // Handle to remote thread.
    HMODULE hKernel32 = NULL;       // Module handle of kernel32.dll.
    FARPROC freeLibrary = NULL;     // Function pointer of LoadLibrary.

    /* Sanity checks. */
    if (hProcess == NULL) {
        goto cleanup;
    }
    if (hRemoteModule == NULL) {
        goto cleanup;
    }

    /* Get Function pointer to LoadLibrary. */
    if ((hKernel32 = ::GetModuleHandleA("kernel32")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "Module handle of \"kernel32\" could not be retrieved: "
            "%u.\n", ::GetLastError());
        goto cleanup;
    }
    if ((freeLibrary = ::GetProcAddress(hKernel32, "FreeLibrary")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "\"FreeLibrary\" could not be found: %u.\n",
            ::GetLastError());
        goto cleanup;
    }

    /* Unload the debug library from the remote process. */
    if ((hThread = ::CreateRemoteThread(hProcess, NULL, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(freeLibrary),
            reinterpret_cast<void *>(hRemoteModule), 0, NULL)) == NULL) {
        goto cleanup;
    }

    /* Wait for FreeLibrary to complete. */
    if (::WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0) {
        dbgPrint(DBGLVL_ERROR, "WaitForSingleObject failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Get exit code, which is the result of remote FreeLibrary. */
    if (!::GetExitCodeThread(hThread, &exitCode)) {
        dbgPrint(DBGLVL_ERROR, "GetExitCodeThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }
    retval = (exitCode != 0);

    /* Clean up. */
cleanup:
    if (hThread != NULL) {
        ::CloseHandle(hThread);
    }

    return retval;
}


/**
 * This function is executed in the remote process to set environment variables.
 *
 * IN CASE YOU ARE NOT "ICH HAB'S ERFUNDEN (5)", READ THIS CAREFULLY:

* It is crucial to place _sizeCrowbarRemoteSetEnvFunc directly after
 * _remoteSetEnvFunc, because it is used to determine the binary code size of
 * _remoteSetEnvFunc! If the operation crashes, the linker might have reordered
 * the functions. You can use the /ORDER linker directive to enforce the order
 * of these two functions. The two pragmas enclosing the two functions should
 * increase the probability that the linker does not do any harmful
 * optimisation.
 *
 * Additionally, it is crucial that _remoteSetEnvFunc and
 * _sizeCrowbarRemoteSetEnvFunc have static linkage to avoid indirect calls
 * that result from incremental linking (see the Codeproject link below for
 * further information).
 *
 * You MUST NOT CALL ANY FUNCTION within _remoteSetEnvFunc, including functions
 * from the C runtime library. Functions that are allowed within this function
 * must reside in kernel32.dll. You should pass them as function pointers via
 * the parameter 'params'.
 *
 * You MUST NOT USE ANY LOCAL ARRAY, including hard-coded constant strings
 * within this function!
 *
 * This dirty hack uses the "CreateRemoteThread & WriteProcessMemory Technique"
 * described on http://www.codeproject.com/threads/winspy.asp. Read this article
 * for further information why it is dangerous.
 *
 * @param params The parameters holding the environment variables to set. The
 *               following layout is assumed:
 *               - 4 byte DWORD n holding the number of variables to set.
 *               - A pointer to SetEnvironmentVariableA. Note that the function
 *               must not determine the address itself as is must not use any
 *               constant string.
 *               - 2n ANSI character strings in order name, value, name,
 *               value ... Each of the strings is zero-terminated as normal.
 *
 * @return zero in case of an error, non-zero in case of success.
 */
#pragma optimize("", off)
#pragma runtime_checks("", off)
static DWORD _remoteSetEnvFunc(void *params) {
    //__asm int 3
    typedef BOOL (WINAPI *SetEnvFunc)(const char *, const char *);
    DWORD cntVars = 0;
    char *name = NULL;
    char *value = NULL;
    SetEnvFunc setEnv = *reinterpret_cast<SetEnvFunc *>(
        static_cast<char *>(params) + sizeof(DWORD));
    // TODO: This is probably not 64-bit compatible!

    cntVars = *static_cast<DWORD *>(params);
    value = static_cast<char *>(params) + sizeof(DWORD) + sizeof(void *);
    //__asm int 3
    for (DWORD i = 0; i < cntVars; i++) {
        name = value;
        while (*value++ != 0);
        //__asm int 3
        if (!setEnv(name, value)) {
            return 0;
        }
        while (*value++ != 0);
    }

    return 1;
}
// DO NOT INSERT ANYTHING HERE!!!
static void _sizeCrowbarRemoteSetEnvFunc(void) {
}
#pragma runtime_checks("", restore)
#pragma optimize("", on)


/**
 * Set environment variables in remote process designated by 'hProcess'. The
 * environment variables must be specified interleaved as parameters. The
 * parameter list must have a NULL pointer as end marker.
 */
static bool RemoteSetEnv(HANDLE hProcess, const char *name0,
        const char *value0, ...) {
    va_list argptr;                 // Variable argument list cursor.
    bool retval = false;            // Overall success of operation.
    DWORD cntVars = 0;              // # of environment variables to set.
    DWORD exitCode = 0;             // Exit code of remote thread.
    DWORD remoteDataSize = 0;       // Size of 'remoteData'.
    DWORD remoteFuncSize = 0;       // Size of 'remoteFunc'.
    HANDLE hThread = NULL;          // Handle to remote thread.
    HMODULE hKernel32 = NULL;       // Module handle of kernel32.dll.
    void *remoteData = NULL;        // Pointer to remote parameter list.
    void *remoteFunc = NULL;        // Pointer to remote thread function.
    char *data = NULL;              // Parameter list for remote thread.
    char *insPos = NULL;            // Write pointer into 'data'.
    const char *arg;                // Pointer on current variable.
    FARPROC fpSetEnv = NULL;        // Pointer to SetEnvironmentVariableA

    /* Sanity check. */
    if (hProcess == NULL) {
        goto cleanup;
    }
    if (name0 == NULL) {
        goto cleanup;
    }
    if (value0 == NULL) {
        goto cleanup;
    }

    /* Get Function pointer to SetEnvironmentVariableA. */
    if ((hKernel32 = ::GetModuleHandleA("kernel32")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "Module handle of \"kernel32\" could not be retrieved: "
            "%u.\n", ::GetLastError());
        goto cleanup;
    }
    if ((fpSetEnv = ::GetProcAddress(hKernel32, "SetEnvironmentVariableA"))
            == NULL) {
        dbgPrint(DBGLVL_ERROR, "\"SetEnvironmentVariableA\" could not be found: %u.\n",
            ::GetLastError());
        goto cleanup;
    }

    /* Compute required size for remote data. */
    cntVars = 2;                    // Included non-variable parameters.
    remoteDataSize = sizeof(DWORD) + sizeof(void *) + (::strlen(name0) + 1)
        * sizeof(char) + (::strlen(value0) + 1) * sizeof(char);

    va_start(argptr, value0);
    while ((arg = va_arg(argptr, const char *)) != NULL) {
        cntVars++;
        remoteDataSize += (::strlen(arg) + 1) * sizeof(char);
    }
    va_end(argptr);
    cntVars /= 2;                   // This will exclude name w/o value, if any.

    /* Allocate parameter list and fill it. */
    try {
        insPos = data = new char[remoteDataSize];
    } catch (std::bad_alloc) {
        goto cleanup;
    }

    ::memcpy(insPos, &cntVars, sizeof(DWORD));
    insPos += sizeof(DWORD);
    ::memcpy(insPos, &fpSetEnv, sizeof(void *));
    insPos += sizeof(void *);

    arg = name0;
    while ((*insPos++ = *arg++) != 0);
    arg = value0;
    while ((*insPos++ = *arg++) != 0);

    va_start(argptr, value0);
    for (DWORD i = 0; i < cntVars - 1; i++) {
        // Name
        arg = va_arg(argptr, const char *);
        while ((*insPos++ = *arg++) != 0);
        // Value
        arg = va_arg(argptr, const char *);
        while ((*insPos++ = *arg++) != 0);
    }
    va_end(argptr);

    /* Allocate memory for parameteters in remote process. */
    if ((remoteData = ::VirtualAllocEx(hProcess, NULL, remoteDataSize,
            MEM_COMMIT, PAGE_READWRITE)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "VirtualAllocEx failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Copy parameters to remote process. */
    if (!::WriteProcessMemory(hProcess, remoteData, data,
            remoteDataSize, NULL)) {
        dbgPrint(DBGLVL_ERROR, "WriteProcessMemory failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Allocate memory for injected function in remote process. */
    remoteFuncSize = reinterpret_cast<BYTE *>(_sizeCrowbarRemoteSetEnvFunc)
        - reinterpret_cast<BYTE *>(_remoteSetEnvFunc);
    dbgPrint(DBGLVL_DEBUG, "Size of _remoteSetEnvFunc: %u B.\n", remoteFuncSize);
    // Note: The maximum of 400 B ist just an assumption. You might check the
    // binary if this assertion fails.
    assert(remoteFuncSize < 400);
    if ((remoteFunc = ::VirtualAllocEx(hProcess, NULL, remoteFuncSize,
            MEM_COMMIT, PAGE_READWRITE)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "VirtualAllocEx failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Copy code of injected function into remote process. */
    if (!::WriteProcessMemory(hProcess, remoteFunc, _remoteSetEnvFunc,
            remoteFuncSize, NULL)) {
        dbgPrint(DBGLVL_ERROR, "WriteProcessMemory failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Run the injected function in a remote thread. */
    if ((hThread = ::CreateRemoteThread(hProcess, NULL, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteFunc),
            remoteData, 0, NULL)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "CreateRemoteThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Wait for the remote function to complete. */
    if (::WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0) {
        dbgPrint(DBGLVL_ERROR, "WaitForSingleObject failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Get the exit code of the remote function. */
    if (!::GetExitCodeThread(hThread, &exitCode)) {
        dbgPrint(DBGLVL_ERROR, "GetExitCodeThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }
    retval = (exitCode != 0);

    /* Clean up. */
cleanup:
    if (hThread != NULL) {
        ::CloseHandle(hThread);
    }
    if (remoteData != NULL) {
        ::VirtualFreeEx(hProcess, remoteData, remoteDataSize, MEM_RELEASE);
    }
    if (remoteFunc != NULL) {
        ::VirtualFreeEx(hProcess, remoteFunc, remoteFuncSize, MEM_RELEASE);
    }
    if (data != NULL) {
        delete[] data;
    }
    return retval;
}


/**
 * TODO: documentation
 *
 * NOTE: THIS FUNCTION BEHAVES SIMILAR TO _remoteSetEnvFunc. READ THE
 * DOCUMENTATION FOR THIS FUNCTION CAREFULLY, TOO, BEFORE MODIFYING ANYTHING
 * HERE!
 *
 * @param params The parameter holds all information to call a remote function.
                The following layout of the parameter is assumed.
 *               - 4 byte pointer to GetProcAddress function. Note that the
 *               function must not determine the address itself as is must not
 *               use any constant string.
 *               - Module handle of the DLL holding the function to lookup and
 *               call.
 *               - Zero terminated name of the function to call. This function
 *               must return an integer and accept no parameter.
 *
 * @return zero in case of an error, non-zero in case of success.
 */
#pragma optimize("", off)
#pragma runtime_checks("", off)
static DWORD _remoteForceUninitFunc(void *params) {
    //__asm int 3
    typedef FARPROC (WINAPI *GetProcAddrFunc)(HMODULE, const char *);
    typedef BOOL (__cdecl *UninitFunc)(void);

    // TODO: This is probably not 64-bit compatible!
    GetProcAddrFunc getProcAddress = *reinterpret_cast<GetProcAddrFunc *>(
        static_cast<char *>(params));
    HMODULE hModule = *reinterpret_cast<HMODULE *>(
        static_cast<char *>(params) + sizeof(GetProcAddrFunc));
    char *funcName = static_cast<char *>(params) + sizeof(GetProcAddrFunc)
        + sizeof(HMODULE);
    //__asm int 3

    UninitFunc uninitialiseDll = reinterpret_cast<UninitFunc>(getProcAddress(
        hModule, funcName));
    if (uninitialiseDll == NULL) {
        //__asm int 3
        return 0;
    }

    return static_cast<DWORD>(uninitialiseDll());
}
// DO NOT INSERT ANYTHING HERE!!!
static void _sizeCrowbarRemoteForceUninitFunc(void) {
}
#pragma runtime_checks("", restore)
#pragma optimize("", on)


/**
 * Call the function "uninitialiseDll" from 'hModule' in 'hProcess'. 'hModule'
 * must be a remote DLL handle of a DLL loaded into 'hProcess'.
 */
static bool RemoteForceUninitialse(HANDLE hProcess, HMODULE hModule) {
    static const char *REMOTE_FUNC_NAME = "uninitialiseDll";
    bool retval = false;            // Overall success of operation.
    DWORD exitCode = 0;             // Exit code of remote thread.
    DWORD remoteDataSize = 0;       // Size of 'remoteData'.
    DWORD remoteFuncSize = 0;       // Size of 'remoteFunc'.
    HANDLE hThread = NULL;          // Handle to remote thread.
    HMODULE hKernel32 = NULL;       // Module handle of kernel32.dll.
    void *remoteData = NULL;        // Pointer to remote parameter list.
    void *remoteFunc = NULL;        // Pointer to remote thread function.
    char *data = NULL;              // Parameter list for remote thread.
    char *insPos = NULL;            // Write pointer into 'data'.
    FARPROC fpGetProcAddr = NULL;   // Pointer to GetProcAddress

    /* Sanity check. */
    if (hProcess == NULL) {
        goto cleanup;
    }
    if (hModule == NULL) {
        goto cleanup;
    }

    /* Get Function pointer to SetEnvironmentVariableA. */
    if ((hKernel32 = ::GetModuleHandleA("kernel32")) == NULL) {
        dbgPrint(DBGLVL_ERROR, "Module handle of \"kernel32\" could not be retrieved: "
            "%u.\n", ::GetLastError());
        goto cleanup;
    }
    if ((fpGetProcAddr = ::GetProcAddress(hKernel32, "GetProcAddress"))
            == NULL) {
        dbgPrint(DBGLVL_ERROR, "\"GetProcAddress\" could not be found: %u.\n",
            ::GetLastError());
        goto cleanup;
    }

    /* Compute required size for remote data. */
    remoteDataSize = sizeof(void *) + sizeof(HMODULE)
        + (::strlen(REMOTE_FUNC_NAME) + 1) * sizeof(char);

    /* Allocate parameter list and fill it. */
    try {
        insPos = data = new char[remoteDataSize];
    } catch (std::bad_alloc) {
        goto cleanup;
    }

    ::memcpy(insPos, &fpGetProcAddr, sizeof(void *));
    insPos += sizeof(void *);
    ::memcpy(insPos, &hModule, sizeof(HMODULE));
    insPos += sizeof(HMODULE);
    ::memcpy(insPos, REMOTE_FUNC_NAME, (::strlen(REMOTE_FUNC_NAME) + 1)
        * sizeof(char));

    /* Allocate memory for parameteters in remote process. */
    if ((remoteData = ::VirtualAllocEx(hProcess, NULL, remoteDataSize,
            MEM_COMMIT, PAGE_READWRITE)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "VirtualAllocEx failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Copy parameters to remote process. */
    if (!::WriteProcessMemory(hProcess, remoteData, data,
            remoteDataSize, NULL)) {
        dbgPrint(DBGLVL_ERROR, "WriteProcessMemory failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Allocate memory for injected function in remote process. */
    remoteFuncSize = reinterpret_cast<BYTE *>(_sizeCrowbarRemoteForceUninitFunc)
        - reinterpret_cast<BYTE *>(_remoteForceUninitFunc);
    dbgPrint(DBGLVL_DEBUG, "Size of _remoteForceUninitFunc: %u B.\n", remoteFuncSize);
    // Note: The maximum of 400 B ist just an assumption. You might check the
    // binary if this assertion fails.
    assert(remoteFuncSize < 400);
    if ((remoteFunc = ::VirtualAllocEx(hProcess, NULL, remoteFuncSize,
            MEM_COMMIT, PAGE_READWRITE)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "VirtualAllocEx failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Copy code of injected function into remote process. */
    if (!::WriteProcessMemory(hProcess, remoteFunc, _remoteForceUninitFunc,
            remoteFuncSize, NULL)) {
        dbgPrint(DBGLVL_ERROR, "WriteProcessMemory failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Run the injected function in a remote thread. */
    if ((hThread = ::CreateRemoteThread(hProcess, NULL, 0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(remoteFunc),
            remoteData, 0, NULL)) == NULL) {
        dbgPrint(DBGLVL_ERROR, "CreateRemoteThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Wait for the remote function to complete. */
    if (::WaitForSingleObject(hThread, INFINITE) != WAIT_OBJECT_0) {
        dbgPrint(DBGLVL_ERROR, "WaitForSingleObject failed: %u.\n", ::GetLastError());
        goto cleanup;
    }

    /* Get the exit code of the remote function. */
    if (!::GetExitCodeThread(hThread, &exitCode)) {
        dbgPrint(DBGLVL_ERROR, "GetExitCodeThread failed: %u.\n", ::GetLastError());
        goto cleanup;
    }
    retval = (exitCode != 0);

    /* Clean up. */
cleanup:
    if (hThread != NULL) {
        ::CloseHandle(hThread);
    }
    if (remoteData != NULL) {
        ::VirtualFreeEx(hProcess, remoteData, remoteDataSize, MEM_RELEASE);
    }
    if (remoteFunc != NULL) {
        ::VirtualFreeEx(hProcess, remoteFunc, remoteFuncSize, MEM_RELEASE);
    }
    if (data != NULL) {
        delete[] data;
    }
    return retval;
}


/**
 * Resume all threads of a remote process.
 */
bool RemoteResumeAllThreads(HANDLE hProcess) {
    HANDLE hSnapshot = NULL;    // Snapshot of all process threads.
    HANDLE hThread = NULL;      // Handle to thread to suspend.
    THREADENTRY32 te;           // Entry of a thread.
    bool retval = true;         // Overall success.

    /* Sanity checks. */
    if (hProcess == NULL) {
        return false;
    }

    if ((hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,
            ::GetProcessId(hProcess))) == INVALID_HANDLE_VALUE) {
        dbgPrint(DBGLVL_ERROR, "CreateToolhelp32Snapshot failed: %u.\n", ::GetLastError());
        return false;
    }

    te.dwSize = sizeof(te);
    if (!::Thread32First(hSnapshot, &te)) {
        dbgPrint(DBGLVL_ERROR, "Thread32First failed: %u.\n", ::GetLastError());
        ::CloseHandle(hSnapshot);
        return false;
    }

    do {
        if ((hThread = ::OpenThread(THREAD_SUSPEND_RESUME, FALSE,
                te.th32ThreadID)) == NULL) {
            dbgPrint(DBGLVL_ERROR, "OpenThread failed: %u.\n", ::GetLastError());
            retval = false;
            continue;
        }

        if (::ResumeThread(hThread) == -1) {
            dbgPrint(DBGLVL_ERROR, "ResumeThread failed: %u.\n", ::GetLastError());
            retval = false;
            continue;
        }

        ::CloseHandle(hThread);
    } while (Thread32Next(hSnapshot, &te));

    ::CloseHandle(hSnapshot);
    return retval;
}


/**
 * Suspend all threads of a remote process.
 */
bool RemoteSuspendAllThreads(HANDLE hProcess) {
    HANDLE hSnapshot = NULL;    // Snapshot of all process threads.
    HANDLE hThread = NULL;      // Handle to thread to suspend.
    THREADENTRY32 te;           // Entry of a thread.

    /* Sanity checks. */
    if (hProcess == NULL) {
        return false;
    }

    if ((hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,
            ::GetProcessId(hProcess))) == INVALID_HANDLE_VALUE) {
        dbgPrint(DBGLVL_ERROR, "CreateToolhelp32Snapshot failed: %u.\n", ::GetLastError());
        return false;
    }

    te.dwSize = sizeof(te);
    if (!::Thread32First(hSnapshot, &te)) {
        dbgPrint(DBGLVL_ERROR, "Thread32First failed: %u.\n", ::GetLastError());
        ::CloseHandle(hSnapshot);
        return false;
    }

    do {
        if ((hThread = ::OpenThread(THREAD_SUSPEND_RESUME, FALSE,
                te.th32ThreadID)) == NULL) {
            dbgPrint(DBGLVL_ERROR, "OpenThread failed: %u.\n", ::GetLastError());
            ::CloseHandle(hSnapshot);
            ::RemoteResumeAllThreads(hProcess);
            return false;
        }

        if (::SuspendThread(hThread) == -1) {
            dbgPrint(DBGLVL_ERROR, "SuspendThread failed: %u.\n", ::GetLastError());
            ::CloseHandle(hThread);
            ::CloseHandle(hSnapshot);
            ::RemoteResumeAllThreads(hProcess);
            return false;
        }

        ::CloseHandle(hThread);
    } while (Thread32Next(hSnapshot, &te));

    ::CloseHandle(hSnapshot);
    return true;
}


/**
 * Repaints 'hWnd' if it belongs to the process designated by the process ID
 * passed as 'lParam'.
 */
static BOOL CALLBACK _updateWindowProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = static_cast<DWORD>(lParam);
    DWORD wndPid = 0;
    ::GetWindowThreadProcessId(hWnd, &wndPid);

    if (wndPid == pid) {
        dbgPrint(DBGLVL_DEBUG, "Updating window of process %u.\n", pid);
        ::InvalidateRect(hWnd, NULL, FALSE);
        ::UpdateWindow(hWnd);
    }

    return TRUE;
}

/**
 * Forces a repaint of all top-level windows belonging to the specified process.
 */
bool RemoteUpdateTopLevelWindows(HANDLE hProcess) {

    /* Sanity checks. */
    if (hProcess == NULL) {
        return false;
    }

    return EnumWindows(::_updateWindowProc, ::GetProcessId(hProcess));
}


/*
 * ::AttachToProcess
 */
bool AttachToProcess(ATTACHMENT_INFORMATION& outAi, DWORD pid,
        DWORD desiredAccess, const char *libPath, const char *smName,
        const char *dbgFuncPath) {
    HMODULE hDetouredDll = NULL;
    char detouredDllPath[_MAX_PATH];

    /* Reset out variable. */
    outAi.hDetours = NULL;
    outAi.hLibrary = NULL;
    outAi.hProcess = NULL;

    /* Get detours marker name for injection. */
    if ((hDetouredDll = ::DetourGetDetouredMarker()) == NULL) {
        dbgPrint(DBGLVL_ERROR, "DetourGetDetouredMarker failed: %u.\n", ::GetLastError());
        return false;
    }
    if (!::GetModuleFileNameA(hDetouredDll, detouredDllPath, _MAX_PATH)) {
        dbgPrint(DBGLVL_ERROR, "GetModuleFileName failed: %u.\n", ::GetLastError());
        return false;
    }

    /* Open process for attachment. */
    outAi.hProcess = ::OpenProcess(desiredAccess | PROCESS_CREATE_THREAD
        | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (outAi.hProcess == NULL) {
        dbgPrint(DBGLVL_ERROR, "OpenProcess failed: %u.\n", ::GetLastError());
        return false;
    }

    /* Set the appropriate environment variables in remote process. */
    if (!::RemoteSetEnv(outAi.hProcess, "GLSL_DEBUGGER_SHMID", smName,
            "GLSL_DEBUGGER_DBGFCTNS_PATH", dbgFuncPath, NULL)) {
        ::CloseHandle(outAi.hProcess);
        outAi.hProcess = NULL;
        return false;
    }

    /* Load detours marker first. */
    outAi.hDetours = ::RemoteLoadLibrary(outAi.hProcess, detouredDllPath);
    if (outAi.hDetours == NULL) {
        dbgPrint(DBGLVL_ERROR, "Attaching detours marker failed.\n");
        ::CloseHandle(outAi.hProcess);
        outAi.hProcess = NULL;
        return false;
    }

    /* Load the debug library into the process. */
    outAi.hLibrary = ::RemoteLoadLibrary(outAi.hProcess, libPath);
    if (outAi.hLibrary == NULL) {
        dbgPrint(DBGLVL_ERROR, "Attaching debug library failed.\n");
        ::RemoteFreeLibrary(outAi.hProcess, outAi.hDetours);
        outAi.hDetours = NULL;
        ::CloseHandle(outAi.hProcess);
        outAi.hProcess = NULL;
        return false;
    }

    // Might dead-lock
    //::RemoteUpdateTopLevelWindows(outAi.hProcess);

    return true;
}



/*
 * ::DetachFromProcess
 */
bool DetachFromProcess(ATTACHMENT_INFORMATION& inOutAi) {
    bool retval = true;

    /* Sanity check. */
    if (inOutAi.hProcess == NULL) {
        return false;
    }

    //::RemoteSuspendAllThreads(inOutAi.hProcess);

    /* Free debug library. */
    // TODO: RemoteForceUninitialse deinstalliert die detours im Gegensatz
    // zu RemoteFreeLibrary sofort, zieht der evtl. laufenden Debug-Operation
    // aber den shared memory unter dem Allerwertesten weg.
    //if (::RemoteForceUninitialse(inOutAi.hProcess, inOutAi.hLibrary)) {
    //    inOutAi.hLibrary = NULL;
    //}
    if (::RemoteFreeLibrary(inOutAi.hProcess, inOutAi.hLibrary)) {
        inOutAi.hLibrary = NULL;
    }
    retval = (inOutAi.hLibrary == NULL) && retval;

    /* Free detours marker DLL. */
    if (::RemoteFreeLibrary(inOutAi.hProcess, inOutAi.hDetours)) {
        inOutAi.hDetours = NULL;
    }
    retval = (inOutAi.hDetours == NULL) && retval;

    // TODO: Clean the environment!

    /* Close process handle, if all libraries have been freed. */
    if (retval) {
        ::CloseHandle(inOutAi.hProcess);
        inOutAi.hProcess = NULL;
    }

    //::RemoteResumeAllThreads(inOutAi.hProcess);

    return retval;
}
#else /* _WIN32 */
#endif /* _WIN32 */
